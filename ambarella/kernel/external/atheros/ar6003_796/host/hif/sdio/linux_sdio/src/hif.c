//------------------------------------------------------------------------------
// <copyright file="hif.c" company="Atheros">
//    Copyright (c) 2004-2010 Atheros Corporation.  All rights reserved.
//
//
// Permission to use, copy, modify, and/or distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//
//------------------------------------------------------------------------------
//==============================================================================
// HIF layer reference implementation for Linux Native MMC stack
//
// Author(s): ="Atheros"
//==============================================================================
#include <linux/mmc/card.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>
#include <linux/mmc/sdio_func.h>
#include <linux/mmc/sdio_ids.h>
#include <linux/mmc/sdio.h>
#include <linux/mmc/sd.h>
#include <linux/kthread.h>
#include "common_drv.h"
#include "host_reg_table.h"

/* by default setup a bounce buffer for the data packets, if the underlying host controller driver
   does not use DMA you may be able to skip this step and save the memory allocation and transfer time */
#define HIF_USE_DMA_BOUNCE_BUFFER 1
#include "hif_internal.h"
#define ATH_MODULE_NAME hif
#include "a_debug.h"

#if HIF_USE_DMA_BOUNCE_BUFFER
/* macro to check if DMA buffer is WORD-aligned and DMA-able.  Most host controllers assume the
 * buffer is DMA'able and will bug-check otherwise (i.e. buffers on the stack).
 * virt_addr_valid check fails on stack memory.
 */
#define BUFFER_NEEDS_BOUNCE(buffer)  (((unsigned long)(buffer) & 0x3) || !virt_addr_valid((buffer)))
#ifdef USE_PREALLOC_BUFFER
extern void *ar6000_dma_buffer_alloc(int size);
extern int ar6000_dma_buffer_free(void *ptr);
#endif
#else
#define BUFFER_NEEDS_BOUNCE(buffer)   (FALSE)
#endif

#define MAX_HIF_DEVICES 2

/* ATHENV */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) && defined(CONFIG_PM)
#define dev_to_sdio_func(d)	container_of(d, struct sdio_func, dev)
#define to_sdio_driver(d)      container_of(d, struct sdio_driver, drv)
static int hifDeviceSuspend(struct device *dev);
static int hifDeviceResume(struct device *dev);
#endif /* CONFIG_PM */
static int hifDeviceInserted(struct sdio_func *func, const struct sdio_device_id *id);
static void hifDeviceRemoved(struct sdio_func *func);
static HIF_DEVICE *addHifDevice(struct sdio_func *func);
static HIF_DEVICE *getHifDevice(struct sdio_func *func);
static void delHifDevice(HIF_DEVICE * device);
static int Func0_CMD52WriteByte(struct mmc_card *card, unsigned int address, unsigned char byte);
static int Func0_CMD52ReadByte(struct mmc_card *card, unsigned int address, unsigned char *byte);
void hif_register_tbl_attach(A_UINT32 hif_type);

int reset_sdio_on_unload = 0;
module_param(reset_sdio_on_unload, int, 0644);

extern A_UINT32 nohifscattersupport;


/* ------ Static Variables ------ */
static const struct sdio_device_id ar6k_id_table[] = {
#ifdef AR6003_HEADERS_DEF
    {  SDIO_DEVICE(MANUFACTURER_CODE, (MANUFACTURER_ID_AR6003_BASE | 0x0))  },
    {  SDIO_DEVICE(MANUFACTURER_CODE, (MANUFACTURER_ID_AR6003_BASE | 0x1))  },
#endif
    { /* null */                                         },
};
MODULE_DEVICE_TABLE(sdio, ar6k_id_table);

static struct sdio_driver ar6k_driver = {
	.name = "ar6k_wlan",
	.id_table = ar6k_id_table,
	.probe = hifDeviceInserted,
	.remove = hifDeviceRemoved,
};

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) && defined(CONFIG_PM)
/* New suspend/resume based on linux-2.6.32
 * Need to patch linux-2.6.32 with mmc2.6.32_suspend.patch
 * Need to patch with msmsdcc2.6.29_suspend.patch for msm_sdcc host
     */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,29)
static struct dev_pm_ops ar6k_device_pm_ops = {
#else
static struct pm_ops ar6k_device_pm_ops = {
#endif
	.suspend = hifDeviceSuspend,
	.resume = hifDeviceResume,
};
#endif /* CONFIG_PM */

/* make sure we only unregister when registered. */
static int registered = 0;

OSDRV_CALLBACKS osdrvCallbacks;
extern A_UINT32 onebitmode;
extern A_UINT32 busspeedlow;
extern A_UINT32 debughif;

static HIF_DEVICE *hif_devices[MAX_HIF_DEVICES];

static void ResetAllCards(void);
static A_STATUS hifDisableFunc(HIF_DEVICE *device, struct sdio_func *func);
static A_STATUS hifEnableFunc(HIF_DEVICE *device, struct sdio_func *func);

#ifdef DEBUG

ATH_DEBUG_INSTANTIATE_MODULE_VAR(hif,
                                 "hif",
                                 "(Linux MMC) Host Interconnect Framework",
                                 ATH_DEBUG_MASK_DEFAULTS,
                                 0,
                                 NULL);

#endif


/* ------ Functions ------ */
A_STATUS HIFInit(OSDRV_CALLBACKS *callbacks)
{
    int status;
    AR_DEBUG_ASSERT(callbacks != NULL);

    A_REGISTER_MODULE_DEBUG_INFO(hif);

    /* store the callback handlers */
    osdrvCallbacks = *callbacks;

    /* Register with bus driver core */
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: HIFInit registering\n"));
    registered = 1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) && defined(CONFIG_PM)
    if (callbacks->deviceSuspendHandler && callbacks->deviceResumeHandler) {
        ar6k_driver.drv.pm = &ar6k_device_pm_ops;
    }
#endif /* CONFIG_PM */
    status = sdio_register_driver(&ar6k_driver);
    AR_DEBUG_ASSERT(status==0);

    if (status != 0) {
        return A_ERROR;
    }

    return A_OK;

}

static A_STATUS
__HIFReadWrite(HIF_DEVICE *device,
             A_UINT32 address,
             A_UCHAR *buffer,
             A_UINT32 length,
             A_UINT32 request,
             void *context)
{
    A_UINT8 opcode;
    A_STATUS    status = A_OK;
    int     ret;
    A_UINT8 *tbuffer;
    A_BOOL   bounced = FALSE;

    AR_DEBUG_ASSERT(device != NULL);
    AR_DEBUG_ASSERT(device->func != NULL);

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Device: 0x%p, buffer:0x%p (addr:0x%X)\n",
                    device, buffer, address));

    do {
        if (request & HIF_EXTENDED_IO) {
            //AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Command type: CMD53\n"));
        } else {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: Invalid command type: 0x%08x\n", request));
            status = A_EINVAL;
            break;
        }

        if (request & HIF_BLOCK_BASIS) {
            /* round to whole block length size */
            length = (length / HIF_MBOX_BLOCK_SIZE) * HIF_MBOX_BLOCK_SIZE;
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE,
                            ("AR6000: Block mode (BlockLen: %d)\n",
                            length));
        } else if (request & HIF_BYTE_BASIS) {
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE,
                            ("AR6000: Byte mode (BlockLen: %d)\n",
                            length));
        } else {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: Invalid data mode: 0x%08x\n", request));
            status = A_EINVAL;
            break;
        }

#if 0
        /* useful for checking register accesses */
        if (length & 0x3) {
            A_PRINTF(KERN_ALERT"AR6000: HIF (%s) is not a multiple of 4 bytes, addr:0x%X, len:%d\n",
                                request & HIF_WRITE ? "write":"read", address, length);
        }
#endif

        if (request & HIF_WRITE) {
            if ((address >= HIF_MBOX_START_ADDR(0)) &&
                (address <= HIF_MBOX_END_ADDR(3)))
            {

                AR_DEBUG_ASSERT(length <= HIF_MBOX_WIDTH);

                /*
                 * Mailbox write. Adjust the address so that the last byte
                 * falls on the EOM address.
                 */
                address += (HIF_MBOX_WIDTH - length);
            }
        }

        if (request & HIF_FIXED_ADDRESS) {
            opcode = CMD53_FIXED_ADDRESS;
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Address mode: Fixed 0x%X\n", address));
        } else if (request & HIF_INCREMENTAL_ADDRESS) {
            opcode = CMD53_INCR_ADDRESS;
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Address mode: Incremental 0x%X\n", address));
        } else {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: Invalid address mode: 0x%08x\n", request));
            status = A_EINVAL;
            break;
        }

        if (request & HIF_WRITE) {
#if HIF_USE_DMA_BOUNCE_BUFFER
            if (BUFFER_NEEDS_BOUNCE(buffer)) {
                AR_DEBUG_ASSERT(device->dma_buffer != NULL);
                tbuffer = device->dma_buffer;
                    /* copy the write data to the dma buffer */
                AR_DEBUG_ASSERT(length <= HIF_DMA_BUFFER_SIZE);
                memcpy(tbuffer, buffer, length);
                bounced = TRUE;
            } else {
                tbuffer = buffer;
            }
#else
	        tbuffer = buffer;
#endif
            if (opcode == CMD53_FIXED_ADDRESS) {
                ret = sdio_writesb(device->func, address, tbuffer, length);
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: writesb ret=%d address: 0x%X, len: %d, 0x%X\n",
						  ret, address, length, *(int *)tbuffer));
            } else {
                ret = sdio_memcpy_toio(device->func, address, tbuffer, length);
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: writeio ret=%d address: 0x%X, len: %d, 0x%X\n",
						  ret, address, length, *(int *)tbuffer));
            }
        } else if (request & HIF_READ) {
#if HIF_USE_DMA_BOUNCE_BUFFER
            if (BUFFER_NEEDS_BOUNCE(buffer)) {
                AR_DEBUG_ASSERT(device->dma_buffer != NULL);
                AR_DEBUG_ASSERT(length <= HIF_DMA_BUFFER_SIZE);
                tbuffer = device->dma_buffer;
                bounced = TRUE;
            } else {
                tbuffer = buffer;
            }
#else
            tbuffer = buffer;
#endif
            if (opcode == CMD53_FIXED_ADDRESS) {
                ret = sdio_readsb(device->func, tbuffer, address, length);
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: readsb ret=%d address: 0x%X, len: %d, 0x%X\n",
						  ret, address, length, *(int *)tbuffer));
            } else {
                ret = sdio_memcpy_fromio(device->func, tbuffer, address, length);
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: readio ret=%d address: 0x%X, len: %d, 0x%X\n",
						  ret, address, length, *(int *)tbuffer));
            }
#if HIF_USE_DMA_BOUNCE_BUFFER
            if (bounced) {
    	           /* copy the read data from the dma buffer */
                memcpy(buffer, tbuffer, length);
            }
#endif
        } else {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: Invalid direction: 0x%08x\n", request));
            status = A_EINVAL;
            break;
        }

        if (ret) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: SDIO bus operation failed! MMC stack returned : %d \n", ret));
            status = A_ERROR;
        }
    } while (FALSE);

    return status;
}

void AddToAsyncList(HIF_DEVICE *device, BUS_REQUEST *busrequest)
{
    unsigned long flags;
    BUS_REQUEST *async;
    BUS_REQUEST *active;

    spin_lock_irqsave(&device->asynclock, flags);
    active = device->asyncreq;
    if (active == NULL) {
        device->asyncreq = busrequest;
        device->asyncreq->inusenext = NULL;
    } else {
        for (async = device->asyncreq;
             async != NULL;
             async = async->inusenext) {
             active =  async;
        }
        active->inusenext = busrequest;
        busrequest->inusenext = NULL;
    }
    spin_unlock_irqrestore(&device->asynclock, flags);
}


/* queue a read/write request */
A_STATUS
HIFReadWrite(HIF_DEVICE *device,
             A_UINT32 address,
             A_UCHAR *buffer,
             A_UINT32 length,
             A_UINT32 request,
             void *context)
{
    A_STATUS    status = A_OK;
    BUS_REQUEST *busrequest;


    AR_DEBUG_ASSERT(device != NULL);
    AR_DEBUG_ASSERT(device->func != NULL);

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Device: %p addr:0x%X\n", device,address));

    do {
        if ((request & HIF_ASYNCHRONOUS) || (request & HIF_SYNCHRONOUS)){
            /* serialize all requests through the async thread */
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Execution mode: %s\n",
                        (request & HIF_ASYNCHRONOUS)?"Async":"Synch"));
            busrequest = hifAllocateBusRequest(device);
            if (busrequest == NULL) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                    ("AR6000: no async bus requests available (%s, addr:0x%X, len:%d) \n",
                        request & HIF_READ ? "READ":"WRITE", address, length));
                return A_ERROR;
            }
            busrequest->address = address;
            busrequest->buffer = buffer;
            busrequest->length = length;
            busrequest->request = request;
            busrequest->context = context;
            if (request & HIF_SYNCHRONOUS) {
                busrequest->status = A_PENDING;
            }

            AddToAsyncList(device, busrequest);

            if (request & HIF_SYNCHRONOUS) {
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: queued sync req: 0x%lX\n", (unsigned long)busrequest));

                /* wake up async_task to do work, then wait for completion */
                up(&device->sem_async);
                if (down_interruptible(&busrequest->sem_req) != 0) {
                    /* interrupted, try to clear busrequest from async list, then exit */
                    A_BOOL found = FALSE;
                    unsigned long flags;
                    BUS_REQUEST *currequest, *prevrequest;
                    A_INT32 cnt;

                    /*
                     * Lock out the async list and try to remove the busrequest.
                     * If we cannot find the busrequest, it must have already been processed.
                     */
                    spin_lock_irqsave(&device->asynclock, flags);

                    currequest = device->asyncreq;
                    prevrequest = NULL;
                    while (currequest != NULL) {
                        if (currequest == busrequest) {
                            /* Found the busrequest.  Dequeue it. */
                            if (currequest == device->asyncreq) {
                                /*
                                 * Request was the 1st request in the async list.  Have to update the pointer to the list.
                                 * Note that request->inusenext may be NULL; this is OK.
                                 */
                                device->asyncreq = currequest->inusenext;
                            } else {
                                /*
                                 * Set the previous request's inusenext pointer to this request's inusenext pointer.  Note that
                                 * this request's inusenext pointer may be NULL; this is OK.
                                 */
                                prevrequest->inusenext = currequest->inusenext;
                            }

                            /* We found and removed the busrequest, so break out of the loop. */
                            found = TRUE;
                            break;
                        }
                        /* Haven't found it yet, keep walking down the list. */
                        prevrequest = currequest;
                        currequest = currequest->inusenext;
                    }
                    spin_unlock_irqrestore(&device->asynclock, flags);

                    if (!found) {
                        /* Wait until status has been updated by async_task */
                        cnt = 100;

                        /*
                         * Need to grab the lock when checking status.  The reason
                         * is that the async_task thread sets the status to the return
                         * value of __HIFReadWrite, and THEN calls up() on the semaphore.
                         * If the system context switches from async_task to this thread
                         * AFTER the async_task thread changes the status of the busrequest but
                         * BEFORE it calls up, then this thread might otherwise continue on and
                         * free the busrequest, then return.  Should async_task continue to run
                         * after the busrequest is freed, up() will then be called, and then
                         * the semaphore would have a nonzero value, which would cause major
                         * problems the next time the busrequest is used.
                         */
                        spin_lock_irqsave(&device->asynclock, flags);
                        while (busrequest->status == A_PENDING && --cnt > 0) {
                            spin_unlock_irqrestore(&device->asynclock, flags);
                            A_MDELAY(10);
                            spin_lock_irqsave(&device->asynclock, flags);
                        }
                        spin_unlock_irqrestore(&device->asynclock, flags);
                        if (!cnt) {
                            /* If we get in here then async_task needed more time */
                            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR, ("Not enough time for async_task to change status from A_PENDING\n"));
                            A_ASSERT(0);
                        }
                    }

                    /* free bus request and exit */
                    hifFreeBusRequest(device, busrequest);
                    return A_ECANCELED;
                }
                status = busrequest->status;
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: sync return freeing 0x%lX: 0x%X\n",
						      (unsigned long)busrequest, busrequest->status));
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: freeing req: 0x%X\n", (unsigned int)request));
                hifFreeBusRequest(device, busrequest);
                return status;
            } else {
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: queued async req: 0x%lX\n", (unsigned long)busrequest));
                up(&device->sem_async);
                return A_PENDING;
            }
        } else {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: Invalid execution mode: 0x%08x\n", (unsigned int)request));
            status = A_EINVAL;
            break;
        }
    } while(0);

    return status;
}
/* thread to serialize all requests, both sync and async */
static int async_task(void *param)
 {
    HIF_DEVICE *device;
    BUS_REQUEST *request;
    A_STATUS status;
    unsigned long flags;

    device = (HIF_DEVICE *)param;
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: async task\n"));
    set_current_state(TASK_INTERRUPTIBLE);
    while(!device->async_shutdown) {
        /* wait for work */
        if (down_interruptible(&device->sem_async) != 0) {
            /* interrupted, exit */
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: async task interrupted\n"));
            break;
        }
        if (device->async_shutdown) {
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: async task stopping\n"));
            break;
        }
        /* we want to hold the host over multiple cmds if possible, but holding the host blocks card interrupts */
        sdio_claim_host(device->func);
        spin_lock_irqsave(&device->asynclock, flags);
        /* pull the request to work on */
        while (device->asyncreq != NULL) {
            request = device->asyncreq;
            if (request->inusenext != NULL) {
                device->asyncreq = request->inusenext;
            } else {
                device->asyncreq = NULL;
            }
            spin_unlock_irqrestore(&device->asynclock, flags);
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: async_task processing req: 0x%lX\n", (unsigned long)request));

            if (request->pScatterReq != NULL) {
                A_ASSERT(device->scatter_enabled);
                    /* this is a queued scatter request, pass the request to scatter routine which
                     * executes it synchronously, note, no need to free the request since scatter requests
                     * are maintained on a separate list */
                status = DoHifReadWriteScatter(device,request);
            } else {
                    /* call HIFReadWrite in sync mode to do the work */
                status = __HIFReadWrite(device, request->address, request->buffer,
                                      request->length, request->request & ~HIF_SYNCHRONOUS, NULL);
                if (request->request & HIF_ASYNCHRONOUS) {
                    void *context = request->context;
                    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: async_task freeing req: 0x%lX\n", (unsigned long)request));
                    hifFreeBusRequest(device, request);
                    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: async_task completion routine req: 0x%lX\n", (unsigned long)request));
                    device->htcCallbacks.rwCompletionHandler(context, status);
                } else {
                    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: async_task upping req: 0x%lX\n", (unsigned long)request));

                    /*
                     * Grab the lock here.  This ensures that, between the
                     * time we set the status to the return value of __HIFReadWrite,
                     * and the time we call up() on the semaphore, if the ioctl thread
                     * has been interrupted and already has woken up, the ioctl thread does
                     * NOT proceed until AFTER up() is called.  This way,
                     * hifFreeBusRequest() is guaranteed to zero out the semaphore
                     * AFTER up() is called, which prevents synchronization issues.
                     */
                    spin_lock_irqsave(&device->asynclock, flags);
                    request->status = status;
                    up(&request->sem_req);
                    spin_unlock_irqrestore(&device->asynclock, flags);
                }
            }
            spin_lock_irqsave(&device->asynclock, flags);
        }
        spin_unlock_irqrestore(&device->asynclock, flags);
        sdio_release_host(device->func);
    }

    complete_and_exit(&device->async_completion, 0);
    return 0;
}

static A_INT32 IssueSDCommand(HIF_DEVICE *device, A_UINT32 opcode, A_UINT32 arg, A_UINT32 flags, A_UINT32 *resp)
{
    struct mmc_command cmd;
    A_INT32 err;
    struct mmc_host *host;
	struct sdio_func *func;

    func = device->func;
    host = func->card->host;

    memset(&cmd, 0, sizeof(struct mmc_command));
    cmd.opcode = opcode;
    cmd.arg = arg;
    cmd.flags = flags;
    err = mmc_wait_for_cmd(host, &cmd, 3);

    if ((!err) && (resp)) {
        *resp = cmd.resp[0];
    }

    return err;
}
A_STATUS ReinitSDIO(HIF_DEVICE *device)
{
    A_INT32 err = 0;
    struct mmc_host *host;
    struct mmc_card *card;
	struct sdio_func *func;
    A_UINT8  cmd52_resp;
    A_UINT32 clock;

    func = device->func;
    card = func->card;
    host = card->host;

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +ReinitSDIO \n"));
    sdio_claim_host(func);

    do {
        /* 2.6.32 kernel does part of the SDIO initalization upon resume */
        A_BOOL lt_2_6_32 = (LINUX_VERSION_CODE<KERNEL_VERSION(2,6,32));
        if (lt_2_6_32) {
            A_UINT32 resp;
            A_UINT16 rca;
            A_UINT32 i;
            int bit = fls(host->ocr_avail) - 1;
            /* emulate the mmc_power_up(...) */
            host->ios.vdd = bit;
            host->ios.chip_select = MMC_CS_DONTCARE;
            host->ios.bus_mode = MMC_BUSMODE_OPENDRAIN;
            host->ios.power_mode = MMC_POWER_UP;
            host->ios.bus_width = MMC_BUS_WIDTH_1;
            host->ios.timing = MMC_TIMING_LEGACY;
            host->ops->set_ios(host, &host->ios);
            /*
             * This delay should be sufficient to allow the power supply
             * to reach the minimum voltage.
             */
            msleep(2);

            host->ios.clock = host->f_min;
            host->ios.power_mode = MMC_POWER_ON;
            host->ops->set_ios(host, &host->ios);

            /*
             * This delay must be at least 74 clock sizes, or 1 ms, or the
             * time required to reach a stable voltage.
             */
            msleep(2);

            /* Issue CMD0. Goto idle state */
	        host->ios.chip_select = MMC_CS_HIGH;
            host->ops->set_ios(host, &host->ios);
	        msleep(1);
            err = IssueSDCommand(device, MMC_GO_IDLE_STATE, 0, (MMC_RSP_NONE | MMC_CMD_BC), NULL);
            host->ios.chip_select = MMC_CS_DONTCARE;
            host->ops->set_ios(host, &host->ios);
	        msleep(1);
            host->use_spi_crc = 0;

            if (err) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD0 failed : %d \n",err));
                break;
            }

            if (!host->ocr) {
                /* Issue CMD5, arg = 0 */
                err = IssueSDCommand(device, SD_IO_SEND_OP_COND, 0, (MMC_RSP_R4 | MMC_CMD_BCR), &resp);
                if (err) {
                    AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD5 failed : %d \n",err));
                    break;
                }
                host->ocr = resp;
            }

            /* Issue CMD5, arg = ocr. Wait till card is ready  */
            for (i=0;i<100;i++) {
                err = IssueSDCommand(device, SD_IO_SEND_OP_COND, host->ocr, (MMC_RSP_R4 | MMC_CMD_BCR), &resp);
                if (err) {
                    AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD5 failed : %d \n",err));
                    break;
                }
                if (resp & MMC_CARD_BUSY) {
                    break;
                }
                msleep(10);
            }

            if ((i == 100) || (err)) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: card in not ready : %d %d \n",i,err));
                break;
            }

            /* Issue CMD3, get RCA */
            err = IssueSDCommand(device, SD_SEND_RELATIVE_ADDR, 0, MMC_RSP_R6 | MMC_CMD_BCR, &resp);
            if (err) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD3 failed : %d \n",err));
                break;
            }
            rca = resp >> 16;
            host->ios.bus_mode = MMC_BUSMODE_PUSHPULL;
            host->ops->set_ios(host, &host->ios);

            /* Issue CMD7, select card  */
            err = IssueSDCommand(device, MMC_SELECT_CARD, (rca << 16), MMC_RSP_R1 | MMC_CMD_AC, NULL);
            if (err) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD7 failed : %d \n",err));
                break;
            }
        }

        /* Enable high speed */
        if (card->host->caps & MMC_CAP_SD_HIGHSPEED) {
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("ReinitSDIO: Set high speed mode\n"));
            err = Func0_CMD52ReadByte(card, SDIO_CCCR_SPEED, &cmd52_resp);
            if (err) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD52 read to CCCR speed register failed  : %d \n",err));
                card->state &= ~MMC_STATE_HIGHSPEED;
                /* no need to break */
            } else {
                err = Func0_CMD52WriteByte(card, SDIO_CCCR_SPEED, (cmd52_resp | SDIO_SPEED_EHS));
                if (err) {
                    AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD52 write to CCCR speed register failed  : %d \n",err));
                    break;
                }
                mmc_card_set_highspeed(card);
                host->ios.timing = MMC_TIMING_SD_HS;
                host->ops->set_ios(host, &host->ios);
            }
        }

        /* Set clock */
        if (mmc_card_highspeed(card)) {
            clock = 50000000;
        } else {
            clock = card->cis.max_dtr;
        }

        if (clock > host->f_max) {
            clock = host->f_max;
        }
        host->ios.clock = clock;
        host->ops->set_ios(host, &host->ios);


        if (card->host->caps & MMC_CAP_4_BIT_DATA) {
            /* CMD52: Set bus width & disable card detect resistor */
            err = Func0_CMD52WriteByte(card, SDIO_CCCR_IF, SDIO_BUS_CD_DISABLE | SDIO_BUS_WIDTH_4BIT);
            if (err) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("ReinitSDIO: CMD52 to set bus mode failed : %d \n",err));
                break;
            }
            host->ios.bus_width = MMC_BUS_WIDTH_4;
            host->ops->set_ios(host, &host->ios);
        }
    } while (0);

    sdio_release_host(func);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -ReinitSDIO \n"));

    return (err) ? A_ERROR : A_OK;
}

#if defined(CONFIG_PM)
/*
 * Setup IRQ mode for deep sleep and WoW
 * Switch back to 1 bits mode when we suspend for WoW in order to
 * detect SDIO irq without clock.
 * Re-enable async 4-bit irq mode for some host controllers after resume
 */
static int SdioEnable4bits(HIF_DEVICE *device,  int enable)
{
    int ret = 0;
    struct sdio_func *func = device->func;
    struct mmc_card *card = func->card;
    struct mmc_host *host = card->host;
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
    unsigned char ctrl = 0;
    unsigned int width;
#ifdef SDIO_BUS_WIDTH_8BIT
    unsigned char wide_mask = (SDIO_BUS_WIDTH_4BIT|SDIO_BUS_WIDTH_8BIT);
#else
    unsigned char wide_mask = (SDIO_BUS_WIDTH_4BIT);
#endif
#endif

    if (!(host->caps & (MMC_CAP_4_BIT_DATA)))
        return 0;

    if (card->cccr.low_speed && !card->cccr.wide_bus)
        return 0;

    sdio_claim_host(func);
    do {
        /* 2.6.34 will setup 1bits automatically. No need to setup */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34)
        ret = Func0_CMD52ReadByte(card, SDIO_CCCR_IF, &ctrl);
        if (ret) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("%s: Fail to read CCCR_IF : %d \n", __func__, ret));
            break;
        }
        if (enable) {
            width = MMC_BUS_WIDTH_4;
            ctrl &= ~(SDIO_BUS_WIDTH_1BIT|wide_mask);
            ctrl |= SDIO_BUS_WIDTH_4BIT;
        } else {
            width = MMC_BUS_WIDTH_1;
            ctrl &= ~(wide_mask);
        }

        ret = Func0_CMD52WriteByte(card, SDIO_CCCR_IF, ctrl);
        if (ret) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("%s: Fail to write CCCR_IF : %d \n", __func__, ret));
             break;
        }
        host->ios.bus_width = width;
        host->ops->set_ios(host, &host->ios);
#endif /* LINUX_VERSION_CODE < KERNEL_VERSION(2,6,34) */

        if ((device->id->device & MANUFACTURER_ID_AR6K_BASE_MASK) >= MANUFACTURER_ID_AR6003_BASE) {
            /* Re-enable 4-bit ASYNC interrupt on AR6003x after system resume for some host controller */
            ret = Func0_CMD52WriteByte(func->card, CCCR_SDIO_IRQ_MODE_REG, enable ? SDIO_IRQ_MODE_ASYNC_4BIT_IRQ : 0);
            if (ret) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("AR6000: failed to setup 4-bit ASYNC IRQ mode into %d err %d \n", enable, ret));
            } else {
                AR_DEBUG_PRINTF(ATH_DEBUG_INFO, ("AR6000: Setup 4-bit ASYNC IRQ mode into %d successfully\n", enable));
            }
        }
    } while (0);
    sdio_release_host(func);
    return ret;
}
#endif
A_STATUS
PowerStateChangeNotify(HIF_DEVICE *device, HIF_DEVICE_POWER_CHANGE_TYPE config)
{
    A_STATUS status = A_OK;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) && defined(CONFIG_PM)
	struct sdio_func *func = device->func;
    int old_reset_val;
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +PowerStateChangeNotify %d\n", config));
    switch (config) {
       case HIF_DEVICE_POWER_DOWN:
            /* Disable 4bits in order to let SDIO bus detect DAT1 as interrupt source */
            SdioEnable4bits(device, 0);
            break;
       case HIF_DEVICE_POWER_CUT:
            old_reset_val = reset_sdio_on_unload;
            reset_sdio_on_unload = 1;
            status = hifDisableFunc(device, func);
            reset_sdio_on_unload = old_reset_val;
            if (!device->is_suspend) {
                device->powerConfig = config;
                mmc_detect_change(device->host, HZ/3);
            }
            break;
       case HIF_DEVICE_POWER_UP:
            if (device->powerConfig == HIF_DEVICE_POWER_CUT) {
                if (device->is_suspend) {
                    status = ReinitSDIO(device);
                    if (status == A_OK) {
                        status = hifEnableFunc(device, func);
                    }
                } else {
                    /* device->func is bad pointer at this time */
                    mmc_detect_change(device->host, 0);
                    return A_PENDING; /* Don't change powerConfig status */
                }
            } else if (device->powerConfig == HIF_DEVICE_POWER_DOWN) {
                int ret = SdioEnable4bits(device, 1);
                status = (ret==0) ? A_OK : A_ERROR;
            }
            break;
    }
    device->powerConfig = config;

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -PowerStateChangeNotify\n"));
#endif
    return status;
}

A_STATUS
HIFConfigureDevice(HIF_DEVICE *device, HIF_DEVICE_CONFIG_OPCODE opcode,
                   void *config, A_UINT32 configLen)
{
    A_UINT32 count;
    A_STATUS status = A_OK;

    switch(opcode) {
        case HIF_DEVICE_GET_MBOX_BLOCK_SIZE:
            ((A_UINT32 *)config)[0] = HIF_MBOX0_BLOCK_SIZE;
            ((A_UINT32 *)config)[1] = HIF_MBOX1_BLOCK_SIZE;
            ((A_UINT32 *)config)[2] = HIF_MBOX2_BLOCK_SIZE;
            ((A_UINT32 *)config)[3] = HIF_MBOX3_BLOCK_SIZE;
            break;

        case HIF_DEVICE_GET_MBOX_ADDR:
            for (count = 0; count < 4; count ++) {
                ((A_UINT32 *)config)[count] = HIF_MBOX_START_ADDR(count);
            }

            if (configLen >= sizeof(HIF_DEVICE_MBOX_INFO)) {
                SetExtendedMboxWindowInfo((A_UINT16)device->func->device,
                                          (HIF_DEVICE_MBOX_INFO *)config);
            }

            break;
        case HIF_DEVICE_GET_PENDING_EVENTS_FUNC:
            AR_DEBUG_PRINTF(ATH_DEBUG_INFO,
                            ("AR6000: configuration opcode %d is not used for Linux SDIO stack", opcode));
            status = A_ERROR;
            break;
        case HIF_DEVICE_GET_IRQ_PROC_MODE:
            *((HIF_DEVICE_IRQ_PROCESSING_MODE *)config) = HIF_DEVICE_IRQ_SYNC_ONLY;
            break;
        case HIF_DEVICE_GET_RECV_EVENT_MASK_UNMASK_FUNC:
            AR_DEBUG_PRINTF(ATH_DEBUG_INFO,
                            ("AR6000: configuration opcode %d is not used for Linux SDIO stack", opcode));
            status = A_ERROR;
            break;
        case HIF_CONFIGURE_QUERY_SCATTER_REQUEST_SUPPORT:
            if (!device->scatter_enabled) {
                return A_ENOTSUP;
            }
            status = SetupHIFScatterSupport(device, (HIF_DEVICE_SCATTER_SUPPORT_INFO *)config);
            if (A_FAILED(status)) {
                device->scatter_enabled = FALSE;
            }
            break;
        case HIF_DEVICE_GET_OS_DEVICE:
                /* pass back a pointer to the SDIO function's "dev" struct */
            if (device->func) {
                ((HIF_DEVICE_OS_DEVICE_INFO *)config)->pOSDevice = &device->func->dev;
            } else {
                ((HIF_DEVICE_OS_DEVICE_INFO *)config)->pOSDevice = NULL;
                status = A_DEVICE_NOT_FOUND;
            }
            break;
        case HIF_DEVICE_POWER_STATE_CHANGE:
            status = PowerStateChangeNotify(device, *(HIF_DEVICE_POWER_CHANGE_TYPE *)config);
            break;
        case HIF_DEVICE_GET_IRQ_YIELD_PARAMS:
            AR_DEBUG_PRINTF(ATH_DEBUG_WARN,
                            ("AR6000: configuration opcode %d is only used for RTOS systems, not Linux systems", opcode));
            status = A_ERROR;
            break;
        default:
            AR_DEBUG_PRINTF(ATH_DEBUG_WARN,
                            ("AR6000: Unsupported configuration opcode: %d\n", opcode));
            status = A_ERROR;
    }

    return status;
}

void
HIFShutDownDevice(HIF_DEVICE *device)
{
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +HIFShutDownDevice\n"));
    if (device != NULL) {
        AR_DEBUG_ASSERT(device->powerConfig==HIF_DEVICE_POWER_CUT || device->func != NULL);
    } else {
        int i;
            /* since we are unloading the driver anyways, reset all cards in case the SDIO card
             * is externally powered and we are unloading the SDIO stack.  This avoids the problem when
             * the SDIO stack is reloaded and attempts are made to re-enumerate a card that is already
             * enumerated */
        AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: HIFShutDownDevice, resetting\n"));
        ResetAllCards();

        /* Unregister with bus driver core */
        if (registered) {
            registered = 0;
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE,
                            ("AR6000: Unregistering with the bus driver\n"));
            sdio_unregister_driver(&ar6k_driver);
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE,
                            ("AR6000: Unregistered\n"));
        }

        for (i=0; i<MAX_HIF_DEVICES; ++i) {
            if (hif_devices[i] && hif_devices[i]->func == NULL) {
                AR_DEBUG_PRINTF(ATH_DEBUG_TRACE,
                                ("AR6000: Remove pending hif_device %p\n", hif_devices[i]));
                delHifDevice(hif_devices[i]);
                hif_devices[i] = NULL;
            }
        }
    }
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -HIFShutDownDevice\n"));
}

static void
hifIRQHandler(struct sdio_func *func)
{
#ifdef DEBUG
    A_STATUS status;
#endif
    HIF_DEVICE *device;
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +hifIRQHandler\n"));

    device = getHifDevice(func);
    atomic_set(&device->irqHandling, 1);
    /* release the host during ints so we can pick it back up when we process cmds */
    sdio_release_host(device->func);
#ifdef DEBUG
    status = device->htcCallbacks.dsrHandler(device->htcCallbacks.context);
#else
    device->htcCallbacks.dsrHandler(device->htcCallbacks.context);
#endif
    sdio_claim_host(device->func);
    atomic_set(&device->irqHandling, 0);
    AR_DEBUG_ASSERT(status == A_OK || status == A_ECANCELED);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -hifIRQHandler\n"));
}

/* handle HTC startup via thread*/
static int startup_task(void *param)
{
    HIF_DEVICE *device;

    device = (HIF_DEVICE *)param;
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: call HTC from startup_task\n"));
        /* start  up inform DRV layer */
    if ((osdrvCallbacks.deviceInsertedHandler(osdrvCallbacks.context,device)) != A_OK) {
        AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Device rejected\n"));
    }
    return 0;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) && defined(CONFIG_PM)
static int enable_task(void *param)
{
    HIF_DEVICE *device;
    device = (HIF_DEVICE *)param;
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: call  from resume_task\n"));

        /* start  up inform DRV layer */
    if (device &&
        device->claimedContext &&
        osdrvCallbacks.devicePowerChangeHandler &&
        osdrvCallbacks.devicePowerChangeHandler(device->claimedContext, HIF_DEVICE_POWER_UP) != A_OK)
    {
        AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: Device rejected\n"));
    }

    return 0;
}
#endif

static void hifAssignTargetHeaders(A_UINT16 SDIO_ID)
{
     switch (SDIO_ID) {
         case MANUFACTURER_ID_AR6003_BASE:
             hif_register_tbl_attach(HIF_TYPE_AR6003);
         break;

         case MANUFACTURER_ID_MCKINLEY_BASE:
             hif_register_tbl_attach(HIF_TYPE_MCKINLEY);
         break;
         default:
             hif_register_tbl_attach(HIF_TYPE_AR6003);
         break;
    }
}

static int hifDeviceInserted(struct sdio_func *func, const struct sdio_device_id *id)
{
    int i;
    int ret;
    HIF_DEVICE * device = NULL;
    int count;

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE,
		    ("AR6000: hifDeviceInserted, Function: 0x%X, Vendor ID: 0x%X, Device ID: 0x%X, block size: 0x%X/0x%X\n",
		     func->num, func->vendor, func->device, func->max_blksize, func->cur_blksize));

    for (i=0; i<MAX_HIF_DEVICES; ++i) {
        HIF_DEVICE *hifdevice = hif_devices[i];
        if (hifdevice && hifdevice->powerConfig == HIF_DEVICE_POWER_CUT &&
                hifdevice->host == func->card->host) {
            hifdevice->func = func;
            hifdevice->powerConfig = HIF_DEVICE_POWER_UP;
            sdio_set_drvdata(func, hifdevice);
            device = getHifDevice(func);
            ret = 0;
            break;
        }
    }

    if (device==NULL) {
        addHifDevice(func);
        device = getHifDevice(func);
        hifAssignTargetHeaders(id->device);

        for (i=0; i<MAX_HIF_DEVICES; ++i) {
            if (hif_devices[i] == NULL) {
                hif_devices[i] = device;
                break;
            }
        }
        if (i==MAX_HIF_DEVICES) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
		        ("AR6000: hifDeviceInserted, No more hif_devices[] slot for %p", device));
        }

        device->id = id;
        device->host = func->card->host;
        device->is_disabled = TRUE;

        spin_lock_init(&device->lock);

        spin_lock_init(&device->asynclock);

        DL_LIST_INIT(&device->ScatterReqHead);

        if (!nohifscattersupport) {
            /* try to allow scatter operation on all instances,
             * unless globally overridden */
            device->scatter_enabled = TRUE;
        }

        /* Initialize the bus requests to be used later */
        A_MEMZERO(device->busRequest, sizeof(device->busRequest));
        for (count = 0; count < BUS_REQUEST_MAX_NUM; count ++) {
            sema_init(&device->busRequest[count].sem_req, 0);
            hifFreeBusRequest(device, &device->busRequest[count]);
        }
        sema_init(&device->sem_async, 0);
    }

    ret = hifEnableFunc(device, func);
    return (ret == A_OK || ret == A_PENDING) ? 0 : -1;
}


void
HIFAckInterrupt(HIF_DEVICE *device)
{
    AR_DEBUG_ASSERT(device != NULL);

    /* Acknowledge our function IRQ */
}

void
HIFUnMaskInterrupt(HIF_DEVICE *device)
{
#ifdef DEBUG
    int ret;
#endif

    AR_DEBUG_ASSERT(device != NULL);
    AR_DEBUG_ASSERT(device->func != NULL);

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: HIFUnMaskInterrupt\n"));

    /* Register the IRQ Handler */
    sdio_claim_host(device->func);
#ifdef DEBUG
    ret = sdio_claim_irq(device->func, hifIRQHandler);
#else
    sdio_claim_irq(device->func, hifIRQHandler);
#endif
    sdio_release_host(device->func);
    AR_DEBUG_ASSERT(ret == 0);
}

void HIFMaskInterrupt(HIF_DEVICE *device)
{
    int ret;
    AR_DEBUG_ASSERT(device != NULL);
    AR_DEBUG_ASSERT(device->func != NULL);

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: HIFMaskInterrupt\n"));

    /* Mask our function IRQ */
    sdio_claim_host(device->func);
    while (atomic_read(&device->irqHandling)) {
        sdio_release_host(device->func);
        schedule_timeout_interruptible(HZ/10);
        sdio_claim_host(device->func);
    }
    ret = sdio_release_irq(device->func);
    sdio_release_host(device->func);
    if (ret) {
        if (ret == -ETIMEDOUT) {
            AR_DEBUG_PRINTF(ATH_DEBUG_WARN,
                       ("AR6000: Timeout to mask interrupt. Card removed?\n"));
        } else {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                       ("AR6000: Unable to mask interrupt %d\n", ret));
            AR_DEBUG_ASSERT(ret == 0);
        }
    }
}

BUS_REQUEST *hifAllocateBusRequest(HIF_DEVICE *device)
{
    BUS_REQUEST *busrequest;
    unsigned long flag;

    /* Acquire lock */
    spin_lock_irqsave(&device->lock, flag);

    /* Remove first in list */
    if((busrequest = device->s_busRequestFreeQueue) != NULL)
    {
        device->s_busRequestFreeQueue = busrequest->next;
    }
    /* Release lock */
    spin_unlock_irqrestore(&device->lock, flag);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: hifAllocateBusRequest: 0x%p\n", busrequest));
    return busrequest;
}

void
hifFreeBusRequest(HIF_DEVICE *device, BUS_REQUEST *busrequest)
{
    unsigned long flag;

    AR_DEBUG_ASSERT(busrequest != NULL);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: hifFreeBusRequest: 0x%p\n", busrequest));
    /* Acquire lock */
    spin_lock_irqsave(&device->lock, flag);


    /* Insert first in list */
    busrequest->next = device->s_busRequestFreeQueue;
    busrequest->inusenext = NULL;
    device->s_busRequestFreeQueue = busrequest;

    /* Reset the semaphore value to 0 (it may have been left at 1 if HIFReadWrite was interrupted) */
    sema_init(&busrequest->sem_req, 0);

    /* Release lock */
    spin_unlock_irqrestore(&device->lock, flag);
}

static A_STATUS hifDisableFunc(HIF_DEVICE *device, struct sdio_func *func)
{
    int ret;
    A_STATUS status = A_OK;

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +hifDisableFunc\n"));
    device = getHifDevice(func);
    if (!IS_ERR(device->async_task)) {
        init_completion(&device->async_completion);
        device->async_shutdown = 1;
        up(&device->sem_async);
        wait_for_completion(&device->async_completion);
        device->async_task = NULL;
        sema_init(&device->sem_async, 0);
    }
    /* Disable the card */
    sdio_claim_host(device->func);
    ret = sdio_disable_func(device->func);
    if (ret) {
        status = A_ERROR;
    }

    if (reset_sdio_on_unload && status == A_OK) {
        /* reset the SDIO interface.  This is useful in automated testing where the card
         * does not need to be removed at the end of the test.  It is expected that the user will
         * also unload/reload the host controller driver to force the bus driver to re-enumerate the slot */
        AR_DEBUG_PRINTF(ATH_DEBUG_INFO, ("AR6000: reseting SDIO card back to uninitialized state \n"));

        /* NOTE : sdio_f0_writeb() cannot be used here, that API only allows access
         *        to undefined registers in the range of: 0xF0-0xFF */

        ret = Func0_CMD52WriteByte(device->func->card, SDIO_CCCR_ABORT, (1 << 3));
        if (ret) {
            status = A_ERROR;
            AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("AR6000: reset failed : %d \n",ret));
        }
    }

    sdio_release_host(device->func);

    if (status == A_OK) {
        device->is_disabled = TRUE;
    }
    CleanupHIFScatterResources(device);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -hifDisableFunc\n"));

    return status;
}

static A_STATUS hifEnableFunc(HIF_DEVICE *device, struct sdio_func *func)
{
    struct task_struct* pTask;
    const char *taskName = NULL;
    int (*taskFunc)(void *) = NULL;
    int ret = A_OK;

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +hifEnableFunc\n"));
    device = getHifDevice(func);

    if (device->is_disabled) {
       /* enable the SDIO function */
        sdio_claim_host(func);

        if ((device->id->device & MANUFACTURER_ID_AR6K_BASE_MASK) >= MANUFACTURER_ID_AR6003_BASE) {
            /* enable 4-bit ASYNC interrupt on AR6003 or later devices */
            ret = Func0_CMD52WriteByte(func->card, CCCR_SDIO_IRQ_MODE_REG, SDIO_IRQ_MODE_ASYNC_4BIT_IRQ);
            if (ret) {
                AR_DEBUG_PRINTF(ATH_DEBUG_ERR, ("AR6000: failed to enable 4-bit ASYNC IRQ mode %d \n",ret));
                sdio_release_host(func);
                return A_ERROR;
            }
            AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: 4-bit ASYNC IRQ mode enabled\n"));
        }
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
        /* give us some time to enable, in ms */
        func->enable_timeout = 100;
#endif
        ret = sdio_enable_func(func);
        if (ret) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR, ("AR6000: %s(), Unable to enable AR6K: 0x%X\n",
					  __FUNCTION__, ret));
            sdio_release_host(func);
            return A_ERROR;
        }
        ret = sdio_set_block_size(func, HIF_MBOX_BLOCK_SIZE);
        sdio_release_host(func);
        if (ret) {
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR, ("AR6000: %s(), Unable to set block size 0x%x  AR6K: 0x%X\n",
					  __FUNCTION__, HIF_MBOX_BLOCK_SIZE, ret));
            return A_ERROR;
        }
        device->is_disabled = FALSE;
        /* create async I/O thread */
        if (!device->async_task) {
            device->async_shutdown = 0;
            device->async_task = kthread_create(async_task,
                                           (void *)device,
                                           "AR6K Async");
           if (IS_ERR(device->async_task)) {
               AR_DEBUG_PRINTF(ATH_DEBUG_ERROR, ("AR6000: %s(), to create async task\n", __FUNCTION__));
                return A_ERROR;
           }
           AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: start async task\n"));
           wake_up_process(device->async_task );
        }
    }

    if (!device->claimedContext) {
        taskFunc = startup_task;
        taskName = "AR6K startup";
        ret = A_OK;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) && defined(CONFIG_PM)
    } else {
        taskFunc = enable_task;
        taskName = "AR6K enable";
        ret = A_PENDING;
#endif /* CONFIG_PM */
    }
    /* create resume thread */
    pTask = kthread_create(taskFunc, (void *)device, taskName);
    if (IS_ERR(pTask)) {
        AR_DEBUG_PRINTF(ATH_DEBUG_ERROR, ("AR6000: %s(), to create enabel task\n", __FUNCTION__));
        return A_ERROR;
    }
    wake_up_process(pTask);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -hifEnableFunc\n"));

    /* task will call the enable func, indicate pending */
    return ret;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27) && defined(CONFIG_PM)
static int hifDeviceSuspend(struct device *dev)
{
    struct sdio_func *func=dev_to_sdio_func(dev);
    A_STATUS status = A_OK;
    HIF_DEVICE *device = getHifDevice(func);

#if defined(MMC_PM_KEEP_POWER) || (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
    struct mmc_host *host = NULL;
    if (device && device->func) {
        host = device->func->card->host;
    }
#endif

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +hifDeviceSuspend\n"));
    if (device && device->claimedContext && osdrvCallbacks.deviceSuspendHandler) {
        device->is_suspend = TRUE; /* set true first for PowerStateChangeNotify(..) */
        status = osdrvCallbacks.deviceSuspendHandler(device->claimedContext);
    }

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -hifDeviceSuspend\n"));

    switch (status) {
    case A_OK:
#if defined(MMC_PM_KEEP_POWER) || (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
        if (host)  {
            host->pm_flags &= ~(MMC_PM_KEEP_POWER|MMC_PM_WAKE_SDIO_IRQ);
        }
#endif
        return 0;
    case A_EBUSY:
#if defined(MMC_PM_KEEP_POWER) || (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,34))
        if (host) {
            /* Some controller need to WAKE_SDIO_IRQ in order to wake up by DAT1 */
            host->pm_flags |=  (MMC_PM_KEEP_POWER|MMC_PM_WAKE_SDIO_IRQ);
            host->pm_flags &= host->pm_caps;
        }
        return 0;
#else
        return -EBUSY; /* Hack for kernel to support deep sleep and wow */
#endif
    default:
        device->is_suspend = FALSE;
        return -1;
    }
}

static int hifDeviceResume(struct device *dev)
{
    struct sdio_func *func=dev_to_sdio_func(dev);
    A_STATUS status = A_OK;
    HIF_DEVICE *device;

    device = getHifDevice(func);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +hifDeviceResume\n"));
    if (device && device->claimedContext && osdrvCallbacks.deviceSuspendHandler) {
        status = osdrvCallbacks.deviceResumeHandler(device->claimedContext);
        device->is_suspend = FALSE;
    }
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -hifDeviceResume\n"));

    return A_SUCCESS(status) ? 0 : status;
}
#endif /* CONFIG_PM */

static void hifDeviceRemoved(struct sdio_func *func)
{
    A_STATUS status = A_OK;
    HIF_DEVICE *device;
    AR_DEBUG_ASSERT(func != NULL);

    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: +hifDeviceRemoved\n"));
    device = getHifDevice(func);

    if (device->powerConfig == HIF_DEVICE_POWER_CUT) {
        device->func = NULL; /* func will be free by mmc stack */
        return; /* Just return for cut-off mode */
    } else {
        int i;
        for (i=0; i<MAX_HIF_DEVICES; ++i) {
            if (hif_devices[i] == device) {
                hif_devices[i] = NULL;
            }
        }
    }

    if (device->claimedContext != NULL) {
        status = osdrvCallbacks.deviceRemovedHandler(device->claimedContext, device);
    }

    if (device->is_disabled) {
        device->is_disabled = FALSE;
    } else {
        status = hifDisableFunc(device, func);
    }

    delHifDevice(device);
    if (status != A_OK) {
        AR_DEBUG_PRINTF(ATH_DEBUG_WARN,
            ("AR6000: Unable to disable sdio func. Card removed?\n"));
    }
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: -hifDeviceRemoved\n"));
}

/*
 * This should be moved to AR6K HTC layer.
 */
A_STATUS hifWaitForPendingRecv(HIF_DEVICE *device)
{
    A_INT32 cnt = 10;
    A_UINT8 host_int_status;
    A_STATUS status = A_OK;

    do {
        A_INT32 irqCnt = 10;
        while (atomic_read(&device->irqHandling) && --irqCnt > 0) {
            /* wait until irq handler finished all the jobs */
            schedule_timeout_interruptible(HZ / 10);
        }
        /* check if there is any pending irq due to force done */
        host_int_status = 0;
        status = HIFReadWrite(device, HOST_INT_STATUS_ADDRESS,
                              (A_UINT8 *)&host_int_status, sizeof(host_int_status),
                              HIF_RD_SYNC_BYTE_INC, NULL);

        switch (status) {
        case A_OK:
            host_int_status = (host_int_status & (1 << 0));
            break;
        case A_ECANCELED:
        case A_NO_MEMORY:
            host_int_status = 1; /* force it to query again due to resources issue*/
            break;
        default:
            AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: SDIO bus operation failed: %d \n", status));
            A_ASSERT(0); /* What can I do here? */
            break;
        }

        if (host_int_status) {
            /* Wait until irq handler finishes its job */
            schedule_timeout_interruptible(1);
        }
    } while (host_int_status && --cnt > 0);

    if (host_int_status || cnt == 0) {
         AR_DEBUG_PRINTF(ATH_DEBUG_ERROR,
                            ("AR6000: %s(), Unable clear up pending IRQ before the system suspended\n", __FUNCTION__));
         return A_ERROR;
     }

    return A_OK;
}


static HIF_DEVICE *
addHifDevice(struct sdio_func *func)
{
    HIF_DEVICE *hifdevice;
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: addHifDevice\n"));
    AR_DEBUG_ASSERT(func != NULL);
    hifdevice = (HIF_DEVICE *)A_MALLOC(sizeof(HIF_DEVICE));
    AR_DEBUG_ASSERT(hifdevice != NULL);
    A_MEMZERO(hifdevice, sizeof(*hifdevice));
#if HIF_USE_DMA_BOUNCE_BUFFER
#ifdef USE_PREALLOC_BUFFER
    hifdevice->dma_buffer = ar6000_dma_buffer_alloc(HIF_DMA_BUFFER_SIZE);
#else
    hifdevice->dma_buffer = A_MALLOC(HIF_DMA_BUFFER_SIZE);
#endif
#endif
    AR_DEBUG_ASSERT(hifdevice->dma_buffer != NULL);
    hifdevice->func = func;
    hifdevice->powerConfig = HIF_DEVICE_POWER_UP;
    sdio_set_drvdata(func, hifdevice);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: addHifDevice; 0x%p\n", hifdevice));
    return hifdevice;
}

static HIF_DEVICE *
getHifDevice(struct sdio_func *func)
{
    AR_DEBUG_ASSERT(func != NULL);
    return (HIF_DEVICE *)sdio_get_drvdata(func);
}

static void
delHifDevice(HIF_DEVICE * device)
{
    AR_DEBUG_ASSERT(device!= NULL);
    AR_DEBUG_PRINTF(ATH_DEBUG_TRACE, ("AR6000: delHifDevice; 0x%p\n", device));
    if (device->dma_buffer != NULL) {
#ifdef USE_PREALLOC_BUFFER
        ar6000_dma_buffer_free(device->dma_buffer);
#else
        A_FREE(device->dma_buffer);
#endif
    }
    A_FREE(device);
}

static void ResetAllCards(void)
{
}

void HIFClaimDevice(HIF_DEVICE  *device, void *context)
{
    device->claimedContext = context;
}

void HIFReleaseDevice(HIF_DEVICE  *device)
{
    device->claimedContext = NULL;
}

A_STATUS HIFAttachHTC(HIF_DEVICE *device, HTC_CALLBACKS *callbacks)
{
    if (device->htcCallbacks.context != NULL) {
            /* already in use! */
        return A_ERROR;
    }
    device->htcCallbacks = *callbacks;
    return A_OK;
}

void HIFDetachHTC(HIF_DEVICE *device)
{
    A_MEMZERO(&device->htcCallbacks,sizeof(device->htcCallbacks));
}

#define SDIO_SET_CMD52_ARG(arg,rw,func,raw,address,writedata) \
    (arg) = (((rw) & 1) << 31)           | \
            (((func) & 0x7) << 28)       | \
            (((raw) & 1) << 27)          | \
            (1 << 26)                    | \
            (((address) & 0x1FFFF) << 9) | \
            (1 << 8)                     | \
            ((writedata) & 0xFF)

#define SDIO_SET_CMD52_READ_ARG(arg,func,address) \
    SDIO_SET_CMD52_ARG(arg,0,(func),0,address,0x00)
#define SDIO_SET_CMD52_WRITE_ARG(arg,func,address,value) \
    SDIO_SET_CMD52_ARG(arg,1,(func),0,address,value)

static int Func0_CMD52WriteByte(struct mmc_card *card, unsigned int address, unsigned char byte)
{
    struct mmc_command ioCmd;
    unsigned long      arg;

    memset(&ioCmd,0,sizeof(ioCmd));
    SDIO_SET_CMD52_WRITE_ARG(arg,0,address,byte);
    ioCmd.opcode = SD_IO_RW_DIRECT;
    ioCmd.arg = arg;
    ioCmd.flags = MMC_RSP_R5 | MMC_CMD_AC;

    return mmc_wait_for_cmd(card->host, &ioCmd, 0);
}

static int Func0_CMD52ReadByte(struct mmc_card *card, unsigned int address, unsigned char *byte)
{
    struct mmc_command ioCmd;
    unsigned long      arg;
    A_INT32 err;

    memset(&ioCmd,0,sizeof(ioCmd));
    SDIO_SET_CMD52_READ_ARG(arg,0,address);
    ioCmd.opcode = SD_IO_RW_DIRECT;
    ioCmd.arg = arg;
    ioCmd.flags = MMC_RSP_R5 | MMC_CMD_AC;

    err = mmc_wait_for_cmd(card->host, &ioCmd, 0);

    if ((!err) && (byte)) {
        *byte =  ioCmd.resp[0] & 0xFF;
    }

    return err;
}

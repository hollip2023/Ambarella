/** @file bt_proc.c
  *
  * @brief This file handle the functions for proc files
  *
  * Copyright (C) 2007-2008, Marvell International Ltd.
  *
  * This software file (the "File") is distributed by Marvell International
  * Ltd. under the terms of the GNU General Public License Version 2, June 1991
  * (the "License").  You may use, redistribute and/or modify this File in
  * accordance with the terms and conditions of the License, a copy of which
  * is available along with the File in the gpl.txt file or by writing to
  * the Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
  * 02111-1307 or on the worldwide web at http://www.gnu.org/licenses/gpl.txt.
  *
  * THE FILE IS DISTRIBUTED AS-IS, WITHOUT WARRANTY OF ANY KIND, AND THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE
  * ARE EXPRESSLY DISCLAIMED.  The License provides additional details about
  * this warranty disclaimer.
  *
  */

#include "include.h"

/** proc diretory root */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,24)
#define PROC_DIR NULL //&proc_root
#else
#define PROC_DIR proc_net
#endif

/** Driver version */
extern char driver_version[];

struct proc_data
{
    /** Read length */
    int rdlen;
    /** Read buffer */
    char *rdbuf;
    /** Write length */
    int wrlen;
    /** Maximum write length */
    int maxwrlen;
    /** Write buffer */
    char *wrbuf;
    void (*on_close) (struct inode *, struct file *);
};

struct item_data
{
    /** Name */
    char name[32];
    /** Size */
    u32 size;
    /** Address */
    u32 addr;
    /** Offset */
    u32 offset;
    /** Flag */
    u32 flag;
};

struct proc_private_data
{
    /** Name */
    char name[32];
    /** File flag */
    u32 fileflag;
    /** Buffer size */
    u32 bufsize;
    /** Number of items */
    u32 num_items;
    /** Item data */
    struct item_data *pdata;
    /** Private structure */
    bt_private *pbt;
    /** File operations */
    struct file_operations *fops;
};

/** Default file permission */
#define DEFAULT_FILE_PERM  0644

/** Bluetooth device offset */
#define OFFSET_BT_DEV		0x01
/** Bluetooth adapter offset */
#define OFFSET_BT_ADAPTER	0x02
/** Show integer */
#define SHOW_INT		0x10
/** Show hex */
#define SHOW_HEX		0x20
/** Show string */
#define SHOW_STRING		0x40

/** Device size */
#define item_dev_size(n) (sizeof ((bt_dev_t *)0)->n)
/** Device address */
#define item_dev_addr(n) ((u32) &((bt_dev_t *)0)->n)

/** Adapter size */
#define item_adapter_size(n) (sizeof ((bt_adapter *)0)->n)
/** Adapter address */
#define item_adapter_addr(n) ((u32) &((bt_adapter *)0)->n)
static struct item_data config_items[] = {
#ifdef	DEBUG_LEVEL1
    {"drvdbg", sizeof(u32), (u32) & drvdbg, 0, SHOW_HEX}
    ,
#endif
    {"psmode", item_dev_size(psmode), 0, item_dev_addr(psmode),
     OFFSET_BT_DEV | SHOW_INT}
    ,
    {"pscmd", item_dev_size(pscmd), 0, item_dev_addr(pscmd),
     OFFSET_BT_DEV | SHOW_INT}
    ,
    {"hsmode", item_dev_size(hsmode), 0, item_dev_addr(hsmode),
     OFFSET_BT_DEV | SHOW_INT}
    ,
    {"hscmd", item_dev_size(hscmd), 0, item_dev_addr(hscmd),
     OFFSET_BT_DEV | SHOW_INT}
    ,
    {"gpio_gap", item_dev_size(gpio_gap), 0, item_dev_addr(gpio_gap),
     OFFSET_BT_DEV | SHOW_HEX}
    ,
    {"hscfgcmd", item_dev_size(hscfgcmd), 0, item_dev_addr(hscfgcmd),
     OFFSET_BT_DEV | SHOW_INT}
    ,
};

static struct item_data status_items[] = {
    {"version", 0, (u32) driver_version, 0, SHOW_STRING},
    {"tx_dnld_rdy", item_dev_size(tx_dnld_rdy), 0, item_dev_addr(tx_dnld_rdy),
     OFFSET_BT_DEV | SHOW_INT},
    {"psmode", item_adapter_size(psmode), 0, item_adapter_addr(psmode),
     OFFSET_BT_ADAPTER | SHOW_INT},
    {"hs_state", item_adapter_size(hs_state), 0, item_adapter_addr(hs_state),
     OFFSET_BT_ADAPTER | SHOW_INT},
    {"ps_state", item_adapter_size(ps_state), 0, item_adapter_addr(ps_state),
     OFFSET_BT_ADAPTER | SHOW_INT},
};

/**
 *  @brief convert string to number
 *
 *  @param s   	   pointer to numbered string
 *  @return 	   converted number from string s
 */
int
string_to_number(char *s)
{
    int r = 0;
    int base = 0;
    int pn = 1;

    if (strncmp(s, "-", 1) == 0) {
        pn = -1;
        s++;
    }
    if ((strncmp(s, "0x", 2) == 0) || (strncmp(s, "0X", 2) == 0)) {
        base = 16;
        s += 2;
    } else
        base = 10;

    for (s = s; *s != 0; s++) {
        if ((*s >= '0') && (*s <= '9'))
            r = (r * base) + (*s - '0');
        else if ((*s >= 'A') && (*s <= 'F'))
            r = (r * base) + (*s - 'A' + 10);
        else if ((*s >= 'a') && (*s <= 'f'))
            r = (r * base) + (*s - 'a' + 10);
        else
            break;
    }

    return (r * pn);
}

/**
 *  @brief This function handle generic proc file close
 *
 *  @param inode   A pointer to inode structure
 *  @param file    A pointer to file structure
 *  @return 	   BT_STATUS_SUCCESS
 */
static int
proc_close(struct inode *inode, struct file *file)
{
    struct proc_data *pdata = file->private_data;
    ENTER();
    if (pdata) {
        if (pdata->on_close != NULL)
            pdata->on_close(inode, file);
        if (pdata->rdbuf)
            kfree(pdata->rdbuf);
        if (pdata->wrbuf)
            kfree(pdata->wrbuf);
        kfree(pdata);
    }
    LEAVE();
    return BT_STATUS_SUCCESS;
}

/**
 *  @brief This function handle generic proc file read
 *
 *  @param file    A pointer to file structure
 *  @param buffer  A pointer to output buffer
 *  @param len     number of byte to read
 *  @param offset  A pointer to offset of file
 *  @return 	   number of output data
 */
static ssize_t
proc_read(struct file *file, char __user * buffer, size_t len, loff_t * offset)
{
    loff_t pos = *offset;
    struct proc_data *pdata = (struct proc_data *) file->private_data;
    if ((!pdata->rdbuf) || (pos < 0))
        return -EINVAL;
    if (pos >= pdata->rdlen)
        return 0;
    if (len > pdata->rdlen - pos)
        len = pdata->rdlen - pos;
    if (copy_to_user(buffer, pdata->rdbuf + pos, len))
        return -EFAULT;
    *offset = pos + len;
    return len;
}

/**
 *  @brief This function handle generic proc file write
 *
 *  @param file    A pointer to file structure
 *  @param buffer  A pointer to input buffer
 *  @param len     number of byte to write
 *  @param offset  A pointer to offset of file
 *  @return 	   number of input data
 */
static ssize_t
proc_write(struct file *file,
           const char __user * buffer, size_t len, loff_t * offset)
{
    loff_t pos = *offset;
    struct proc_data *pdata = (struct proc_data *) file->private_data;

    if (!pdata->wrbuf || (pos < 0))
        return -EINVAL;
    if (pos >= pdata->maxwrlen)
        return 0;
    if (len > pdata->maxwrlen - pos)
        len = pdata->maxwrlen - pos;
    if (copy_from_user(pdata->wrbuf + pos, buffer, len))
        return -EFAULT;
    if (pos + len > pdata->wrlen)
        pdata->wrlen = len + file->f_pos;
    *offset = pos + len;
    return len;
}

/**
 *  @brief This function handle the generic file close
 *
 *  @param inode   A pointer to inode structure
 *  @param file    A pointer to file structure
 *  @return 	   BT_STATUS_SUCCESS or other
 */
static void
proc_on_close(struct inode *inode, struct file *file)
{
    struct proc_dir_entry *entry = PDE(inode);
    struct proc_private_data *priv = entry->data;
    struct proc_data *pdata = file->private_data;
    char *line;
    int i;
    ENTER();
    if (!pdata->wrlen)
        return;
    line = pdata->wrbuf;
    while (line[0]) {
        for (i = 0; i < priv->num_items; i++) {
            if (!strncmp
                (line, priv->pdata[i].name, strlen(priv->pdata[i].name))) {
                line += strlen(priv->pdata[i].name) + 1;
                if (priv->pdata[i].size == 1)
                    *((u8 *) priv->pdata[i].addr) = (u8) string_to_number(line);
                else if (priv->pdata[i].size == 2)
                    *((u16 *) priv->pdata[i].addr) =
                        (u16) string_to_number(line);
                else if (priv->pdata[i].size == 4)
                    *((u32 *) priv->pdata[i].addr) =
                        (u32) string_to_number(line);
            }
        }
        while (line[0] && line[0] != '\n')
            line++;
        if (line[0])
            line++;
    }
    if (priv->pbt->bt_dev.hscmd || priv->pbt->bt_dev.pscmd
        || priv->pbt->bt_dev.hscfgcmd) {
        bt_prepare_command(priv->pbt);
        wake_up_interruptible(&priv->pbt->MainThread.waitQ);
    }
    LEAVE();
    return;
}

/**
 *  @brief This function handle the generic file open
 *
 *  @param inode   A pointer to inode structure
 *  @param file    A pointer to file structure
 *  @return 	   BT_STATUS_SUCCESS or other
 */
static int
proc_open(struct inode *inode, struct file *file)
{
    struct proc_dir_entry *entry = PDE(inode);
    struct proc_private_data *priv = entry->data;
    struct proc_data *pdata;
    int i;
    char *p;
    u32 val = 0;
    ENTER();
    if ((file->private_data =
         kzalloc(sizeof(struct proc_data), GFP_KERNEL)) == NULL) {
        PRINTM(ERROR, "Can not alloc mem for proc_data\n");
        LEAVE();
        return -ENOMEM;
    }
    pdata = (struct proc_data *) file->private_data;
    if ((pdata->rdbuf = kmalloc(priv->bufsize, GFP_KERNEL)) == NULL) {
        PRINTM(ERROR, "Can not alloc mem for rdbuf\n");
        kfree(file->private_data);
        LEAVE();
        return -ENOMEM;
    }
    if (priv->fileflag == DEFAULT_FILE_PERM) {
        if ((pdata->wrbuf = kzalloc(priv->bufsize, GFP_KERNEL)) == NULL) {
            PRINTM(ERROR, "Can not alloc mem for wrbuf\n");
            kfree(pdata->rdbuf);
            kfree(file->private_data);
            return -ENOMEM;
        }
        pdata->maxwrlen = priv->bufsize;
        pdata->on_close = proc_on_close;
    }
    p = pdata->rdbuf;
    for (i = 0; i < priv->num_items; i++) {
        if (priv->pdata[i].size == 1)
            val = *((u8 *) priv->pdata[i].addr);
        else if (priv->pdata[i].size == 2)
            val = *((u16 *) priv->pdata[i].addr);
        else if (priv->pdata[i].size == 4)
            val = *((u32 *) priv->pdata[i].addr);
        if (priv->pdata[i].flag & SHOW_INT)
            p += sprintf(p, "%s=%d\n", priv->pdata[i].name, val);
        else if (priv->pdata[i].flag & SHOW_HEX)
            p += sprintf(p, "%s=0x%x\n", priv->pdata[i].name, val);
        else if (priv->pdata[i].flag & SHOW_STRING)
            p += sprintf(p, "%s=%s\n", priv->pdata[i].name,
                         (char *) priv->pdata[i].addr);
    }
    pdata->rdlen = strlen(pdata->rdbuf);
    LEAVE();
    return BT_STATUS_SUCCESS;
}

static struct file_operations proc_read_ops = {
    .read = proc_read,
    .open = proc_open,
    .release = proc_close
};

static struct file_operations proc_rw_ops = {
    .read = proc_read,
    .write = proc_write,
    .open = proc_open,
    .release = proc_close
};

static struct proc_private_data proc_files[] = {
    {"status", S_IRUGO, 1024, sizeof(status_items) / sizeof(status_items[0]),
     &status_items[0], NULL, &proc_read_ops}
    ,
    {"config", DEFAULT_FILE_PERM, 512,
     sizeof(config_items) / sizeof(config_items[0]), &config_items[0], NULL,
     &proc_rw_ops}
    ,
};

/**
 *  @brief This function init proc entry
 *
 *  @param priv    A pointer to bt_private structure
 *  @return 	   BT_STATUS_SUCCESS
 */
int
bt_proc_init(bt_private * priv)
{
    u8 ret = BT_STATUS_SUCCESS;
    struct proc_dir_entry *entry;
    int i, j;
    ENTER();
    if (!priv->proc_mbt) {
        priv->proc_mbt = proc_mkdir("mbt", PROC_DIR);
        if (!priv->proc_mbt) {
            PRINTM(ERROR, "Could not mkdir mbt!\n");
            ret = BT_STATUS_FAILURE;
            goto done;
        }
        priv->proc_entry =
            proc_mkdir(priv->bt_dev.hcidev->name, priv->proc_mbt);
        if (!priv->proc_entry) {
            PRINTM(ERROR, "Could not mkdir %s!\n", priv->bt_dev.hcidev->name);
            ret = BT_STATUS_FAILURE;
            goto done;
        }
        for (j = 0; j < sizeof(proc_files) / sizeof(proc_files[0]); j++) {
            for (i = 0; i < proc_files[j].num_items; i++) {
                if (proc_files[j].pdata[i].flag & OFFSET_BT_DEV)
                    proc_files[j].pdata[i].addr =
                        proc_files[j].pdata[i].offset + (u32) & priv->bt_dev;
                if (proc_files[j].pdata[i].flag & OFFSET_BT_ADAPTER)
                    proc_files[j].pdata[i].addr =
                        proc_files[j].pdata[i].offset + (u32) priv->adapter;
            }
            proc_files[j].pbt = priv;
            entry =
                create_proc_entry(proc_files[j].name,
                                  S_IFREG | proc_files[j].fileflag,
                                  priv->proc_entry);
            if (entry) {
                entry->data = &proc_files[j];
                entry->proc_fops = proc_files[j].fops;
            }
        }
    }
  done:
    LEAVE();
    return ret;
}

/**
 *  @brief remove proc file
 *
 *  @param priv	   pointer wlan_private
 *  @return 	   N/A
 */
void
bt_proc_remove(bt_private * priv)
{
    int j;
    ENTER();
    if (priv->proc_mbt) {
        if (priv->proc_entry) {
            for (j = 0; j < sizeof(proc_files) / sizeof(proc_files[0]); j++)
                remove_proc_entry(proc_files[j].name, priv->proc_entry);
        }
        remove_proc_entry(priv->bt_dev.hcidev->name, priv->proc_mbt);
        remove_proc_entry("mbt", PROC_DIR);
        priv->proc_entry = NULL;
        priv->proc_mbt = NULL;
    }
    LEAVE();
    return;
}

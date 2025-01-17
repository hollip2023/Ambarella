//------------------------------------------------------------------------------
// <copyright file="addrs.h" company="Atheros">
//    Copyright (c) 2004-2009 Atheros Corporation.  All rights reserved.
// 
// The software source and binaries included in this development package are
// licensed, not sold. You, or your company, received the package under one
// or more license agreements. The rights granted to you are specifically
// listed in these license agreement(s). All other rights remain with Atheros
// Communications, Inc., its subsidiaries, or the respective owner including
// those listed on the included copyright notices.  Distribution of any
// portion of this package must be in strict compliance with the license
// agreement(s) terms.
// </copyright>
// 
// <summary>
// 	Wifi driver for AR6003
// </summary>
//
//------------------------------------------------------------------------------
//==============================================================================
// Author(s): ="Atheros"
//==============================================================================

#ifndef __ADDRS_H__
#define __ADDRS_H__

/* 
 * Special AR6000 Addresses that may be needed by special
 * applications (e.g. ART) on the Host as well as Target.
 * Note that these are all uncached addresses.
 */

#define AR6K_RAM_START 0xa0000000
#define AR6K_RAM_ADDR(byte_offset) (AR6K_RAM_START+(byte_offset))
#define TARG_RAM_ADDRS(byte_offset) AR6K_RAM_ADDR(byte_offset)
#define TARG_RAM_OFFSET(vaddr) ((A_UINT32)(vaddr) & 0x0fffffff)
#define TARG_ROM_OFFSET(vaddr) ((A_UINT32)(vaddr) & 0x0fffffff)
#define TARG_RAM_SZ (80*1024)

#define AR6000_FLASH_START 0xa2000000
#define AR6000_FLASH_ADDR(byte_offset) (AR6000_FLASH_START+(byte_offset))
#define TARG_FLASH_ADDRS(byte_offset) AR6000_FLASH_ADDR(byte_offset)

#define AR6K_ROM_START 0xa1000000
#define AR6K_ROM_ADDR(byte_offset) (AR6K_ROM_START+(byte_offset))
#define TARG_ROM_ADDRS(byte_offset) AR6K_ROM_ADDR(byte_offset)

/*
 * At this ROM address is a pointer to the start of the ROM DataSet Index.
 * If there are no ROM DataSets, there's a 0 at this address.
 */
#define ROM_DATASET_INDEX_ADDR          0xa103fff8
#define ROM_BANK1_MBIST_CKSUM_ADDR      0xa103fffc

/* Size of Board Data, in bytes */
#define AR6000_BOARD_DATA_SZ 512
#define BOARD_DATA_SZ AR6000_BOARD_DATA_SZ

/*
 * Calibration data and other Board Data is located in flash in the
 * last 512 bytes of the first Half Megabyte.  The API A_BOARD_DATA_ADDR()
 * is the proper way to get a read pointer to the data -- it returns a RAM
 * version of the source Board Data which may have come directly from the
 * flash copy or from elsewhere.
 *
 * (NB: Flash address of Board Data is 0xa207fe00)
 */
#define AR6000_BOARD_DATA_OFFSET (0x80000 - AR6000_BOARD_DATA_SZ)
#define AR6000_BOARD_DATA_ADDR TARG_FLASH_ADDRS(AR6000_BOARD_DATA_OFFSET)

/*
 * Pointer to index of DataSets in Flash,
 * located at flash offset 512KB-512-4,
 * which is the word just before the Board Data.
 */
#define AR6000_FLASH_DATASET_INDEX_OFFSET \
    (AR6000_BOARD_DATA_OFFSET - sizeof(void *))
#define FLASH_DATASET_INDEX_OFFSET AR6000_FLASH_DATASET_INDEX_OFFSET
#define FLASH_DATASET_INDEX_ADDR TARG_FLASH_ADDRS(FLASH_DATASET_INDEX_OFFSET)

/*
 * Constants used by ASM code to access fields of host_interest_s,
 * which is at a fixed location in RAM.
 */
#define FLASH_IS_PRESENT_TARGADDR       0x8000060c

#endif /* __ADDRS_H__ */

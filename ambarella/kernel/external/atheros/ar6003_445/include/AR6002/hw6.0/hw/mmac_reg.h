// ------------------------------------------------------------------
// Copyright (c) 2004-2007 Atheros Corporation.  All rights reserved.
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
// 	Wifi driver for AR6002
// </summary>
//
// ------------------------------------------------------------------
//===================================================================
// Author(s): ="Atheros"
//===================================================================

/* Copyright (C) 2010 Denali Software Inc.  All rights reserved              */
/* THIS FILE IS AUTOMATICALLY GENERATED BY DENALI BLUEPRINT, DO NOT EDIT     */


#ifndef _MMAC_REG_REG_H_
#define _MMAC_REG_REG_H_


/* macros for RX_FRAME0 */
#define RX_FRAME0_ADDRESS                                                                 0x00000000
#define RX_FRAME0_OFFSET                                                                  0x00000000
#define RX_FRAME0_OWN_MSB                                                                          0
#define RX_FRAME0_OWN_LSB                                                                          0
#define RX_FRAME0_OWN_MASK                                                                0x00000001
#define RX_FRAME0_OWN_GET(x)                                               (((x) & 0x00000001) >> 0)
#define RX_FRAME0_OWN_SET(x)                                               (((x) << 0) & 0x00000001)
#define RX_FRAME0_LEN_MSB                                                                         12
#define RX_FRAME0_LEN_LSB                                                                          1
#define RX_FRAME0_LEN_MASK                                                                0x00001ffe
#define RX_FRAME0_LEN_GET(x)                                               (((x) & 0x00001ffe) >> 1)
#define RX_FRAME0_SEQ_NUM_MSB                                                                     14
#define RX_FRAME0_SEQ_NUM_LSB                                                                     13
#define RX_FRAME0_SEQ_NUM_MASK                                                            0x00006000
#define RX_FRAME0_SEQ_NUM_GET(x)                                          (((x) & 0x00006000) >> 13)

/* macros for RX_FRAME_0 */
#define RX_FRAME_0_ADDRESS                                                                0x00000004
#define RX_FRAME_0_OFFSET                                                                 0x00000004
#define RX_FRAME_0_ADDR_MSB                                                                       31
#define RX_FRAME_0_ADDR_LSB                                                                        0
#define RX_FRAME_0_ADDR_MASK                                                              0xffffffff
#define RX_FRAME_0_ADDR_GET(x)                                             (((x) & 0xffffffff) >> 0)
#define RX_FRAME_0_ADDR_SET(x)                                             (((x) << 0) & 0xffffffff)

/* macros for RX_FRAME1 */
#define RX_FRAME1_ADDRESS                                                                 0x00000008
#define RX_FRAME1_OFFSET                                                                  0x00000008
#define RX_FRAME1_OWN_MSB                                                                          0
#define RX_FRAME1_OWN_LSB                                                                          0
#define RX_FRAME1_OWN_MASK                                                                0x00000001
#define RX_FRAME1_OWN_GET(x)                                               (((x) & 0x00000001) >> 0)
#define RX_FRAME1_OWN_SET(x)                                               (((x) << 0) & 0x00000001)
#define RX_FRAME1_LEN_MSB                                                                         12
#define RX_FRAME1_LEN_LSB                                                                          1
#define RX_FRAME1_LEN_MASK                                                                0x00001ffe
#define RX_FRAME1_LEN_GET(x)                                               (((x) & 0x00001ffe) >> 1)
#define RX_FRAME1_SEQ_NUM_MSB                                                                     14
#define RX_FRAME1_SEQ_NUM_LSB                                                                     13
#define RX_FRAME1_SEQ_NUM_MASK                                                            0x00006000
#define RX_FRAME1_SEQ_NUM_GET(x)                                          (((x) & 0x00006000) >> 13)

/* macros for RX_FRAME_1 */
#define RX_FRAME_1_ADDRESS                                                                0x0000000c
#define RX_FRAME_1_OFFSET                                                                 0x0000000c
#define RX_FRAME_1_ADDR_MSB                                                                       31
#define RX_FRAME_1_ADDR_LSB                                                                        0
#define RX_FRAME_1_ADDR_MASK                                                              0xffffffff
#define RX_FRAME_1_ADDR_GET(x)                                             (((x) & 0xffffffff) >> 0)
#define RX_FRAME_1_ADDR_SET(x)                                             (((x) << 0) & 0xffffffff)

/* macros for MMAC_INTERRUPT_RAW */
#define MMAC_INTERRUPT_RAW_ADDRESS                                                        0x00000010
#define MMAC_INTERRUPT_RAW_OFFSET                                                         0x00000010
#define MMAC_INTERRUPT_RAW_RX_DONE0_MSB                                                            0
#define MMAC_INTERRUPT_RAW_RX_DONE0_LSB                                                            0
#define MMAC_INTERRUPT_RAW_RX_DONE0_MASK                                                  0x00000001
#define MMAC_INTERRUPT_RAW_RX_DONE0_GET(x)                                 (((x) & 0x00000001) >> 0)
#define MMAC_INTERRUPT_RAW_RX_DONE0_SET(x)                                 (((x) << 0) & 0x00000001)
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL0_MSB                                                        1
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL0_LSB                                                        1
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL0_MASK                                              0x00000002
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL0_GET(x)                             (((x) & 0x00000002) >> 1)
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL0_SET(x)                             (((x) << 1) & 0x00000002)
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL0_MSB                                                      2
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL0_LSB                                                      2
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL0_MASK                                            0x00000004
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL0_GET(x)                           (((x) & 0x00000004) >> 2)
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL0_SET(x)                           (((x) << 2) & 0x00000004)
#define MMAC_INTERRUPT_RAW_RX_DONE1_MSB                                                            3
#define MMAC_INTERRUPT_RAW_RX_DONE1_LSB                                                            3
#define MMAC_INTERRUPT_RAW_RX_DONE1_MASK                                                  0x00000008
#define MMAC_INTERRUPT_RAW_RX_DONE1_GET(x)                                 (((x) & 0x00000008) >> 3)
#define MMAC_INTERRUPT_RAW_RX_DONE1_SET(x)                                 (((x) << 3) & 0x00000008)
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL1_MSB                                                        4
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL1_LSB                                                        4
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL1_MASK                                              0x00000010
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL1_GET(x)                             (((x) & 0x00000010) >> 4)
#define MMAC_INTERRUPT_RAW_RX_CRC_FAIL1_SET(x)                             (((x) << 4) & 0x00000010)
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL1_MSB                                                      5
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL1_LSB                                                      5
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL1_MASK                                            0x00000020
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL1_GET(x)                           (((x) & 0x00000020) >> 5)
#define MMAC_INTERRUPT_RAW_ACK_RESP_FAIL1_SET(x)                           (((x) << 5) & 0x00000020)
#define MMAC_INTERRUPT_RAW_RX_ERR_OVERFLOW_MSB                                                     6
#define MMAC_INTERRUPT_RAW_RX_ERR_OVERFLOW_LSB                                                     6
#define MMAC_INTERRUPT_RAW_RX_ERR_OVERFLOW_MASK                                           0x00000040
#define MMAC_INTERRUPT_RAW_RX_ERR_OVERFLOW_GET(x)                          (((x) & 0x00000040) >> 6)
#define MMAC_INTERRUPT_RAW_RX_ERR_OVERFLOW_SET(x)                          (((x) << 6) & 0x00000040)
#define MMAC_INTERRUPT_RAW_TX_DONE_MSB                                                             7
#define MMAC_INTERRUPT_RAW_TX_DONE_LSB                                                             7
#define MMAC_INTERRUPT_RAW_TX_DONE_MASK                                                   0x00000080
#define MMAC_INTERRUPT_RAW_TX_DONE_GET(x)                                  (((x) & 0x00000080) >> 7)
#define MMAC_INTERRUPT_RAW_TX_DONE_SET(x)                                  (((x) << 7) & 0x00000080)
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_MISSING_MSB                                                 8
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_MISSING_LSB                                                 8
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_MISSING_MASK                                       0x00000100
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_MISSING_GET(x)                      (((x) & 0x00000100) >> 8)
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_MISSING_SET(x)                      (((x) << 8) & 0x00000100)
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_RECEIVED_MSB                                                9
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_RECEIVED_LSB                                                9
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_RECEIVED_MASK                                      0x00000200
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_RECEIVED_GET(x)                     (((x) & 0x00000200) >> 9)
#define MMAC_INTERRUPT_RAW_TX_DONE_ACK_RECEIVED_SET(x)                     (((x) << 9) & 0x00000200)
#define MMAC_INTERRUPT_RAW_TX_ERROR_MSB                                                           10
#define MMAC_INTERRUPT_RAW_TX_ERROR_LSB                                                           10
#define MMAC_INTERRUPT_RAW_TX_ERROR_MASK                                                  0x00000400
#define MMAC_INTERRUPT_RAW_TX_ERROR_GET(x)                                (((x) & 0x00000400) >> 10)
#define MMAC_INTERRUPT_RAW_TX_ERROR_SET(x)                                (((x) << 10) & 0x00000400)

/* macros for MMAC_INTERRUPT_EN */
#define MMAC_INTERRUPT_EN_ADDRESS                                                         0x00000014
#define MMAC_INTERRUPT_EN_OFFSET                                                          0x00000014
#define MMAC_INTERRUPT_EN_RX_DONE0_MSB                                                             0
#define MMAC_INTERRUPT_EN_RX_DONE0_LSB                                                             0
#define MMAC_INTERRUPT_EN_RX_DONE0_MASK                                                   0x00000001
#define MMAC_INTERRUPT_EN_RX_DONE0_GET(x)                                  (((x) & 0x00000001) >> 0)
#define MMAC_INTERRUPT_EN_RX_DONE0_SET(x)                                  (((x) << 0) & 0x00000001)
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL0_MSB                                                         1
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL0_LSB                                                         1
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL0_MASK                                               0x00000002
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL0_GET(x)                              (((x) & 0x00000002) >> 1)
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL0_SET(x)                              (((x) << 1) & 0x00000002)
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL0_MSB                                                       2
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL0_LSB                                                       2
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL0_MASK                                             0x00000004
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL0_GET(x)                            (((x) & 0x00000004) >> 2)
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL0_SET(x)                            (((x) << 2) & 0x00000004)
#define MMAC_INTERRUPT_EN_RX_DONE1_MSB                                                             3
#define MMAC_INTERRUPT_EN_RX_DONE1_LSB                                                             3
#define MMAC_INTERRUPT_EN_RX_DONE1_MASK                                                   0x00000008
#define MMAC_INTERRUPT_EN_RX_DONE1_GET(x)                                  (((x) & 0x00000008) >> 3)
#define MMAC_INTERRUPT_EN_RX_DONE1_SET(x)                                  (((x) << 3) & 0x00000008)
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL1_MSB                                                         4
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL1_LSB                                                         4
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL1_MASK                                               0x00000010
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL1_GET(x)                              (((x) & 0x00000010) >> 4)
#define MMAC_INTERRUPT_EN_RX_CRC_FAIL1_SET(x)                              (((x) << 4) & 0x00000010)
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL1_MSB                                                       5
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL1_LSB                                                       5
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL1_MASK                                             0x00000020
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL1_GET(x)                            (((x) & 0x00000020) >> 5)
#define MMAC_INTERRUPT_EN_ACK_RESP_FAIL1_SET(x)                            (((x) << 5) & 0x00000020)
#define MMAC_INTERRUPT_EN_RX_ERR_OVERFLOW_MSB                                                      6
#define MMAC_INTERRUPT_EN_RX_ERR_OVERFLOW_LSB                                                      6
#define MMAC_INTERRUPT_EN_RX_ERR_OVERFLOW_MASK                                            0x00000040
#define MMAC_INTERRUPT_EN_RX_ERR_OVERFLOW_GET(x)                           (((x) & 0x00000040) >> 6)
#define MMAC_INTERRUPT_EN_RX_ERR_OVERFLOW_SET(x)                           (((x) << 6) & 0x00000040)
#define MMAC_INTERRUPT_EN_TX_DONE_MSB                                                              7
#define MMAC_INTERRUPT_EN_TX_DONE_LSB                                                              7
#define MMAC_INTERRUPT_EN_TX_DONE_MASK                                                    0x00000080
#define MMAC_INTERRUPT_EN_TX_DONE_GET(x)                                   (((x) & 0x00000080) >> 7)
#define MMAC_INTERRUPT_EN_TX_DONE_SET(x)                                   (((x) << 7) & 0x00000080)
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_MISSING_MSB                                                  8
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_MISSING_LSB                                                  8
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_MISSING_MASK                                        0x00000100
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_MISSING_GET(x)                       (((x) & 0x00000100) >> 8)
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_MISSING_SET(x)                       (((x) << 8) & 0x00000100)
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_RECEIVED_MSB                                                 9
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_RECEIVED_LSB                                                 9
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_RECEIVED_MASK                                       0x00000200
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_RECEIVED_GET(x)                      (((x) & 0x00000200) >> 9)
#define MMAC_INTERRUPT_EN_TX_DONE_ACK_RECEIVED_SET(x)                      (((x) << 9) & 0x00000200)
#define MMAC_INTERRUPT_EN_TX_ERROR_MSB                                                            10
#define MMAC_INTERRUPT_EN_TX_ERROR_LSB                                                            10
#define MMAC_INTERRUPT_EN_TX_ERROR_MASK                                                   0x00000400
#define MMAC_INTERRUPT_EN_TX_ERROR_GET(x)                                 (((x) & 0x00000400) >> 10)
#define MMAC_INTERRUPT_EN_TX_ERROR_SET(x)                                 (((x) << 10) & 0x00000400)

/* macros for RX_PARAM1 */
#define RX_PARAM1_ADDRESS                                                                 0x00000018
#define RX_PARAM1_OFFSET                                                                  0x00000018
#define RX_PARAM1_VAP_ADDR_L_MSB                                                                  31
#define RX_PARAM1_VAP_ADDR_L_LSB                                                                   0
#define RX_PARAM1_VAP_ADDR_L_MASK                                                         0xffffffff
#define RX_PARAM1_VAP_ADDR_L_GET(x)                                        (((x) & 0xffffffff) >> 0)
#define RX_PARAM1_VAP_ADDR_L_SET(x)                                        (((x) << 0) & 0xffffffff)

/* macros for RX_PARAM0 */
#define RX_PARAM0_ADDRESS                                                                 0x0000001c
#define RX_PARAM0_OFFSET                                                                  0x0000001c
#define RX_PARAM0_VAP_ADDR_U_MSB                                                                  15
#define RX_PARAM0_VAP_ADDR_U_LSB                                                                   0
#define RX_PARAM0_VAP_ADDR_U_MASK                                                         0x0000ffff
#define RX_PARAM0_VAP_ADDR_U_GET(x)                                        (((x) & 0x0000ffff) >> 0)
#define RX_PARAM0_VAP_ADDR_U_SET(x)                                        (((x) << 0) & 0x0000ffff)
#define RX_PARAM0_SIFS_MSB                                                                        21
#define RX_PARAM0_SIFS_LSB                                                                        16
#define RX_PARAM0_SIFS_MASK                                                               0x003f0000
#define RX_PARAM0_SIFS_GET(x)                                             (((x) & 0x003f0000) >> 16)
#define RX_PARAM0_SIFS_SET(x)                                             (((x) << 16) & 0x003f0000)
#define RX_PARAM0_CAPTURE_MODE_MSB                                                                23
#define RX_PARAM0_CAPTURE_MODE_LSB                                                                22
#define RX_PARAM0_CAPTURE_MODE_MASK                                                       0x00c00000
#define RX_PARAM0_CAPTURE_MODE_GET(x)                                     (((x) & 0x00c00000) >> 22)
#define RX_PARAM0_CAPTURE_MODE_SET(x)                                     (((x) << 22) & 0x00c00000)
#define RX_PARAM0_TYPE_FILTER_MSB                                                                 26
#define RX_PARAM0_TYPE_FILTER_LSB                                                                 24
#define RX_PARAM0_TYPE_FILTER_MASK                                                        0x07000000
#define RX_PARAM0_TYPE_FILTER_GET(x)                                      (((x) & 0x07000000) >> 24)
#define RX_PARAM0_TYPE_FILTER_SET(x)                                      (((x) << 24) & 0x07000000)
#define RX_PARAM0_LIVE_MODE_MSB                                                                   27
#define RX_PARAM0_LIVE_MODE_LSB                                                                   27
#define RX_PARAM0_LIVE_MODE_MASK                                                          0x08000000
#define RX_PARAM0_LIVE_MODE_GET(x)                                        (((x) & 0x08000000) >> 27)
#define RX_PARAM0_LIVE_MODE_SET(x)                                        (((x) << 27) & 0x08000000)

/* macros for TX_COMMAND0 */
#define TX_COMMAND0_ADDRESS                                                               0x00000020
#define TX_COMMAND0_OFFSET                                                                0x00000020
#define TX_COMMAND0_LEN_MSB                                                                       11
#define TX_COMMAND0_LEN_LSB                                                                        0
#define TX_COMMAND0_LEN_MASK                                                              0x00000fff
#define TX_COMMAND0_LEN_GET(x)                                             (((x) & 0x00000fff) >> 0)
#define TX_COMMAND0_LEN_SET(x)                                             (((x) << 0) & 0x00000fff)
#define TX_COMMAND0_CRC_MSB                                                                       12
#define TX_COMMAND0_CRC_LSB                                                                       12
#define TX_COMMAND0_CRC_MASK                                                              0x00001000
#define TX_COMMAND0_CRC_GET(x)                                            (((x) & 0x00001000) >> 12)
#define TX_COMMAND0_CRC_SET(x)                                            (((x) << 12) & 0x00001000)
#define TX_COMMAND0_EXP_ACK_MSB                                                                   13
#define TX_COMMAND0_EXP_ACK_LSB                                                                   13
#define TX_COMMAND0_EXP_ACK_MASK                                                          0x00002000
#define TX_COMMAND0_EXP_ACK_GET(x)                                        (((x) & 0x00002000) >> 13)
#define TX_COMMAND0_EXP_ACK_SET(x)                                        (((x) << 13) & 0x00002000)

/* macros for TX_COMMAND */
#define TX_COMMAND_ADDRESS                                                                0x00000024
#define TX_COMMAND_OFFSET                                                                 0x00000024
#define TX_COMMAND_ADDR_MSB                                                                       31
#define TX_COMMAND_ADDR_LSB                                                                        0
#define TX_COMMAND_ADDR_MASK                                                              0xffffffff
#define TX_COMMAND_ADDR_GET(x)                                             (((x) & 0xffffffff) >> 0)
#define TX_COMMAND_ADDR_SET(x)                                             (((x) << 0) & 0xffffffff)

/* macros for TX_PARAM */
#define TX_PARAM_ADDRESS                                                                  0x00000028
#define TX_PARAM_OFFSET                                                                   0x00000028
#define TX_PARAM_ACK_MODE_EN_MSB                                                                   0
#define TX_PARAM_ACK_MODE_EN_LSB                                                                   0
#define TX_PARAM_ACK_MODE_EN_MASK                                                         0x00000001
#define TX_PARAM_ACK_MODE_EN_GET(x)                                        (((x) & 0x00000001) >> 0)
#define TX_PARAM_ACK_MODE_EN_SET(x)                                        (((x) << 0) & 0x00000001)
#define TX_PARAM_ACK_TIMEOUT_MSB                                                                   6
#define TX_PARAM_ACK_TIMEOUT_LSB                                                                   1
#define TX_PARAM_ACK_TIMEOUT_MASK                                                         0x0000007e
#define TX_PARAM_ACK_TIMEOUT_GET(x)                                        (((x) & 0x0000007e) >> 1)
#define TX_PARAM_ACK_TIMEOUT_SET(x)                                        (((x) << 1) & 0x0000007e)
#define TX_PARAM_BACKOFF_MSB                                                                      14
#define TX_PARAM_BACKOFF_LSB                                                                       7
#define TX_PARAM_BACKOFF_MASK                                                             0x00007f80
#define TX_PARAM_BACKOFF_GET(x)                                            (((x) & 0x00007f80) >> 7)
#define TX_PARAM_BACKOFF_SET(x)                                            (((x) << 7) & 0x00007f80)
#define TX_PARAM_FORCE_ACKF_RSSI_MSB                                                              15
#define TX_PARAM_FORCE_ACKF_RSSI_LSB                                                              15
#define TX_PARAM_FORCE_ACKF_RSSI_MASK                                                     0x00008000
#define TX_PARAM_FORCE_ACKF_RSSI_GET(x)                                   (((x) & 0x00008000) >> 15)
#define TX_PARAM_FORCE_ACKF_RSSI_SET(x)                                   (((x) << 15) & 0x00008000)
#define TX_PARAM_ACKF_RSSI_MSB                                                                    23
#define TX_PARAM_ACKF_RSSI_LSB                                                                    16
#define TX_PARAM_ACKF_RSSI_MASK                                                           0x00ff0000
#define TX_PARAM_ACKF_RSSI_GET(x)                                         (((x) & 0x00ff0000) >> 16)
#define TX_PARAM_ACKF_RSSI_SET(x)                                         (((x) << 16) & 0x00ff0000)

/* macros for BEACON_PARAM */
#define BEACON_PARAM_ADDRESS                                                              0x0000002c
#define BEACON_PARAM_OFFSET                                                               0x0000002c
#define BEACON_PARAM_INTERVAL_MSB                                                                 15
#define BEACON_PARAM_INTERVAL_LSB                                                                  0
#define BEACON_PARAM_INTERVAL_MASK                                                        0x0000ffff
#define BEACON_PARAM_INTERVAL_GET(x)                                       (((x) & 0x0000ffff) >> 0)
#define BEACON_PARAM_INTERVAL_SET(x)                                       (((x) << 0) & 0x0000ffff)
#define BEACON_PARAM_LEN_MSB                                                                      27
#define BEACON_PARAM_LEN_LSB                                                                      16
#define BEACON_PARAM_LEN_MASK                                                             0x0fff0000
#define BEACON_PARAM_LEN_GET(x)                                           (((x) & 0x0fff0000) >> 16)
#define BEACON_PARAM_LEN_SET(x)                                           (((x) << 16) & 0x0fff0000)
#define BEACON_PARAM_EN_MSB                                                                       28
#define BEACON_PARAM_EN_LSB                                                                       28
#define BEACON_PARAM_EN_MASK                                                              0x10000000
#define BEACON_PARAM_EN_GET(x)                                            (((x) & 0x10000000) >> 28)
#define BEACON_PARAM_EN_SET(x)                                            (((x) << 28) & 0x10000000)
#define BEACON_PARAM_CRC_MSB                                                                      29
#define BEACON_PARAM_CRC_LSB                                                                      29
#define BEACON_PARAM_CRC_MASK                                                             0x20000000
#define BEACON_PARAM_CRC_GET(x)                                           (((x) & 0x20000000) >> 29)
#define BEACON_PARAM_CRC_SET(x)                                           (((x) << 29) & 0x20000000)
#define BEACON_PARAM_RESET_TS_MSB                                                                 30
#define BEACON_PARAM_RESET_TS_LSB                                                                 30
#define BEACON_PARAM_RESET_TS_MASK                                                        0x40000000
#define BEACON_PARAM_RESET_TS_GET(x)                                      (((x) & 0x40000000) >> 30)
#define BEACON_PARAM_RESET_TS_SET(x)                                      (((x) << 30) & 0x40000000)

/* macros for BEACON */
#define BEACON_ADDRESS                                                                    0x00000030
#define BEACON_OFFSET                                                                     0x00000030
#define BEACON_ADDR_MSB                                                                           31
#define BEACON_ADDR_LSB                                                                            0
#define BEACON_ADDR_MASK                                                                  0xffffffff
#define BEACON_ADDR_GET(x)                                                 (((x) & 0xffffffff) >> 0)
#define BEACON_ADDR_SET(x)                                                 (((x) << 0) & 0xffffffff)

/* macros for TSF_L */
#define TSF_L_ADDRESS                                                                     0x00000034
#define TSF_L_OFFSET                                                                      0x00000034
#define TSF_L_COUNT_MSB                                                                           31
#define TSF_L_COUNT_LSB                                                                            0
#define TSF_L_COUNT_MASK                                                                  0xffffffff
#define TSF_L_COUNT_GET(x)                                                 (((x) & 0xffffffff) >> 0)
#define TSF_L_COUNT_SET(x)                                                 (((x) << 0) & 0xffffffff)

/* macros for TSF_U */
#define TSF_U_ADDRESS                                                                     0x00000038
#define TSF_U_OFFSET                                                                      0x00000038
#define TSF_U_COUNT_MSB                                                                           31
#define TSF_U_COUNT_LSB                                                                            0
#define TSF_U_COUNT_MASK                                                                  0xffffffff
#define TSF_U_COUNT_GET(x)                                                 (((x) & 0xffffffff) >> 0)
#define TSF_U_COUNT_SET(x)                                                 (((x) << 0) & 0xffffffff)


#ifndef __ASSEMBLER__

typedef struct mmac_reg_reg_s {
  volatile unsigned int RX_FRAME0;                                     /*        0x0 - 0x4        */
  volatile unsigned int RX_FRAME_0;                                    /*        0x4 - 0x8        */
  volatile unsigned int RX_FRAME1;                                     /*        0x8 - 0xc        */
  volatile unsigned int RX_FRAME_1;                                    /*        0xc - 0x10       */
  volatile unsigned int MMAC_INTERRUPT_RAW;                            /*       0x10 - 0x14       */
  volatile unsigned int MMAC_INTERRUPT_EN;                             /*       0x14 - 0x18       */
  volatile unsigned int RX_PARAM1;                                     /*       0x18 - 0x1c       */
  volatile unsigned int RX_PARAM0;                                     /*       0x1c - 0x20       */
  volatile unsigned int TX_COMMAND0;                                   /*       0x20 - 0x24       */
  volatile unsigned int TX_COMMAND;                                    /*       0x24 - 0x28       */
  volatile unsigned int TX_PARAM;                                      /*       0x28 - 0x2c       */
  volatile unsigned int BEACON_PARAM;                                  /*       0x2c - 0x30       */
  volatile unsigned int BEACON;                                        /*       0x30 - 0x34       */
  volatile unsigned int TSF_L;                                         /*       0x34 - 0x38       */
  volatile unsigned int TSF_U;                                         /*       0x38 - 0x3c       */
} mmac_reg_reg_t;

#endif /* __ASSEMBLER__ */

#endif /* _MMAC_REG_REG_H_ */

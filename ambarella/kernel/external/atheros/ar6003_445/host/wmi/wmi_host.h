//------------------------------------------------------------------------------
// <copyright file="wmi_host.h" company="Atheros">
//    Copyright (c) 2004-2010 Atheros Corporation.  All rights reserved.
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
//------------------------------------------------------------------------------
//==============================================================================
// This file contains local definitios for the wmi host module.
//
// Author(s): ="Atheros"
//==============================================================================
#ifndef _WMI_HOST_H_
#define _WMI_HOST_H_

#include "roaming.h"
#ifdef __cplusplus
extern "C" {
#endif

struct wmi_stats {
    A_UINT32    cmd_len_err;
    A_UINT32    cmd_id_err;
};

#define SSID_IE_LEN_INDEX 13


/* Host side link management data structures */
#define SIGNAL_QUALITY_THRESHOLD_LEVELS 6
#define SIGNAL_QUALITY_UPPER_THRESHOLD_LEVELS SIGNAL_QUALITY_THRESHOLD_LEVELS
#define SIGNAL_QUALITY_LOWER_THRESHOLD_LEVELS SIGNAL_QUALITY_THRESHOLD_LEVELS
typedef struct sq_threshold_params_s {
    A_INT16 upper_threshold[SIGNAL_QUALITY_UPPER_THRESHOLD_LEVELS];
    A_INT16 lower_threshold[SIGNAL_QUALITY_LOWER_THRESHOLD_LEVELS];
    A_UINT32 upper_threshold_valid_count;
    A_UINT32 lower_threshold_valid_count;
    A_UINT32 polling_interval;
    A_UINT8 weight;
    A_UINT8  last_rssi; //normally you would expect this to be bss specific but we keep only one instance because its only valid when the device is in a connected state. Not sure if it belongs to host or target.
    A_UINT8  last_rssi_poll_event; //Not sure if it belongs to host or target
} SQ_THRESHOLD_PARAMS;

/*
 * Virtual device specific wmi_t data structure
 */ 

struct wmi_t {
    void                           *wmi_devt;
    struct ieee80211_node_table     wmi_scan_table;
    A_UINT8                         wmi_bssid[ATH_MAC_LEN];
    A_UINT8                         wmi_powerMode;
    A_UINT8                         wmi_phyMode;
    A_UINT8                         wmi_keepaliveInterval;
    SQ_THRESHOLD_PARAMS             wmi_SqThresholdParams[SIGNAL_QUALITY_METRICS_NUM_MAX];
    CRYPTO_TYPE                     wmi_pair_crypto_type;
    CRYPTO_TYPE                     wmi_grp_crypto_type;
    WMI_SET_HT_CAP_CMD              wmi_ht_cap[A_NUM_BANDS];
    A_BOOL                          wmi_is_wmm_enabled;
    A_UINT8                         wmi_dev_index;
    struct wmi_stats                wmi_stats;
};

/*
 * Virtual device independent wmi data structure
 */ 

struct wmi_priv_t {
    A_BOOL                          wmi_ready;
    A_BOOL                          wmi_numQoSStream;
    A_UINT8                         wmi_fatPipeExists;
    A_UINT16                        wmi_streamExistsForAC[WMM_NUM_AC];
    HTC_ENDPOINT_ID                 wmi_endpoint_id;
    A_MUTEX_T                       wmi_lock;
};
#define LOCK_WMI(w)     A_MUTEX_LOCK(&(w)->wmi_lock);
#define UNLOCK_WMI(w)   A_MUTEX_UNLOCK(&(w)->wmi_lock);

#ifdef __cplusplus
}
#endif

#endif /* _WMI_HOST_H_ */

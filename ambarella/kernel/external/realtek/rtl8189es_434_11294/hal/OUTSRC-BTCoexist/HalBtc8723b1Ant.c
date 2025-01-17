//============================================================
// Description:
//
// This file is for RTL8723B Co-exist mechanism
//
// History
// 2012/11/15 Cosa first check in.
//
//============================================================

//============================================================
// include files
//============================================================
#include "Mp_Precomp.h"
#if(BT_30_SUPPORT == 1)
//============================================================
// Global variables, these are static variables
//============================================================
static COEX_DM_8723B_1ANT		GLCoexDm8723b1Ant;
static PCOEX_DM_8723B_1ANT 	pCoexDm=&GLCoexDm8723b1Ant;
static COEX_STA_8723B_1ANT		GLCoexSta8723b1Ant;
static PCOEX_STA_8723B_1ANT	pCoexSta=&GLCoexSta8723b1Ant;

const char *const GLBtInfoSrc8723b1Ant[]={
	"BT Info[wifi fw]",
	"BT Info[bt rsp]",
	"BT Info[bt auto report]",
};

u4Byte	GLCoexVerDate8723b1Ant=20140411;
u4Byte	GLCoexVer8723b1Ant=0x4c;

//============================================================
// local function proto type if needed
//============================================================
//============================================================
// local function start with halbtc8723b1ant_
//============================================================
u1Byte
halbtc8723b1ant_BtRssiState(
	u1Byte			levelNum,
	u1Byte			rssiThresh,
	u1Byte			rssiThresh1
	)
{
	s4Byte			btRssi=0;
	u1Byte			btRssiState=pCoexSta->preBtRssiState;

	btRssi = pCoexSta->btRssi;

	if(levelNum == 2)
	{			
		if( (pCoexSta->preBtRssiState == BTC_RSSI_STATE_LOW) ||
			(pCoexSta->preBtRssiState == BTC_RSSI_STATE_STAY_LOW))
		{
			if(btRssi >= (rssiThresh+BTC_RSSI_COEX_THRESH_TOL_8723B_1ANT))
			{
				btRssiState = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to High\n"));
			}
			else
			{
				btRssiState = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at Low\n"));
			}
		}
		else
		{
			if(btRssi < rssiThresh)
			{
				btRssiState = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Low\n"));
			}
			else
			{
				btRssiState = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at High\n"));
			}
		}
	}
	else if(levelNum == 3)
	{
		if(rssiThresh > rssiThresh1)
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi thresh error!!\n"));
			return pCoexSta->preBtRssiState;
		}
		
		if( (pCoexSta->preBtRssiState == BTC_RSSI_STATE_LOW) ||
			(pCoexSta->preBtRssiState == BTC_RSSI_STATE_STAY_LOW))
		{
			if(btRssi >= (rssiThresh+BTC_RSSI_COEX_THRESH_TOL_8723B_1ANT))
			{
				btRssiState = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Medium\n"));
			}
			else
			{
				btRssiState = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at Low\n"));
			}
		}
		else if( (pCoexSta->preBtRssiState == BTC_RSSI_STATE_MEDIUM) ||
			(pCoexSta->preBtRssiState == BTC_RSSI_STATE_STAY_MEDIUM))
		{
			if(btRssi >= (rssiThresh1+BTC_RSSI_COEX_THRESH_TOL_8723B_1ANT))
			{
				btRssiState = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to High\n"));
			}
			else if(btRssi < rssiThresh)
			{
				btRssiState = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Low\n"));
			}
			else
			{
				btRssiState = BTC_RSSI_STATE_STAY_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at Medium\n"));
			}
		}
		else
		{
			if(btRssi < rssiThresh1)
			{
				btRssiState = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state switch to Medium\n"));
			}
			else
			{
				btRssiState = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_RSSI_STATE, ("[BTCoex], BT Rssi state stay at High\n"));
			}
		}
	}
		
	pCoexSta->preBtRssiState = btRssiState;

	return btRssiState;
}

u1Byte
halbtc8723b1ant_WifiRssiState(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte			index,
	IN	u1Byte			levelNum,
	IN	u1Byte			rssiThresh,
	IN	u1Byte			rssiThresh1
	)
{
	s4Byte			wifiRssi=0;
	u1Byte			wifiRssiState=pCoexSta->preWifiRssiState[index];

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_S4_WIFI_RSSI, &wifiRssi);
	
	if(levelNum == 2)
	{
		if( (pCoexSta->preWifiRssiState[index] == BTC_RSSI_STATE_LOW) ||
			(pCoexSta->preWifiRssiState[index] == BTC_RSSI_STATE_STAY_LOW))
		{
			if(wifiRssi >= (rssiThresh+BTC_RSSI_COEX_THRESH_TOL_8723B_1ANT))
			{
				wifiRssiState = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to High\n"));
			}
			else
			{
				wifiRssiState = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at Low\n"));
			}
		}
		else
		{
			if(wifiRssi < rssiThresh)
			{
				wifiRssiState = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Low\n"));
			}
			else
			{
				wifiRssiState = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at High\n"));
			}
		}
	}
	else if(levelNum == 3)
	{
		if(rssiThresh > rssiThresh1)
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI thresh error!!\n"));
			return pCoexSta->preWifiRssiState[index];
		}
		
		if( (pCoexSta->preWifiRssiState[index] == BTC_RSSI_STATE_LOW) ||
			(pCoexSta->preWifiRssiState[index] == BTC_RSSI_STATE_STAY_LOW))
		{
			if(wifiRssi >= (rssiThresh+BTC_RSSI_COEX_THRESH_TOL_8723B_1ANT))
			{
				wifiRssiState = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Medium\n"));
			}
			else
			{
				wifiRssiState = BTC_RSSI_STATE_STAY_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at Low\n"));
			}
		}
		else if( (pCoexSta->preWifiRssiState[index] == BTC_RSSI_STATE_MEDIUM) ||
			(pCoexSta->preWifiRssiState[index] == BTC_RSSI_STATE_STAY_MEDIUM))
		{
			if(wifiRssi >= (rssiThresh1+BTC_RSSI_COEX_THRESH_TOL_8723B_1ANT))
			{
				wifiRssiState = BTC_RSSI_STATE_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to High\n"));
			}
			else if(wifiRssi < rssiThresh)
			{
				wifiRssiState = BTC_RSSI_STATE_LOW;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Low\n"));
			}
			else
			{
				wifiRssiState = BTC_RSSI_STATE_STAY_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at Medium\n"));
			}
		}
		else
		{
			if(wifiRssi < rssiThresh1)
			{
				wifiRssiState = BTC_RSSI_STATE_MEDIUM;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state switch to Medium\n"));
			}
			else
			{
				wifiRssiState = BTC_RSSI_STATE_STAY_HIGH;
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_WIFI_RSSI_STATE, ("[BTCoex], wifi RSSI state stay at High\n"));
			}
		}
	}
		
	pCoexSta->preWifiRssiState[index] = wifiRssiState;

	return wifiRssiState;
}

VOID
halbtc8723b1ant_UpdateRaMask(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bForceExec,
	IN	u4Byte				disRateMask
	)
{
	pCoexDm->curRaMask = disRateMask;
	
	if( bForceExec || (pCoexDm->preRaMask != pCoexDm->curRaMask))
	{
		pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_UPDATE_RAMASK, &pCoexDm->curRaMask);
	}
	pCoexDm->preRaMask = pCoexDm->curRaMask;
}

VOID
halbtc8723b1ant_AutoRateFallbackRetry(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bForceExec,
	IN	u1Byte				type
	)
{
	BOOLEAN	bWifiUnderBMode=FALSE;
	
	pCoexDm->curArfrType = type;

	if( bForceExec || (pCoexDm->preArfrType != pCoexDm->curArfrType))
	{
		switch(pCoexDm->curArfrType)
		{
			case 0:	// normal mode
				pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x430, pCoexDm->backupArfrCnt1);
				pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x434, pCoexDm->backupArfrCnt2);
				break;
			case 1:	
				pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_UNDER_B_MODE, &bWifiUnderBMode);
				if(bWifiUnderBMode)
				{
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x430, 0x0);
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x434, 0x01010101);
				}
				else
				{
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x430, 0x0);
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x434, 0x04030201);
				}
				break;
			default:
				break;
		}
	}

	pCoexDm->preArfrType = pCoexDm->curArfrType;
}

VOID
halbtc8723b1ant_RetryLimit(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bForceExec,
	IN	u1Byte				type
	)
{
	pCoexDm->curRetryLimitType = type;

	if( bForceExec || (pCoexDm->preRetryLimitType != pCoexDm->curRetryLimitType))
	{
		switch(pCoexDm->curRetryLimitType)
		{
			case 0:	// normal mode
				pBtCoexist->fBtcWrite2Byte(pBtCoexist, 0x42a, pCoexDm->backupRetryLimit);
				break;
			case 1:	// retry limit=8
				pBtCoexist->fBtcWrite2Byte(pBtCoexist, 0x42a, 0x0808);
				break;
			default:
				break;
		}
	}

	pCoexDm->preRetryLimitType = pCoexDm->curRetryLimitType;
}

VOID
halbtc8723b1ant_AmpduMaxTime(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bForceExec,
	IN	u1Byte				type
	)
{
	pCoexDm->curAmpduTimeType = type;

	if( bForceExec || (pCoexDm->preAmpduTimeType != pCoexDm->curAmpduTimeType))
	{
		switch(pCoexDm->curAmpduTimeType)
		{
			case 0:	// normal mode
				pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x456, pCoexDm->backupAmpduMaxTime);
				break;
			case 1:	// AMPDU timw = 0x38 * 32us
				pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x456, 0x38);
				break;
			default:
				break;
		}
	}

	pCoexDm->preAmpduTimeType = pCoexDm->curAmpduTimeType;
}

VOID
halbtc8723b1ant_LimitedTx(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bForceExec,
	IN	u1Byte				raMaskType,
	IN	u1Byte				arfrType,
	IN	u1Byte				retryLimitType,
	IN	u1Byte				ampduTimeType
	)
{
	switch(raMaskType)
	{
		case 0:	// normal mode
			halbtc8723b1ant_UpdateRaMask(pBtCoexist, bForceExec, 0x0);
			break;
		case 1:	// disable cck 1/2
			halbtc8723b1ant_UpdateRaMask(pBtCoexist, bForceExec, 0x00000003);
			break;
		case 2:	// disable cck 1/2/5.5, ofdm 6/9/12/18/24, mcs 0/1/2/3/4
			halbtc8723b1ant_UpdateRaMask(pBtCoexist, bForceExec, 0x0001f1f7);
			break;
		default:
			break;
	}

	halbtc8723b1ant_AutoRateFallbackRetry(pBtCoexist, bForceExec, arfrType);
	halbtc8723b1ant_RetryLimit(pBtCoexist, bForceExec, retryLimitType);
	halbtc8723b1ant_AmpduMaxTime(pBtCoexist, bForceExec, ampduTimeType);
}

VOID
halbtc8723b1ant_LimitedRx(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bForceExec,
	IN	BOOLEAN				bRejApAggPkt,
	IN	BOOLEAN				bBtCtrlAggBufSize,
	IN	u1Byte				aggBufSize
	)
{
	BOOLEAN	bRejectRxAgg=bRejApAggPkt;
	BOOLEAN	bBtCtrlRxAggSize=bBtCtrlAggBufSize;
	u1Byte	rxAggSize=aggBufSize;

	//============================================
	//	Rx Aggregation related setting
	//============================================
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_TO_REJ_AP_AGG_PKT, &bRejectRxAgg);
	// decide BT control aggregation buf size or not
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_BT_CTRL_AGG_SIZE, &bBtCtrlRxAggSize);
	// aggregation buf size, only work when BT control Rx aggregation size.
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_U1_AGG_BUF_SIZE, &rxAggSize);
	// real update aggregation setting
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_AGGREGATE_CTRL, NULL);


}

VOID
halbtc8723b1ant_QueryBtInfo(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	u1Byte			H2C_Parameter[1] ={0};

	pCoexSta->bC2hBtInfoReqSent = TRUE;

	H2C_Parameter[0] |= BIT0;	// trigger

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, ("[BTCoex], Query Bt Info, FW write 0x61=0x%x\n", 
		H2C_Parameter[0]));

	pBtCoexist->fBtcFillH2c(pBtCoexist, 0x61, 1, H2C_Parameter);
}

VOID
halbtc8723b1ant_MonitorBtCtr(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	u4Byte 			regHPTxRx, regLPTxRx, u4Tmp, u4Tmp1;
	u4Byte			regHPTx=0, regHPRx=0, regLPTx=0, regLPRx=0;
	u1Byte			u1Tmp, u1Tmp1;
	s4Byte			wifiRssi;
	static u1Byte		NumOfBtCounterChk = 0;
	
       //to avoid 0x76e[3] = 1 (WLAN_Act control by PTA) during IPS
	if (! (pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x76e) & 0x8) ) 
	{
		pCoexSta->highPriorityTx = 65535;
		pCoexSta->highPriorityRx = 65535;
		pCoexSta->lowPriorityTx = 65535;
		pCoexSta->lowPriorityRx = 65535;
		return;
	}
		
	regHPTxRx = 0x770;
	regLPTxRx = 0x774;

	u4Tmp = pBtCoexist->fBtcRead4Byte(pBtCoexist, regHPTxRx);
	regHPTx = u4Tmp & bMaskLWord;
	regHPRx = (u4Tmp & bMaskHWord)>>16;

	u4Tmp = pBtCoexist->fBtcRead4Byte(pBtCoexist, regLPTxRx);
	regLPTx = u4Tmp & bMaskLWord;
	regLPRx = (u4Tmp & bMaskHWord)>>16;
		
	pCoexSta->highPriorityTx = regHPTx;
	pCoexSta->highPriorityRx = regHPRx;
	pCoexSta->lowPriorityTx = regLPTx;
	pCoexSta->lowPriorityRx = regLPRx;

	if( (pCoexSta->lowPriorityTx >= 1050)  && (!pCoexSta->bC2hBtInquiryPage))
		pCoexSta->popEventCnt++;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Hi-Pri Rx/Tx: %d/%d, Lo-Pri Rx/Tx: %d/%d\n",
		regHPRx, regHPTx, regLPRx, regLPTx));

	// reset counter
	pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x76e, 0xc);

	if ((regHPTx == 0) && (regHPRx ==0) && (regLPTx == 0) && (regLPRx == 0))
	{
		NumOfBtCounterChk++;
		if (NumOfBtCounterChk >= 3)
{
			halbtc8723b1ant_QueryBtInfo(pBtCoexist);
			NumOfBtCounterChk = 0;
		}
	}
}

BOOLEAN
halbtc8723b1ant_IsWifiStatusChanged(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	static BOOLEAN	bPreWifiBusy=FALSE, bPreUnder4way=FALSE, bPreBtHsOn=FALSE;
	BOOLEAN bWifiBusy=FALSE, bUnder4way=FALSE, bBtHsOn=FALSE;
	BOOLEAN bWifiConnected=FALSE;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_BUSY, &bWifiBusy);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_4_WAY_PROGRESS, &bUnder4way);

	if(bWifiConnected)
	{
		if(bWifiBusy != bPreWifiBusy)
		{
			bPreWifiBusy = bWifiBusy;
			return TRUE;
		}
		if(bUnder4way != bPreUnder4way)
		{
			bPreUnder4way = bUnder4way;
			return TRUE;
		}
		if(bBtHsOn != bPreBtHsOn)
		{
			bPreBtHsOn = bBtHsOn;
			return TRUE;
		}
	}

	return FALSE;
}

VOID
halbtc8723b1ant_UpdateBtLinkInfo(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BT_LINK_INFO	pBtLinkInfo=&pBtCoexist->btLinkInfo;
	BOOLEAN				bBtHsOn=FALSE;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);

	pBtLinkInfo->bBtLinkExist = pCoexSta->bBtLinkExist;
	pBtLinkInfo->bScoExist = pCoexSta->bScoExist;
	pBtLinkInfo->bA2dpExist = pCoexSta->bA2dpExist;
	pBtLinkInfo->bPanExist = pCoexSta->bPanExist;
	pBtLinkInfo->bHidExist = pCoexSta->bHidExist;

	// work around for HS mode.
	if(bBtHsOn)
	{
		pBtLinkInfo->bPanExist = TRUE;
		pBtLinkInfo->bBtLinkExist = TRUE;
	}

	// check if Sco only
	if( pBtLinkInfo->bScoExist &&
		!pBtLinkInfo->bA2dpExist &&
		!pBtLinkInfo->bPanExist &&
		!pBtLinkInfo->bHidExist )
		pBtLinkInfo->bScoOnly = TRUE;
	else
		pBtLinkInfo->bScoOnly = FALSE;

	// check if A2dp only
	if( !pBtLinkInfo->bScoExist &&
		pBtLinkInfo->bA2dpExist &&
		!pBtLinkInfo->bPanExist &&
		!pBtLinkInfo->bHidExist )
		pBtLinkInfo->bA2dpOnly = TRUE;
	else
		pBtLinkInfo->bA2dpOnly = FALSE;

	// check if Pan only
	if( !pBtLinkInfo->bScoExist &&
		!pBtLinkInfo->bA2dpExist &&
		pBtLinkInfo->bPanExist &&
		!pBtLinkInfo->bHidExist )
		pBtLinkInfo->bPanOnly = TRUE;
	else
		pBtLinkInfo->bPanOnly = FALSE;
	
	// check if Hid only
	if( !pBtLinkInfo->bScoExist &&
		!pBtLinkInfo->bA2dpExist &&
		!pBtLinkInfo->bPanExist &&
		pBtLinkInfo->bHidExist )
		pBtLinkInfo->bHidOnly = TRUE;
	else
		pBtLinkInfo->bHidOnly = FALSE;
}

u1Byte
halbtc8723b1ant_ActionAlgorithm(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BT_LINK_INFO	pBtLinkInfo=&pBtCoexist->btLinkInfo;
	BOOLEAN				bBtHsOn=FALSE;
	u1Byte				algorithm=BT_8723B_1ANT_COEX_ALGO_UNDEFINED;
	u1Byte				numOfDiffProfile=0;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);

	if(!pBtLinkInfo->bBtLinkExist)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], No BT link exists!!!\n"));
		return algorithm;
	}

	if(pBtLinkInfo->bScoExist)
		numOfDiffProfile++;
	if(pBtLinkInfo->bHidExist)
		numOfDiffProfile++;
	if(pBtLinkInfo->bPanExist)
		numOfDiffProfile++;
	if(pBtLinkInfo->bA2dpExist)
		numOfDiffProfile++;
	
	if(numOfDiffProfile == 1)
	{
		if(pBtLinkInfo->bScoExist)
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO only\n"));
			algorithm = BT_8723B_1ANT_COEX_ALGO_SCO;
		}
		else
		{
			if(pBtLinkInfo->bHidExist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = HID only\n"));
				algorithm = BT_8723B_1ANT_COEX_ALGO_HID;
			}
			else if(pBtLinkInfo->bA2dpExist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = A2DP only\n"));
				algorithm = BT_8723B_1ANT_COEX_ALGO_A2DP;
			}
			else if(pBtLinkInfo->bPanExist)
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = PAN(HS) only\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANHS;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = PAN(EDR) only\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANEDR;
				}
			}
		}
	}
	else if(numOfDiffProfile == 2)
	{
		if(pBtLinkInfo->bScoExist)
		{
			if(pBtLinkInfo->bHidExist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + HID\n"));
				algorithm = BT_8723B_1ANT_COEX_ALGO_HID;
			}
			else if(pBtLinkInfo->bA2dpExist)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + A2DP ==> SCO\n"));
				algorithm = BT_8723B_1ANT_COEX_ALGO_SCO;
			}
			else if(pBtLinkInfo->bPanExist)
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + PAN(HS)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_SCO;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + PAN(EDR)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		}
		else
		{
			if( pBtLinkInfo->bHidExist &&
				pBtLinkInfo->bA2dpExist )
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = HID + A2DP\n"));
				algorithm = BT_8723B_1ANT_COEX_ALGO_HID_A2DP;
			}
			else if( pBtLinkInfo->bHidExist &&
				pBtLinkInfo->bPanExist )
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = HID + PAN(HS)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_HID_A2DP;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = HID + PAN(EDR)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
			else if( pBtLinkInfo->bPanExist &&
				pBtLinkInfo->bA2dpExist )
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = A2DP + PAN(HS)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_A2DP_PANHS;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = A2DP + PAN(EDR)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANEDR_A2DP;
				}
			}
		}
	}
	else if(numOfDiffProfile == 3)
	{
		if(pBtLinkInfo->bScoExist)
		{
			if( pBtLinkInfo->bHidExist &&
				pBtLinkInfo->bA2dpExist )
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + HID + A2DP ==> HID\n"));
				algorithm = BT_8723B_1ANT_COEX_ALGO_HID;
			}
			else if( pBtLinkInfo->bHidExist &&
				pBtLinkInfo->bPanExist )
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + HID + PAN(HS)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_HID_A2DP;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + HID + PAN(EDR)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
			else if( pBtLinkInfo->bPanExist &&
				pBtLinkInfo->bA2dpExist )
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + A2DP + PAN(HS)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_SCO;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + A2DP + PAN(EDR) ==> HID\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		}
		else
		{
			if( pBtLinkInfo->bHidExist &&
				pBtLinkInfo->bPanExist &&
				pBtLinkInfo->bA2dpExist )
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = HID + A2DP + PAN(HS)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_HID_A2DP;
				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = HID + A2DP + PAN(EDR)\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_HID_A2DP_PANEDR;
				}
			}
		}
	}
	else if(numOfDiffProfile >= 3)
	{
		if(pBtLinkInfo->bScoExist)
		{
			if( pBtLinkInfo->bHidExist &&
				pBtLinkInfo->bPanExist &&
				pBtLinkInfo->bA2dpExist )
			{
				if(bBtHsOn)
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Error!!! BT Profile = SCO + HID + A2DP + PAN(HS)\n"));

				}
				else
				{
					BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT Profile = SCO + HID + A2DP + PAN(EDR)==>PAN(EDR)+HID\n"));
					algorithm = BT_8723B_1ANT_COEX_ALGO_PANEDR_HID;
				}
			}
		}
	}

	return algorithm;
}

VOID
halbtc8723b1ant_SetBtAutoReport(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bEnableAutoReport
	)
{
	u1Byte			H2C_Parameter[1] ={0};
	
	H2C_Parameter[0] = 0;

	if(bEnableAutoReport)
	{
		H2C_Parameter[0] |= BIT0;
	}

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, ("[BTCoex], BT FW auto report : %s, FW write 0x68=0x%x\n", 
		(bEnableAutoReport? "Enabled!!":"Disabled!!"), H2C_Parameter[0]));

	pBtCoexist->fBtcFillH2c(pBtCoexist, 0x68, 1, H2C_Parameter);	
}

VOID
halbtc8723b1ant_BtAutoReport(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bForceExec,
	IN	BOOLEAN			bEnableAutoReport
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, ("[BTCoex], %s BT Auto report = %s\n",  
		(bForceExec? "force to":""), ((bEnableAutoReport)? "Enabled":"Disabled")));
	pCoexDm->bCurBtAutoReport = bEnableAutoReport;

	if(!bForceExec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], bPreBtAutoReport=%d, bCurBtAutoReport=%d\n", 
			pCoexDm->bPreBtAutoReport, pCoexDm->bCurBtAutoReport));

		if(pCoexDm->bPreBtAutoReport == pCoexDm->bCurBtAutoReport) 
			return;
	}
	halbtc8723b1ant_SetBtAutoReport(pBtCoexist, pCoexDm->bCurBtAutoReport);

	pCoexDm->bPreBtAutoReport = pCoexDm->bCurBtAutoReport;
}

VOID
halbtc8723b1ant_SetSwPenaltyTxRateAdaptive(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bLowPenaltyRa
	)
{
	u1Byte			H2C_Parameter[6] ={0};
	
	H2C_Parameter[0] = 0x6;	// opCode, 0x6= Retry_Penalty

	if(bLowPenaltyRa)
	{
		H2C_Parameter[1] |= BIT0;
		H2C_Parameter[2] = 0x00;  //normal rate except MCS7/6/5, OFDM54/48/36
		H2C_Parameter[3] = 0xf7;  //MCS7 or OFDM54
		H2C_Parameter[4] = 0xf8;  //MCS6 or OFDM48
		H2C_Parameter[5] = 0xf9;	//MCS5 or OFDM36	
	}

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, ("[BTCoex], set WiFi Low-Penalty Retry: %s", 
		(bLowPenaltyRa? "ON!!":"OFF!!") ));

	pBtCoexist->fBtcFillH2c(pBtCoexist, 0x69, 6, H2C_Parameter);
}

VOID
halbtc8723b1ant_LowPenaltyRa(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bForceExec,
	IN	BOOLEAN			bLowPenaltyRa
	)
{
	pCoexDm->bCurLowPenaltyRa = bLowPenaltyRa;

	if(!bForceExec)
	{
		if(pCoexDm->bPreLowPenaltyRa == pCoexDm->bCurLowPenaltyRa) 
			return;
	}
	halbtc8723b1ant_SetSwPenaltyTxRateAdaptive(pBtCoexist, pCoexDm->bCurLowPenaltyRa);

	pCoexDm->bPreLowPenaltyRa = pCoexDm->bCurLowPenaltyRa;
}

VOID
halbtc8723b1ant_SetCoexTable(
	IN	PBTC_COEXIST	pBtCoexist,
	IN	u4Byte		val0x6c0,
	IN	u4Byte		val0x6c4,
	IN	u4Byte		val0x6c8,
	IN	u1Byte		val0x6cc
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, ("[BTCoex], set coex table, set 0x6c0=0x%x\n", val0x6c0));
	pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x6c0, val0x6c0);

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, ("[BTCoex], set coex table, set 0x6c4=0x%x\n", val0x6c4));
	pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x6c4, val0x6c4);

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, ("[BTCoex], set coex table, set 0x6c8=0x%x\n", val0x6c8));
	pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x6c8, val0x6c8);

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_EXEC, ("[BTCoex], set coex table, set 0x6cc=0x%x\n", val0x6cc));
	pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x6cc, val0x6cc);
}

VOID
halbtc8723b1ant_CoexTable(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bForceExec,
	IN	u4Byte			val0x6c0,
	IN	u4Byte			val0x6c4,
	IN	u4Byte			val0x6c8,
	IN	u1Byte			val0x6cc
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW, ("[BTCoex], %s write Coex Table 0x6c0=0x%x, 0x6c4=0x%x, 0x6cc=0x%x\n", 
		(bForceExec? "force to":""), val0x6c0, val0x6c4, val0x6cc));
	pCoexDm->curVal0x6c0 = val0x6c0;
	pCoexDm->curVal0x6c4 = val0x6c4;
	pCoexDm->curVal0x6c8 = val0x6c8;
	pCoexDm->curVal0x6cc = val0x6cc;

	if(!bForceExec)
	{
		//BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_DETAIL, ("[BTCoex], preVal0x6c0=0x%x, preVal0x6c4=0x%x, preVal0x6c8=0x%x, preVal0x6cc=0x%x !!\n", 
		//	pCoexDm->preVal0x6c0, pCoexDm->preVal0x6c4, pCoexDm->preVal0x6c8, pCoexDm->preVal0x6cc));
		//BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_SW_DETAIL, ("[BTCoex], curVal0x6c0=0x%x, curVal0x6c4=0x%x, curVal0x6c8=0x%x, curVal0x6cc=0x%x !!\n", 
		//	pCoexDm->curVal0x6c0, pCoexDm->curVal0x6c4, pCoexDm->curVal0x6c8, pCoexDm->curVal0x6cc));
	
		if( (pCoexDm->preVal0x6c0 == pCoexDm->curVal0x6c0) &&
			(pCoexDm->preVal0x6c4 == pCoexDm->curVal0x6c4) &&
			(pCoexDm->preVal0x6c8 == pCoexDm->curVal0x6c8) &&
			(pCoexDm->preVal0x6cc == pCoexDm->curVal0x6cc) )
			return;
	}
	halbtc8723b1ant_SetCoexTable(pBtCoexist, val0x6c0, val0x6c4, val0x6c8, val0x6cc);

	pCoexDm->preVal0x6c0 = pCoexDm->curVal0x6c0;
	pCoexDm->preVal0x6c4 = pCoexDm->curVal0x6c4;
	pCoexDm->preVal0x6c8 = pCoexDm->curVal0x6c8;
	pCoexDm->preVal0x6cc = pCoexDm->curVal0x6cc;
}

VOID
halbtc8723b1ant_CoexTableWithType(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bForceExec,
	IN	u1Byte				type
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], ********** CoexTable(%d) **********\n", type));
	
	switch(type)
	{
		case 0:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0x55555555, 0x55555555, 0xffffff, 0x3);
			break;
		case 1:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0x55555555, 0x5a5a5a5a, 0xffffff, 0x3);
			break;
		case 2:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0x5a5a5a5a, 0x5a5a5a5a, 0xffffff, 0x3);
			break;
		case 3:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0x5a5a5a5a, 0xaaaaaaaa, 0xffffff, 0x3);
			break;
		case 4:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0x55555555, 0xaaaa5a5a, 0xffffff, 0x3);
			break;
		case 5:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0x5a5a5a5a, 0xaaaa5a5a, 0xffffff, 0x3);
			break;
		case 6:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0x55555555, 0xaaaaaaaa, 0xffffff, 0x3);
			break;
		case 7:
			halbtc8723b1ant_CoexTable(pBtCoexist, bForceExec, 0xaaaaaaaa, 0xaaaaaaaa, 0xffffff, 0x3);
			break;
		default:
			break;
	}
}

VOID
halbtc8723b1ant_SetFwIgnoreWlanAct(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bEnable
	)
{
	u1Byte			H2C_Parameter[1] ={0};
		
	if(bEnable)
	{
		H2C_Parameter[0] |= BIT0;		// function enable
	}
	
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, ("[BTCoex], set FW for BT Ignore Wlan_Act, FW write 0x63=0x%x\n", 
		H2C_Parameter[0]));

	pBtCoexist->fBtcFillH2c(pBtCoexist, 0x63, 1, H2C_Parameter);
}

VOID
halbtc8723b1ant_IgnoreWlanAct(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bForceExec,
	IN	BOOLEAN			bEnable
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, ("[BTCoex], %s turn Ignore WlanAct %s\n", 
		(bForceExec? "force to":""), (bEnable? "ON":"OFF")));
	pCoexDm->bCurIgnoreWlanAct = bEnable;

	if(!bForceExec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], bPreIgnoreWlanAct = %d, bCurIgnoreWlanAct = %d!!\n", 
			pCoexDm->bPreIgnoreWlanAct, pCoexDm->bCurIgnoreWlanAct));

		if(pCoexDm->bPreIgnoreWlanAct == pCoexDm->bCurIgnoreWlanAct)
			return;
	}
	halbtc8723b1ant_SetFwIgnoreWlanAct(pBtCoexist, bEnable);

	pCoexDm->bPreIgnoreWlanAct = pCoexDm->bCurIgnoreWlanAct;
}

VOID
halbtc8723b1ant_SetFwPstdma(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte			byte1,
	IN	u1Byte			byte2,
	IN	u1Byte			byte3,
	IN	u1Byte			byte4,
	IN	u1Byte			byte5
	)
{
	u1Byte			H2C_Parameter[5] ={0};
	u1Byte			realByte1=byte1, realByte5=byte5;
	BOOLEAN			bApEnable=FALSE;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_AP_MODE_ENABLE, &bApEnable);

	if(bApEnable)
	{
		if(byte1&BIT4 && !(byte1&BIT5))
		{			
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], FW for 1Ant AP mode\n"));
			realByte1 &= ~BIT4;
			realByte1 |= BIT5;

			realByte5 |= BIT5;
			realByte5 &= ~BIT6;
		}
	}

	H2C_Parameter[0] = realByte1;	
	H2C_Parameter[1] = byte2;	
	H2C_Parameter[2] = byte3;
	H2C_Parameter[3] = byte4;
	H2C_Parameter[4] = realByte5;

	pCoexDm->psTdmaPara[0] = realByte1;
	pCoexDm->psTdmaPara[1] = byte2;
	pCoexDm->psTdmaPara[2] = byte3;
	pCoexDm->psTdmaPara[3] = byte4;
	pCoexDm->psTdmaPara[4] = realByte5;
	
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, ("[BTCoex], PS-TDMA H2C cmd =0x%x%08x\n", 
		H2C_Parameter[0], 
		H2C_Parameter[1]<<24|H2C_Parameter[2]<<16|H2C_Parameter[3]<<8|H2C_Parameter[4]));

	pBtCoexist->fBtcFillH2c(pBtCoexist, 0x60, 5, H2C_Parameter);
}

VOID
halbtc8723b1ant_SetLpsRpwm(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte			lpsVal,
	IN	u1Byte			rpwmVal
	)
{
	u1Byte	lps=lpsVal;
	u1Byte	rpwm=rpwmVal;
	
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_U1_LPS_VAL, &lps);
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_U1_RPWM_VAL, &rpwm);
}

VOID
halbtc8723b1ant_LpsRpwm(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bForceExec,
	IN	u1Byte			lpsVal,
	IN	u1Byte			rpwmVal
	)
{
	BOOLEAN	bForceExecPwrCmd=FALSE;
	
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, ("[BTCoex], %s set lps/rpwm=0x%x/0x%x \n", 
		(bForceExec? "force to":""), lpsVal, rpwmVal));
	pCoexDm->curLps = lpsVal;
	pCoexDm->curRpwm = rpwmVal;

	if(!bForceExec)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], LPS-RxBeaconMode=0x%x , LPS-RPWM=0x%x!!\n", 
			 pCoexDm->curLps, pCoexDm->curRpwm));

		if( (pCoexDm->preLps == pCoexDm->curLps) &&
			(pCoexDm->preRpwm == pCoexDm->curRpwm) )
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], LPS-RPWM_Last=0x%x , LPS-RPWM_Now=0x%x!!\n", 
				 pCoexDm->preRpwm, pCoexDm->curRpwm));

			return;
		}
	}
	halbtc8723b1ant_SetLpsRpwm(pBtCoexist, lpsVal, rpwmVal);

	pCoexDm->preLps = pCoexDm->curLps;
	pCoexDm->preRpwm = pCoexDm->curRpwm;
}

VOID
halbtc8723b1ant_SwMechanism(
	IN	PBTC_COEXIST	pBtCoexist,	
	IN	BOOLEAN 		bLowPenaltyRA
	) 
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, ("[BTCoex], SM[LpRA] = %d\n", 
		bLowPenaltyRA));

	halbtc8723b1ant_LowPenaltyRa(pBtCoexist, NORMAL_EXEC, bLowPenaltyRA);
}

VOID
halbtc8723b1ant_SetAntPath(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte				antPosType,
	IN	BOOLEAN				bInitHwCfg,
	IN	BOOLEAN				bWifiOff
	)
{
	PBTC_BOARD_INFO pBoardInfo=&pBtCoexist->boardInfo;
	u4Byte			fwVer=0, u4Tmp=0, cntBtCalChk=0;
	BOOLEAN			bPgExtSwitch=FALSE;
	BOOLEAN			bUseExtSwitch=FALSE;
	BOOLEAN			bIsInMpMode = FALSE;
	u1Byte			H2C_Parameter[2] ={0}, u1Tmp = 0;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_EXT_SWITCH, &bPgExtSwitch);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_FW_VER, &fwVer);	// [31:16]=fw ver, [15:0]=fw sub ver

	if((fwVer>0 && fwVer<0xc0000) || bPgExtSwitch)
		bUseExtSwitch = TRUE;

	if(bInitHwCfg)
	{
		pBtCoexist->fBtcSetRfReg(pBtCoexist, BTC_RF_A, 0x1, 0xfffff, 0x780); //WiFi TRx Mask on
		pBtCoexist->fBtcSetBtReg(pBtCoexist, BTC_BT_REG_RF, 0x3c, 0x15); //BT TRx Mask on

		if(fwVer >= 0x180000)
		{
		/* Use H2C to set GNT_BT to HIGH */
		H2C_Parameter[0] = 1;
		pBtCoexist->fBtcFillH2c(pBtCoexist, 0x6E, 1, H2C_Parameter);
		}
		else
		{
			// set grant_bt to high
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x765, 0x18); 
		}
		//set wlan_act control by PTA
		pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x76e, 0x4); 

		pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x67, 0x20, 0x1); //BT select s0/s1 is controlled by WiFi

		pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x39, 0x8, 0x1);
		pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x974, 0xff);
		pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x944, 0x3, 0x3);
		pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x930, 0x77);
	}
	else if(bWifiOff)
	{
		if(fwVer >= 0x180000)
		{
		/* Use H2C to set GNT_BT to HIGH */
		H2C_Parameter[0] = 1;
		pBtCoexist->fBtcFillH2c(pBtCoexist, 0x6E, 1, H2C_Parameter);
		}
		else
		{
			// set grant_bt to high
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x765, 0x18); 
		}
		//set wlan_act to always low
		pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x76e, 0x4);
		
		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_IS_IN_MP_MODE, &bIsInMpMode);
		if(!bIsInMpMode)	
			pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x67, 0x20, 0x0); //BT select s0/s1 is controlled by BT
		else
			pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x67, 0x20, 0x1); //BT select s0/s1 is controlled by WiFi
	
		// 0x4c[24:23]=00, Set Antenna control by BT_RFE_CTRL	BT Vendor 0xac=0xf002
		u4Tmp = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x4c);
		u4Tmp &= ~BIT23;
		u4Tmp &= ~BIT24;
		pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x4c, u4Tmp);
	}
	else
	{			
		/* Use H2C to set GNT_BT to LOW */
		if(fwVer >= 0x180000)
		{
		if (pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x765) != 0)
		{		
			H2C_Parameter[0] = 0;
			pBtCoexist->fBtcFillH2c(pBtCoexist, 0x6E, 1, H2C_Parameter);
		}
		}
		else
		{
			// BT calibration check
			while(cntBtCalChk <= 20)
			{
				u1Tmp = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x49d);
				cntBtCalChk++;
				if(u1Tmp & BIT0)
				{
					BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], ########### BT is calibrating (wait cnt=%d) ###########\n", cntBtCalChk));
					delay_ms(50);
				}
				else
				{
					BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], ********** BT is NOT calibrating (wait cnt=%d)**********\n", cntBtCalChk));
					break;
				}
			}

			// set grant_bt to PTA
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x765, 0x0);
		}

		if (pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x76e) != 0xc)
		{
			//set wlan_act control by PTA
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x76e, 0xc); 
		}
	}
	
	if(bUseExtSwitch)
	{
		if(bInitHwCfg)
		{
			// 0x4c[23]=0, 0x4c[24]=1  Antenna control by WL/BT
			u4Tmp = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x4c);
			u4Tmp &=~BIT23;
			u4Tmp |= BIT24;
			pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x4c, u4Tmp);

			pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x0); // fixed internal switch S1->WiFi, S0->BT

			if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
			{
				//tell firmware "no antenna inverse"
				H2C_Parameter[0] = 0;
				H2C_Parameter[1] = 1;  //ext switch type
				pBtCoexist->fBtcFillH2c(pBtCoexist, 0x65, 2, H2C_Parameter);	
			}
			else
			{
				//tell firmware "antenna inverse"
				H2C_Parameter[0] = 1;
				H2C_Parameter[1] = 1;  //ext switch type
				pBtCoexist->fBtcFillH2c(pBtCoexist, 0x65, 2, H2C_Parameter);	
			}
		}
		
		
		// ext switch setting
		switch(antPosType)
		{
			case BTC_ANT_PATH_WIFI:
				if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
					pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x92c, 0x3, 0x1);
				else
					pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x92c, 0x3, 0x2);
				break;
			case BTC_ANT_PATH_BT:
				if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
					pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x92c, 0x3, 0x2);
				else					
					pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x92c, 0x3, 0x1);
				break;
			default:
			case BTC_ANT_PATH_PTA:
				if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
					pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x92c, 0x3, 0x1);
				else
					pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x92c, 0x3, 0x2);
				break;
		}
	
	}
	else
	{
		if(bInitHwCfg)
		{
			// 0x4c[23]=1, 0x4c[24]=0  Antenna control by 0x64
			u4Tmp = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x4c);
			u4Tmp |= BIT23;
			u4Tmp &=~BIT24;	      
			pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x4c, u4Tmp);

			//Fix Ext switch Main->S1, Aux->S0
			pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x64, 0x1, 0x0); 

			if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
			{
	
				//tell firmware "no antenna inverse"
				H2C_Parameter[0] = 0;
				H2C_Parameter[1] = 0;  //internal switch type
				pBtCoexist->fBtcFillH2c(pBtCoexist, 0x65, 2, H2C_Parameter);	
			}
			else
			{
		
				//tell firmware "antenna inverse"
				H2C_Parameter[0] = 1;
				H2C_Parameter[1] = 0;  //internal switch type
				pBtCoexist->fBtcFillH2c(pBtCoexist, 0x65, 2, H2C_Parameter);	
			}
		}
	

		// internal switch setting
		switch(antPosType)
		{
			case BTC_ANT_PATH_WIFI:
				if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x0);
				else
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x280);
				break;
			case BTC_ANT_PATH_BT:
				if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
 					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x280);
				else
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x0);
				break;
			default:
			case BTC_ANT_PATH_PTA:
				if(pBoardInfo->btdmAntPos == BTC_ANTENNA_AT_MAIN_PORT)
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x200);					
				else
					pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x80);
				break;
		}
	}
}

VOID
halbtc8723b1ant_SetAntPathDCut(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bAntennaAux,	//For 1-Ant--> 1: Antenna at S0, 0: Antenna at S1. Set 0 for 2-Ant
	IN	BOOLEAN				bExtSwitch,		// 1: Ext Switch (SPDT) exist on module, 0: no Ext Switch (SPDT) exist on module
	IN	BOOLEAN				bTwoAntenna,	// 1: 2-Antenna, 0:1-Antenna
	IN	u1Byte				antennaPos,		//Set Antenna Pos, For 1-Ant: BTC_ANT_PATH_WIFI, BTC_ANT_PATH_BT, BTC_ANT_PATH_PTA, For 2-Ant:BTC_ANT_WIFI_AT_MAIN, BTC_ANT_WIFI_AT_Aux
	IN	u1Byte				wifiState		//BTC_WIFI_STAT_INIT, BTC_WIFI_STAT_IQK, BTC_WIFI_STAT_NORMAL_OFF, BTC_WIFI_STAT_MP_OFF, BTC_WIFI_STAT_NORMAL, BTC_WIFI_STAT_ANT_DIV
	)
{
	u1Byte	dataLen=5;
	u1Byte	buf[6] = {0};

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, ("[BTCoex], set BT Ant, bAntennaAux/bExtSwitch/bTwoAntenna/antennaPos/wifiState=%d/%d/%d/%d/%d\n",
		bAntennaAux, bExtSwitch, bTwoAntenna, antennaPos, wifiState));

	buf[0] = dataLen; 
	
	if(bAntennaAux)
		buf[1] = 0x1;
	
	if(bExtSwitch)
		buf[2] = 0x1;			
	
	if(bTwoAntenna)
		buf[3] = 0x1;

	buf[4] = antennaPos;

	buf[5] = wifiState;
	
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_CTRL_8723B_ANT, (PVOID)&buf[0]);	
}

VOID
halbtc8723b1ant_PsTdma(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bForceExec,
	IN	BOOLEAN			bTurnOn,
	IN	u1Byte			type
	)
{
	PBTC_BOARD_INFO	pBoardInfo=&pBtCoexist->boardInfo;
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	BOOLEAN			bTurnOnByCnt=FALSE, bWifiBusy=FALSE;
	u1Byte			psTdmaTypeByCnt=0, rssiAdjustVal=0;
	u1Byte			psTdmaByte4Val = 0x50;
	//u4Byte			fwVer=0;

	//BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, ("[BTCoex], %s turn %s PS TDMA, type=%d\n", 
	//	(bForceExec? "force to":""), (bTurnOn? "ON":"OFF"), type));
	pCoexDm->bCurPsTdmaOn = bTurnOn;
	pCoexDm->curPsTdma = type;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_BUSY, &bWifiBusy);	

	if (pCoexDm->bCurPsTdmaOn)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], ********** TDMA(on, %d) **********\n", 
				pCoexDm->curPsTdma));
	}
	else
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], ********** TDMA(off, %d) **********\n", 
				pCoexDm->curPsTdma));
	}
			
	if(!bForceExec)
	{	
		if( (pCoexDm->bPrePsTdmaOn == pCoexDm->bCurPsTdmaOn) &&
			(pCoexDm->prePsTdma == pCoexDm->curPsTdma) )
			return;
	}
	if(bTurnOn)
	{
		if (pBtLinkInfo->bSlaveRole == TRUE)
		{
			psTdmaByte4Val = 0x51;  //0x778 = 0x1 at wifi slot (no blocking BT Low-Pri pkts)
		}
		 
		
		switch(type)
		{
			default:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x1a, 0x1a, 0x0, psTdmaByte4Val);
				break;
			case 1:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x3a, 0x03, 0x10, psTdmaByte4Val);				
				break;
			case 2:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x2d, 0x03, 0x10, psTdmaByte4Val);				
				break;
			case 3:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x1d, 0x1d, 0x0, psTdmaByte4Val);
				break;
			case 4:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x93, 0x15, 0x3, 0x14, 0x0);
				break;
			case 5:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x61, 0x15, 0x3, 0x11, 0x10);
				break;
			case 6:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x61, 0x20, 0x3, 0x11, 0x11);
				break;
			case 7:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x13, 0xc, 0x5, 0x0, 0x0);
				break;
			case 8:	
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x93, 0x25, 0x3, 0x10, 0x0);
				break;
			case 9:	
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x21, 0x3, 0x10, psTdmaByte4Val);				
				break;
			case 10:	
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x13, 0xa, 0xa, 0x0, 0x40);
				break;
			case 11:	
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x21, 0x03, 0x10, psTdmaByte4Val);			
				break;
			case 12:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x0a, 0x0a, 0x0, 0x50);
				break;
			case 13:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x12, 0x12, 0x0, psTdmaByte4Val);
				break;
			case 14:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x21, 0x3, 0x10, psTdmaByte4Val);
				break;
			case 15:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x13, 0xa, 0x3, 0x8, 0x0);
				break;
			case 16:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x93, 0x15, 0x3, 0x10, 0x0);
				break;
			case 18:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x93, 0x25, 0x3, 0x10, 0x0);
				break;			
			case 20:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x61, 0x35, 0x03, 0x11, 0x10);
				break;
			case 21:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x61, 0x25, 0x03, 0x11, 0x11);
				break;
			case 22:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x61, 0x25, 0x03, 0x11, 0x10);
				break;
			case 23:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xe3, 0x25, 0x3, 0x31, 0x18);
				break;
			case 24:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xe3, 0x15, 0x3, 0x31, 0x18);
				break;
			case 25:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xe3, 0xa, 0x3, 0x31, 0x18);
				break;
			case 26:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xe3, 0xa, 0x3, 0x31, 0x18);
				break;
			case 27:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xe3, 0x25, 0x3, 0x31, 0x98);
				break;
			case 28:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x69, 0x25, 0x3, 0x31, 0x0);
				break;
			case 29:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xab, 0x1a, 0x1a, 0x1, 0x10);
				break;
			case 30:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x51, 0x30, 0x3, 0x10, 0x10);
				break;
			case 31:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xd3, 0x1a, 0x1a, 0, 0x58);
				break;
			case 32:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x61, 0x35, 0x3, 0x11, 0x11);
				break;
			case 33:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xa3, 0x25, 0x3, 0x30, 0x90);
				break;
			case 34:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x53, 0x1a, 0x1a, 0x0, 0x10);
				break;
			case 35:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x63, 0x1a, 0x1a, 0x0, 0x10);
				break;
			case 36:
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0xd3, 0x12, 0x3, 0x14, 0x50);
				break;
			case 40: // SoftAP only with no sta associated,BT disable ,TDMA mode for power saving
				/* here softap mode screen off will cost 70-80mA for phone */
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x23, 0x18, 0x00, 0x10, 0x24);
				break;	
		}
	}
	else
	{		

		// disable PS tdma
		switch(type)
		{
			case 8: //PTA Control
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x8, 0x0, 0x0, 0x0, 0x0);
				//halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_PTA, BTC_WIFI_STAT_NORMAL);
				halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_PTA, FALSE, FALSE);
				break;
			case 0:
			default:  //Software control, Antenna at BT side
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x0, 0x0, 0x0, 0x0, 0x0);
				//halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_BT, BTC_WIFI_STAT_NORMAL);
				halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_BT, FALSE, FALSE);
				break;
			case 9:   //Software control, Antenna at WiFi side
				halbtc8723b1ant_SetFwPstdma(pBtCoexist, 0x0, 0x0, 0x0, 0x0, 0x0);
				//halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_WIFI, BTC_WIFI_STAT_NORMAL);
				halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_WIFI, FALSE, FALSE);
				break;			
		}
	}
	rssiAdjustVal =0;
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_U1_RSSI_ADJ_VAL_FOR_1ANT_COEX_TYPE, &rssiAdjustVal);

	// update pre state
	pCoexDm->bPrePsTdmaOn = pCoexDm->bCurPsTdmaOn;
	pCoexDm->prePsTdma = pCoexDm->curPsTdma;
}

VOID
halbtc8723b1ant_CoexAllOff(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	// sw all off
	halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);

	// hw all off
	halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 0);
}

BOOLEAN
halbtc8723b1ant_IsCommonAction(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	BOOLEAN			bCommon=FALSE, bWifiConnected=FALSE, bWifiBusy=FALSE;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_BUSY, &bWifiBusy);

	if(!bWifiConnected && 
		BT_8723B_1ANT_BT_STATUS_NON_CONNECTED_IDLE == pCoexDm->btStatus)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi non connected-idle + BT non connected-idle!!\n"));
		
 		//halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);

		bCommon = TRUE;
	}
	else if(bWifiConnected && 
		(BT_8723B_1ANT_BT_STATUS_NON_CONNECTED_IDLE == pCoexDm->btStatus) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi connected + BT non connected-idle!!\n"));

		//halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);

		bCommon = TRUE;
	}
	else if(!bWifiConnected && 
		(BT_8723B_1ANT_BT_STATUS_CONNECTED_IDLE == pCoexDm->btStatus) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi non connected-idle + BT connected-idle!!\n"));

		//halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);

		bCommon = TRUE;
	}
	else if(bWifiConnected && 
		(BT_8723B_1ANT_BT_STATUS_CONNECTED_IDLE == pCoexDm->btStatus) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi connected + BT connected-idle!!\n"));

		//halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);

		bCommon = TRUE;
	}
	else if(!bWifiConnected && 
		(BT_8723B_1ANT_BT_STATUS_CONNECTED_IDLE != pCoexDm->btStatus) )
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi non connected-idle + BT Busy!!\n"));

		//halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
		
		bCommon = TRUE;
	}
	else
	{
		if (bWifiBusy)			
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi Connected-Busy + BT Busy!!\n"));
		}
		else
		{
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Wifi Connected-Idle + BT Busy!!\n"));
		}
		
		bCommon = FALSE;
	}
	
	return bCommon;
}


VOID
halbtc8723b1ant_TdmaDurationAdjustForAcl(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte				wifiStatus
	)
{
	static s4Byte		up,dn,m,n,WaitCount;
	s4Byte			result;   //0: no change, +1: increase WiFi duration, -1: decrease WiFi duration
	u1Byte			retryCount=0, btInfoExt;
	static BOOLEAN	bPreWifiBusy=FALSE;
	BOOLEAN			bWifiBusy = FALSE;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW, ("[BTCoex], TdmaDurationAdjustForAcl()\n"));

	if(BT_8723B_1ANT_WIFI_STATUS_CONNECTED_BUSY == wifiStatus) 
		bWifiBusy = TRUE;
	else
		bWifiBusy = FALSE;	 	

	if( (BT_8723B_1ANT_WIFI_STATUS_NON_CONNECTED_ASSO_AUTH_SCAN == wifiStatus) ||
		(BT_8723B_1ANT_WIFI_STATUS_CONNECTED_SCAN == wifiStatus) ||
		(BT_8723B_1ANT_WIFI_STATUS_CONNECTED_SPECIAL_PKT == wifiStatus) )
	{
		if( pCoexDm->curPsTdma != 1 &&
			pCoexDm->curPsTdma != 2 &&
			pCoexDm->curPsTdma != 3 &&
			pCoexDm->curPsTdma != 9 )
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 9);
			pCoexDm->psTdmaDuAdjType = 9;

			up = 0;
			dn = 0;
			m = 1;
			n= 3;
			result = 0;
			WaitCount = 0;
		}		
		return;
	}

	if(!pCoexDm->bAutoTdmaAdjust)
	{
		pCoexDm->bAutoTdmaAdjust = TRUE;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], first run TdmaDurationAdjust()!!\n"));

		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 2);
		pCoexDm->psTdmaDuAdjType = 2;
		//============
		up = 0;
		dn = 0;
		m = 1;
		n= 3;
		result = 0;
		WaitCount = 0;
	}
	else
	{
		//accquire the BT TRx retry count from BT_Info byte2
		retryCount = pCoexSta->btRetryCnt;
		btInfoExt = pCoexSta->btInfoExt;
		//BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], retryCount = %d\n", retryCount));
		//BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], up=%d, dn=%d, m=%d, n=%d, WaitCount=%d\n", 
		//	up, dn, m, n, WaitCount));

		if ( (pCoexSta->lowPriorityTx) > 1050 ||  (pCoexSta->lowPriorityRx) > 1250 )
			retryCount++;	
		
		result = 0;
		WaitCount++; 
		  
		if(retryCount == 0)  // no retry in the last 2-second duration
		{
			up++;
			dn--;

			if (dn <= 0)
				dn = 0;				 

			if(up >= n)	// if 連續 n 個2秒 retry count為0, 則調寬WiFi duration
			{
				WaitCount = 0; 
				n = 3;
				up = 0;
				dn = 0;
				result = 1; 
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], Increase wifi duration!!\n"));
			}
		}
		else if (retryCount <= 3)	// <=3 retry in the last 2-second duration
		{
			up--; 
			dn++;

			if (up <= 0)
				up = 0;

			if (dn == 2)	// if 連續 2 個2秒 retry count< 3, 則調窄WiFi duration
			{
				if (WaitCount <= 2)
					m++; // 避免一直在兩個level中來回
				else
					m = 1;

				if ( m >= 20) //m 最大值 = 20 ' 最大120秒 recheck是否調整 WiFi duration.
					m = 20;

				n = 3*m;
				up = 0;
				dn = 0;
				WaitCount = 0;
				result = -1; 
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], Decrease wifi duration for retryCounter<3!!\n"));
			}
		}
		else  //retry count > 3, 只要1次 retry count > 3, 則調窄WiFi duration
		{
			if (WaitCount == 1)
				m++; // 避免一直在兩個level中來回
			else
				m = 1;

			if ( m >= 20) //m 最大值 = 20 ' 最大120秒 recheck是否調整 WiFi duration.
				m = 20;

			n = 3*m;
			up = 0;
			dn = 0;
			WaitCount = 0; 
			result = -1;
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], Decrease wifi duration for retryCounter>3!!\n"));
		}

		if(result == -1)
		{
			if( (BT_INFO_8723B_1ANT_A2DP_BASIC_RATE(btInfoExt)) &&
				((pCoexDm->curPsTdma == 1) ||(pCoexDm->curPsTdma == 2)) )
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 9);
				pCoexDm->psTdmaDuAdjType = 9;
			}
			else if(pCoexDm->curPsTdma == 1)
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 2);
				pCoexDm->psTdmaDuAdjType = 2;
			}
			else if(pCoexDm->curPsTdma == 2)
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 9);
				pCoexDm->psTdmaDuAdjType = 9;
			}
			else if(pCoexDm->curPsTdma == 9)
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 11);
				pCoexDm->psTdmaDuAdjType = 11;
			}
		}
		else if(result == 1)
		{
			if( (BT_INFO_8723B_1ANT_A2DP_BASIC_RATE(btInfoExt)) &&
				((pCoexDm->curPsTdma == 1) ||(pCoexDm->curPsTdma == 2)) )
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 9);
				pCoexDm->psTdmaDuAdjType = 9;
			}
			else if(pCoexDm->curPsTdma == 11)
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 9);
				pCoexDm->psTdmaDuAdjType = 9;
			}
			else if(pCoexDm->curPsTdma == 9)
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 2);
				pCoexDm->psTdmaDuAdjType = 2;
			}
			else if(pCoexDm->curPsTdma == 2)
			{
				halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 1);
				pCoexDm->psTdmaDuAdjType = 1;
			}
		}
		else	  //no change
		{
			/* Bryant Modify	
			if(bWifiBusy != bPreWifiBusy)  //if busy / idle change
			{
				bPreWifiBusy = bWifiBusy;
				halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, TRUE, pCoexDm->curPsTdma);
			}
			*/
		
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_DETAIL, ("[BTCoex], ********** TDMA(on, %d) **********\n", 
				pCoexDm->curPsTdma));
		}

		if( pCoexDm->curPsTdma != 1 &&
			pCoexDm->curPsTdma != 2 &&
			pCoexDm->curPsTdma != 9 &&
			pCoexDm->curPsTdma != 11 )
		{
			// recover to previous adjust type
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, pCoexDm->psTdmaDuAdjType);
		}
	}
}

VOID
halbtc8723b1ant_PsTdmaCheckForPowerSaveState(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN			bNewPsState
	)
{
	u1Byte	lpsMode=0x0;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U1_LPS_MODE, &lpsMode);
	
	if(lpsMode)	// already under LPS state
	{
		if(bNewPsState)		
		{
			// keep state under LPS, do nothing.
		}
		else
		{
			// will leave LPS state, turn off psTdma first
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 0);
		}
	}
	else						// NO PS state
	{
		if(bNewPsState)
		{
			// will enter LPS state, turn off psTdma first
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 0);
		}
		else
		{
			// keep state under NO PS state, do nothing.
		}
	}
}

VOID
halbtc8723b1ant_PowerSaveState(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte				psType,
	IN	u1Byte				lpsVal,
	IN	u1Byte				rpwmVal
	)
{
	BOOLEAN		bLowPwrDisable=FALSE;
	
	switch(psType)
	{
		case BTC_PS_WIFI_NATIVE:
			// recover to original 32k low power setting
			bLowPwrDisable = FALSE;
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_DISABLE_LOW_POWER, &bLowPwrDisable);
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_NORMAL_LPS, NULL);
			break;
		case BTC_PS_LPS_ON:
			halbtc8723b1ant_PsTdmaCheckForPowerSaveState(pBtCoexist, TRUE);
			halbtc8723b1ant_LpsRpwm(pBtCoexist, NORMAL_EXEC, lpsVal, rpwmVal);			
			// when coex force to enter LPS, do not enter 32k low power.
			bLowPwrDisable = TRUE;
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_DISABLE_LOW_POWER, &bLowPwrDisable);
			// power save must executed before psTdma.			
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_ENTER_LPS, NULL);
			break;
		case BTC_PS_LPS_OFF:
			halbtc8723b1ant_PsTdmaCheckForPowerSaveState(pBtCoexist, FALSE);
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_LEAVE_LPS, NULL);
			break;
		default:
			break;
	}
}

VOID
halbtc8723b1ant_ActionWifiOnly(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 0);
	halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 9);	
}

VOID
halbtc8723b1ant_MonitorBtEnableDisable(
	IN 	PBTC_COEXIST		pBtCoexist
	)
{
	static BOOLEAN	bPreBtDisabled=FALSE;
	static u4Byte		btDisableCnt=0;
	BOOLEAN			bBtActive=TRUE, bBtDisabled=FALSE;

	// This function check if bt is disabled

	if(	pCoexSta->highPriorityTx == 0 &&
		pCoexSta->highPriorityRx == 0 &&
		pCoexSta->lowPriorityTx == 0 &&
		pCoexSta->lowPriorityRx == 0)
	{
		bBtActive = FALSE;
	}
	if(	pCoexSta->highPriorityTx == 0xffff &&
		pCoexSta->highPriorityRx == 0xffff &&
		pCoexSta->lowPriorityTx == 0xffff &&
		pCoexSta->lowPriorityRx == 0xffff)
	{
		bBtActive = FALSE;
	}
	if(bBtActive)
	{
		btDisableCnt = 0;
		bBtDisabled = FALSE;
		pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_BT_DISABLE, &bBtDisabled);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, ("[BTCoex], BT is enabled !!\n"));
	}
	else
	{
		btDisableCnt++;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, ("[BTCoex], bt all counters=0, %d times!!\n", 
				btDisableCnt));
		if(btDisableCnt >= 2)
		{
			bBtDisabled = TRUE;
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_BT_DISABLE, &bBtDisabled);
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, ("[BTCoex], BT is disabled !!\n"));
			halbtc8723b1ant_ActionWifiOnly(pBtCoexist);
		}
	}
	if(bPreBtDisabled != bBtDisabled)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_BT_MONITOR, ("[BTCoex], BT is from %s to %s!!\n", 
			(bPreBtDisabled ? "disabled":"enabled"), 
			(bBtDisabled ? "disabled":"enabled")));
		bPreBtDisabled = bBtDisabled;
		if(!bBtDisabled)
		{
		}
		else
		{
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_LEAVE_LPS, NULL);
			pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_ACT_NORMAL_LPS, NULL);
		}
	}
}

//=============================================
//
//	Software Coex Mechanism start
//
//=============================================

// SCO only or SCO+PAN(HS)

/*
VOID
halbtc8723b1ant_ActionSco(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, TRUE);
}


VOID
halbtc8723b1ant_ActionHid(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, TRUE);
}

//A2DP only / PAN(EDR) only/ A2DP+PAN(HS)
VOID
halbtc8723b1ant_ActionA2dp(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
}

VOID
halbtc8723b1ant_ActionA2dpPanHs(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
}

VOID
halbtc8723b1ant_ActionPanEdr(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
}

//PAN(HS) only
VOID
halbtc8723b1ant_ActionPanHs(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
}

//PAN(EDR)+A2DP
VOID
halbtc8723b1ant_ActionPanEdrA2dp(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
}

VOID
halbtc8723b1ant_ActionPanEdrHid(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, TRUE);
}

// HID+A2DP+PAN(EDR)
VOID
halbtc8723b1ant_ActionHidA2dpPanEdr(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, TRUE);
}

VOID
halbtc8723b1ant_ActionHidA2dp(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_SwMechanism(pBtCoexist, TRUE);
}

*/

//=============================================
//
//	Non-Software Coex Mechanism start
//
//=============================================
VOID
halbtc8723b1ant_ActionWifiMultiPort(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
	
	halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);
	halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
}

VOID
halbtc8723b1ant_ActionHs(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 5);
	halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
}

VOID
halbtc8723b1ant_ActionBtInquiry(
	IN	PBTC_COEXIST		pBtCoexist
	)
{	
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	BOOLEAN			bWifiConnected=FALSE, bApEnable=FALSE, bWifiBusy=FALSE, bBtBusy=FALSE;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_AP_MODE_ENABLE, &bApEnable);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_BUSY, &bWifiBusy);
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_BT_TRAFFIC_BUSY, &bBtBusy);

	if ( (!bWifiConnected) && (!pCoexSta->bWiFiIsHighPriTask) )
	{
		halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);

	 	halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 0);		
	}
	else if( (pBtLinkInfo->bScoExist) || 	(pBtLinkInfo->bHidExist) || 	(pBtLinkInfo->bA2dpExist)  )
	{
		// SCO/HID/A2DP busy
		halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 32);
		
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);		
	}
	else if ( (pBtLinkInfo->bPanExist) || (bWifiBusy) )
	{
		halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);		
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 20);		
		
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
	}
	else
	{
		halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);

	 	halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 7);		
	}
}

VOID
halbtc8723b1ant_ActionBtScoHidOnlyBusy(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte				wifiStatus
	)
{
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	BOOLEAN	bWifiConnected=FALSE;
	u1Byte	wifiRssiState=BTC_RSSI_STATE_HIGH;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);

	// tdma and coex table

	if(pBtLinkInfo->bScoExist)
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 5);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 5);
	}
	else //HID
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 6);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 5);
	}
}

VOID
halbtc8723b1ant_ActionWifiConnectedBtAclBusy(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte				wifiStatus
	)
{
	u1Byte		btRssiState;
	
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	btRssiState = halbtc8723b1ant_BtRssiState(2, 28, 0);	

	if ( (pCoexSta->lowPriorityRx >= 1000)  && (pCoexSta->lowPriorityRx != 65535) )
	{
		pBtLinkInfo->bSlaveRole = TRUE;
	}
	else
	{
		pBtLinkInfo->bSlaveRole = FALSE;
	}

	if(pBtLinkInfo->bHidOnly)  //HID
	{
		halbtc8723b1ant_ActionBtScoHidOnlyBusy(pBtCoexist, wifiStatus);
		pCoexDm->bAutoTdmaAdjust = FALSE;
		return;
	}
	else if(pBtLinkInfo->bA2dpOnly)  //A2DP		
	{
		if(BT_8723B_1ANT_WIFI_STATUS_CONNECTED_IDLE == wifiStatus)
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 32);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
			pCoexDm->bAutoTdmaAdjust = FALSE;
		}
		else
		{
			halbtc8723b1ant_TdmaDurationAdjustForAcl(pBtCoexist, wifiStatus);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
			pCoexDm->bAutoTdmaAdjust = TRUE;
		}
	}
	else if(pBtLinkInfo->bHidExist&&pBtLinkInfo->bA2dpExist)  //HID+A2DP
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 14);
		pCoexDm->bAutoTdmaAdjust = FALSE;

		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
	}
	else if( (pBtLinkInfo->bPanOnly) || (pBtLinkInfo->bHidExist&&pBtLinkInfo->bPanExist) ) //PAN(OPP,FTP), HID+PAN(OPP,FTP)			
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 3);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
		pCoexDm->bAutoTdmaAdjust = FALSE;
	}
	else if ( ((pBtLinkInfo->bA2dpExist) && (pBtLinkInfo->bPanExist)) ||
		       (pBtLinkInfo->bHidExist&&pBtLinkInfo->bA2dpExist&&pBtLinkInfo->bPanExist) ) //A2DP+PAN(OPP,FTP), HID+A2DP+PAN(OPP,FTP)
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 13);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
		pCoexDm->bAutoTdmaAdjust = FALSE;
	}
	else
	{		
		//BT no-profile busy (0x9)
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 32);	
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
		pCoexDm->bAutoTdmaAdjust = FALSE;
	}	
}

VOID
halbtc8723b1ant_ActionWifiNotConnected(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	// power save state
	halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, FALSE, 8);
	halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 0);
}

VOID
halbtc8723b1ant_ActionWifiNotConnectedScan(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	
	halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	if(BT_8723B_1ANT_BT_STATUS_ACL_BUSY == pCoexDm->btStatus)
	{
		if (pBtLinkInfo->bA2dpExist)
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 32);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
		}
		else if (pBtLinkInfo->bA2dpExist && pBtLinkInfo->bPanExist)
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 22);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
		}
	 	else
	 	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 20);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);			
		}
	}
	else if( (BT_8723B_1ANT_BT_STATUS_SCO_BUSY == pCoexDm->btStatus) ||
			(BT_8723B_1ANT_BT_STATUS_ACL_SCO_BUSY == pCoexDm->btStatus) )
	{
		halbtc8723b1ant_ActionBtScoHidOnlyBusy(pBtCoexist,
			BT_8723B_1ANT_WIFI_STATUS_CONNECTED_SCAN);
	}
	else
	{
		//Bryant Add
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
	}
}

VOID
halbtc8723b1ant_ActionWifiNotConnectedAssoAuth(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	
	halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	if( (pBtLinkInfo->bScoExist)  || (pBtLinkInfo->bHidExist) )
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 32);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);		
	}
	else if( (pBtLinkInfo->bA2dpExist)  || (pBtLinkInfo->bPanExist) ) 			
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 22);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);		
	}
	else
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
	}
}

VOID
halbtc8723b1ant_ActionWifiConnectedScan(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	
	halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	if(BT_8723B_1ANT_BT_STATUS_ACL_BUSY == pCoexDm->btStatus)
	{
		if (pBtLinkInfo->bA2dpExist)
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 32);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
		}
		else if (pBtLinkInfo->bA2dpExist && pBtLinkInfo->bPanExist)
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 22);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
		}
	 	else
	 	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 20);
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);
	}
	}
	else if( (BT_8723B_1ANT_BT_STATUS_SCO_BUSY == pCoexDm->btStatus) ||
			(BT_8723B_1ANT_BT_STATUS_ACL_SCO_BUSY == pCoexDm->btStatus) )
	{
		halbtc8723b1ant_ActionBtScoHidOnlyBusy(pBtCoexist,
			BT_8723B_1ANT_WIFI_STATUS_CONNECTED_SCAN);
	}
	else
	{
		//Bryant Add
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
	}
}

VOID
halbtc8723b1ant_ActionWifiConnectedSpecialPacket(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;

	halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	if( (pBtLinkInfo->bScoExist)  || (pBtLinkInfo->bHidExist) || (pBtLinkInfo->bA2dpExist) )
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 32);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);		
	}
	else if(pBtLinkInfo->bPanExist)
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, TRUE, 22);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 4);		
	}
	else
	{
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
	}
}

VOID
halbtc8723b1ant_ActionWifiConnected(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	BOOLEAN 	bWifiBusy=FALSE;
	BOOLEAN		bScan=FALSE, bLink=FALSE, bRoam=FALSE;
	BOOLEAN		bUnder4way=FALSE, bApEnable=FALSE;
	u4Byte		wifiBw;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], CoexForWifiConnect()===>\n"));

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_4_WAY_PROGRESS, &bUnder4way);
	if(bUnder4way)
	{
		halbtc8723b1ant_ActionWifiConnectedSpecialPacket(pBtCoexist);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], CoexForWifiConnect(), return for wifi is under 4way<===\n"));
		return;
	}
	
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_SCAN, &bScan);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_LINK, &bLink);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_ROAM, &bRoam);
	if(bScan || bLink || bRoam)
	{
		if(bScan)	
			halbtc8723b1ant_ActionWifiConnectedScan(pBtCoexist);
		else
			halbtc8723b1ant_ActionWifiConnectedSpecialPacket(pBtCoexist);
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], CoexForWifiConnect(), return for wifi is under scan<===\n"));
		return;
	}

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_AP_MODE_ENABLE, &bApEnable);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_BUSY, &bWifiBusy);
	// power save state
	if(!bApEnable && BT_8723B_1ANT_BT_STATUS_ACL_BUSY == pCoexDm->btStatus && !pBtCoexist->btLinkInfo.bHidOnly)
	{
		if(!bWifiBusy && pBtCoexist->btLinkInfo.bA2dpOnly)	//A2DP
			halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		else if ((pCoexSta->bPanExist == FALSE) && (pCoexSta->bA2dpExist == FALSE) && (pCoexSta->bHidExist == FALSE))
 			halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		else
			halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_LPS_ON, 0x50, 0x4);
	}
	else
		halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);

	// tdma and coex table
	if(!bWifiBusy)
	{
		if(BT_8723B_1ANT_BT_STATUS_ACL_BUSY == pCoexDm->btStatus)
		{
			halbtc8723b1ant_ActionWifiConnectedBtAclBusy(pBtCoexist, 
				BT_8723B_1ANT_WIFI_STATUS_CONNECTED_IDLE);
		}
		else if( (BT_8723B_1ANT_BT_STATUS_SCO_BUSY == pCoexDm->btStatus) ||
			(BT_8723B_1ANT_BT_STATUS_ACL_SCO_BUSY == pCoexDm->btStatus) )
		{
			halbtc8723b1ant_ActionBtScoHidOnlyBusy(pBtCoexist,
				BT_8723B_1ANT_WIFI_STATUS_CONNECTED_IDLE);
		}
		else
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);
			
			if ( (pCoexSta->highPriorityTx) + (pCoexSta->highPriorityRx) <= 60 )
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
			else
				halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 7); 		 
		}
	}
	else
	{
		if(BT_8723B_1ANT_BT_STATUS_ACL_BUSY == pCoexDm->btStatus)
		{
			halbtc8723b1ant_ActionWifiConnectedBtAclBusy(pBtCoexist,
				BT_8723B_1ANT_WIFI_STATUS_CONNECTED_BUSY);
		}
		else if( (BT_8723B_1ANT_BT_STATUS_SCO_BUSY == pCoexDm->btStatus) ||
			(BT_8723B_1ANT_BT_STATUS_ACL_SCO_BUSY == pCoexDm->btStatus) )
		{
			halbtc8723b1ant_ActionBtScoHidOnlyBusy(pBtCoexist,
				BT_8723B_1ANT_WIFI_STATUS_CONNECTED_BUSY);
		}
		else 
		{
			halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 8);
				
			if ( (pCoexSta->highPriorityTx) + (pCoexSta->highPriorityRx) <= 60 )
			halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
			else
				halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 7); 
		}
	}
}

VOID
halbtc8723b1ant_RunSwCoexistMechanism(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	u1Byte				algorithm=0;

	algorithm = halbtc8723b1ant_ActionAlgorithm(pBtCoexist);
	pCoexDm->curAlgorithm = algorithm;

	if(halbtc8723b1ant_IsCommonAction(pBtCoexist))
	{

	}
	else
	{
		switch(pCoexDm->curAlgorithm)
		{
			case BT_8723B_1ANT_COEX_ALGO_SCO:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = SCO.\n"));
				//halbtc8723b1ant_ActionSco(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_HID:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HID.\n"));
				//halbtc8723b1ant_ActionHid(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_A2DP:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = A2DP.\n"));
				//halbtc8723b1ant_ActionA2dp(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_A2DP_PANHS:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = A2DP+PAN(HS).\n"));
				//halbtc8723b1ant_ActionA2dpPanHs(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_PANEDR:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = PAN(EDR).\n"));
				//halbtc8723b1ant_ActionPanEdr(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_PANHS:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HS mode.\n"));
				//halbtc8723b1ant_ActionPanHs(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_PANEDR_A2DP:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = PAN+A2DP.\n"));
				//halbtc8723b1ant_ActionPanEdrA2dp(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_PANEDR_HID:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = PAN(EDR)+HID.\n"));
				//halbtc8723b1ant_ActionPanEdrHid(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_HID_A2DP_PANEDR:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HID+A2DP+PAN.\n"));
				//halbtc8723b1ant_ActionHidA2dpPanEdr(pBtCoexist);
				break;
			case BT_8723B_1ANT_COEX_ALGO_HID_A2DP:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = HID+A2DP.\n"));
				//halbtc8723b1ant_ActionHidA2dp(pBtCoexist);
				break;
			default:
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Action algorithm = coexist All Off!!\n"));
				//halbtc8723b1ant_CoexAllOff(pBtCoexist);
				break;
		}
		pCoexDm->preAlgorithm = pCoexDm->curAlgorithm;
	}
}

VOID
halbtc8723b1ant_RunCoexistMechanism(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BT_LINK_INFO pBtLinkInfo=&pBtCoexist->btLinkInfo;
	BOOLEAN	bWifiConnected=FALSE, bBtHsOn=FALSE;
	BOOLEAN	bIncreaseScanDevNum=FALSE;
	BOOLEAN	bBtCtrlAggBufSize=FALSE;
	u1Byte	aggBufSize=5;
	u1Byte	wifiRssiState=BTC_RSSI_STATE_HIGH;
	u4Byte	wifiLinkStatus=0;
	u4Byte	numOfWifiLink=0;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], RunCoexistMechanism()===>\n"));

	if(pBtCoexist->bManualControl)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], RunCoexistMechanism(), return for Manual CTRL <===\n"));
		return;
	}

	if(pBtCoexist->bStopCoexDm)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], RunCoexistMechanism(), return for Stop Coex DM <===\n"));
		return;
	}

	if(pCoexSta->bUnderIps)
	{
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], wifi is under IPS !!!\n"));
		return;
	}

	if( (BT_8723B_1ANT_BT_STATUS_ACL_BUSY == pCoexDm->btStatus) ||
		(BT_8723B_1ANT_BT_STATUS_SCO_BUSY == pCoexDm->btStatus) ||
		(BT_8723B_1ANT_BT_STATUS_ACL_SCO_BUSY == pCoexDm->btStatus) )
	{
		bIncreaseScanDevNum = TRUE;
	}

	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_INC_SCAN_DEV_NUM, &bIncreaseScanDevNum);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_LINK_STATUS, &wifiLinkStatus);
	numOfWifiLink = wifiLinkStatus>>16;
	
	if((numOfWifiLink>=2) || (wifiLinkStatus&WIFI_P2P_GO_CONNECTED))
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("############# [BTCoex],  Multi-Port numOfWifiLink = %d, wifiLinkStatus = 0x%x\n", numOfWifiLink,wifiLinkStatus) );
		halbtc8723b1ant_LimitedTx(pBtCoexist, NORMAL_EXEC, 0, 0, 0, 0);
		halbtc8723b1ant_LimitedRx(pBtCoexist, NORMAL_EXEC, FALSE, bBtCtrlAggBufSize, aggBufSize);

		if ( (pBtLinkInfo->bA2dpExist) && (pCoexSta->bC2hBtInquiryPage) )
		{
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("############# [BTCoex],  BT Is Inquirying \n") );	
			halbtc8723b1ant_ActionBtInquiry(pBtCoexist);
		}
		else
		halbtc8723b1ant_ActionWifiMultiPort(pBtCoexist);
		
		return;
	}

	if ( (pBtLinkInfo->bBtLinkExist) && (bWifiConnected) )
	{
		halbtc8723b1ant_LimitedTx(pBtCoexist, NORMAL_EXEC, 1, 1, 0, 1); 

		if(pBtLinkInfo->bScoExist)
			halbtc8723b1ant_LimitedRx(pBtCoexist, NORMAL_EXEC, FALSE, TRUE, 0x5);
		else
			halbtc8723b1ant_LimitedRx(pBtCoexist, NORMAL_EXEC, FALSE, TRUE, 0x8);

		halbtc8723b1ant_SwMechanism(pBtCoexist, TRUE);
		halbtc8723b1ant_RunSwCoexistMechanism(pBtCoexist);  //just print debug message
	}
	else
	{
		halbtc8723b1ant_LimitedTx(pBtCoexist, NORMAL_EXEC, 0, 0, 0, 0);

		halbtc8723b1ant_LimitedRx(pBtCoexist, NORMAL_EXEC, FALSE, FALSE, 0x5);

		halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
		halbtc8723b1ant_RunSwCoexistMechanism(pBtCoexist); ////just print debug message
	}
	
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);
	if(pCoexSta->bC2hBtInquiryPage)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("############# [BTCoex],  BT Is Inquirying \n") );		
		halbtc8723b1ant_ActionBtInquiry(pBtCoexist);
		return;
	}
	else if(bBtHsOn)
	{
		halbtc8723b1ant_ActionHs(pBtCoexist);
		return;
	}

	
	if(!bWifiConnected)
	{
		BOOLEAN	bScan=FALSE, bLink=FALSE, bRoam=FALSE;
		
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], wifi is non connected-idle !!!\n"));

		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_SCAN, &bScan);
		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_LINK, &bLink);
		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_ROAM, &bRoam);

		if(bScan || bLink || bRoam)
		{
			 if (bScan)	
		   		halbtc8723b1ant_ActionWifiNotConnectedScan(pBtCoexist);	
			 else
		    		halbtc8723b1ant_ActionWifiNotConnectedAssoAuth(pBtCoexist);	
		}
		else
			halbtc8723b1ant_ActionWifiNotConnected(pBtCoexist);
	}
	else	// wifi LPS/Busy
	{
		halbtc8723b1ant_ActionWifiConnected(pBtCoexist);
	}
}

VOID
halbtc8723b1ant_InitCoexDm(
	IN	PBTC_COEXIST		pBtCoexist
	)
{	
	// force to reset coex mechanism

	// sw all off
	halbtc8723b1ant_SwMechanism(pBtCoexist, FALSE);
	
	//halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, FALSE, 8);
	halbtc8723b1ant_CoexTableWithType(pBtCoexist, FORCE_EXEC, 0);

	pCoexSta->popEventCnt = 0;
}

VOID
halbtc8723b1ant_InitHwConfig(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bBackUp,
	IN	BOOLEAN				bWifiOnly
	)
{
	PBTC_BOARD_INFO		pBoardInfo=&pBtCoexist->boardInfo;
	u4Byte				u4Tmp=0;//, fwVer;
	u2Byte				u2Tmp=0;
	u1Byte				u1Tmp=0, u1Tmpa=0, u1Tmpb=0;
	u1Byte				H2C_Parameter[2] ={0};

	BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], 1Ant Init HW Config!!\n"));
#if 0//move to BTC_MEDIA_CONNECT
	if(bBackUp)
	{
		pCoexDm->backupArfrCnt1 = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x430);
		pCoexDm->backupArfrCnt2 = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x434);
		pCoexDm->backupRetryLimit = pBtCoexist->fBtcRead2Byte(pBtCoexist, 0x42a);
		pCoexDm->backupAmpduMaxTime = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x456);
	}
#endif
	pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x550, 0x8, 0x1);  //enable TBTT nterrupt

	// 0x790[5:0]=0x5	
	pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x790, 0x5);
	
	// Enable counter statistics
	//pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x76e, 0xc); //0x76e[3] =1, WLAN_Act control by PTA
	pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x778, 0x1);
	pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x40, 0x20, 0x1);


	//pBtCoexist->fBtcWrite1ByteBitMask(pBtCoexist, 0x67, 0x20, 0x1); //BT select s0/s1 is controlled by WiFi

	//Antenna config
#if 1
	if(bWifiOnly)
	{
		halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_WIFI, TRUE, FALSE);
		halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, FALSE, 9);
	}
	else
		halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_BT, TRUE, FALSE);
#endif	


#if 0
	if(bWifiOnly)
	{
		halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_WIFI, BTC_WIFI_STAT_INIT);
		halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, FALSE, 9);
	}
	else
		halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_BT, BTC_WIFI_STAT_INIT);		
#endif



	// PTA parameter
	halbtc8723b1ant_CoexTableWithType(pBtCoexist, FORCE_EXEC, 0);

	u4Tmp = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x948);
	u1Tmpa = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x765);
	u1Tmpb = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x67);

	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("############# [BTCoex], 0x948=0x%x, 0x765=0x%x, 0x67=0x%x\n",
			u4Tmp,  u1Tmpa, u1Tmpb));
}

/*
VOID
halbtc8723b1ant_WifiOffHwCfg(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	// set wlan_act to low
	//pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x76e, 0x4);
}
*/

//============================================================
// work around function start with wa_halbtc8723b1ant_
//============================================================
//============================================================
// extern function start with EXhalbtc8723b1ant_
//============================================================
VOID
EXhalbtc8723b1ant_PowerOnSetting(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BOARD_INFO 	pBoardInfo=&pBtCoexist->boardInfo;
	u1Byte u1Tmp=0x0;
	u2Byte u2Tmp=0x0;

	pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x67, 0x20);

	// enable BB, REG_SYS_FUNC_EN such that we can write 0x948 correctly.
	u2Tmp = pBtCoexist->fBtcRead2Byte(pBtCoexist, 0x2);
	pBtCoexist->fBtcWrite2Byte(pBtCoexist, 0x2, u2Tmp|BIT0|BIT1);

	// set GRAN_BT = 1
	pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x765, 0x18);
	// set WLAN_ACT = 0
	pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x76e, 0x4);	

	// 
	// S0 or S1 setting and Local register setting(By the setting fw can get ant number, S0/S1, ... info)
	// Local setting bit define
	//	BIT0: "0" for no antenna inverse; "1" for antenna inverse 
	//	BIT1: "0" for internal switch; "1" for external switch
	//	BIT2: "0" for one antenna; "1" for two antenna
	// NOTE: here default all internal switch and 1-antenna ==> BIT1=0 and BIT2=0
	if(pBtCoexist->chipInterface == BTC_INTF_USB)
	{
		// fixed at S0 for USB interface
		pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x0);

	 	u1Tmp |= 0x1;	// antenna inverse
		pBtCoexist->fBtcWriteLocalReg1Byte(pBtCoexist, 0xfe08, u1Tmp);

		pBoardInfo->btdmAntPos = BTC_ANTENNA_AT_AUX_PORT;
	}
	else
	{
		// for PCIE and SDIO interface, we check efuse 0xc3[6]
		if(pBoardInfo->singleAntPath == 0)
		{
			// set to S1
			pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x280);
			pBoardInfo->btdmAntPos = BTC_ANTENNA_AT_MAIN_PORT;
		}
		else if(pBoardInfo->singleAntPath == 1)
		{
			// set to S0
			pBtCoexist->fBtcWrite4Byte(pBtCoexist, 0x948, 0x0);
			u1Tmp |= 0x1;	// antenna inverse
			pBoardInfo->btdmAntPos = BTC_ANTENNA_AT_AUX_PORT;
		}

		if(pBtCoexist->chipInterface == BTC_INTF_PCI)
		{	
			pBtCoexist->fBtcWriteLocalReg1Byte(pBtCoexist, 0x384, u1Tmp);
		}
		else if(pBtCoexist->chipInterface == BTC_INTF_SDIO)
		{
			pBtCoexist->fBtcWriteLocalReg1Byte(pBtCoexist, 0x60, u1Tmp);
		}			
	}
}

VOID
EXhalbtc8723b1ant_InitHwConfig(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	BOOLEAN				bWifiOnly
	)
{
	halbtc8723b1ant_InitHwConfig(pBtCoexist, TRUE, bWifiOnly);
}

VOID
EXhalbtc8723b1ant_InitCoexDm(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], Coex Mechanism Init!!\n"));

	pBtCoexist->bStopCoexDm = FALSE;
	
	halbtc8723b1ant_InitCoexDm(pBtCoexist);

	halbtc8723b1ant_QueryBtInfo(pBtCoexist);
}

VOID
EXhalbtc8723b1ant_DisplayCoexInfo(
	IN	PBTC_COEXIST		pBtCoexist
	)
{
	PBTC_BOARD_INFO		pBoardInfo=&pBtCoexist->boardInfo;
	PBTC_STACK_INFO		pStackInfo=&pBtCoexist->stackInfo;
	PBTC_BT_LINK_INFO	pBtLinkInfo=&pBtCoexist->btLinkInfo;
	pu1Byte				cliBuf=pBtCoexist->cliBuf;
	u1Byte				u1Tmp[4], i, btInfoExt, psTdmaCase=0;
	u2Byte				u2Tmp[4];
	u4Byte				u4Tmp[4];
	BOOLEAN				bRoam=FALSE, bScan=FALSE, bLink=FALSE, bWifiUnder5G=FALSE, bWifiUnderBMode = FALSE;
	BOOLEAN				bBtHsOn=FALSE, bWifiBusy=FALSE;
	s4Byte				wifiRssi=0, btHsRssi=0;
	u4Byte				wifiBw, wifiTrafficDir, faOfdm, faCck, wifiLinkStatus;
	u1Byte				wifiDot11Chnl, wifiHsChnl;
	u4Byte				fwVer=0, btPatchVer=0;
	static u1Byte			PopReportIn10s = 0;	

	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n ============[BT Coexist info]============");
	CL_PRINTF(cliBuf);

	if(pBtCoexist->bManualControl)
	{
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n ============[Under Manual Control]============");
		CL_PRINTF(cliBuf);
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n ==========================================");
		CL_PRINTF(cliBuf);
	}
	if(pBtCoexist->bStopCoexDm)
	{
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n ============[Coex is STOPPED]============");
		CL_PRINTF(cliBuf);
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n ==========================================");
		CL_PRINTF(cliBuf);
	}

	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d", "Ant PG Num/ Ant Mech/ Ant Pos:", \
		pBoardInfo->pgAntNum, pBoardInfo->btdmAntNum, pBoardInfo->btdmAntPos);
	CL_PRINTF(cliBuf);	
	
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s / %d", "BT stack/ hci ext ver", \
		((pStackInfo->bProfileNotified)? "Yes":"No"), pStackInfo->hciVersion);
	CL_PRINTF(cliBuf);

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_BT_PATCH_VER, &btPatchVer);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_FW_VER, &fwVer);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d_%x/ 0x%x/ 0x%x(%d)", "CoexVer/ FwVer/ PatchVer", \
		GLCoexVerDate8723b1Ant, GLCoexVer8723b1Ant, fwVer, btPatchVer, btPatchVer);
	CL_PRINTF(cliBuf);

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U1_WIFI_DOT11_CHNL, &wifiDot11Chnl);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U1_WIFI_HS_CHNL, &wifiHsChnl);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d / %d(%d)", "Dot11 channel / HsChnl(HsMode)", \
		wifiDot11Chnl, wifiHsChnl, bBtHsOn);
	CL_PRINTF(cliBuf);

	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %02x %02x %02x ", "H2C Wifi inform bt chnl Info", \
		pCoexDm->wifiChnlInfo[0], pCoexDm->wifiChnlInfo[1],
		pCoexDm->wifiChnlInfo[2]);
	CL_PRINTF(cliBuf);

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_S4_WIFI_RSSI, &wifiRssi);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_S4_HS_RSSI, &btHsRssi);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d", "Wifi rssi/ HS rssi", \
		wifiRssi, btHsRssi);
	CL_PRINTF(cliBuf);

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_SCAN, &bScan);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_LINK, &bLink);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_ROAM, &bRoam);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %s", "Wifi bLink/ bRoam/ bScan/ bHi-Pri", \
		bLink, bRoam, bScan,((pCoexSta->bWiFiIsHighPriTask)? "1":"0"));
	CL_PRINTF(cliBuf);

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_UNDER_5G, &bWifiUnder5G);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_BW, &wifiBw);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_BUSY, &bWifiBusy);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_TRAFFIC_DIRECTION, &wifiTrafficDir);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_UNDER_B_MODE, &bWifiUnderBMode);
	
	
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s / %s/ %s ", "Wifi status", \
		(bWifiUnder5G? "5G":"2.4G"),
		((bWifiUnderBMode)? "11b": ((BTC_WIFI_BW_LEGACY==wifiBw)? "11bg": (((BTC_WIFI_BW_HT40==wifiBw)? "HT40":"HT20")))),
		((!bWifiBusy)? "idle": ((BTC_WIFI_TRAFFIC_TX==wifiTrafficDir)? "uplink":"downlink")));
	CL_PRINTF(cliBuf);

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_LINK_STATUS, &wifiLinkStatus);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d/ %d/ %d/ %d", "sta/vwifi/hs/p2pGo/p2pGc", \
		((wifiLinkStatus&WIFI_STA_CONNECTED)? 1:0), ((wifiLinkStatus&WIFI_AP_CONNECTED)? 1:0), 
		((wifiLinkStatus&WIFI_HS_CONNECTED)? 1:0), ((wifiLinkStatus&WIFI_P2P_GO_CONNECTED)? 1:0), 
		((wifiLinkStatus&WIFI_P2P_GC_CONNECTED)? 1:0) );
	CL_PRINTF(cliBuf);
	

	PopReportIn10s++;
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = [%s/ %d/ %d/ %d] ", "BT [status/ rssi/ retryCnt/ popCnt]", \
		((pBtCoexist->btInfo.bBtDisabled)? ("disabled"):	((pCoexSta->bC2hBtInquiryPage)?("inquiry/page scan"):((BT_8723B_1ANT_BT_STATUS_NON_CONNECTED_IDLE == pCoexDm->btStatus)? "non-connected idle":
		(  (BT_8723B_1ANT_BT_STATUS_CONNECTED_IDLE == pCoexDm->btStatus)? "connected-idle":"busy")))),
		pCoexSta->btRssi, pCoexSta->btRetryCnt, pCoexSta->popEventCnt);
	CL_PRINTF(cliBuf);
	
	if (PopReportIn10s >= 5)
	{
		pCoexSta->popEventCnt = 0;	
		PopReportIn10s = 0;
	}
	
				
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d / %d / %d / %d", "SCO/HID/PAN/A2DP", \
		pBtLinkInfo->bScoExist, pBtLinkInfo->bHidExist, pBtLinkInfo->bPanExist, pBtLinkInfo->bA2dpExist);
	CL_PRINTF(cliBuf);
	pBtCoexist->fBtcDispDbgMsg(pBtCoexist, BTC_DBG_DISP_BT_LINK_INFO);	

	btInfoExt = pCoexSta->btInfoExt;
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s", "BT Info A2DP rate", \
		(btInfoExt&BIT0)? "Basic rate":"EDR rate");
	CL_PRINTF(cliBuf);	

	for(i=0; i<BT_INFO_SRC_8723B_1ANT_MAX; i++)
	{
		if(pCoexSta->btInfoC2hCnt[i])
		{				
			CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %02x %02x %02x %02x %02x %02x %02x(%d)", GLBtInfoSrc8723b1Ant[i], \
				pCoexSta->btInfoC2h[i][0], pCoexSta->btInfoC2h[i][1],
				pCoexSta->btInfoC2h[i][2], pCoexSta->btInfoC2h[i][3],
				pCoexSta->btInfoC2h[i][4], pCoexSta->btInfoC2h[i][5],
				pCoexSta->btInfoC2h[i][6], pCoexSta->btInfoC2hCnt[i]);
			CL_PRINTF(cliBuf);
		}
	}
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/%s, (0x%x/0x%x)", "PS state, IPS/LPS, (lps/rpwm)", \
		((pCoexSta->bUnderIps? "IPS ON":"IPS OFF")),
		((pCoexSta->bUnderLps? "LPS ON":"LPS OFF")), 
		pBtCoexist->btInfo.lpsVal, 
		pBtCoexist->btInfo.rpwmVal);
	CL_PRINTF(cliBuf);
	pBtCoexist->fBtcDispDbgMsg(pBtCoexist, BTC_DBG_DISP_FW_PWR_MODE_CMD);

	if(!pBtCoexist->bManualControl)
	{
		// Sw mechanism	
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s", "============[Sw mechanism]============");
		CL_PRINTF(cliBuf);
	
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d", "SM[LowPenaltyRA]", \
			pCoexDm->bCurLowPenaltyRa);
		CL_PRINTF(cliBuf);

		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %s/ %s/ %d ", "DelBA/ BtCtrlAgg/ AggSize", \
			(pBtCoexist->btInfo.bRejectAggPkt? "Yes":"No"), (pBtCoexist->btInfo.bBtCtrlAggBufSize? "Yes":"No"),
				pBtCoexist->btInfo.aggBufSize);
		CL_PRINTF(cliBuf);
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x ", "Rate Mask", \
				pBtCoexist->btInfo.raMask);
		CL_PRINTF(cliBuf);

		// Fw mechanism		
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s", "============[Fw mechanism]============");
		CL_PRINTF(cliBuf);	

		psTdmaCase = pCoexDm->curPsTdma;
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %02x %02x %02x %02x %02x case-%d (auto:%d)", "PS TDMA", \
			pCoexDm->psTdmaPara[0], pCoexDm->psTdmaPara[1],
			pCoexDm->psTdmaPara[2], pCoexDm->psTdmaPara[3],
			pCoexDm->psTdmaPara[4], psTdmaCase, pCoexDm->bAutoTdmaAdjust);
		CL_PRINTF(cliBuf);
		
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d", "IgnWlanAct", \
			pCoexDm->bCurIgnoreWlanAct);
		CL_PRINTF(cliBuf);
	
		CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x ", "Latest error condition(should be 0)", \
			pCoexDm->errorCondition);
		CL_PRINTF(cliBuf);
	}

	// Hw setting		
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s", "============[Hw setting]============");
	CL_PRINTF(cliBuf);	

	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/0x%x/0x%x/0x%x", "backup ARFR1/ARFR2/RL/AMaxTime", \
		pCoexDm->backupArfrCnt1, pCoexDm->backupArfrCnt2, pCoexDm->backupRetryLimit, pCoexDm->backupAmpduMaxTime);
	CL_PRINTF(cliBuf);

	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x430);
	u4Tmp[1] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x434);
	u2Tmp[0] = pBtCoexist->fBtcRead2Byte(pBtCoexist, 0x42a);
	u1Tmp[0] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x456);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/0x%x/0x%x/0x%x", "0x430/0x434/0x42a/0x456", \
		u4Tmp[0], u4Tmp[1], u2Tmp[0], u1Tmp[0]);
	CL_PRINTF(cliBuf);

	u1Tmp[0] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x778);
	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x6cc);
	u4Tmp[1] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x880);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x", "0x778/0x6cc/0x880[29:25]", \
		u1Tmp[0], u4Tmp[0],  (u4Tmp[1]&0x3e000000) >> 25);
	CL_PRINTF(cliBuf);

	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x948);
	u1Tmp[0] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x67);
	u4Tmp[1] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x764);
	u1Tmp[1] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x76e);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x", "0x948/ 0x67[5] / 0x764 / 0x76e", \
		u4Tmp[0], ((u1Tmp[0]&0x20)>> 5), (u4Tmp[1] & 0xffff), u1Tmp[1]);
	CL_PRINTF(cliBuf);

	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x92c);
	u4Tmp[1] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x930);
	u4Tmp[2] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x944);	
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x", "0x92c[1:0]/ 0x930[7:0]/0x944[1:0]", \
		u4Tmp[0]&0x3, u4Tmp[1]&0xff, u4Tmp[2]&0x3);
	CL_PRINTF(cliBuf);

	u1Tmp[0] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x39);
	u1Tmp[1] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x40);
	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x4c);
	u1Tmp[2] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x64);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x/ 0x%x", "0x38[11]/0x40/0x4c[24:23]/0x64[0]", \
		((u1Tmp[0] & 0x8)>>3), u1Tmp[1], ((u4Tmp[0]&0x01800000)>>23), u1Tmp[2]&0x1);
	CL_PRINTF(cliBuf);

	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x550);
	u1Tmp[0] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x522);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x", "0x550(bcn ctrl)/0x522", \
		u4Tmp[0], u1Tmp[0]);
	CL_PRINTF(cliBuf);

	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0xc50);
	u1Tmp[0] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x49c);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x", "0xc50(dig)/0x49c(null-drop)", \
		u4Tmp[0]&0xff, u1Tmp[0]);
	CL_PRINTF(cliBuf);

	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0xda0);
	u4Tmp[1] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0xda4);
	u4Tmp[2] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0xda8);
	u4Tmp[3] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0xcf0);

	u1Tmp[0] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0xa5b);
	u1Tmp[1] = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0xa5c);

	faOfdm = ((u4Tmp[0]&0xffff0000) >> 16) +  ((u4Tmp[1]&0xffff0000) >> 16) + (u4Tmp[1] & 0xffff) +  (u4Tmp[2] & 0xffff) + \
		             ((u4Tmp[3]&0xffff0000) >> 16) + (u4Tmp[3] & 0xffff) ;
	faCck = (u1Tmp[0] << 8) + u1Tmp[1];
	
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x", "OFDM-CCA/OFDM-FA/CCK-FA", \
		u4Tmp[0]&0xffff, faOfdm, faCck);
	CL_PRINTF(cliBuf);

	u4Tmp[0] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x6c0);
	u4Tmp[1] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x6c4);
	u4Tmp[2] = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x6c8);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = 0x%x/ 0x%x/ 0x%x", "0x6c0/0x6c4/0x6c8(coexTable)", \
		u4Tmp[0], u4Tmp[1], u4Tmp[2]);
	CL_PRINTF(cliBuf);

	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d", "0x770(high-pri rx/tx)", \
		pCoexSta->highPriorityRx, pCoexSta->highPriorityTx);
	CL_PRINTF(cliBuf);
	CL_SPRINTF(cliBuf, BT_TMP_BUF_SIZE, "\r\n %-35s = %d/ %d", "0x774(low-pri rx/tx)", \
		pCoexSta->lowPriorityRx, pCoexSta->lowPriorityTx);
	CL_PRINTF(cliBuf);
#if(BT_AUTO_REPORT_ONLY_8723B_1ANT == 1)
	//halbtc8723b1ant_MonitorBtCtr(pBtCoexist);
#endif
	pBtCoexist->fBtcDispDbgMsg(pBtCoexist, BTC_DBG_DISP_COEX_STATISTICS);
}


VOID
EXhalbtc8723b1ant_IpsNotify(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte			type
	)
{
	u4Byte	u4Tmp=0;

	if(pBtCoexist->bManualControl ||	pBtCoexist->bStopCoexDm)
		return;

	if(BTC_IPS_ENTER == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], IPS ENTER notify\n"));
		pCoexSta->bUnderIps = TRUE;
		
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 0);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 0);
		halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_BT, FALSE, TRUE);	
		//halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_BT, BTC_WIFI_STAT_NORMAL_OFF);
	}
	else if(BTC_IPS_LEAVE == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], IPS LEAVE notify\n"));
		pCoexSta->bUnderIps = FALSE;

		halbtc8723b1ant_InitHwConfig(pBtCoexist, FALSE, FALSE);
		halbtc8723b1ant_InitCoexDm(pBtCoexist);
		halbtc8723b1ant_QueryBtInfo(pBtCoexist);
	}
}

VOID
EXhalbtc8723b1ant_LpsNotify(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte			type
	)
{
	if(pBtCoexist->bManualControl || pBtCoexist->bStopCoexDm)
		return;

	if(BTC_LPS_ENABLE == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], LPS ENABLE notify\n"));
		pCoexSta->bUnderLps = TRUE;
	}
	else if(BTC_LPS_DISABLE == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], LPS DISABLE notify\n"));
		pCoexSta->bUnderLps = FALSE;
	}
}

VOID
EXhalbtc8723b1ant_ScanNotify(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte			type
	)
{
	BOOLEAN bWifiConnected=FALSE, bBtHsOn=FALSE;	
	u4Byte	wifiLinkStatus=0;
	u4Byte	numOfWifiLink=0;
	BOOLEAN	bBtCtrlAggBufSize=FALSE;
	u1Byte	aggBufSize=5;

	u1Byte u1Tmpa, u1Tmpb;
	u4Byte u4Tmp;

	if(pBtCoexist->bManualControl ||
		pBtCoexist->bStopCoexDm )
		return;

	if(BTC_SCAN_START == type)
	{
		pCoexSta->bWiFiIsHighPriTask = TRUE;
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], SCAN START notify\n"));

		halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, FALSE, 8);  //Force antenna setup for no scan result issue
		u4Tmp = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x948);
		u1Tmpa = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x765);
		u1Tmpb = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x67);
          
	   
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], 0x948=0x%x, 0x765=0x%x, 0x67=0x%x\n",
			u4Tmp,  u1Tmpa, u1Tmpb));
	}
	else
	{
		pCoexSta->bWiFiIsHighPriTask = FALSE;	
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], SCAN FINISH notify\n"));
	}

	if(pBtCoexist->btInfo.bBtDisabled)
		return;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);

	halbtc8723b1ant_QueryBtInfo(pBtCoexist);
	
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_LINK_STATUS, &wifiLinkStatus);
	numOfWifiLink = wifiLinkStatus>>16;
	if(numOfWifiLink >= 2)
	{
		halbtc8723b1ant_LimitedTx(pBtCoexist, NORMAL_EXEC, 0, 0, 0, 0);
		halbtc8723b1ant_LimitedRx(pBtCoexist, NORMAL_EXEC, FALSE, bBtCtrlAggBufSize, aggBufSize);
		halbtc8723b1ant_ActionWifiMultiPort(pBtCoexist);
		return;
	}
	
	if(pCoexSta->bC2hBtInquiryPage)
	{
		halbtc8723b1ant_ActionBtInquiry(pBtCoexist);
		return;
	}
	else if(bBtHsOn)
	{
		halbtc8723b1ant_ActionHs(pBtCoexist);
		return;
	}

	if(BTC_SCAN_START == type)
	{	
		//BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], SCAN START notify\n"));
		if(!bWifiConnected)	// non-connected scan
		{
			halbtc8723b1ant_ActionWifiNotConnectedScan(pBtCoexist);
		}
		else	// wifi is connected
		{
			halbtc8723b1ant_ActionWifiConnectedScan(pBtCoexist);
		}
	}
	else if(BTC_SCAN_FINISH == type)
	{
		//BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], SCAN FINISH notify\n"));
		if(!bWifiConnected)	// non-connected scan
		{
			halbtc8723b1ant_ActionWifiNotConnected(pBtCoexist);
		}
		else
		{
			halbtc8723b1ant_ActionWifiConnected(pBtCoexist);
		}
	}
}

VOID
EXhalbtc8723b1ant_ConnectNotify(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	u1Byte			type
	)
{
	BOOLEAN	bWifiConnected=FALSE, bBtHsOn=FALSE;	
	u4Byte	wifiLinkStatus=0;
	u4Byte	numOfWifiLink=0;
	BOOLEAN	bBtCtrlAggBufSize=FALSE;
	u1Byte	aggBufSize=5;

	if(pBtCoexist->bManualControl ||
		pBtCoexist->bStopCoexDm ||
		pBtCoexist->btInfo.bBtDisabled )
		return;

	if(BTC_ASSOCIATE_START == type)
	{
		pCoexSta->bWiFiIsHighPriTask = TRUE;
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], CONNECT START notify\n"));	
		 pCoexDm->nArpCnt = 0;
	}
	else
	{
		pCoexSta->bWiFiIsHighPriTask = FALSE;	
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], CONNECT FINISH notify\n"));	
		//pCoexDm->nArpCnt = 0;
	}

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_LINK_STATUS, &wifiLinkStatus);
	numOfWifiLink = wifiLinkStatus>>16;
	if(numOfWifiLink >= 2)
	{
		halbtc8723b1ant_LimitedTx(pBtCoexist, NORMAL_EXEC, 0, 0, 0, 0);
		halbtc8723b1ant_LimitedRx(pBtCoexist, NORMAL_EXEC, FALSE, bBtCtrlAggBufSize, aggBufSize);
		halbtc8723b1ant_ActionWifiMultiPort(pBtCoexist);
		return;
	}

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);
	if(pCoexSta->bC2hBtInquiryPage)
	{
		halbtc8723b1ant_ActionBtInquiry(pBtCoexist);
		return;
	}
	else if(bBtHsOn)
	{
		halbtc8723b1ant_ActionHs(pBtCoexist);
		return;
	}

	if(BTC_ASSOCIATE_START == type)
	{
		//BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], CONNECT START notify\n"));		
		halbtc8723b1ant_ActionWifiNotConnectedAssoAuth(pBtCoexist);
	}
	else if(BTC_ASSOCIATE_FINISH == type)
	{
		//BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], CONNECT FINISH notify\n"));
		
		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);
		if(!bWifiConnected) // non-connected scan
		{
			halbtc8723b1ant_ActionWifiNotConnected(pBtCoexist);
		}
		else
		{
			halbtc8723b1ant_ActionWifiConnected(pBtCoexist);
		}
	}
}

VOID
EXhalbtc8723b1ant_MediaStatusNotify(
	IN	PBTC_COEXIST			pBtCoexist,
	IN	u1Byte				type
	)
{
	u1Byte			H2C_Parameter[3] ={0};
	u4Byte			wifiBw;
	u1Byte			wifiCentralChnl;
	BOOLEAN			bWifiUnderBMode = FALSE;

	if(pBtCoexist->bManualControl ||
		pBtCoexist->bStopCoexDm ||
		pBtCoexist->btInfo.bBtDisabled )
		return;

	if(BTC_MEDIA_CONNECT == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], MEDIA connect notify\n"));

		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_UNDER_B_MODE, &bWifiUnderBMode);

		//Set CCK Tx/Rx high Pri except 11b mode
		if (bWifiUnderBMode)
		{
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x6cd, 0x00); //CCK Tx
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x6cf, 0x00); //CCK Rx
		}
		else
		{
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x6cd, 0x10); //CCK Tx
			pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x6cf, 0x10); //CCK Rx
		}
		
		pCoexDm->backupArfrCnt1 = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x430);
		pCoexDm->backupArfrCnt2 = pBtCoexist->fBtcRead4Byte(pBtCoexist, 0x434);
		pCoexDm->backupRetryLimit = pBtCoexist->fBtcRead2Byte(pBtCoexist, 0x42a);
		pCoexDm->backupAmpduMaxTime = pBtCoexist->fBtcRead1Byte(pBtCoexist, 0x456);
	}
	else
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], MEDIA disconnect notify\n"));
		pCoexDm->nArpCnt = 0;

		pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x6cd, 0x0); //CCK Tx
		pBtCoexist->fBtcWrite1Byte(pBtCoexist, 0x6cf, 0x0); //CCK Rx
	}

	// only 2.4G we need to inform bt the chnl mask
	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U1_WIFI_CENTRAL_CHNL, &wifiCentralChnl);
	if( (BTC_MEDIA_CONNECT == type) &&
		(wifiCentralChnl <= 14) )
	{
		//H2C_Parameter[0] = 0x1;
		H2C_Parameter[0] = 0x0;
		H2C_Parameter[1] = wifiCentralChnl;
		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_BW, &wifiBw);
		if(BTC_WIFI_BW_HT40 == wifiBw)
			H2C_Parameter[2] = 0x30;
		else
			H2C_Parameter[2] = 0x20;
	}
		
	pCoexDm->wifiChnlInfo[0] = H2C_Parameter[0];
	pCoexDm->wifiChnlInfo[1] = H2C_Parameter[1];
	pCoexDm->wifiChnlInfo[2] = H2C_Parameter[2];
	
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE_FW_EXEC, ("[BTCoex], FW write 0x66=0x%x\n", 
		H2C_Parameter[0]<<16|H2C_Parameter[1]<<8|H2C_Parameter[2]));

	pBtCoexist->fBtcFillH2c(pBtCoexist, 0x66, 3, H2C_Parameter);
}

VOID
EXhalbtc8723b1ant_SpecialPacketNotify(
	IN	PBTC_COEXIST			pBtCoexist,
	IN	u1Byte				type
	)
{
	BOOLEAN	bBtHsOn=FALSE;
	u4Byte	wifiLinkStatus=0;
	u4Byte	numOfWifiLink=0;
	BOOLEAN	bBtCtrlAggBufSize=FALSE;
	u1Byte	aggBufSize=5;
	
	if(pBtCoexist->bManualControl ||
		pBtCoexist->bStopCoexDm ||
		pBtCoexist->btInfo.bBtDisabled )
		return;

	if( BTC_PACKET_DHCP == type || 
		BTC_PACKET_EAPOL == type ||
		BTC_PACKET_ARP == type )
	{
		if (BTC_PACKET_ARP == type)
		{
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], special Packet ARP notify\n"));		 

			pCoexDm->nArpCnt++;
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], ARP Packet Count = %d\n", pCoexDm->nArpCnt));
			
			if(pCoexDm->nArpCnt >= 10) // if APR PKT > 10 after connect, do not go to ActionWifiConnectedSpecialPacket(pBtCoexist) 
			{
				pCoexSta->bWiFiIsHighPriTask = FALSE;					
			}			
			else
			{
				pCoexSta->bWiFiIsHighPriTask = TRUE;
			}
		}
		else
		{
			pCoexSta->bWiFiIsHighPriTask = TRUE;
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], special Packet DHCP or EAPOL notify\n"));		 
		}
	} 
	else
	{
		pCoexSta->bWiFiIsHighPriTask = FALSE;
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], special Packet [Type = %d] notify\n", type));
	}

	pCoexSta->specialPktPeriodCnt = 0;

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_LINK_STATUS, &wifiLinkStatus);
	numOfWifiLink = wifiLinkStatus>>16;
	if(numOfWifiLink >= 2)
	{
		halbtc8723b1ant_LimitedTx(pBtCoexist, NORMAL_EXEC, 0, 0, 0, 0);
		halbtc8723b1ant_LimitedRx(pBtCoexist, NORMAL_EXEC, FALSE, bBtCtrlAggBufSize, aggBufSize);
		halbtc8723b1ant_ActionWifiMultiPort(pBtCoexist);
		return;
	}

	pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_HS_OPERATION, &bBtHsOn);
	if(pCoexSta->bC2hBtInquiryPage)
	{
		halbtc8723b1ant_ActionBtInquiry(pBtCoexist);
		return;
	}
	else if(bBtHsOn)
	{
		halbtc8723b1ant_ActionHs(pBtCoexist);
		return;
	}

	if( BTC_PACKET_DHCP == type ||
		BTC_PACKET_EAPOL == type || 
		( (BTC_PACKET_ARP == type ) && (pCoexSta->bWiFiIsHighPriTask) ) )
		{
		halbtc8723b1ant_ActionWifiConnectedSpecialPacket(pBtCoexist);
	}
}

VOID
EXhalbtc8723b1ant_BtInfoNotify(
	IN	PBTC_COEXIST		pBtCoexist,
	IN	pu1Byte			tmpBuf,
	IN	u1Byte			length
	)
{
	PBTC_BT_LINK_INFO	pBtLinkInfo=&pBtCoexist->btLinkInfo;
	u1Byte				btInfo=0;
	u1Byte				i, rspSource=0;
	BOOLEAN				bWifiConnected=FALSE;
	BOOLEAN				bBtBusy=FALSE;
	
	pCoexSta->bC2hBtInfoReqSent = FALSE;

	rspSource = tmpBuf[0]&0xf;
	if(rspSource >= BT_INFO_SRC_8723B_1ANT_MAX)
		rspSource = BT_INFO_SRC_8723B_1ANT_WIFI_FW;
	pCoexSta->btInfoC2hCnt[rspSource]++;

	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], Bt info[%d], length=%d, hex data=[", rspSource, length));
	for(i=0; i<length; i++)
	{
		pCoexSta->btInfoC2h[rspSource][i] = tmpBuf[i];
		if(i == 1)
			btInfo = tmpBuf[i];
		if(i == length-1)
		{
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("0x%02x]\n", tmpBuf[i]));
		}
		else
		{
			BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("0x%02x, ", tmpBuf[i]));
		}
	}

	if(BT_INFO_SRC_8723B_1ANT_WIFI_FW != rspSource)
	{
		pCoexSta->btRetryCnt =	// [3:0]
			pCoexSta->btInfoC2h[rspSource][2]&0xf;

		if (pCoexSta->btRetryCnt >= 1)
			pCoexSta->popEventCnt++;

		if (pCoexSta->btInfoC2h[rspSource][2]&0x20)
			pCoexSta->bC2hBtPage = TRUE;
		else
			pCoexSta->bC2hBtPage = FALSE;			

		pCoexSta->btRssi =
			pCoexSta->btInfoC2h[rspSource][3]*2+10;

		pCoexSta->btInfoExt = 
			pCoexSta->btInfoC2h[rspSource][4];
		
		pCoexSta->bBtTxRxMask = (pCoexSta->btInfoC2h[rspSource][2]&0x40);
		pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_BT_TX_RX_MASK, &pCoexSta->bBtTxRxMask);
		if(!pCoexSta->bBtTxRxMask)
		{
			/* BT into is responded by BT FW and BT RF REG 0x3C != 0x15 => Need to switch BT TRx Mask */				
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], Switch BT TRx Mask since BT RF REG 0x3C != 0x15\n"));
			pBtCoexist->fBtcSetBtReg(pBtCoexist, BTC_BT_REG_RF, 0x3c, 0x15);
		}
		
		// Here we need to resend some wifi info to BT
		// because bt is reset and loss of the info.
		if(pCoexSta->btInfoExt & BIT1)
		{			
			BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT ext info bit1 check, send wifi BW&Chnl to BT!!\n"));
			pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_BL_WIFI_CONNECTED, &bWifiConnected);
			if(bWifiConnected)
			{
				EXhalbtc8723b1ant_MediaStatusNotify(pBtCoexist, BTC_MEDIA_CONNECT);
			}
			else
			{
				EXhalbtc8723b1ant_MediaStatusNotify(pBtCoexist, BTC_MEDIA_DISCONNECT);
			}
		}
				
		if(pCoexSta->btInfoExt & BIT3)
		{
			if(!pBtCoexist->bManualControl && !pBtCoexist->bStopCoexDm)
			{
				BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BT ext info bit3 check, set BT NOT to ignore Wlan active!!\n"));
				halbtc8723b1ant_IgnoreWlanAct(pBtCoexist, FORCE_EXEC, FALSE);
			}
		}
		else
		{
			// BT already NOT ignore Wlan active, do nothing here.
		}
#if(BT_AUTO_REPORT_ONLY_8723B_1ANT == 0)
		if( (pCoexSta->btInfoExt & BIT4) )
		{
			// BT auto report already enabled, do nothing
		}
		else
		{
			halbtc8723b1ant_BtAutoReport(pBtCoexist, FORCE_EXEC, TRUE);
		}
#endif
	}
		
	// check BIT2 first ==> check if bt is under inquiry or page scan
	if(btInfo & BT_INFO_8723B_1ANT_B_INQ_PAGE)
		pCoexSta->bC2hBtInquiryPage = TRUE;
	else
		pCoexSta->bC2hBtInquiryPage = FALSE;

	// set link exist status
	if(!(btInfo&BT_INFO_8723B_1ANT_B_CONNECTION))
	{
		pCoexSta->bBtLinkExist = FALSE;
		pCoexSta->bPanExist = FALSE;
		pCoexSta->bA2dpExist = FALSE;
		pCoexSta->bHidExist = FALSE;
		pCoexSta->bScoExist = FALSE;
	}
	else	// connection exists
	{
		pCoexSta->bBtLinkExist = TRUE;
		if(btInfo & BT_INFO_8723B_1ANT_B_FTP)
			pCoexSta->bPanExist = TRUE;
		else
			pCoexSta->bPanExist = FALSE;
		if(btInfo & BT_INFO_8723B_1ANT_B_A2DP)
			pCoexSta->bA2dpExist = TRUE;
		else
			pCoexSta->bA2dpExist = FALSE;
		if(btInfo & BT_INFO_8723B_1ANT_B_HID)
			pCoexSta->bHidExist = TRUE;
		else
			pCoexSta->bHidExist = FALSE;
		if(btInfo & BT_INFO_8723B_1ANT_B_SCO_ESCO)
			pCoexSta->bScoExist = TRUE;
		else
			pCoexSta->bScoExist = FALSE;
	}

	halbtc8723b1ant_UpdateBtLinkInfo(pBtCoexist);
	
	btInfo = btInfo & 0x1f;  //mask profile bit for connect-ilde identification ( for CSR case: A2DP idle --> 0x41)
	
	if(!(btInfo&BT_INFO_8723B_1ANT_B_CONNECTION))
	{
		pCoexDm->btStatus = BT_8723B_1ANT_BT_STATUS_NON_CONNECTED_IDLE;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), BT Non-Connected idle!!!\n"));
	}
	else if(btInfo == BT_INFO_8723B_1ANT_B_CONNECTION)	// connection exists but no busy
	{
		pCoexDm->btStatus = BT_8723B_1ANT_BT_STATUS_CONNECTED_IDLE;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), BT Connected-idle!!!\n"));
	}
	else if((btInfo&BT_INFO_8723B_1ANT_B_SCO_ESCO) ||
		(btInfo&BT_INFO_8723B_1ANT_B_SCO_BUSY))
	{
		pCoexDm->btStatus = BT_8723B_1ANT_BT_STATUS_SCO_BUSY;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), BT SCO busy!!!\n"));
	}
	else if(btInfo&BT_INFO_8723B_1ANT_B_ACL_BUSY)
	{
		if(BT_8723B_1ANT_BT_STATUS_ACL_BUSY != pCoexDm->btStatus)
			pCoexDm->bAutoTdmaAdjust = FALSE;
		pCoexDm->btStatus = BT_8723B_1ANT_BT_STATUS_ACL_BUSY;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), BT ACL busy!!!\n"));
	}
	else
	{
		pCoexDm->btStatus = BT_8723B_1ANT_BT_STATUS_MAX;
		BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], BtInfoNotify(), BT Non-Defined state!!!\n"));
	}

	if( (BT_8723B_1ANT_BT_STATUS_ACL_BUSY == pCoexDm->btStatus) ||
		(BT_8723B_1ANT_BT_STATUS_SCO_BUSY == pCoexDm->btStatus) ||
		(BT_8723B_1ANT_BT_STATUS_ACL_SCO_BUSY == pCoexDm->btStatus) )
		bBtBusy = TRUE;
	else
		bBtBusy = FALSE;
	pBtCoexist->fBtcSet(pBtCoexist, BTC_SET_BL_BT_TRAFFIC_BUSY, &bBtBusy);

	halbtc8723b1ant_RunCoexistMechanism(pBtCoexist);
}

VOID
EXhalbtc8723b1ant_RfStatusNotify(
	IN	PBTC_COEXIST			pBtCoexist,
	IN	u1Byte					type
	)
{
	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], RF Status notify\n"));

	if(BTC_RF_ON == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], RF is turned ON!!\n"));
		pBtCoexist->bStopCoexDm = FALSE;
	}
	else if(BTC_RF_OFF == type)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], RF is turned OFF!!\n"));
		
		halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, FALSE, 0);
		halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_BT, FALSE, TRUE);
		//halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_BT, BTC_WIFI_STAT_NORMAL_OFF);
		
		halbtc8723b1ant_IgnoreWlanAct(pBtCoexist, FORCE_EXEC, TRUE);
		pBtCoexist->bStopCoexDm = TRUE;
	}
}

VOID
EXhalbtc8723b1ant_HaltNotify(
	IN	PBTC_COEXIST			pBtCoexist
	)
{
	u4Byte	u4Tmp;
	
	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], Halt notify\n"));

	halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
	halbtc8723b1ant_PsTdma(pBtCoexist, FORCE_EXEC, FALSE, 0);
	halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_BT, FALSE, TRUE);
	//halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_BT, BTC_WIFI_STAT_NORMAL_OFF);

	halbtc8723b1ant_IgnoreWlanAct(pBtCoexist, FORCE_EXEC, TRUE);

	EXhalbtc8723b1ant_MediaStatusNotify(pBtCoexist, BTC_MEDIA_DISCONNECT);

	pBtCoexist->bStopCoexDm = TRUE;	
}

VOID
EXhalbtc8723b1ant_PnpNotify(
	IN	PBTC_COEXIST			pBtCoexist,
	IN	u1Byte				pnpState
	)
{
	BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], Pnp notify\n"));

	if(BTC_WIFI_PNP_SLEEP == pnpState)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], Pnp notify to SLEEP\n"));

		halbtc8723b1ant_PowerSaveState(pBtCoexist, BTC_PS_WIFI_NATIVE, 0x0, 0x0);
		halbtc8723b1ant_PsTdma(pBtCoexist, NORMAL_EXEC, FALSE, 0);
		halbtc8723b1ant_CoexTableWithType(pBtCoexist, NORMAL_EXEC, 2);
		halbtc8723b1ant_SetAntPath(pBtCoexist, BTC_ANT_PATH_BT, FALSE, TRUE);
		//halbtc8723b1ant_SetAntPathDCut(pBtCoexist, FALSE, FALSE, FALSE, BTC_ANT_PATH_BT, BTC_WIFI_STAT_NORMAL_OFF);

		pBtCoexist->bStopCoexDm = TRUE;
	}
	else if(BTC_WIFI_PNP_WAKE_UP == pnpState)
	{
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_NOTIFY, ("[BTCoex], Pnp notify to WAKE UP\n"));
		pBtCoexist->bStopCoexDm = FALSE;
		halbtc8723b1ant_InitHwConfig(pBtCoexist, FALSE, FALSE);
		halbtc8723b1ant_InitCoexDm(pBtCoexist);
		halbtc8723b1ant_QueryBtInfo(pBtCoexist);
	}
}

VOID
EXhalbtc8723b1ant_CoexDmReset(
	IN	PBTC_COEXIST			pBtCoexist
	)
{
	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], *****************Coex DM Reset*****************\n"));

	halbtc8723b1ant_InitHwConfig(pBtCoexist, FALSE, FALSE);
	//pBtCoexist->fBtcSetRfReg(pBtCoexist, BTC_RF_A, 0x1, 0xfffff, 0x0);
	//pBtCoexist->fBtcSetRfReg(pBtCoexist, BTC_RF_A, 0x2, 0xfffff, 0x0);
	halbtc8723b1ant_InitCoexDm(pBtCoexist);
}

VOID
EXhalbtc8723b1ant_Periodical(
	IN	PBTC_COEXIST			pBtCoexist
	)
{
	static u1Byte		disVerInfoCnt=0;
	u4Byte				fwVer=0, btPatchVer=0;
	PBTC_BOARD_INFO		pBoardInfo=&pBtCoexist->boardInfo;
	PBTC_STACK_INFO		pStackInfo=&pBtCoexist->stackInfo;

	BTC_PRINT(BTC_MSG_ALGORITHM, ALGO_TRACE, ("[BTCoex], ==========================Periodical===========================\n"));

	if(disVerInfoCnt <= 5)
	{
		disVerInfoCnt += 1;
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], ****************************************************************\n"));
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], Ant PG Num/ Ant Mech/ Ant Pos = %d/ %d/ %d\n", \
			pBoardInfo->pgAntNum, pBoardInfo->btdmAntNum, pBoardInfo->btdmAntPos));
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], BT stack/ hci ext ver = %s / %d\n", \
			((pStackInfo->bProfileNotified)? "Yes":"No"), pStackInfo->hciVersion));
		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_BT_PATCH_VER, &btPatchVer);
		pBtCoexist->fBtcGet(pBtCoexist, BTC_GET_U4_WIFI_FW_VER, &fwVer);
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], CoexVer/ FwVer/ PatchVer = %d_%x/ 0x%x/ 0x%x(%d)\n", \
			GLCoexVerDate8723b1Ant, GLCoexVer8723b1Ant, fwVer, btPatchVer, btPatchVer));
		BTC_PRINT(BTC_MSG_INTERFACE, INTF_INIT, ("[BTCoex], ****************************************************************\n"));
	}

#if(BT_AUTO_REPORT_ONLY_8723B_1ANT == 0)
	halbtc8723b1ant_QueryBtInfo(pBtCoexist);
	//halbtc8723b1ant_MonitorBtCtr(pBtCoexist);
	halbtc8723b1ant_MonitorBtEnableDisable(pBtCoexist);
#else
	halbtc8723b1ant_MonitorBtCtr(pBtCoexist);

	if( halbtc8723b1ant_IsWifiStatusChanged(pBtCoexist) ||
		pCoexDm->bAutoTdmaAdjust )
	{
		//if(pCoexSta->specialPktPeriodCnt > 2)
		//{
			halbtc8723b1ant_RunCoexistMechanism(pBtCoexist);	
		//}
	}

	pCoexSta->specialPktPeriodCnt++;
#endif
}


#endif


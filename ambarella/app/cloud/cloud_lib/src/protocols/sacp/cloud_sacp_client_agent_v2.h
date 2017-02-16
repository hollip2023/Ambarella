/**
 * cloud_sacp_client_agent_v2.h
 *
 * History:
 *  2014/08/25 - [Zhi He] create file
 *
 * Copyright (C) 2014 - 2024, the Ambarella Inc.
 *
 * All rights reserved. No Part of this file may be reproduced, stored
 * in a retrieval system, or transmitted, in any form, or by any means,
 * electronic, mechanical, photocopying, recording, or otherwise,
 * without the prior consent of the Ambarella Inc.
 */

#ifndef __CLOUD_SACP_CLIENT_AGENT_V2_H__
#define __CLOUD_SACP_CLIENT_AGENT_V2_H__

DCONFIG_COMPILE_OPTION_HEADERFILE_BEGIN

DCODE_DELIMITER;

//-----------------------------------------------------------------------
//
//  CSACPCloudClientAgentV2
//
//-----------------------------------------------------------------------

class CSACPCloudClientAgentV2: public ICloudClientAgentV2
{
public:
    static CSACPCloudClientAgentV2 *Create();

protected:
    EECode Construct();

    CSACPCloudClientAgentV2();
    virtual ~CSACPCloudClientAgentV2();

public:
    virtual EECode ConnectToServer(TChar *server_url, TU16 server_port, TChar *account_name, TChar *password, TU16 local_port = 0);

    virtual EECode SetHardwareAuthenticateCallback(TClientAgentAuthenticateCallBack callback);
    virtual EECode DisconnectToServer();

public:
    //setup API
    virtual EECode SetupVideoParams(ESACPDataChannelSubType video_format, TU32 framerate, TU32 res_width, TU32 res_height, TU32 bitrate, TU8 *extradata, TU16 extradata_size, TU32 gop = 0);
    virtual EECode SetupAudioParams(ESACPDataChannelSubType audio_format, TU8 audio_channnel_number, TU32 audio_sample_frequency, TU32 audio_bitrate, TU8 *extradata, TU16 extradata_size);

public:
    //upload data
    virtual EECode Uploading(TU8 *p_data, TMemSize size, ESACPDataChannelSubType data_type, TU32 seq_number, TU8 extra_flag = 0);

public:
    //sync API
    virtual EECode VideoSync(TTime video_current_time, TU32 video_seq_number, ETimeUnit time_unit = ETimeUnit_native);
    virtual EECode AudioSync(TTime audio_current_time, TU32 audio_seq_number, ETimeUnit time_unit = ETimeUnit_native);
    virtual EECode DiscontiousIndicator();

public:
    virtual EECode UpdateVideoBitrate(TU32 bitrate);
    virtual EECode UpdateVideoFramerate(TU32 framerate);

public:
    virtual EECode DebugPrintCloudPipeline(TU32 index, TU32 param_0);
    virtual EECode DebugPrintStreamingPipeline(TU32 index, TU32 param_0);
    virtual EECode DebugPrintRecordingPipeline(TU32 index, TU32 param_0);
    virtual EECode DebugPrintChannel(TU32 index, TU32 param_0);
    virtual EECode DebugPrintAllChannels(TU32 param_0);

public:
    virtual void Delete();

private:
    TClientAgentAuthenticateCallBack mfAuthenticateCallback;

private:
    TU16 mLocalPort;
    TU16 mServerPort;

    TU8 mbConnected;
    TU8 mbAuthenticated;
    TU8 mbFirstConnection;
    TU8 mbGetPeerVersion;

    TSocketHandler mSocket;

    TU32 mCmdSeqCount;

    TU8 mbReceiveConnectStatus;
    TU8 mReserved1;

    TU8 mEncryptionType1;
    TU8 mEncryptionType2;

private:
    TU8 *mpSenderBuffer;
    TMemSize mSenderBufferSize;

private:
    SSACPHeader *mpSACPHeader;

private:
    SCloudAgentVersion mVersion;

private:
    TU32 mLastErrorHint1;
    TU32 mLastErrorHint2;
    EECode mLastErrorCode;

private:
    IMutex *mpMutex;
};

DCONFIG_COMPILE_OPTION_HEADERFILE_END

#endif

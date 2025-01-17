/**
 * am_fileparser_aac.h
 *
 * History:
 *  2014/12/10 - [Zhi He] create file
 *
 * Copyright (c) 2015 Ambarella, Inc.
 *
 * This file and its contents ("Software") are protected by intellectual
 * property rights including, without limitation, U.S. and/or foreign
 * copyrights. This Software is also the confidential and proprietary
 * information of Ambarella, Inc. and its licensors. You may not use, reproduce,
 * disclose, distribute, modify, or otherwise prepare derivative works of this
 * Software or any portion thereof except pursuant to a signed license agreement
 * or nondisclosure agreement with Ambarella, Inc. or its authorized affiliates.
 * In the absence of such an agreement, you agree to promptly notify and return
 * this Software to Ambarella, Inc.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF NON-INFRINGEMENT,
 * MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AMBARELLA, INC. OR ITS AFFILIATES BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; COMPUTER FAILURE OR MALFUNCTION; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __AM_FILEPARSER_AAC_H__
#define __AM_FILEPARSER_AAC_H__

class CAACFileParser: public IMediaFileParser
{
public:
  static CAACFileParser *Create();
  virtual void Delete();

protected:
  CAACFileParser();
  ~CAACFileParser();
  EECode Construct();

public:
  virtual EECode SetProperty(EMediaFileParserProperty property);

public:
  virtual EECode OpenFile(TChar *name);
  virtual EECode CloseFile();

  virtual EECode SeekTo(TIOSize offset);
  virtual EECode ReadPacket(SMediaPacket *packet);
  virtual EECode GetMediaInfo(SMediaInfo *info);
  virtual EECode GetExtradata(TU8 *&p_extradata, TU32 &extradata_size);

private:
  EECode getMediaInfo();
  EECode readDataFromFile();

private:
  IIO *mpIO;
  TU8 *mpReadBuffer;
  TIOSize mReadBufferSize;
  TIOSize mReadBlockSize;
  TIOSize mReadNextThreashold;

  TU8 *mpCurrentPtr;
  TIOSize mBufRemaingSize;

  TIOSize mFileTotalSize;
  TIOSize mFileCurrentOffset;

private:
  TU8 mbFileOpend;
  TU8 mbFileEOF;
  TU8 mbParseMediaInfo;
  TU8 mbStartReadFile;

private:
  TU8 mAudioChannelNumber;
  TU8 mAudioSampleSize;
  TU8 mAudioProfile;
  TU8 mReserved1;

  TU32 mAudioSampleRate;
  TU32 mAudioFrameSize;

private:
  SADTSHeader mADTSHeader;

private:
  TU8 *mpExtradata;
  TU32 mExtradataLen;
};

#endif


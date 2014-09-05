//
//  AgoraAudioSDK.h
//  agorasdk
//
//  Created by taosiming on 14-6-19.
//  Copyright (c) 2014 Agoralab. All rights reserved.
//

#ifndef AGORA_AUDIO_SDK_H
#define AGORA_AUDIO_SDK_H

#if defined(_WIN32)
#if defined(MEDIASDK_EXPORT)
#define AGORA_API __declspec(dllexport)
#else
#define AGORA_API __declspec(dllimport)
#endif
#else
#define AGORA_API
#endif

class IAgoraAudioEventHandler
{
public:
	virtual void onLoadAudioEngineSuccess() = 0;
    virtual void onGetAudioSvrAddrSuccess(const char* msg) = 0;
//    virtual void onCreateChannelSuccess(const char* msg) = 0;
    virtual void onJoinSuccess(const char* msg) = 0;
    virtual void onError(int rescode, const char* msg) = 0;
    virtual void onLogEvent(const char* msg) = 0;
	virtual void onQuality(unsigned int uid, unsigned short delay, unsigned short lost, unsigned short jitter) = 0;
};

class IAgoraAudio
{
public:
    virtual void joinChannel(const char* key, const char* name, const char* info, unsigned int uid) = 0;
	virtual void connectAudioServers(unsigned int sid, unsigned int uid, const char* audioServers) = 0;
    virtual void leave() = 0;
    virtual void mute(bool bMute) = 0;
	virtual void setSpeaker(bool enable) = 0;
	virtual void setAecType(int type) = 0;
	virtual void setAgcStatus(bool enable) = 0;
	virtual void setNsStatus(bool enable) = 0;
	virtual void setCodec(int index) = 0;
	virtual int getCodecCount() = 0;
	virtual bool getCodecInfo(int index, char* codecInfo, size_t maxLength) = 0;
	virtual int getJitterBufferMaxMetric() = 0;
	virtual bool setupProfile(const char* deviceId, const char* dataDir) = 0;
	virtual const char* getDeviceProfile() = 0;
	virtual void setDeviceProfile(const char* profile, bool merge) = 0;
	virtual void setAudioEngineOptions(const char* options) = 0;
	virtual void release() = 0;
};

extern "C" AGORA_API IAgoraAudio* createAgoraAudioInstance(IAgoraAudioEventHandler* pHandler);
extern "C" AGORA_API void finalDestroyAgoraAudioInstance();

#endif

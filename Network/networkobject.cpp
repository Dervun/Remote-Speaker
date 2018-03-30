#include "networkobject.h"


NetworkObject::NetworkObject()
{
    //
}

void NetworkObject::updateParameters(const QAudioDeviceInfo newDeviceInfo, const QAudioFormat newAudioFormat)
{
    deviceInfo = newDeviceInfo;
    audioFormat = newAudioFormat;
}


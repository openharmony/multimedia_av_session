/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "audio_device_manager.h"
#include "avsession_log.h"
#include "migrate_avsession_constant.h"
 
namespace OHOS::AVSession {
AudioDeviceManager &AudioDeviceManager::GetInstance()
{
    static AudioDeviceManager instance;
    return instance;
}
 
bool AudioDeviceManager::GetSessionInfoSyncState()
{
    AudioStandard::AudioRoutingManager *audioRoutingManager =
        AudioStandard::AudioRoutingManager::GetInstance();
    std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> devices =
        audioRoutingManager->GetAvailableDevices(AudioStandard::AudioDeviceUsage::MEDIA_OUTPUT_DEVICES);
    bool isConnected = false;
    for (auto& device : devices) {
        if (device != nullptr &&
            AudioStandard::DeviceCategory::BT_CAR == device->deviceCategory_ &&
            AudioStandard::DeviceType::DEVICE_TYPE_BLUETOOTH_A2DP == device->deviceType_) {
            isConnected = true;
            break;
        }
    }
    bool isSourceAudio = AUDIO_OUTPUT_SOURCE == outputDevice_;
    return isConnected || isSourceAudio;
}

void AudioDeviceManager::InitAudioStateCallback(std::shared_ptr<MigrateAVSessionServer> migrateAVSession,
    std::string deviceId)
{
    SLOGI("enter RegisterAudioDeviceChangeCallback");
    if (isRegistered_) {
        SLOGW("device change callback already registered");
        return;
    }
    RegisterAudioDeviceChangeCallback();
    RegisterPreferedOutputDeviceChangeCallback();
    migrateSession_ = migrateAVSession;
    isRegistered_ = true;
    deviceId_ = deviceId;
}

void AudioDeviceManager::UnInitAudioStateCallback()
{
    UnRegisterAudioDeviceChangeCallback();
    UnRegisterPreferedOutputDeviceChangeCallback();
    isRegistered_ = false;
}

void AudioDeviceManager::RegisterAudioDeviceChangeCallback()
{
    SLOGI("enter RegisterAudioDeviceChangeCallback");
    if (audioDeviceChangeCallback_ == nullptr) {
        audioDeviceChangeCallback_ = std::make_shared<DeviceChangeCallback>();
    }
    AudioStandard::AudioSystemManager *audioSystemManager =
        AudioStandard::AudioSystemManager::GetInstance();
    if (audioSystemManager == nullptr) {
        SLOGE("audioSystemManager is null");
        return;
    }
    audioSystemManager->SetDeviceChangeCallback(AudioStandard::DeviceFlag::OUTPUT_DEVICES_FLAG,
        audioDeviceChangeCallback_);
}
 
void AudioDeviceManager::UnRegisterAudioDeviceChangeCallback()
{
    SLOGI("enter UnRegisterAudioDeviceChangeCallback");
    AudioStandard::AudioSystemManager *audioSystemManager =
        AudioStandard::AudioSystemManager::GetInstance();
    if (audioSystemManager == nullptr) {
        SLOGE("audioSystemManager is null");
        return;
    }
    audioSystemManager->UnsetDeviceChangeCallback(AudioStandard::DeviceFlag::OUTPUT_DEVICES_FLAG);
}

void AudioDeviceManager::RegisterPreferedOutputDeviceChangeCallback()
{
    if (audioPreferedOutputDeviceChangeCallback_ == nullptr) {
        audioPreferedOutputDeviceChangeCallback_ = std::make_shared<OutputDeviceChangeCallback>();
    }
    AudioStandard::AudioRoutingManager *audioRoutingManager =
        AudioStandard::AudioRoutingManager::GetInstance();
    if (audioRoutingManager == nullptr) {
        SLOGE("audioRoutingManager is null");
        return;
    }
    AudioStandard::AudioRendererInfo rendererInfo;
    rendererInfo.streamUsage = AudioStandard::STREAM_USAGE_MUSIC;
    audioRoutingManager->SetPreferredOutputDeviceChangeCallback(rendererInfo,
        audioPreferedOutputDeviceChangeCallback_);
}

void AudioDeviceManager::UnRegisterPreferedOutputDeviceChangeCallback()
{
    SLOGI("enter UnRegisterPreferedOutputDeviceChangeCallback");
    AudioStandard::AudioRoutingManager *audioRoutingManager = AudioStandard::AudioRoutingManager::GetInstance();
    if (audioRoutingManager == nullptr) {
        SLOGE("audioRoutingManger is null");
        return;
    }
    audioRoutingManager->UnsetPreferredOutputDeviceChangeCallback();
}
 
void AudioDeviceManager::SendRemoteAvSessionInfo(const std::string &deviceId)
{
    if (migrateSession_ != nullptr) {
        migrateSession_->SendRemoteControllerList(deviceId);
        migrateSession_->SendRemoteHistorySessionList(deviceId);
    }
}

void AudioDeviceManager::ClearRemoteAvSessionInfo(const std::string &deviceId)
{
    if (migrateSession_ != nullptr) {
        migrateSession_->ClearRemoteControllerList(deviceId);
        migrateSession_->ClearRemoteHistorySessionList(deviceId);
    }
}

std::string AudioDeviceManager::GetDeviceId()
{
    return deviceId_;
}

int32_t AudioDeviceManager::GetAudioState()
{
    return outputDevice_;
}

void AudioDeviceManager::SetAudioState(int32_t state)
{
    SLOGI("current set audio is %{public}d", state);
    outputDevice_ = state;
}
 
void DeviceChangeCallback::OnDeviceChange(const AudioStandard::DeviceChangeAction &deviceChangeAction)
{
    SLOGI("receive OnDeviceChange");
    AudioStandard::AudioSystemManager *audioSystemManager =
        AudioStandard::AudioSystemManager::GetInstance();
    if (audioSystemManager == nullptr) {
        SLOGE("audioSystemManager is null");
        return;
    }
    if (AudioStandard::DeviceChangeType::DISCONNECT == deviceChangeAction.type) {
        std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> deviceDescriptors =
            deviceChangeAction.deviceDescriptors;
        for (auto &device : deviceDescriptors) {
            if (device != nullptr &&
                AudioStandard::DeviceCategory::BT_CAR == device->deviceCategory_ &&
                AudioStandard::DeviceType::DEVICE_TYPE_BLUETOOTH_A2DP == device->deviceType_) {
                AudioDeviceManager::GetInstance().SendRemoteAvSessionInfo(
                    AudioDeviceManager::GetInstance().GetDeviceId());
                break;
            }
        }
    }
}

void OutputDeviceChangeCallback::OnPreferredOutputDeviceUpdated(
    const std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> &desc)
{
    SLOGI("receive OnPreferedOutputDeviceUpdated");
    if (desc.empty()) {
        SLOGE("device is empty");
        return;
    }

    auto deviceDesc = desc[0];
    if (deviceDesc == nullptr) {
        SLOGE("device is null");
        return;
    }
    std::lock_guard lockGuard(lock_);
    std::string deviceId = AudioDeviceManager::GetInstance().GetDeviceId();
    if (AudioStandard::LOCAL_NETWORK_ID == deviceDesc->networkId_) {
        if (AUDIO_OUTPUT_SOURCE == AudioDeviceManager::GetInstance().GetAudioState()) {
            return;
        }
        AudioDeviceManager::GetInstance().ClearRemoteAvSessionInfo(deviceId);
        AudioDeviceManager::GetInstance().SetAudioState(AUDIO_OUTPUT_SOURCE);
    } else {
        if (AUDIO_OUTPUT_SINK == AudioDeviceManager::GetInstance().GetAudioState()) {
            return;
        }
        SLOGI("receive OnPreferedOutputDeviceUpdated send remote session");
        AudioDeviceManager::GetInstance().SetAudioState(AUDIO_OUTPUT_SINK);
        AudioDeviceManager::GetInstance().SendRemoteAvSessionInfo(deviceId);
    }
}
}
/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>

#include "avsession_item.h"
#include "iav_session.h"
#include "iremote_stub.h"
#include "avsession_stub.h"
#include "avsession_callback_proxy.h"
#include "avsession_controller_stub.h"
#include "avsession_service.h"
#include "avsession_errors.h"
#include "system_ability_definition.h"
#include "avcontrol_command.h"
#include "avcall_meta_data.h"
#include "avcall_state.h"
#include "avsessionitem_fuzzer.h"

using namespace std;
namespace OHOS {
namespace AVSession {
static const int32_t MAX_CODE_TEST = 24;
static const int32_t MAX_CODE_LEN = 512;
static const int32_t MIN_SIZE_NUM = 4;

void AvSessionItemFuzzer::AvSessionItemFuzzerTest(uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size > MAX_CODE_LEN) || (size < MIN_SIZE_NUM)) {
        return;
    }
    uint32_t code = *(reinterpret_cast<const uint32_t*>(data));
    if (code >= MAX_CODE_TEST) {
        return;
    }
    std::string tag(reinterpret_cast<const char*>(data), size);
    int32_t type = *reinterpret_cast<const int32_t*>(data);
    std::string bundleName(reinterpret_cast<const char*>(data), size);
    std::string abilityName(reinterpret_cast<const char*>(data), size);

    AppExecFwk::ElementName elementName;
    elementName.SetBundleName(bundleName);
    elementName.SetAbilityName(abilityName);

    sptr<AVSessionService> service = new AVSessionService(AVSESSION_SERVICE_ID);
    if (!service) {
        SLOGI("service is null");
        return;
    }
    sptr<IRemoteObject> avSessionItemObj = service->CreateSessionInner(tag, type, elementName);
    sptr<AVSessionItem> avSessionItem = (sptr<AVSessionItem>&)avSessionItemObj;
    if (!avSessionItem) {
        SLOGI("avSessionItem is null");
        return;
    }
    MessageParcel dataMessageParcel;
    MessageParcel reply;
    MessageOption option;
    if (!dataMessageParcel.WriteInterfaceToken(IAVSession::GetDescriptor())) {
        return;
    }
    size -= sizeof(uint32_t);
    dataMessageParcel.WriteBuffer(data + sizeof(uint32_t), size);
    dataMessageParcel.RewindRead(0);
    avSessionItem->OnRemoteRequest(code, dataMessageParcel, reply, option);
}

void AvSessionItemTest(uint8_t *data, size_t size)
{
    if ((data == nullptr) || (size > MAX_CODE_LEN) || (size < MIN_SIZE_NUM)) {
        return;
    }
    sptr<AVSessionService> service = new AVSessionService(AVSESSION_SERVICE_ID);
    if (!service) {
        SLOGI("service is null");
        return;
    }
    std::string tag("audio");
    int32_t type = 0;
    AppExecFwk::ElementName elementName;
    elementName.SetBundleName("bundleName");
    elementName.SetAbilityName("abilityName");
    sptr<IRemoteObject> avSessionItemObj = service->CreateSessionInner(tag, type, elementName);
    sptr<AVSessionItem> avSessionItem = (sptr<AVSessionItem>&)avSessionItemObj;
    if (!avSessionItem) {
        SLOGI("avSessionItem is null");
        return;
    }
    AvSessionItemTestImpl(avSessionItem, data, size);
    AvSessionCallItemTest(avSessionItem, data, size);
    AvSessionItemTestImplExtension(avSessionItem, data, size);
    AvSessionCallItemTestExtension(avSessionItem, data, size);
}

void AvSessionItemTestImpl(sptr<AVSessionItem> avSessionItem, const uint8_t* data, size_t size)
{
    AVPlaybackState avState;
    int32_t state = *(reinterpret_cast<const int32_t *>(data));
    avState.SetState(state);

    AVMetaData metaData;
    std::string assetId(reinterpret_cast<const char *>(data), size);
    metaData.SetAssetId(assetId);

    std::vector<int32_t> cmds;
    int32_t fuzzCmds = *(reinterpret_cast<const int32_t *>(data));
    cmds.push_back(fuzzCmds);

    bool top = *(reinterpret_cast<const bool *>(data));

    AVControlCommand controlCommand;
    int32_t cmd = *(reinterpret_cast<const int32_t *>(data));
    controlCommand.SetCommand(cmd);

    OutputDeviceInfo info;
    DeviceInfo deviceInfo;
    deviceInfo.castCategory_ = 0;
    std::string deviceId(reinterpret_cast<const char *>(data), size);
    deviceInfo.deviceId_= deviceId;
    std::string deviceName(reinterpret_cast<const char *>(data), size);
    deviceInfo.deviceName_ = deviceName;
    info.deviceInfos_.push_back(deviceInfo);

    avSessionItem->ExecuteControllerCommand(controlCommand);
    avSessionItem->SetTop(top);
    avSessionItem->SetOutputDevice(info);
    avSessionItem->GetOutputDevice(info);
    avSessionItem->AddSupportCommand(controlCommand.GetCommand());
    avSessionItem->DeleteSupportCommand(controlCommand.GetCommand());
    avSessionItem->GetSessionId();
    avSessionItem->GetAVMetaData(metaData);
    avSessionItem->SetAVMetaData(metaData);
    avSessionItem->GetAVPlaybackState(avState);
    avSessionItem->Activate();
    avSessionItem->Deactivate();
    avSessionItem->IsActive();
    avSessionItem->Destroy();
    avSessionItem->SetAVPlaybackState(avState);
    avSessionItem->GetPlaybackState();
    avSessionItem->GetMetaData();
    avSessionItem->GetSupportCommand();
    avSessionItem->GetPid();
    avSessionItem->GetUid();
    avSessionItem->GetAbilityName();
    avSessionItem->GetRemoteSource();
}

void AvSessionItemTestImplExtension(sptr<AVSessionItem> avSessionItem, const uint8_t* data, size_t size)
{
    int32_t state = *(reinterpret_cast<const int32_t *>(data));
    int32_t itemId = *(reinterpret_cast<const int32_t *>(data));
    int32_t pid = *(reinterpret_cast<const int32_t *>(data));
    int32_t uid = *(reinterpret_cast<const int32_t *>(data));

    OutputDeviceInfo info;
    DeviceInfo deviceInfo;
    deviceInfo.castCategory_ = 0;
    std::string deviceId(reinterpret_cast<const char *>(data), size);
    deviceInfo.deviceId_= deviceId;
    std::string deviceName(reinterpret_cast<const char *>(data), size);
    deviceInfo.deviceName_ = deviceName;
    info.deviceInfos_.push_back(deviceInfo);

    std::vector<AVQueueItem> avQueueItems;
    AVQueueItem avQueueItem;
    avQueueItem.SetItemId(*(reinterpret_cast<const int32_t *>(data)));
    avQueueItems.push_back(avQueueItem);

    std::string title(reinterpret_cast<const char *>(data), size);
    std::string commonCommand(reinterpret_cast<const char *>(data), size);

    auto wantAgentPtr = std::make_shared<AbilityRuntime::WantAgent::WantAgent>();

    AAFwk::WantParams wantParams;

    auto keyEvent = MMI::KeyEvent::Create();
    keyEvent->SetKeyCode(*(reinterpret_cast<const int32_t*>(data)));
    MMI::KeyEvent::KeyItem keyItem;
    keyItem.SetKeyCode(*(reinterpret_cast<const int32_t*>(data)));
    keyEvent->AddKeyItem(keyItem);

    sptr<AVControllerItem> avControllerItem = new(std::nothrow) AVControllerItem(pid, avSessionItem);
    if (avControllerItem == nullptr) {
        return;
    }

    avSessionItem->SetAVQueueItems(avQueueItems);
    avSessionItem->GetAVQueueItems(avQueueItems);
    avSessionItem->SetAVQueueTitle(title);
    avSessionItem->GetAVQueueTitle(title);
    avSessionItem->SetLaunchAbility(*wantAgentPtr);
    avSessionItem->SetExtras(wantParams);
    avSessionItem->GetExtras(wantParams);
    avSessionItem->HandleMediaKeyEvent(*keyEvent);
    avSessionItem->HandleOutputDeviceChange(state, info);
    avSessionItem->HandleSkipToQueueItem(itemId);
    avSessionItem->ExecueCommonCommand(commonCommand, wantParams);
    avSessionItem->AddController(pid, avControllerItem);
    avSessionItem->SetPid(pid);
    avSessionItem->SetUid(uid);
    avSessionItem->HandleControllerRelease(pid);
}

void AvSessionCallItemTest(sptr<AVSessionItem> avSessionItem, const uint8_t* data, size_t size)
{
    AVCallMetaData callMetaData;
    int32_t numberDate = *(reinterpret_cast<const int32_t*>(data));
    std::string dataToS(std::to_string(numberDate));
    std::string strCallMetaData(dataToS);
    callMetaData.SetName(strCallMetaData);
    callMetaData.SetPhoneNumber(strCallMetaData);

    AVCallState avCallState;
    int32_t callState = std::stoi(dataToS);
    avCallState.SetAVCallState(callState);
    bool mute = std::stoi(dataToS);
    avCallState.SetAVCallMuted(mute);

    avSessionItem->SetAVCallMetaData(callMetaData);
    avSessionItem->SetAVCallState(avCallState);
}

void AvSessionCallItemTestExtension(sptr<AVSessionItem> avSessionItem, const uint8_t* data, size_t size)
{
    string sinkDevice(reinterpret_cast<const char *>(data), size);
    string event(reinterpret_cast<const char *>(data), size);

    auto releaseAndStartCallback = [](AVSessionItem& item) {};
    auto updateSessionCallback = [](string str, bool flag) {};

    AAFwk::WantParams wantParams;

    avSessionItem->GetSessionType();
    avSessionItem->DestroyTask();
    avSessionItem->GetDescriptor();
    avSessionItem->GetAVCallState();
    avSessionItem->GetAVCallMetaData();
    avSessionItem->GetQueueItems();
    avSessionItem->GetQueueTitle();
    avSessionItem->GetExtras();
    avSessionItem->GetLaunchAbility();
    avSessionItem->GetBundleName();
    avSessionItem->SetServiceCallbackForRelease(releaseAndStartCallback);
    avSessionItem->SetServiceCallbackForCallStart(releaseAndStartCallback);
    avSessionItem->SourceCancelCastAudio(sinkDevice);
    avSessionItem->SinkCancelCastAudio();
    avSessionItem->SetSessionEvent(event, wantParams);
    avSessionItem->SetServiceCallbackForAVQueueInfo(releaseAndStartCallback);
    avSessionItem->SetServiceCallbackForUpdateSession(updateSessionCallback);
}

void AvSessionItemOnRemoteRequest(uint8_t *data, size_t size)
{
    auto avSessionItem = std::make_unique<AvSessionItemFuzzer>();
    if (avSessionItem == nullptr) {
        SLOGI("avSessionItem is null");
        return;
    }
    avSessionItem->AvSessionItemFuzzerTest(data, size);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    AvSessionItemOnRemoteRequest(const_cast<uint8_t*>(data), size);
    AvSessionItemTest(const_cast<uint8_t*>(data), size);
    return 0;
}
} // namespace AVSession
} // namespace OHOS

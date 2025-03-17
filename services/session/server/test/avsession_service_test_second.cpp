/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include <memory>
#include <fstream>
#include <cstdio>

#include "avsession_log.h"
#include "avsession_errors.h"
#include "avmeta_data.h"
#include "avplayback_state.h"
#include "avsession_info.h"
#include "avsession_service.h"
#include "audio_info.h"
#include "system_ability_definition.h"
#include "system_ability_ondemand_reason.h"

using namespace testing::ext;
using namespace OHOS::AVSession;
using namespace OHOS::AudioStandard;
static AVMetaData g_metaData;
static AVPlaybackState g_playbackState;
static char g_testSessionTag[] = "test";
static char g_testAnotherBundleName[] = "testAnother.ohos.avsession";
static char g_testAnotherAbilityName[] = "testAnother.ability";
static OHOS::sptr<AVSessionService> g_AVSessionService {nullptr};
static const int32_t COLLABORATION_SA_ID = 70633;
static const int32_t CAST_ENGINE_SA_ID = 65546;
static const int32_t AVSESSION_CONTINUE = 1;
const int32_t KEYCODE_CLEAR = 5;
const int32_t KEYCODE_HEADSETHOOK = 6;
static bool g_isCallOnSessionCreate = false;
static bool g_isCallOnSessionRelease = false;
static bool g_isCallOnTopSessionChange = false;

class TestISessionListener : public ISessionListener {
    TestISessionListener() = default;
    virtual ~TestISessionListener() = default;
    void OnSessionCreate(const AVSessionDescriptor& descriptor) override;
    void OnSessionRelease(const AVSessionDescriptor& descriptor) override;
    void OnTopSessionChange(const AVSessionDescriptor& descriptor) override;
    void OnAudioSessionChecked(const int32_t uid) override {};
    void OnDeviceAvailable(const OutputDeviceInfo& castOutputDeviceInfo) override {};
    void OnDeviceLogEvent(const DeviceLogEventCode eventId, const int64_t param) override {};
    void OnDeviceOffline(const std::string& deviceId) override {};
    void OnRemoteDistributedSessionChange(
        const std::vector<OHOS::sptr<IRemoteObject>>& sessionControllers) override {};
    OHOS::sptr<IRemoteObject> AsObject() override { return nullptr; };
};

void TestISessionListener::OnSessionCreate(const AVSessionDescriptor& descriptor)
{
    g_isCallOnSessionCreate = true;
}

void TestISessionListener::OnSessionRelease(const AVSessionDescriptor& descriptor)
{
    g_isCallOnSessionRelease = true;
}

void TestISessionListener::OnTopSessionChange(const AVSessionDescriptor& descriptor)
{
    g_isCallOnTopSessionChange = true;
}

class TestSessionListener : public SessionListener {
    TestSessionListener() = default;
    virtual ~TestSessionListener() = default;
    void OnSessionCreate(const AVSessionDescriptor& descriptor) override;
    void OnSessionRelease(const AVSessionDescriptor& descriptor) override;
    void OnTopSessionChange(const AVSessionDescriptor& descriptor) override;
    void OnAudioSessionChecked(const int32_t uid) override {};
    void OnDeviceAvailable(const OutputDeviceInfo& castOutputDeviceInfo) override {};
    void OnDeviceLogEvent(const DeviceLogEventCode eventId, const int64_t param) override {};
    void OnDeviceOffline(const std::string& deviceId) override {};
    void OnRemoteDistributedSessionChange(
        const std::vector<OHOS::sptr<IRemoteObject>>& sessionControllers) override {};
};

void TestSessionListener::OnSessionCreate(const AVSessionDescriptor& descriptor)
{
    g_isCallOnSessionCreate = true;
}

void TestSessionListener::OnSessionRelease(const AVSessionDescriptor& descriptor)
{
    g_isCallOnSessionRelease = true;
}

void TestSessionListener::OnTopSessionChange(const AVSessionDescriptor& descriptor)
{
    g_isCallOnTopSessionChange = true;
}


class TestIClientDeath : public IClientDeath {
    TestIClientDeath() = default;
    virtual ~TestIClientDeath() = default;
    OHOS::sptr<IRemoteObject> AsObject() override
    {
        OHOS::AppExecFwk::ElementName elementName;
        elementName.SetBundleName(g_testAnotherBundleName);
        elementName.SetAbilityName(g_testAnotherAbilityName);
        OHOS::sptr<AVSessionItem> avsessionHere_ = g_AVSessionService->CreateSessionInner(
            g_testSessionTag, AVSession::SESSION_TYPE_VOICE_CALL, false, elementName);
        std::string sessionId = avsessionHere_->GetSessionId();
        OHOS::sptr<IRemoteObject> object = nullptr;
        g_AVSessionService->CreateControllerInner(sessionId, object);
        return object;
    }
};

void OnClientDied(pid_t pid)
{
    SLOGI("OnClientDied pid = %{public}d", pid);
}

class AVSessionServiceTestSecond : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    OHOS::sptr<AVSessionItem> CreateSession();
    std::shared_ptr<AVSession> avsession_ = nullptr;
};

void AVSessionServiceTestSecond::SetUpTestCase()
{
    SLOGI("set up AVSessionServiceTestSecond");
    system("killall -9 com.example.hiMusicDemo");
}

void AVSessionServiceTestSecond::TearDownTestCase()
{
}

void AVSessionServiceTestSecond::SetUp()
{
    SLOGI("set up test function in AVSessionServiceTestSecond");\
    g_AVSessionService = new AVSessionService(OHOS::AVSESSION_SERVICE_ID);
    g_AVSessionService->InitKeyEvent();
}

void AVSessionServiceTestSecond::TearDown()
{
    SLOGI("tear down test function in AVSessionServiceTestSecond");
    [[maybe_unused]] int32_t ret = AVSESSION_ERROR;
    if (avsession_ != nullptr) {
        ret = avsession_->Destroy();
        avsession_ = nullptr;
    }
}

OHOS::sptr<AVSessionItem> AVSessionServiceTestSecond::CreateSession()
{
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(g_testAnotherBundleName);
    elementName.SetAbilityName(g_testAnotherAbilityName);
    OHOS::sptr<AVSessionItem> avsessionHere =
        g_AVSessionService->CreateSessionInner(g_testSessionTag, AVSession::SESSION_TYPE_AUDIO, false, elementName);
    return avsessionHere;
}

/**
* @tc.name: IsParamInvalid001
* @tc.desc: Verifying IsParamInvalid with an empty tag.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, IsParamInvalid001, TestSize.Level1)
{
    SLOGD("IsParamInvalid001 begin!");
    std::string tag = "";
    int32_t type = AVSession::SESSION_TYPE_AUDIO;
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(g_testAnotherBundleName);
    elementName.SetAbilityName(g_testAnotherAbilityName);
    bool result = g_AVSessionService->IsParamInvalid(tag, type, elementName);
    EXPECT_FALSE(result);
    SLOGD("IsParamInvalid001 end!");
}

/**
* @tc.name: IsParamInvalid002
* @tc.desc: Verifying IsParamInvalid with an invalid session type.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, IsParamInvalid002, TestSize.Level1)
{
    SLOGD("IsParamInvalid002 begin!");
    std::string tag = "testTag";
    int32_t type = AVSession::SESSION_TYPE_INVALID;
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName("com.example.bundle");
    elementName.SetAbilityName("com.example.ability");
    bool result = g_AVSessionService->IsParamInvalid(tag, type, elementName);
    EXPECT_FALSE(result);
    SLOGD("IsParamInvalid002 end!");
}

/**
* @tc.name: IsParamInvalid003
* @tc.desc: Verifying IsParamInvalid with an empty bundle name.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, IsParamInvalid003, TestSize.Level1)
{
    SLOGD("IsParamInvalid003 begin!");
    std::string tag = "testTag";
    int32_t type = AVSession::SESSION_TYPE_AUDIO;
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName("");
    elementName.SetAbilityName(g_testAnotherAbilityName);
    bool result = g_AVSessionService->IsParamInvalid(tag, type, elementName);
    EXPECT_FALSE(result);
    SLOGD("IsParamInvalid003 end!");
}

/**
* @tc.name: IsParamInvalid004
* @tc.desc: Verifying IsParamInvalid with an empty ability name.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, IsParamInvalid004, TestSize.Level1)
{
    SLOGD("IsParamInvalid004 begin!");
    std::string tag = "testTag";
    int32_t type = AVSession::SESSION_TYPE_AUDIO;
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(g_testAnotherBundleName);
    elementName.SetAbilityName("");
    bool result = g_AVSessionService->IsParamInvalid(tag, type, elementName);
    EXPECT_FALSE(result);
    SLOGD("IsParamInvalid004 end!");
}

/**
* @tc.name: IsLocalDevice001
* @tc.desc: Verifying IsLocalDevice with a valid local network ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, IsLocalDevice001, TestSize.Level1)
{
    SLOGD("IsLocalDevice001 begin!");
    const std::string networkId = "LocalDevice";
    bool result = g_AVSessionService->IsLocalDevice(networkId);
    EXPECT_TRUE(result);
    SLOGD("IsLocalDevice001 end!");
}

/**
* @tc.name: GetDeviceInfo002
* @tc.desc: Verifying GetDeviceInfo with a valid session item and descriptors.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, GetDeviceInfo002, TestSize.Level1)
{
    SLOGI("GetDeviceInfo002 begin!");
    std::shared_ptr<AVSessionDescriptor> histroyDescriptor = std::make_shared<AVSessionDescriptor>();
    auto avsessionHere = OHOS::sptr<AVSessionItem>::MakeSptr(*histroyDescriptor);
    std::vector<OHOS::AudioStandard::AudioDeviceDescriptor> descriptors = {
        OHOS::AudioStandard::AudioDeviceDescriptor(OHOS::AudioStandard::DeviceType::DEVICE_TYPE_EARPIECE,
            OHOS::AudioStandard::DeviceRole::INPUT_DEVICE, 1, 1, "LocalDevice")
    };
    g_AVSessionService->SetDeviceInfo(descriptors, avsessionHere);
    g_AVSessionService->GetDeviceInfo(avsessionHere, descriptors, descriptors, descriptors);
    EXPECT_NE(avsessionHere, nullptr);
    SLOGI("GetDeviceInfo002 end!");
}

/**
* @tc.name: CastAudioProcess001
* @tc.desc: Verifying CastAudioProcess with a valid session item and descriptors.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, CastAudioProcess001, TestSize.Level1)
{
    SLOGI("CastAudioProcess001 begin!");
    EXPECT_TRUE(g_AVSessionService != nullptr);
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(g_testAnotherBundleName);
    elementName.SetAbilityName(g_testAnotherAbilityName);
    OHOS::sptr<AVSessionItem> avsessionHere = g_AVSessionService->CreateSessionInner(g_testSessionTag,
        AVSession::SESSION_TYPE_VOICE_CALL, false, elementName);
    std::vector<OHOS::AudioStandard::AudioDeviceDescriptor> descriptors;
    g_AVSessionService->GetDeviceInfo(avsessionHere, descriptors, descriptors, descriptors);
    const std::string  sourceSessionInfo = "123443";
    auto ret = g_AVSessionService->CastAudioProcess(descriptors, sourceSessionInfo, avsessionHere);
    EXPECT_EQ(ret, AVSESSION_SUCCESS);
    SLOGI("CastAudioProcess001 end!");
}

/**
* @tc.name: CastAudioProcess002
* @tc.desc: Verifying CastAudioProcess with a valid session item and descriptors.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, CastAudioProcess002, TestSize.Level1)
{
    SLOGI("CastAudioProcess002 begin!");
    std::shared_ptr<AVSessionDescriptor> histroyDescriptor = std::make_shared<AVSessionDescriptor>();
    auto avsessionHere = OHOS::sptr<AVSessionItem>::MakeSptr(*histroyDescriptor);
    std::vector<OHOS::AudioStandard::AudioDeviceDescriptor> descriptors = {
        OHOS::AudioStandard::AudioDeviceDescriptor(OHOS::AudioStandard::DeviceType::DEVICE_TYPE_EARPIECE,
            OHOS::AudioStandard::DeviceRole::INPUT_DEVICE, 1, 1, "LocalDevice")
    };
    g_AVSessionService->SetDeviceInfo(descriptors, avsessionHere);
    g_AVSessionService->GetDeviceInfo(avsessionHere, descriptors, descriptors, descriptors);
    EXPECT_NE(avsessionHere, nullptr);
    const std::string  sourceSessionInfo = "123443";
    auto ret = g_AVSessionService->CastAudioProcess(descriptors, sourceSessionInfo, avsessionHere);
    EXPECT_EQ(ret, AVSESSION_SUCCESS);
    SLOGI("CastAudioProcess002 end!");
}

/**
* @tc.name: CastAudioInner001
* @tc.desc: Verifying CastAudioInner with a valid session item and descriptors.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, CastAudioInner001, TestSize.Level1)
{
    SLOGI("CastAudioInner001 begin!");
    EXPECT_TRUE(g_AVSessionService != nullptr);
    std::shared_ptr<AVSessionDescriptor> histroyDescriptor = std::make_shared<AVSessionDescriptor>();
    auto avsessionHere = OHOS::sptr<AVSessionItem>::MakeSptr(*histroyDescriptor);
    std::vector<OHOS::AudioStandard::AudioDeviceDescriptor> descriptors = {
        OHOS::AudioStandard::AudioDeviceDescriptor(OHOS::AudioStandard::DeviceType::DEVICE_TYPE_EARPIECE,
            OHOS::AudioStandard::DeviceRole::INPUT_DEVICE, 1, 1, "LocalDevice")
    };
    const std::string  sourceSessionInfo = "123443";
    auto ret = g_AVSessionService->CastAudioInner(descriptors, sourceSessionInfo, avsessionHere);
    EXPECT_EQ(ret, AVSESSION_ERROR);
    SLOGI("CastAudioInner001 end!");
}

/**
* @tc.name: CancelCastAudioInner001
* @tc.desc: Verifying CancelCastAudioInner with a valid session item and descriptors.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, CancelCastAudioInner001, TestSize.Level1)
{
    SLOGI("CancelCastAudioInner001 begin!");
    EXPECT_TRUE(g_AVSessionService != nullptr);
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(g_testAnotherBundleName);
    elementName.SetAbilityName(g_testAnotherAbilityName);
    OHOS::sptr<AVSessionItem> avsessionHere = g_AVSessionService->CreateSessionInner(g_testSessionTag,
        AVSession::SESSION_TYPE_VOICE_CALL, false, elementName);
    std::vector<OHOS::AudioStandard::AudioDeviceDescriptor> descriptors = {
        OHOS::AudioStandard::AudioDeviceDescriptor(OHOS::AudioStandard::DeviceType::DEVICE_TYPE_EARPIECE,
            OHOS::AudioStandard::DeviceRole::INPUT_DEVICE, 1, 1, "LocalDevice")
    };
    const std::string  sourceSessionInfo = "123443";
    auto ret = g_AVSessionService->CancelCastAudioInner(descriptors, sourceSessionInfo, avsessionHere);
    EXPECT_EQ(ret, AVSESSION_SUCCESS);
    SLOGI("CancelCastAudioInner001 end!");
}

/**
* @tc.name: CastAudioForAll001
* @tc.desc: Verifying CastAudioForAll with valid descriptors.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, CastAudioForAll001, TestSize.Level1)
{
    SLOGI("CastAudioForAll001 begin!");
    EXPECT_TRUE(g_AVSessionService != nullptr);
    std::vector<OHOS::AudioStandard::AudioDeviceDescriptor> descriptors = {
        OHOS::AudioStandard::AudioDeviceDescriptor(OHOS::AudioStandard::DeviceType::DEVICE_TYPE_EARPIECE,
            OHOS::AudioStandard::DeviceRole::INPUT_DEVICE, 1, 1, "LocalDevice")
    };
    auto ret = g_AVSessionService->CastAudioForAll(descriptors);
    EXPECT_EQ(ret, AVSESSION_ERROR);
    SLOGI("CastAudioForAll001 end!");
}

/**
* @tc.name: ClearControllerForClientDiedNoLock002
* @tc.desc: Verifying ClearControllerForClientDiedNoLock with a valid PID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, ClearControllerForClientDiedNoLock002, TestSize.Level1)
{
    SLOGI("ClearControllerForClientDiedNoLock002 begin!");
    EXPECT_TRUE(g_AVSessionService != nullptr);
    pid_t pid = 1234;
    g_AVSessionService->ClearControllerForClientDiedNoLock(pid);
    EXPECT_TRUE(g_AVSessionService != nullptr);
    SLOGI("ClearControllerForClientDiedNoLock002 end!");
}

/**
* @tc.name: CheckAndCreateDir001
* @tc.desc: Verifying CheckAndCreateDir with a valid file path.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, CheckAndCreateDir001, TestSize.Level1)
{
    SLOGI("CheckAndCreateDir001 begin!");
    EXPECT_TRUE(g_AVSessionService != nullptr);
    const string filePath = "/data/path";
    bool ret = g_AVSessionService->CheckAndCreateDir(filePath);
    EXPECT_TRUE(ret);
    SLOGI("CheckAndCreateDir001 end!");
}

/**
* @tc.name: SaveStringToFileEx001
* @tc.desc: Verifying SaveStringToFileEx with a valid file path and content.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, SaveStringToFileEx001, TestSize.Level1)
{
    SLOGI("SaveStringToFileEx001 begin!");
    std::string filePath =  "uripath";
    std::string content = "123456";
    bool ret = g_AVSessionService->SaveStringToFileEx(filePath, content);
    EXPECT_EQ(ret, true);
    SLOGI("SaveStringToFileEx001 end!");
}

/**
* @tc.name: SaveStringToFileEx002
* @tc.desc: Verifying SaveStringToFileEx with an empty content.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, SaveStringToFileEx002, TestSize.Level1)
{
    SLOGI("SaveStringToFileEx002 begin!");
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(g_testAnotherBundleName);
    elementName.SetAbilityName(g_testAnotherAbilityName);
    OHOS::sptr<AVSessionItem> avsessionItem = g_AVSessionService->CreateSessionInner(g_testSessionTag,
        AVSession::SESSION_TYPE_AUDIO, false, elementName);

    std::string filePath = g_AVSessionService->GetAVSortDir();
    std::string content = "";
    bool ret = g_AVSessionService->SaveStringToFileEx(filePath, content);
    EXPECT_EQ(ret, false);
    SLOGI("SaveStringToFileEx002 end!");
}

/**
* @tc.name: RemoveExpired001
* @tc.desc: Verifying RemoveExpired with an expired time point.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, RemoveExpired001, TestSize.Level1)
{
    SLOGD("RemoveExpired001 begin!");
    std::list<std::chrono::system_clock::time_point> timeList;
    auto now = std::chrono::system_clock::now();
    int32_t timeThreshold = 5;
    timeList.push_back(now - std::chrono::seconds(10));
    g_AVSessionService->RemoveExpired(timeList, now, timeThreshold);

    EXPECT_TRUE(timeList.empty());
    SLOGD("RemoveExpired001 end!");
}

/**
* @tc.name: RemoveExpired002
* @tc.desc: Verifying RemoveExpired with a non-expired time point.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, RemoveExpired002, TestSize.Level1)
{
    SLOGD("RemoveExpired002 begin!");
    std::list<std::chrono::system_clock::time_point> timeList;
    auto now = std::chrono::system_clock::now();
    int32_t timeThreshold = 5;
    timeList.push_back(now - std::chrono::seconds(3));

    g_AVSessionService->RemoveExpired(timeList, now, timeThreshold);
    EXPECT_EQ(timeList.size(), 1);
    SLOGD("RemoveExpired002 end!");
}

/**
* @tc.name: RemoveExpired003
* @tc.desc: Verifying RemoveExpired with an empty time list.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, RemoveExpired003, TestSize.Level1)
{
    SLOGD("RemoveExpired003 begin!");
    std::list<std::chrono::system_clock::time_point> timeList;
    auto now = std::chrono::system_clock::now();
    int32_t timeThreshold = 5;
    g_AVSessionService->RemoveExpired(timeList, now, timeThreshold);
    EXPECT_TRUE(timeList.empty());
    SLOGD("RemoveExpired003 end!");
}

/**
* @tc.name: NotifyFlowControl001
* @tc.desc: Verifying NotifyFlowControl with a full flow control list.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyFlowControl001, TestSize.Level1)
{
    SLOGD("NotifyFlowControl001 begin!");
    g_AVSessionService->flowControlPublishTimestampList_.clear();
    const size_t count = 3;
    for (size_t i = 0; i < count; ++i) {
        g_AVSessionService->flowControlPublishTimestampList_.push_back(std::chrono::system_clock::now());
    }
    bool result = g_AVSessionService->NotifyFlowControl();
    EXPECT_TRUE(result);
    EXPECT_EQ(g_AVSessionService->flowControlPublishTimestampList_.size(), count);
    SLOGD("NotifyFlowControl001 end!");
}

/**
* @tc.name: NotifyFlowControl002
* @tc.desc: Verifying NotifyFlowControl with a non-full flow control list.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyFlowControl002, TestSize.Level1)
{
    SLOGD("NotifyFlowControl002 begin!");
    const size_t count = 3;
    g_AVSessionService->flowControlPublishTimestampList_.clear();
    for (size_t i = 0; i < count - 1; ++i) {
        g_AVSessionService->flowControlPublishTimestampList_.push_back(std::chrono::system_clock::now());
    }
    bool result = g_AVSessionService->NotifyFlowControl();
    EXPECT_FALSE(result);
    SLOGD("NotifyFlowControl002 end!");
}

/**
* @tc.name: NotifyRemoteDistributedSessionControllersChanged001
* @tc.desc: Verifying NotifyRemoteDistributedSessionControllersChanged with a valid session controller.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyRemoteDistributedSessionControllersChanged001, TestSize.Level1)
{
    SLOGD("NotifyRemoteDistributedSessionControllersChanged001 begin!");
    std::vector<OHOS::sptr<IRemoteObject>> sessionControllers;
    g_AVSessionService->NotifyRemoteDistributedSessionControllersChanged(sessionControllers);
    OHOS::sptr<OHOS::ISystemAbilityManager> mgr =
        OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    EXPECT_TRUE(mgr != nullptr);
    OHOS::sptr<IRemoteObject> obj = mgr->GetSystemAbility(OHOS::AVSESSION_SERVICE_ID);
    sessionControllers.push_back(obj);
    g_AVSessionService->NotifyRemoteDistributedSessionControllersChanged(sessionControllers);
    EXPECT_TRUE(g_AVSessionService != nullptr);
    SLOGD("NotifyRemoteDistributedSessionControllersChanged001 end!");
}

static HWTEST_F(AVSessionServiceTestSecond, OnReceiveEvent004, TestSize.Level1)
{
    SLOGI("OnReceiveEvent004 begin!");
    OHOS::EventFwk::CommonEventData eventData;
    string action = OHOS::EventFwk::CommonEventSupport::COMMON_EVENT_SCREEN_OFF;
    OHOS::AAFwk::Want want = eventData.GetWant();
    want.SetAction(action);
    eventData.SetWant(want);
    OHOS::EventFwk::MatchingSkills matchingSkills;
    OHOS::EventFwk::CommonEventSubscribeInfo subscriberInfo(matchingSkills);
    EventSubscriber eventSubscriber(subscriberInfo, nullptr);
    eventSubscriber.OnReceiveEvent(eventData);
    EXPECT_EQ(0, AVSESSION_SUCCESS);
    SLOGI("OnReceiveEvent004 end!");
}

/**
* @tc.name: OnIdleWithSessions002
* @tc.desc: Verifying the OnIdle method with none zero count sessions.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnIdleWithSessions002, TestSize.Level1)
{
    SLOGD("OnIdleWithSessions002 begin!");
    OHOS::sptr<AVSessionItem> item = nullptr;
    g_AVSessionService->GetUsersManager().GetContainerFromAll().AddSession(getpid(), g_testAnotherAbilityName, item);
    const OHOS::SystemAbilityOnDemandReason idleReason(
        OHOS::OnDemandReasonId::INTERFACE_CALL, "INTERFACE_CALL", "TestValue", 12345);
    int32_t result = g_AVSessionService->OnIdle(idleReason);
    EXPECT_EQ(result, AVSESSION_ERROR);
    SLOGD("OnIdleWithSessions002 end!");
}

/**
* @tc.name: OnAddSystemAbility002
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is MULTIMODAL_INPUT_SERVICE_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility002, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility002 begin!");
    std::shared_ptr<AVSessionDescriptor> histroyDescriptor = std::make_shared<AVSessionDescriptor>();
    g_AVSessionService->topSession_ = OHOS::sptr<AVSessionItem>::MakeSptr(*histroyDescriptor);

    int32_t systemAbilityId = OHOS::MULTIMODAL_INPUT_SERVICE_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_NE(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility002 end!");
}

/**
* @tc.name: OnAddSystemAbility003
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is AUDIO_POLICY_SERVICE_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility003, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility003 begin!");
    int32_t systemAbilityId = OHOS::AUDIO_POLICY_SERVICE_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility003 end!");
}

/**
* @tc.name: OnAddSystemAbility004
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is APP_MGR_SERVICE_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility004, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility004 begin!");
    int32_t systemAbilityId = OHOS::APP_MGR_SERVICE_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility004 end!");
}

/**
* @tc.name: OnAddSystemAbility005
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility005, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility005 begin!");
    int32_t systemAbilityId = OHOS::DISTRIBUTED_HARDWARE_DEVICEMANAGER_SA_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility005 end!");
}

/**
* @tc.name: OnAddSystemAbility006
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is BUNDLE_MGR_SERVICE_SYS_ABILITY_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility006, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility006 begin!");
    int32_t systemAbilityId = OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility006 end!");
}

/**
* @tc.name: OnAddSystemAbility007
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is COLLABORATION_SA_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility007, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility007 begin!");
    int32_t systemAbilityId = COLLABORATION_SA_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility007 end!");
}

/**
* @tc.name: OnAddSystemAbility008
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is MEMORY_MANAGER_SA_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility008, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility008 begin!");
    int32_t systemAbilityId = OHOS::MEMORY_MANAGER_SA_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility008 end!");
}

/**
* @tc.name: OnAddSystemAbility009
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility009, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility009 begin!");
    int32_t systemAbilityId = OHOS::SUBSYS_ACCOUNT_SYS_ABILITY_ID_BEGIN;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility009 end!");
}

/**
* @tc.name: OnAddSystemAbility010
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is COMMON_EVENT_SERVICE_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility010, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility010 begin!");
    int32_t systemAbilityId = OHOS::COMMON_EVENT_SERVICE_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
    SLOGD("OnAddSystemAbility010 end!");
}

/**
* @tc.name: OnAddSystemAbility011
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is CAST_ENGINE_SA_ID.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility011, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility011 begin!");
#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    int32_t systemAbilityId = CAST_ENGINE_SA_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
#else
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
#endif
    SLOGD("OnAddSystemAbility011 end!");
}

/**
* @tc.name: OnAddSystemAbility012
* @tc.desc: Verifying the OnAddSystemAbility method with systemAbilityId is CAST_ENGINE_SA_ID and is2in1_ is true.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, OnAddSystemAbility012, TestSize.Level1)
{
    SLOGD("OnAddSystemAbility012 begin!");
#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    int32_t systemAbilityId = CAST_ENGINE_SA_ID;
    const std::string deviceId = "AUDIO";
    g_AVSessionService->is2in1_ = true;
    g_AVSessionService->OnAddSystemAbility(systemAbilityId, deviceId);
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
#else
    EXPECT_EQ(g_AVSessionService->topSession_, nullptr);
#endif
    SLOGD("OnAddSystemAbility012 end!");
}

/**
* @tc.name: InitBMS002
* @tc.desc: Verifying the InitBMS method with invalid userid.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, InitBMS002, TestSize.Level1)
{
    SLOGD("InitBMS002 begin!");
    std::shared_ptr<AVSessionDescriptor> histroyDescriptor = std::make_shared<AVSessionDescriptor>();
    g_AVSessionService->topSession_ = OHOS::sptr<AVSessionItem>::MakeSptr(*histroyDescriptor);
    g_AVSessionService->GetUsersManager().curUserId_ = -1;
    g_AVSessionService->InitBMS();
    EXPECT_NE(g_AVSessionService->topSession_, nullptr);
    SLOGD("InitBMS002 end!");
}

/**
* @tc.name: NotifySessionCreate001
* @tc.desc: Verifying the NotifySessionCreate method with listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifySessionCreate001, TestSize.Level1)
{
    SLOGD("NotifySessionCreate001 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    OHOS::sptr<TestISessionListener> iListener = new TestISessionListener();
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, iListener));
    g_AVSessionService->NotifySessionCreate(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnSessionCreate, true);
    SLOGD("NotifySessionCreate001 end!");
}

/**
* @tc.name: NotifySessionCreate002
* @tc.desc: Verifying the NotifySessionCreate method with nullptr listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifySessionCreate002, TestSize.Level1)
{
    SLOGD("NotifySessionCreate002 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, nullptr));
    g_AVSessionService->NotifySessionCreate(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnSessionCreate, true);
    SLOGD("NotifySessionCreate002 end!");
}

/**
* @tc.name: NotifySessionCreate003
* @tc.desc: Verifying the NotifySessionCreate method with listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifySessionCreate003, TestSize.Level1)
{
    SLOGD("NotifySessionCreate003 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    TestSessionListener* listener = new TestSessionListener();
    g_AVSessionService->innerSessionListeners_.push_back(listener);
    g_AVSessionService->NotifySessionCreate(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnSessionCreate, true);
    if (listener != nullptr) {
        delete listener;
        listener = nullptr;
    }
    SLOGD("NotifySessionCreate003 end!");
}

/**
* @tc.name: NotifySessionCreate004
* @tc.desc: Verifying the NotifySessionCreate method with null listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifySessionCreate004, TestSize.Level1)
{
    SLOGD("NotifySessionCreate004 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    g_AVSessionService->innerSessionListeners_.push_back(nullptr);
    g_AVSessionService->NotifySessionCreate(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnSessionCreate, true);
    SLOGD("NotifySessionCreate004 end!");
}

/**
* @tc.name: NotifyTopSessionChanged001
* @tc.desc: Verifying the NotifyTopSessionChanged method with listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyTopSessionChanged001, TestSize.Level1)
{
    SLOGD("NotifyTopSessionChanged001 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    OHOS::sptr<TestISessionListener> iListener = new TestISessionListener();
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, iListener));
    g_AVSessionService->NotifyTopSessionChanged(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnTopSessionChange, true);
    SLOGD("NotifyTopSessionChanged001 end!");
}

/**
* @tc.name: NotifyTopSessionChanged002
* @tc.desc: Verifying the NotifyTopSessionChanged method with nullptr listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyTopSessionChanged002, TestSize.Level1)
{
    SLOGD("NotifyTopSessionChanged002 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, nullptr));
    g_AVSessionService->NotifyTopSessionChanged(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnTopSessionChange, true);
    SLOGD("NotifyTopSessionChanged002 end!");
}

/**
* @tc.name: NotifyTopSessionChanged003
* @tc.desc: Verifying the NotifySessionCreate method with null listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyTopSessionChanged003, TestSize.Level1)
{
    SLOGD("NotifyTopSessionChanged003 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    g_AVSessionService->innerSessionListeners_.push_back(nullptr);
    g_AVSessionService->NotifyTopSessionChanged(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnTopSessionChange, true);
    SLOGD("NotifyTopSessionChanged003 end!");
}

/**
* @tc.name: NotifySessionRelease001
* @tc.desc: Verifying the NotifySessionRelease method with listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifySessionRelease001, TestSize.Level1)
{
    SLOGD("NotifySessionRelease001 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    OHOS::sptr<TestISessionListener> iListener = new TestISessionListener();
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, iListener));
    g_AVSessionService->NotifySessionRelease(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnSessionRelease, true);
    SLOGD("NotifySessionRelease001 end!");
}

/**
* @tc.name: NotifySessionRelease002
* @tc.desc: Verifying the NotifySessionRelease method with nullptr listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifySessionRelease002, TestSize.Level1)
{
    SLOGD("NotifySessionRelease002 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, nullptr));
    g_AVSessionService->NotifySessionRelease(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnSessionRelease, true);
    SLOGD("NotifySessionRelease002 end!");
}

/**
* @tc.name: NotifySessionRelease003
* @tc.desc: Verifying the NotifySessionRelease method with listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifySessionRelease003, TestSize.Level1)
{
    SLOGD("NotifySessionRelease003 begin!");
    AVSessionDescriptor aVSessionDescriptor;
    TestSessionListener* listener = new TestSessionListener();
    g_AVSessionService->innerSessionListeners_.push_back(listener);
    g_AVSessionService->NotifySessionRelease(aVSessionDescriptor);
    EXPECT_EQ(g_isCallOnSessionRelease, true);
    g_isCallOnSessionRelease = false;
    if (listener != nullptr) {
        delete listener;
        listener = nullptr;
    }
    SLOGD("NotifySessionRelease003 end!");
}

/**
* @tc.name: PlayStateCheck002
* @tc.desc: Verifying the PlayStateCheck method with valid parameters.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, PlayStateCheck002, TestSize.Level1)
{
    SLOGD("PlayStateCheck002 begin!");
    auto avsessionHere = CreateSession();
    ASSERT_TRUE(avsessionHere != nullptr);
    avsessionHere->playbackState_.SetState(AVPlaybackState::PLAYBACK_STATE_PREPARE);
    avsessionHere->SetUid(1);
    avsessionHere->Activate();
    g_AVSessionService->UpdateTopSession(avsessionHere);

    StreamUsage streamUsage = StreamUsage::STREAM_USAGE_MEDIA;
    RendererState rendererState = RendererState::RENDERER_RUNNING;
    g_AVSessionService->PlayStateCheck(1, streamUsage, rendererState);
    EXPECT_NE(g_AVSessionService->topSession_, nullptr);
    SLOGD("PlayStateCheck002 end!");
}

/**
* @tc.name: HandleKeyEvent001
* @tc.desc: Verifying the HandleKeyEvent method with valid parameters.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, HandleKeyEvent001, TestSize.Level1)
{
    SLOGD("HandleKeyEvent001 begin!");
    auto keyEvent = OHOS::MMI::KeyEvent(KEYCODE_CLEAR);
    keyEvent.SetKeyCode(KEYCODE_CLEAR);
    auto ret = g_AVSessionService->HandleKeyEvent(keyEvent);
    EXPECT_EQ(ret, AVSESSION_CONTINUE);
    SLOGD("HandleKeyEvent001 end!");
}

/**
* @tc.name: SendSystemAVKeyEvent010
* @tc.desc: Verifying the SendSystemAVKeyEvent method with valid parameters.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, SendSystemAVKeyEvent010, TestSize.Level1)
{
    SLOGD("SendSystemAVKeyEvent010 begin!");
    OHOS::AAFwk::Want bluetoothWant;
    std::string activeAddress = "00:00:00:00:00:00";
    bluetoothWant.SetParam("deviceId", activeAddress);
    auto keyEvent = OHOS::MMI::KeyEvent(KEYCODE_HEADSETHOOK);
    keyEvent.SetKeyCode(KEYCODE_HEADSETHOOK);
    auto ret = g_AVSessionService->SendSystemAVKeyEvent(keyEvent, bluetoothWant);
    EXPECT_EQ(ret, AVSESSION_SUCCESS);
    SLOGD("SendSystemAVKeyEvent010 end!");
}

/**
* @tc.name: ConvertKeyCodeToCommand001
* @tc.desc: Verifying the ConvertKeyCodeToCommand method with valid parameters.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, ConvertKeyCodeToCommand001, TestSize.Level1)
{
    SLOGD("ConvertKeyCodeToCommand001 begin!");
    auto ret = g_AVSessionService->ConvertKeyCodeToCommand(OHOS::MMI::KeyEvent::KEYCODE_MEDIA_PAUSE);
    EXPECT_EQ(ret, AVControlCommand::SESSION_CMD_PAUSE);
    SLOGD("ConvertKeyCodeToCommand001 end!");
}

/**
* @tc.name: NotifyRemoteDistributedSessionControllersChanged002
* @tc.desc: Verifying NotifyRemoteDistributedSessionControllersChanged with valid parameters.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyRemoteDistributedSessionControllersChanged002, TestSize.Level1)
{
    SLOGD("NotifyRemoteDistributedSessionControllersChanged002 begin!");
    std::vector<OHOS::sptr<IRemoteObject>> sessionControllers;
    OHOS::sptr<TestISessionListener> iListener = new TestISessionListener();
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, iListener));
    g_AVSessionService->NotifyRemoteDistributedSessionControllersChanged(sessionControllers);
    SLOGD("NotifyRemoteDistributedSessionControllersChanged002 end!");
}

/**
* @tc.name: NotifyRemoteDistributedSessionControllersChanged003
* @tc.desc: Verifying NotifyRemoteDistributedSessionControllersChanged with null listener.
* @tc.type: FUNC
* @tc.require: #I5Y4MZ
*/
static HWTEST_F(AVSessionServiceTestSecond, NotifyRemoteDistributedSessionControllersChanged003, TestSize.Level1)
{
    SLOGD("NotifyRemoteDistributedSessionControllersChanged003 begin!");
    std::vector<OHOS::sptr<IRemoteObject>> sessionControllers;
    g_AVSessionService->GetUsersManager().sessionListenersMap_.insert(std::make_pair(1, nullptr));
    g_AVSessionService->NotifyRemoteDistributedSessionControllersChanged(sessionControllers);
    SLOGD("NotifyRemoteDistributedSessionControllersChanged003 end!");
}

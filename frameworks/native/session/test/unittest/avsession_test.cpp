/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "avsession_manager.h"
#include "avsession_errors.h"
#include "avsession_callback_client.h"
#include "want_agent.h"
#include "avmeta_data.h"
#include "avplayback_state.h"
#include "avcall_state.h"
#include "avcall_meta_data.h"
#include "avmedia_description.h"
#include "avqueue_item.h"
#include "avsession_log.h"
#include "avcontrol_command.h"

#include "accesstoken_kit.h"
#include "bool_wrapper.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
namespace AVSession {
static int32_t g_onCall = AVSESSION_ERROR;
static int32_t g_sessionId = AVSESSION_ERROR;
static AVMetaData g_metaData;
static AVCallMetaData g_avCallMetaData;
static AVCallState g_avCallState;
static AVPlaybackState g_playbackState;
static char g_testSessionTag[] = "test";
static char g_testBundleName[] = "test.ohos.avsession";
static char g_testAbilityName[] = "test.ability";
static uint64_t g_selfTokenId = 0;

static HapInfoParams g_info = {
    .userID = 100,
    .bundleName = "ohos.permission_test.demo",
    .instIndex = 0,
    .appIDDesc = "ohos.permission_test.demo",
    .isSystemApp = true
};

static HapPolicyParams g_policy = {
    .apl = APL_NORMAL,
    .domain = "test.domain",
    .permList = {
        {
            .permissionName = "ohos.permission.MANAGE_MEDIA_RESOURCES",
            .bundleName = "ohos.permission_test.demo",
            .grantMode = 1,
            .availableLevel = APL_NORMAL,
            .label = "label",
            .labelId = 1,
            .description = "test",
            .descriptionId = 1
        }
    },
    .permStateList = {
        {
            .permissionName = "ohos.permission.MANAGE_MEDIA_RESOURCES",
            .isGeneral = true,
            .resDeviceID = { "local" },
            .grantStatus = { PermissionState::PERMISSION_GRANTED },
            .grantFlags = { 1 }
        }
    }
};

class AvsessionTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp() override;
    void TearDown() override;

    std::shared_ptr<AVSession> avsession_ = nullptr;
    std::shared_ptr<AVSessionController> controller_ = nullptr;

    static constexpr int SESSION_LEN = 64;
};

void AvsessionTest::SetUpTestCase()
{
    g_selfTokenId = GetSelfTokenID();
    AccessTokenKit::AllocHapToken(g_info, g_policy);
    AccessTokenIDEx tokenID = AccessTokenKit::GetHapTokenIDEx(g_info.userID, g_info.bundleName, g_info.instIndex);
    SetSelfTokenID(tokenID.tokenIDEx);
}

void AvsessionTest::TearDownTestCase()
{
    SetSelfTokenID(g_selfTokenId);
    auto tokenId = AccessTokenKit::GetHapTokenID(g_info.userID, g_info.bundleName, g_info.instIndex);
    AccessTokenKit::DeleteToken(tokenId);
}

void AvsessionTest::SetUp()
{
    OHOS::AppExecFwk::ElementName elementName;
    elementName.SetBundleName(g_testBundleName);
    elementName.SetAbilityName(g_testAbilityName);
    avsession_ = AVSessionManager::GetInstance().CreateSession(g_testSessionTag, AVSession::SESSION_TYPE_AUDIO,
                                                               elementName);
    ASSERT_NE(avsession_, nullptr);
    g_sessionId++;
    auto ret = AVSessionManager::GetInstance().CreateController(avsession_->GetSessionId(), controller_);
    ASSERT_EQ(ret, AVSESSION_SUCCESS);
    ASSERT_NE(controller_, nullptr);
}

void AvsessionTest::TearDown()
{
    [[maybe_unused]] int32_t ret = AVSESSION_ERROR;
    if (avsession_ != nullptr) {
        ret = avsession_->Destroy();
        avsession_ = nullptr;
    }
    if (controller_ != nullptr) {
        ret = controller_->Destroy();
        controller_ = nullptr;
    }
    g_onCall = AVSESSION_ERROR;
}

class AVSessionDemo : public AVSession {
public:
    std::string GetSessionId() override;
    std::string GetSessionType() override;
    int32_t GetAVMetaData(AVMetaData& meta) override;
    int32_t SetAVMetaData(const AVMetaData& meta) override;
    int32_t SetAVCallMetaData(const AVCallMetaData& meta) override;
    int32_t SetAVCallState(const AVCallState& avCallState) override;
    int32_t GetAVPlaybackState(AVPlaybackState& state) override;
    int32_t SetAVPlaybackState(const AVPlaybackState& state) override;
    int32_t GetAVQueueItems(std::vector<AVQueueItem>& items) override;
    int32_t SetAVQueueItems(const std::vector<AVQueueItem>& items) override;
    int32_t GetAVQueueTitle(std::string& title) override;
    int32_t SetAVQueueTitle(const std::string& title) override;
    int32_t SetLaunchAbility(const AbilityRuntime::WantAgent::WantAgent& ability) override;
    int32_t GetExtras(AAFwk::WantParams& extras) override;
    int32_t SetExtras(const AAFwk::WantParams& extras) override;
    std::shared_ptr<AVSessionController> GetController() override;
    int32_t RegisterCallback(const std::shared_ptr<AVSessionCallback>& callback) override;
    int32_t Activate() override;
    int32_t Deactivate() override;
    bool IsActive() override;
    int32_t Destroy() override;
    int32_t AddSupportCommand(const int32_t cmd) override;
    int32_t DeleteSupportCommand(const int32_t cmd) override;
    int32_t SetSessionEvent(const std::string& event, const AAFwk::WantParams& args) override;
#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    std::shared_ptr<AVCastController> GetAVCastController() override;
    int32_t ReleaseCast(bool continuePlay = false) override;
    int32_t StartCastDisplayListener() override;
    int32_t StopCastDisplayListener() override;
    int32_t GetAllCastDisplays(std::vector<CastDisplayInfo>& castDisplays) override;
#endif
};

std::string AVSessionDemo::GetSessionId()
{
    return "";
}

std::string AVSessionDemo::GetSessionType()
{
    return "";
}

int32_t AVSessionDemo::GetAVMetaData(AVMetaData& meta)
{
    return 0;
}

int32_t AVSessionDemo::SetAVMetaData(const AVMetaData& meta)
{
    return 0;
}

int32_t AVSessionDemo::SetAVCallMetaData(const AVCallMetaData& meta)
{
    return 0;
}

int32_t AVSessionDemo::SetAVCallState(const AVCallState& avCallState)
{
    return 0;
}

int32_t AVSessionDemo::GetAVPlaybackState(AVPlaybackState& state)
{
    return 0;
}

int32_t AVSessionDemo::SetAVPlaybackState(const AVPlaybackState& state)
{
    return 0;
}

int32_t AVSessionDemo::GetAVQueueItems(std::vector<AVQueueItem>& items)
{
    return 0;
}

int32_t AVSessionDemo::SetAVQueueItems(const std::vector<AVQueueItem>& items)
{
    return 0;
}

int32_t AVSessionDemo::GetAVQueueTitle(std::string& title)
{
    return 0;
}

int32_t AVSessionDemo::SetAVQueueTitle(const std::string& title)
{
    return 0;
}

int32_t AVSessionDemo::SetLaunchAbility(const AbilityRuntime::WantAgent::WantAgent& ability)
{
    return 0;
}

int32_t AVSessionDemo::GetExtras(AAFwk::WantParams& extras)
{
    return 0;
}

int32_t AVSessionDemo::SetExtras(const AAFwk::WantParams& extras)
{
    return 0;
}

std::shared_ptr<AVSessionController> AVSessionDemo::GetController()
{
    return nullptr;
}

int32_t AVSessionDemo::RegisterCallback(const std::shared_ptr<AVSessionCallback>& callback)
{
    return 0;
}

int32_t AVSessionDemo::Activate()
{
    return 0;
}

int32_t AVSessionDemo::Deactivate()
{
    return 0;
}

bool AVSessionDemo::IsActive()
{
    return false;
}

int32_t AVSessionDemo::Destroy()
{
    return 0;
}

int32_t AVSessionDemo::AddSupportCommand(const int32_t cmd)
{
    return 0;
}

int32_t AVSessionDemo::DeleteSupportCommand(const int32_t cmd)
{
    return 0;
}

int32_t AVSessionDemo::SetSessionEvent(const std::string& event, const AAFwk::WantParams& args)
{
    return 0;
}

#ifdef CASTPLUS_CAST_ENGINE_ENABLE
std::shared_ptr<AVCastController> AVSessionDemo::GetAVCastController()
{
    return nullptr;
}

int32_t AVSessionDemo::ReleaseCast(bool continuePlay = false)
{
    return 0;
}

int32_t AVSessionDemo::StartCastDisplayListener()
{
    return 0;
}

int32_t AVSessionDemo::StopCastDisplayListener()
{
    return 0;
}

int32_t AVSessionDemo::GetAllCastDisplays(std::vector<CastDisplayInfo>& castDisplays)
{
    return 0;
}
#endif

class AVSessionCallbackImpl : public AVSessionCallback {
public:
    void OnPlay() override;
    void OnPause() override;
    void OnStop() override;
    void OnPlayNext() override;
    void OnPlayPrevious() override;
    void OnFastForward(int64_t time) override;
    void OnRewind(int64_t time) override;
    void OnSeek(int64_t time) override;
    void OnSetSpeed(double speed) override;
    void OnSetLoopMode(int32_t loopMode) override;
    void OnSetTargetLoopMode(int32_t targetLoopMode) override;
    void OnToggleFavorite(const std::string& mediaId) override;
    void OnMediaKeyEvent(const OHOS::MMI::KeyEvent& keyEvent) override;
    void OnOutputDeviceChange(const int32_t connectionState,
        const OHOS::AVSession::OutputDeviceInfo& outputDeviceInfo) override {};
    void OnCommonCommand(const std::string& commonCommand, const OHOS::AAFwk::WantParams& commandArgs) override;
    void OnSkipToQueueItem(int32_t itemId) override {};
    void OnAVCallAnswer() override {};
    void OnAVCallHangUp() override {};
    void OnAVCallToggleCallMute() override {};
    void OnPlayFromAssetId(int64_t assetId) override;
    void OnPlayWithAssetId(const std::string& assetId) override;
    void OnCastDisplayChange(const CastDisplayInfo& castDisplayInfo) override;

    ~AVSessionCallbackImpl() override;
};

void AVSessionCallbackImpl::OnPlay()
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnPlay %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnPause()
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnPause %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnStop()
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnStop %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnPlayNext()
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnPlayNext %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnPlayPrevious()
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnPlayPrevious %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnFastForward(int64_t time)
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnFastForward %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnRewind(int64_t time)
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnRewind %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnSeek(int64_t time)
{
    SLOGE("OnSeek %{public}" PRId64, time);
    g_onCall = AVSESSION_SUCCESS;
}

void AVSessionCallbackImpl::OnSetSpeed(double speed)
{
    SLOGE("OnSetSpeed %{public}f", speed);
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnSetSpeed %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnSetLoopMode(int32_t loopMode)
{
    SLOGE("OnSetLoopMode %{public}d", loopMode);
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnSetLoopMode %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnSetTargetLoopMode(int32_t targetLoopMode)
{
    SLOGE("OnSetTargetLoopMode %{public}d", targetLoopMode);
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnSetTargetLoopMode %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnToggleFavorite(const std::string& mediaId)
{
    SLOGE("OnToggleFavorite %{public}s", mediaId.c_str());
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnToggleFavorite %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnMediaKeyEvent(const OHOS::MMI::KeyEvent& keyEvent)
{
    SLOGE("OnMediaKeyEvent");
    g_onCall = AVSESSION_SUCCESS;
}

void AVSessionCallbackImpl::OnCommonCommand(const std::string& commonCommand,
    const OHOS::AAFwk::WantParams& commandArgs)
{
    SLOGI("OnCommonCommand");
    g_onCall = AVSESSION_SUCCESS;
}

void AVSessionCallbackImpl::OnPlayFromAssetId(int64_t assetId)
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnPlayFromAssetId %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnPlayWithAssetId(const std::string& assetId)
{
    g_onCall = AVSESSION_SUCCESS;
    SLOGE("OnPlayWithAssetId %{public}d", g_onCall);
}

void AVSessionCallbackImpl::OnCastDisplayChange(const CastDisplayInfo& castDisplayInfo)
{
    SLOGE("OnCastDisplayChange");
    g_onCall = AVSESSION_SUCCESS;
}

AVSessionCallbackImpl::~AVSessionCallbackImpl()
{
}

/**
 * @tc.name: SetAVCallMetaData001
 * @tc.desc: Return the result of set avcall meta data
 * @tc.type: FUNC
 */
HWTEST_F(AvsessionTest, SetAVCallMetaData001, TestSize.Level1)
{
    SLOGE("SetAVCallMetaData001 Begin");
    g_avCallMetaData.Reset();
    g_avCallMetaData.SetName("");
    g_avCallMetaData.SetPhoneNumber("123456789");

    EXPECT_EQ(avsession_->SetAVCallMetaData(g_avCallMetaData), AVSESSION_SUCCESS);
    SLOGE("SetAVCallMetaData001 enable optional params End");
}

/**
 * @tc.name: SetAVCallMetaData002
 * @tc.desc: Return the result of set avcall meta data
 * @tc.type: FUNC
 */
HWTEST_F(AvsessionTest, SetAVCallMetaData002, TestSize.Level1)
{
    SLOGE("SetAVCallMetaData002 Begin");
    g_avCallMetaData.Reset();
    g_avCallMetaData.SetName("xiaoming");
    g_avCallMetaData.SetPhoneNumber("123456789");

    EXPECT_EQ(avsession_->SetAVCallMetaData(g_avCallMetaData), AVSESSION_SUCCESS);
    SLOGE("SetAVCallMetaData002 End");
}

/**
 * @tc.name: SetAVCallState001
 * @tc.desc: Return the result of set avcall meta data
 * @tc.type: FUNC
 */
HWTEST_F(AvsessionTest, SetAVCallState001, TestSize.Level1)
{
    SLOGE("SetAVCallState001 Begin");
    g_avCallState.SetAVCallState(2);
    g_avCallState.SetAVCallMuted(false);

    EXPECT_EQ(avsession_->SetAVCallState(g_avCallState), AVSESSION_SUCCESS);
    SLOGE("SetAVCallState001 End");
}

/**
* @tc.name: GetSessionId001
* @tc.desc: Return is Session ID
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, GetSessionId001, TestSize.Level1)
{
    SLOGE("GetSessionId001 Begin");
    auto sessionId = avsession_->GetSessionId();
    EXPECT_EQ(sessionId.length() == SESSION_LEN, true);
    SLOGE("GetSessionId001 End");
}

/**
* @tc.name: SetAVMetaData001
* @tc.desc: Return the result of set av meta data
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, SetAVMetaData001, TestSize.Level1)
{
    SLOGE("SetAVMetaData001 Begin");
    g_metaData.Reset();
    g_metaData.SetAssetId("");
    g_metaData.SetTitle("Black Humor");
    g_metaData.SetArtist("zhoujielun");
    g_metaData.SetAuthor("zhoujielun");
    g_metaData.SetAlbum("Jay");
    g_metaData.SetWriter("zhoujielun");
    g_metaData.SetComposer("zhoujielun");
    g_metaData.SetDuration(40000);
    g_metaData.SetMediaImageUri("xxxxx");
    g_metaData.SetSubTitle("fac");
    g_metaData.SetDescription("for friends");
    g_metaData.SetLyric("xxxxx");
    EXPECT_EQ(avsession_->SetAVMetaData(g_metaData), AVSESSION_ERROR);
    SLOGE("SetAVMetaData001 End");
}

/**
* @tc.name: SetAVMetaData002
* @tc.desc: Return the result of set av meta data
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, SetAVMetaData002, TestSize.Level1)
{
    SLOGE("SetAVMetaData002 Begin");
    g_metaData.Reset();
    g_metaData.SetAssetId("123");
    g_metaData.SetTitle("Black Humor");
    g_metaData.SetArtist("zhoujielun");
    g_metaData.SetAuthor("zhoujielun");
    g_metaData.SetAlbum("Jay");
    g_metaData.SetWriter("zhoujielun");
    g_metaData.SetComposer("zhoujielun");
    g_metaData.SetDuration(40000);
    g_metaData.SetMediaImageUri("xxxxx");
    g_metaData.SetSubTitle("fac");
    g_metaData.SetDescription("for friends");
    g_metaData.SetLyric("xxxxx");
    EXPECT_EQ(avsession_->SetAVMetaData(g_metaData), AVSESSION_SUCCESS);
    SLOGE("SetAVMetaData002 End");
}

/**
* @tc.name: GetAVMetaData001
* @tc.desc: Return get av meta data result
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, GetAVMetaData001, TestSize.Level1)
{
    SLOGE("GetAVMetaData001 Begin");
    EXPECT_EQ(avsession_->SetAVMetaData(g_metaData), AVSESSION_SUCCESS);
    AVMetaData metaData;
    metaData.Reset();
    EXPECT_EQ(avsession_->GetAVMetaData(metaData), AVSESSION_SUCCESS);
    EXPECT_EQ(metaData.GetAssetId(), g_metaData.GetAssetId());
    std::string title1 = metaData.GetTitle();
    SLOGE("title1 %{public}s", title1.c_str());
    std::string title2 = g_metaData.GetTitle();
    SLOGE("title2 %{public}s", title2.c_str());
    EXPECT_EQ(title1, title2);
    EXPECT_EQ(metaData.GetTitle(), g_metaData.GetTitle());
    EXPECT_EQ(metaData.GetArtist(), g_metaData.GetArtist());
    EXPECT_EQ(metaData.GetAuthor(), g_metaData.GetAuthor());
    EXPECT_EQ(metaData.GetAlbum(), g_metaData.GetAlbum());
    EXPECT_EQ(metaData.GetWriter(), g_metaData.GetWriter());
    EXPECT_EQ(metaData.GetComposer(), g_metaData.GetComposer());
    EXPECT_EQ(metaData.GetDuration(), g_metaData.GetDuration());
    EXPECT_EQ(metaData.GetMediaImageUri(), g_metaData.GetMediaImageUri());
    EXPECT_EQ(metaData.GetSubTitle(), g_metaData.GetSubTitle());
    EXPECT_EQ(metaData.GetDescription(), g_metaData.GetDescription());
    EXPECT_EQ(metaData.GetLyric(), g_metaData.GetLyric());
    SLOGE("GetAVMetaData001 End");
}

/**
* @tc.name: SetAVPlaybackState001
* @tc.desc: Return the result of set av playback state
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, SetAVPlaybackState001, TestSize.Level1)
{
    SLOGE("SetAVPlaybackState001 Begin");
    AVMetaData metaData;
    metaData.Reset();
    metaData.SetAssetId("playback");
    metaData.SetDuration(20);
    EXPECT_EQ(avsession_->SetAVMetaData(metaData), AVSESSION_SUCCESS);
    g_playbackState.SetState(1);
    g_playbackState.SetSpeed(1.5);
    g_playbackState.SetPosition({80000, 0});
    g_playbackState.SetBufferedTime(60000);
    g_playbackState.SetLoopMode(2);
    g_playbackState.SetFavorite(true);
    EXPECT_EQ(avsession_->SetAVPlaybackState(g_playbackState), AVSESSION_SUCCESS);
    SLOGE("SetAVPlaybackState001 End");
}

/**
* @tc.name: SetAVPlaybackState002
* @tc.desc: Return the result of set av playback state
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, SetAVPlaybackState002, TestSize.Level1)
{
    SLOGE("SetAVPlaybackState002 Begin");
    g_playbackState.SetState(1);
    g_playbackState.SetSpeed(1);
    g_playbackState.SetPosition({80000, 0});
    g_playbackState.SetBufferedTime(700000);
    g_playbackState.SetLoopMode(1);
    g_playbackState.SetFavorite(true);
    EXPECT_EQ(avsession_->SetAVPlaybackState(g_playbackState), AVSESSION_SUCCESS);
    SLOGE("SetAVPlaybackState002 End");
}

/**
* @tc.name: SetAVPlaybackState003
* @tc.desc: Return the result of set av playback state
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, SetAVPlaybackState003, TestSize.Level1)
{
    SLOGE("SetAVPlaybackState003 Begin");
    AVMetaData metaData;
    metaData.Reset();
    metaData.SetAssetId("playback");
    metaData.SetDuration(2000000);
    EXPECT_EQ(avsession_->SetAVMetaData(metaData), AVSESSION_SUCCESS);
    g_playbackState.SetState(1);
    g_playbackState.SetSpeed(1);
    g_playbackState.SetPosition({80000, 0});
    g_playbackState.SetBufferedTime(17000);
    g_playbackState.SetLoopMode(1);
    g_playbackState.SetFavorite(true);
    EXPECT_EQ(avsession_->SetAVPlaybackState(g_playbackState), AVSESSION_SUCCESS);
    SLOGE("SetAVPlaybackState003 End");
}

/**
* @tc.name: SetAVPlaybackState004
* @tc.desc: Return the result of set av playback state
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, SetAVPlaybackState004, TestSize.Level1)
{
    SLOGE("SetAVPlaybackState004 Begin");
    AVMetaData metaData;
    metaData.Reset();
    metaData.SetAssetId("playback");
    metaData.SetDuration(2000000);
    EXPECT_EQ(avsession_->SetAVMetaData(metaData), AVSESSION_SUCCESS);
    g_playbackState.SetState(1);
    g_playbackState.SetSpeed(1);
    g_playbackState.SetPosition({10000, 0});
    g_playbackState.SetBufferedTime(17000);
    g_playbackState.SetLoopMode(1);
    g_playbackState.SetFavorite(true);
    EXPECT_EQ(avsession_->SetAVPlaybackState(g_playbackState), AVSESSION_SUCCESS);
    SLOGE("SetAVPlaybackState004 End");
}

/**
* @tc.name: SetSessionEventTest001
* @tc.desc: Set session event
* @tc.type: FUNC
* @tc.require: I6C6IN
*/
HWTEST_F(AvsessionTest, SetSessionEventTest001, TestSize.Level1)
{
    SLOGE("SetSessionEventTest001 Begin");
    std::string event = "eventName";
    OHOS::AAFwk::WantParams args;
    EXPECT_EQ(avsession_->SetSessionEvent(event, args), AVSESSION_SUCCESS);
    SLOGE("SetSessionEventTest001 End");
}

/**
* @tc.name: SetSessionEventTest002
* @tc.desc: Set session event - large number of calls
* @tc.type: FUNC
* @tc.require: I6C6IN
*/
HWTEST_F(AvsessionTest, SetSessionEventTest002, TestSize.Level2)
{
    SLOGE("SetSessionEventTest002 Begin");
    std::string event = "eventName";
    OHOS::AAFwk::WantParams args;

    // Test the interface through 500 calls
    for (int i = 0; i < 500; i++) {
        EXPECT_EQ(avsession_->SetSessionEvent(event, args), AVSESSION_SUCCESS);
    }
    SLOGE("SetSessionEventTest002 End");
}

/**
* @tc.name: GetAVPlaybackState001
* @tc.desc: Return the result of get av playback state
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, GetAVPlaybackState001, TestSize.Level1)
{
    SLOGE("GetAVPlaybackState001 Begin");
    AVMetaData metaData;
    metaData.Reset();
    metaData.SetAssetId("playback");
    metaData.SetDuration(2000000);
    EXPECT_EQ(avsession_->SetAVMetaData(metaData), AVSESSION_SUCCESS);
    g_playbackState.SetState(1);
    g_playbackState.SetSpeed(1);
    g_playbackState.SetPosition({80000, 0});
    g_playbackState.SetBufferedTime(87000);
    g_playbackState.SetLoopMode(1);
    g_playbackState.SetFavorite(true);
    EXPECT_EQ(avsession_->SetAVPlaybackState(g_playbackState), AVSESSION_SUCCESS);
    AVPlaybackState state;
    EXPECT_EQ(avsession_->GetAVPlaybackState(state), AVSESSION_SUCCESS);
    EXPECT_EQ(state.GetState(), g_playbackState.GetState());
    EXPECT_EQ(state.GetSpeed(), g_playbackState.GetSpeed());
    EXPECT_EQ(state.GetPosition().elapsedTime_, g_playbackState.GetPosition().elapsedTime_);
    EXPECT_EQ(state.GetBufferedTime(), g_playbackState.GetBufferedTime());
    EXPECT_EQ(state.GetLoopMode(), g_playbackState.GetLoopMode());
    EXPECT_EQ(state.GetFavorite(), g_playbackState.GetFavorite());
    SLOGE("GetAVPlaybackState001 End");
}

/**
* @tc.name: SetLaunchAbility001
* @tc.desc: Return the result of set av launch ability
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, SetLaunchAbility001, TestSize.Level1)
{
    SLOGE("SetLaunchAbility001 Begin");
    OHOS::AbilityRuntime::WantAgent::WantAgent ability;
    EXPECT_EQ(avsession_->SetLaunchAbility(ability), AVSESSION_SUCCESS);
    SLOGE("SetLaunchAbility001 End");
}

/**
* @tc.name: GetController001
* @tc.desc: Return shared_ptr of controller
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, GetController001, TestSize.Level1)
{
    SLOGE("GetController001 Begin");
    auto controller = avsession_->GetController();
    EXPECT_NE(controller, nullptr);
    SLOGE("GetController001 End");
}

/**
* @tc.name: GetController002
* @tc.desc: Return shared_ptr of controller
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, GetController002, TestSize.Level1)
{
    SLOGE("GetController002 Begin");
    auto controller = avsession_->GetController();
    auto controller1 = avsession_->GetController();
    EXPECT_NE(controller, nullptr);
    EXPECT_NE(controller1, nullptr);
    EXPECT_EQ(controller->Destroy(), AVSESSION_SUCCESS);
    EXPECT_EQ(controller1->Destroy(), ERR_CONTROLLER_NOT_EXIST);
    SLOGE("GetController002 End");
}

/**
* @tc.name: SetAVQueueItems
* @tc.desc: Set queue items of current application
* @tc.type: FUNC
* @tc.require: I6RJST
*/
HWTEST_F(AvsessionTest, SetAVQueueItems, TestSize.Level1)
{
    SLOGE("SetAVQueueItems Begin");
    std::vector<AVQueueItem> items;
    AVQueueItem queueItem;
    queueItem.SetItemId(1);
    AVMediaDescription description;
    description.SetMediaId("id");
    queueItem.SetDescription(std::make_shared<AVMediaDescription>(description));
    items.push_back(queueItem);
    EXPECT_EQ(avsession_->SetAVQueueItems(items), AVSESSION_SUCCESS);
    SLOGE("SetAVQueueItems End");
}

/**
* @tc.name: SetAVQueueTitle
* @tc.desc: Set queue title of current application
* @tc.type: FUNC
* @tc.require: I6RJST
*/
HWTEST_F(AvsessionTest, SetAVQueueTitle, TestSize.Level1)
{
    SLOGE("SetAVQueueTitle Begin");
    std::string title = "AVQueueTitle";
    EXPECT_EQ(avsession_->SetAVQueueTitle(title), AVSESSION_SUCCESS);
    SLOGE("SetAVQueueTitle End");
}

/**
* @tc.name: SetExtrasTest001
* @tc.desc: Set custom media packets
* @tc.type: FUNC
* @tc.require: I6TD43
*/
HWTEST_F(AvsessionTest, SetExtrasTest001, TestSize.Level1)
{
    SLOGI("SetExtrasTest001 Begin");
    std::shared_ptr<OHOS::AAFwk::WantParams> wantParamsIn = nullptr;
    wantParamsIn = std::make_shared<OHOS::AAFwk::WantParams>();
    std::string keyStr = "1234567";
    bool valueBool = true;
    wantParamsIn->SetParam(keyStr, OHOS::AAFwk::Boolean::Box(valueBool));

    EXPECT_EQ(avsession_->SetExtras(*wantParamsIn), AVSESSION_SUCCESS);
    SLOGI("SetExtrasTest001 End");
}

/**
* @tc.name: SetExtrasTest002
* @tc.desc: Set custom media packets - large number of calls
* @tc.type: FUNC
* @tc.require: I6TD43
*/
HWTEST_F(AvsessionTest, SetExtrasTest002, TestSize.Level2)
{
    SLOGI("SetExtrasTest002 Begin");
    std::shared_ptr<OHOS::AAFwk::WantParams> wantParamsIn = nullptr;
    wantParamsIn = std::make_shared<OHOS::AAFwk::WantParams>();
    std::string keyStr = "1234567";
    bool valueBool = true;
    wantParamsIn->SetParam(keyStr, OHOS::AAFwk::Boolean::Box(valueBool));

    // Test the interface through 500 calls
    for (int i = 0; i < 500; i++) {
        EXPECT_EQ(avsession_->SetExtras(*wantParamsIn), AVSESSION_SUCCESS);
    }
    SLOGI("SetExtrasTest002 End");
}

/**
* @tc.name: RegisterCallback001
* @tc.desc: register avsession callback
* @tc.type: FUNC
* @tc.require: AR000H31JG
*/
HWTEST_F(AvsessionTest, RegisterCallback001, TestSize.Level1)
{
    SLOGE("RegisterCallback001 Begin");
    std::shared_ptr<AVSessionCallback> callback = std::make_shared<AVSessionCallbackImpl>();
    EXPECT_EQ(avsession_->RegisterCallback(callback), AVSESSION_SUCCESS);
    SLOGE("RegisterCallback001 End");
}

/**
* @tc.name: RegisterCallback002
* @tc.desc: register avsession callback
* @tc.type: FUNC
* @tc.require: AR000H31JG
*/
HWTEST_F(AvsessionTest, RegisterCallback002, TestSize.Level1)
{
    SLOGE("RegisterCallback002 Begin");
    std::shared_ptr<AVSessionCallback> callback = nullptr;
    EXPECT_NE(avsession_->RegisterCallback(callback), AVSESSION_SUCCESS);
    SLOGE("RegisterCallback002 End");
}

/**
* @tc.name: RegisterCallback003
* @tc.desc: register avsession callback
* @tc.type: FUNC
* @tc.require: AR000H31JG
*/
HWTEST_F(AvsessionTest, RegisterCallback003, TestSize.Level1)
{
    SLOGE("RegisterCallback003 Begin");
    std::shared_ptr<AVSessionCallback> callback = std::make_shared<AVSessionCallbackImpl>();
    EXPECT_EQ(avsession_->RegisterCallback(callback), AVSESSION_SUCCESS);
    for (int32_t cmd = AVControlCommand::SESSION_CMD_PLAY; cmd < AVControlCommand::SESSION_CMD_MAX; cmd++) {
        if (cmd == AVControlCommand::SESSION_CMD_MEDIA_KEY_SUPPORT) {
            continue;
        }
        AVControlCommand controlCommand;
        EXPECT_EQ(avsession_->AddSupportCommand(cmd), AVSESSION_SUCCESS);
        EXPECT_EQ(controlCommand.SetCommand(cmd), AVSESSION_SUCCESS);
        switch (cmd) {
            case AVControlCommand::SESSION_CMD_SEEK : controlCommand.SetSeekTime(100000);
                break;
            case AVControlCommand::SESSION_CMD_SET_SPEED : controlCommand.SetSpeed(1.5);
                break;
            case AVControlCommand::SESSION_CMD_SET_LOOP_MODE : controlCommand.SetLoopMode(2);
                break;
            case AVControlCommand::SESSION_CMD_SET_TARGET_LOOP_MODE : controlCommand.SetTargetLoopMode(2);
                break;
            case AVControlCommand::SESSION_CMD_TOGGLE_FAVORITE : controlCommand.SetAssetId("callback");
                break;
            case AVControlCommand::SESSION_CMD_FAST_FORWARD : controlCommand.SetForwardTime(10);
                break;
            case AVControlCommand::SESSION_CMD_REWIND : controlCommand.SetRewindTime(10);
                break;
            case AVControlCommand::SESSION_CMD_PLAY_FROM_ASSETID: controlCommand.SetPlayFromAssetId(0);
                break;
            case AVControlCommand::SESSION_CMD_PLAY_WITH_ASSETID: controlCommand.SetPlayWithAssetId("0");
                break;
            default:
                break;
        }
        bool isActive = false;
        controller_->IsSessionActive(isActive);
        if (!isActive) {
            avsession_->Activate();
        }
        EXPECT_EQ(controller_->SendControlCommand(controlCommand), AVSESSION_SUCCESS);
        sleep(1);
        EXPECT_EQ(g_onCall, AVSESSION_SUCCESS);
        g_onCall = false;
    }
    SLOGE("RegisterCallback003 End");
}

/**
* @tc.name: RegisterCallback004
* @tc.desc: register avsession callback
* @tc.type: FUNC
* @tc.require: AR000H31JG
*/
HWTEST_F(AvsessionTest, RegisterCallback004, TestSize.Level1)
{
    SLOGE("RegisterCallback004 Begin");
    std::shared_ptr<AVSessionCallback> callback001 = std::make_shared<AVSessionCallbackImpl>();
    EXPECT_NE(callback001, nullptr);
    EXPECT_EQ(avsession_->RegisterCallback(callback001), AVSESSION_SUCCESS);
    EXPECT_EQ(avsession_->RegisterCallback(callback001), AVSESSION_SUCCESS);
    std::shared_ptr<AVSessionCallback> callback002 = std::make_shared<AVSessionCallbackImpl>();
    EXPECT_NE(callback002, nullptr);
    EXPECT_EQ(avsession_->RegisterCallback(callback002), AVSESSION_SUCCESS);
    SLOGE("RegisterCallback004 End");
}

/**
* @tc.name: Active001
* @tc.desc: Return the active result
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, Activate001, TestSize.Level1)
{
    SLOGE("Activate001 Begin");
    bool isActive = false;
    EXPECT_EQ(avsession_->Activate(), AVSESSION_SUCCESS);
    isActive = avsession_->IsActive();
    EXPECT_EQ(isActive, true);
    SLOGE("Activate001 End");
}

/**
* @tc.name: Disactive001
* @tc.desc: Return is Session Actived
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, Deactivate001, TestSize.Level1)
{
    SLOGE("Deactivate001 Begin");
    bool isActive = true;
    EXPECT_EQ(avsession_->Deactivate(), AVSESSION_SUCCESS);
    isActive = avsession_->IsActive();
    EXPECT_EQ(isActive, false);
    SLOGE("Deactivate001 End");
}

/**
* @tc.name: AddSupportCommand001
* @tc.desc: add supported commands
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, AddSupportCommand001, TestSize.Level1)
{
    SLOGE("AddSupportCommand001 Begin");
    EXPECT_EQ(avsession_->AddSupportCommand(AVControlCommand::SESSION_CMD_PLAY), AVSESSION_SUCCESS);
    EXPECT_EQ(avsession_->AddSupportCommand(AVControlCommand::SESSION_CMD_PAUSE), AVSESSION_SUCCESS);
    SLOGE("AddSupportCommand001 End");
}

/**
* @tc.name: AddSupportCommand002
* @tc.desc: add supported commands
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, AddSupportCommand002, TestSize.Level1)
{
    SLOGE("AddSupportCommand002 Begin");
    EXPECT_EQ(avsession_->AddSupportCommand(AVControlCommand::SESSION_CMD_INVALID), AVSESSION_ERROR);
    EXPECT_EQ(avsession_->AddSupportCommand(AVControlCommand::SESSION_CMD_MAX), AVSESSION_ERROR);
    SLOGE("AddSupportCommand002 End");
}

/**
* @tc.name: AddSupportCommand003
* @tc.desc: add supported commands
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, AddSupportCommand003, TestSize.Level1)
{
    SLOGE("AddSupportCommand003 Begin");
    for (int32_t cmd = AVControlCommand::SESSION_CMD_PLAY; cmd < AVControlCommand::SESSION_CMD_MAX; cmd++) {
        EXPECT_EQ(avsession_->AddSupportCommand(cmd), AVSESSION_SUCCESS);
    }
    std::vector<int32_t> cmds;
    EXPECT_EQ(controller_->GetValidCommands(cmds), AVSESSION_SUCCESS);
    EXPECT_EQ(cmds.size(), (AVControlCommand::SESSION_CMD_MAX - 1));
    for (int32_t index = 0; index < cmds.size(); index++) {
        EXPECT_EQ(cmds[index] > AVControlCommand::SESSION_CMD_INVALID, true);
        EXPECT_EQ(cmds[index] < AVControlCommand::SESSION_CMD_MAX, true);
        if (index >= AVControlCommand::SESSION_CMD_MEDIA_KEY_SUPPORT) {
            EXPECT_EQ(cmds[index], index + 1);
        } else {
            EXPECT_EQ(cmds[index], index);
        }
    }
    SLOGE("AddSupportCommand003 End");
}

/**
* @tc.name: deleteSupportCommand001
* @tc.desc: delete supported commands
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, deleteSupportCommand001, TestSize.Level1)
{
    SLOGE("deleteSupportCommand001 Begin");
    for (int32_t cmd = AVControlCommand::SESSION_CMD_PLAY; cmd < AVControlCommand::SESSION_CMD_MAX; cmd++) {
        EXPECT_EQ(avsession_->AddSupportCommand(cmd), AVSESSION_SUCCESS);
    }
    std::vector<int32_t> cmds;
    EXPECT_EQ(controller_->GetValidCommands(cmds), AVSESSION_SUCCESS);
    EXPECT_EQ(cmds.size(), (AVControlCommand::SESSION_CMD_MAX - 1));
    for (int32_t cmd = AVControlCommand::SESSION_CMD_PLAY; cmd < AVControlCommand::SESSION_CMD_MAX; cmd++) {
        EXPECT_EQ(avsession_->DeleteSupportCommand(cmd), AVSESSION_SUCCESS);
    }
    cmds.clear();
    EXPECT_EQ(controller_->GetValidCommands(cmds), AVSESSION_SUCCESS);
    EXPECT_EQ(cmds.size(), 0);
    SLOGE("deleteSupportCommand001 End");
}

/**
* @tc.name: deleteSupportCommand002
* @tc.desc: delete supported commands
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, deleteSupportCommand002, TestSize.Level1)
{
    SLOGE("deleteSupportCommand002 Begin");
    for (int32_t cmd = AVControlCommand::SESSION_CMD_PLAY; cmd < AVControlCommand::SESSION_CMD_MAX; cmd++) {
        EXPECT_EQ(avsession_->DeleteSupportCommand(cmd), AVSESSION_SUCCESS);
    }
    SLOGE("deleteSupportCommand002 End");
}

/**
* @tc.name: deleteSupportCommand003
* @tc.desc: delete supported commands
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, deleteSupportCommand003, TestSize.Level1)
{
    SLOGE("deleteSupportCommand003 Begin");
    for (int32_t cmd = AVControlCommand::SESSION_CMD_PLAY; cmd < AVControlCommand::SESSION_CMD_MAX; cmd++) {
        EXPECT_EQ(avsession_->AddSupportCommand(cmd), AVSESSION_SUCCESS);
    }
    std::vector<int32_t> cmds;
    EXPECT_EQ(controller_->GetValidCommands(cmds), AVSESSION_SUCCESS);
    EXPECT_EQ(cmds.size(), (AVControlCommand::SESSION_CMD_MAX - 1));
    EXPECT_EQ(avsession_->DeleteSupportCommand(AVControlCommand::SESSION_CMD_INVALID), AVSESSION_ERROR);
    EXPECT_EQ(avsession_->DeleteSupportCommand(AVControlCommand::SESSION_CMD_MAX), AVSESSION_ERROR);
    SLOGE("deleteSupportCommand003 End");
}

/**
* @tc.name: Destroy001
* @tc.desc: Return is Session destroyed result
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, Destroy001, TestSize.Level1)
{
    SLOGE("Destroy001 Begin");
    EXPECT_EQ(avsession_->Destroy(), AVSESSION_SUCCESS);
    avsession_ = nullptr;
    SLOGE("Destroy001 End");
}

/**
* @tc.name: Destroy002
* @tc.desc: Return is Session destroyed result
* @tc.type: FUNC
* @tc.require: AR000H31JF
*/
HWTEST_F(AvsessionTest, Destroy002, TestSize.Level1)
{
    SLOGE("Destroy002 Begin");
    EXPECT_EQ(avsession_->Destroy(), AVSESSION_SUCCESS);
    EXPECT_EQ(avsession_->Destroy(), ERR_SESSION_NOT_EXIST);
    avsession_ = nullptr;
    SLOGE("Destroy002 End");
}

/**
* @tc.name: GetSessionType001
* @tc.desc: GetSessionType
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(AvsessionTest, GetSessionType001, TestSize.Level1)
{
    SLOGD("GetSessionType001 Begin");
    EXPECT_EQ(avsession_->GetSessionType(), "audio");
    SLOGD("GetSessionType001 End");
}

/**
* @tc.name: GetAVQueueItems001
* @tc.desc: get que items
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(AvsessionTest, GetAVQueueItems001, TestSize.Level1)
{
    SLOGD("GetAVQueueItems001 Begin");
    std::vector<AVQueueItem> items;
    EXPECT_EQ(avsession_->GetAVQueueItems(items), AVSESSION_SUCCESS);
    SLOGD("GetAVQueueItems001 End");
}

/**
* @tc.name: GetAVQueueTitle001
* @tc.desc: get av que title
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(AvsessionTest, GetAVQueueTitle001, TestSize.Level1)
{
    SLOGD("GetAVQueueTitle001 Begin");
    std::string title1 = "AVQueueTitle";
    std::string title2;
    EXPECT_EQ(avsession_->SetAVQueueTitle(title1), AVSESSION_SUCCESS);
    EXPECT_EQ(avsession_->GetAVQueueTitle(title2), AVSESSION_SUCCESS);
    SLOGD("GetAVQueueTitle001 End");
}

/**
* @tc.name: GetExtras001
* @tc.desc: get extras
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(AvsessionTest, GetExtras001, TestSize.Level1)
{
    SLOGD("GetExtras001 Begin");
    std::shared_ptr<OHOS::AAFwk::WantParams> wantParamsIn = nullptr;
    wantParamsIn = std::make_shared<OHOS::AAFwk::WantParams>();
    std::string keyStr = "1234567";
    bool valueBool = true;
    wantParamsIn->SetParam(keyStr, OHOS::AAFwk::Boolean::Box(valueBool));
    OHOS::AAFwk::WantParams wantParamsOut;
    EXPECT_EQ(avsession_->SetExtras(*wantParamsIn), AVSESSION_SUCCESS);
    EXPECT_EQ(avsession_->GetExtras(wantParamsOut), AVSESSION_SUCCESS);
    SLOGD("GetExtras001 End");
}

#ifdef CASTPLUS_CAST_ENGINE_ENABLE
/**
* @tc.name: GetAllCastDisplays001
* @tc.desc: get all cast displays
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(AvsessionTest, GetAllCastDisplays001, TestSize.Level1)
{
    SLOGD("GetAllCastDisplays001 Begin");
    std::vector<CastDisplayInfo> castDisplays;
    EXPECT_EQ(avsession_->GetAllCastDisplays(castDisplays), AVSESSION_SUCCESS);
    SLOGD("GetAllCastDisplays001 End");
}
#endif

/**
* @tc.name: UpdateAVQueueInfo001
* @tc.desc: update avqueue info
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(AvsessionTest, UpdateAVQueueInfo001, TestSize.Level1)
{
    SLOGD("UpdateAVQueueInfo001 Begin");
    AVQueueInfo info = AVQueueInfo();
    EXPECT_EQ(avsession_->UpdateAVQueueInfo(info), AVSESSION_SUCCESS);
    SLOGD("UpdateAVQueueInfo001 End");
}

/**
* @tc.name: AVSessionDemoUpdateAVQueueInfo001
* @tc.desc: create AVSessionDemo and UpdateAVQueueInfo
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(AvsessionTest, AVSessionDemoUpdateAVQueueInfo001, TestSize.Level1)
{
    SLOGE("AVSessionDemoUpdateAVQueueInfo001 Begin");
    AVSessionDemo avsessionDemo = AVSessionDemo();
    AVQueueInfo info = AVQueueInfo();
    EXPECT_EQ(avsessionDemo.UpdateAVQueueInfo(info), AVSESSION_SUCCESS);
    SLOGE("AVSessionDemoUpdateAVQueueInfo001 End");
}
} // namespace AVSession
} // namespace OHOS

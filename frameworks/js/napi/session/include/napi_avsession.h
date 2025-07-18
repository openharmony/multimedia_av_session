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

#ifndef OHOS_NAPI_AVSESSION_H
#define OHOS_NAPI_AVSESSION_H

#include <chrono>
#include <mutex>

#include <map>
#include "napi_async_work.h"
#include "napi_base_context.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "av_session.h"
#include "avsession_info.h"
#include "avsession_log.h"
#include "avcontrol_command.h"
#include "avsession_errors.h"
#include "napi_avsession_callback.h"
#include "want_agent.h"
#include "avsession_event_handler.h"

namespace OHOS::AVSession {
class NapiAVSession {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_status NewInstance(napi_env env, std::shared_ptr<AVSession>& nativeSession, napi_value& out,
        std::string tag, AppExecFwk::ElementName elementName);
    static napi_status ReCreateInstance();

    NapiAVSession();
    ~NapiAVSession();

    using OnEventHandlerType = std::function<napi_status(napi_env, NapiAVSession*, napi_value)>;
    using OffEventHandlerType = std::function<napi_status(napi_env, NapiAVSession*, napi_value)>;

    static constexpr size_t TIME_OUT_SECOND = 5;

private:
    static napi_value ConstructorCallback(napi_env env, napi_callback_info info);

    static napi_value OnEvent(napi_env env, napi_callback_info info);
    static napi_value OffEvent(napi_env env, napi_callback_info info);

    static napi_value SetAVCallMetaData(napi_env env, napi_callback_info info);
    static napi_value SetAVCallState(napi_env env, napi_callback_info info);
    static napi_value SetAVMetaData(napi_env env, napi_callback_info info);
    static napi_value SetAVPlaybackState(napi_env env, napi_callback_info info);
    static napi_value SetLaunchAbility(napi_env env, napi_callback_info info);
    static napi_value SetAudioStreamId(napi_env env, napi_callback_info info);
    static napi_value GetController(napi_env env, napi_callback_info info);
    static napi_value GetAVCastController(napi_env env, napi_callback_info info);
    static napi_value GetOutputDevice(napi_env env, napi_callback_info info);
    static napi_value GetOutputDeviceSync(napi_env env, napi_callback_info info);
    static napi_value Activate(napi_env env, napi_callback_info info);
    static napi_value Deactivate(napi_env env, napi_callback_info info);
    static napi_value Destroy(napi_env env, napi_callback_info info);
    static napi_value SetSessionEvent(napi_env env, napi_callback_info info);
    static napi_value SetAVQueueItems(napi_env env, napi_callback_info info);
    static napi_value SetAVQueueTitle(napi_env env, napi_callback_info info);
    static napi_value SetExtras(napi_env env, napi_callback_info info);
    static napi_value ReleaseCast(napi_env env, napi_callback_info info);
    static napi_value GetAllCastDisplays(napi_env env, napi_callback_info info);
    static napi_value SendCustomData(napi_env env, napi_callback_info info);

    static std::function<void()> PlaybackStateSyncExecutor(std::shared_ptr<AVSession> session,
        AVPlaybackState playBackState);
    static std::function<void()> PlaybackStateAsyncExecutor(std::shared_ptr<ContextBase> context);
    static std::function<void()> AVQueueImgDownloadSyncExecutor(NapiAVSession* napiSession,
        OHOS::AVSession::AVMetaData metaData);

    static napi_status OnPlay(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnPause(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnStop(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnPlayNext(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnPlayPrevious(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnFastForward(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnRewind(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnSeek(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnSetSpeed(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnSetLoopMode(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnSetTargetLoopMode(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnToggleFavorite(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnMediaKeyEvent(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnOutputDeviceChange(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnCommonCommand(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnSkipToQueueItem(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnAVCallAnswer(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnAVCallHangUp(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnAVCallToggleCallMute(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnPlayFromAssetId(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnPlayWithAssetId(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnCastDisplayChange(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OnCustomData(napi_env env, NapiAVSession* napiSession, napi_value callback);

    static napi_status OffPlay(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffPause(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffStop(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffPlayNext(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffPlayPrevious(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffFastForward(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffRewind(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffSeek(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffSetSpeed(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffSetLoopMode(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffSetTargetLoopMode(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffToggleFavorite(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffMediaKeyEvent(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffOutputDeviceChange(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffCommonCommand(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffSkipToQueueItem(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffAVCallAnswer(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffAVCallHangUp(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffAVCallToggleCallMute(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffPlayFromAssetId(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffPlayWithAssetId(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffCastDisplayChange(napi_env env, NapiAVSession* napiSession, napi_value callback);
    static napi_status OffCustomData(napi_env env, NapiAVSession* napiSession, napi_value callback);

    static void ErrCodeToMessage(int32_t errCode, std::string& message);

    static napi_value ThrowErrorAndReturn(napi_env env, const std::string& message, int32_t errCode);
    static napi_value ThrowErrorAndReturnByErrCode(napi_env env, const std::string& message, int32_t errCode);

    static void DoLastMetaDataRefresh(NapiAVSession* napiAVSession);
    static bool CheckMetaOutOfDate(NapiAVSession* napiAVSession, OHOS::AVSession::AVMetaData& curMeta);

    napi_ref wrapperRef_ {};
    std::string sessionId_ ;
    std::string sessionType_ ;
    std::string sessionTag_ ;
    AppExecFwk::ElementName elementName_;
    std::shared_ptr<AVSession> session_;
    std::shared_ptr<NapiAVSessionCallback> callback_;
    std::string latestMetadataUri_;
    std::string latestMetadataAssetId_;
    std::string latestDownloadedUri_;
    std::string latestDownloadedAssetId_;
    std::string latestDownloadedAVQueueId_;
    AVMetaData metaData_;

    static std::mutex lock_;
    static std::mutex syncMutex_;
    static std::mutex syncAsyncMutex_;
    static std::mutex downloadAVQImgMutex_;
    static std::condition_variable syncCond_;
    static std::condition_variable syncAsyncCond_;
    static int32_t playBackStateRet_;
    static std::shared_ptr<NapiAVSession> napiAVSession_;
    static std::recursive_mutex destroyLock_;
    static bool isNapiSessionDestroy_;

    static std::map<std::string, OnEventHandlerType> onEventHandlers_;
    static std::map<std::string, OffEventHandlerType> offEventHandlers_;

    static constexpr size_t ARGC_ONE = 1;
    static constexpr size_t ARGC_TWO = 2;
    static constexpr size_t ARGC_THREE = 3;

    static constexpr size_t ARGV_FIRST = 0;
    static constexpr size_t ARGV_SECOND = 1;
    static constexpr size_t ARGV_THIRD = 2;

    static constexpr size_t UNMASK_CHAR_NUM = 3;
};
} // namespace OHOS::AVSession
#endif // OHOS_NAPI_AVSESSION_H

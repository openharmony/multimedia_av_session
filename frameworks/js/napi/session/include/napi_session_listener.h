/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_NAPI_SESSIONLISTENER_H
#define OHOS_NAPI_SESSIONLISTENER_H

#include <list>
#include <memory>
#include "avsession_info.h"
#include "avsession_manager.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "napi_utils.h"
#include "napi_async_callback.h"

namespace OHOS::AVSession {
class NapiSessionListener : public SessionListener {
public:
    enum {
        EVENT_SESSION_CREATED,
        EVENT_SESSION_DESTROYED,
        EVENT_TOP_SESSION_CHANGED,
        EVENT_AUDIO_SESSION_CHECKED,
        EVENT_DEVICE_AVAILABLE,
        EVENT_DEVICE_LOG_EVENT,
        EVENT_DEVICE_OFFLINE,
        EVENT_REMOTE_DISTRIBUTED_SESSION_CHANGED,
        EVENT_DEVICE_STATE_CHANGED,
        EVENT_TYPE_MAX
    };

    NapiSessionListener();
    ~NapiSessionListener() override;

    void OnSessionCreate(const AVSessionDescriptor& descriptor) override;
    void OnSessionRelease(const AVSessionDescriptor& descriptor) override;
    void OnTopSessionChange(const AVSessionDescriptor& descriptor) override;
    void OnAudioSessionChecked(const int32_t uid) override;
    void OnDeviceAvailable(const OutputDeviceInfo& castOutputDeviceInfo) override;
    void OnDeviceLogEvent(const DeviceLogEventCode eventId, const int64_t param) override;
    void OnDeviceOffline(const std::string& deviceId) override;
    void OnRemoteDistributedSessionChange(
        const std::vector<sptr<IRemoteObject>>& sessionControllers) override;
    void OnDeviceStateChange(const DeviceState& deviceState) override;

    napi_status AddCallback(napi_env env, int32_t event, napi_value callback);
    napi_status RemoveCallback(napi_env env, int32_t event, napi_value callback);
    int32_t GetCallbackNum(int32_t event);

private:
    template<typename T>
    void HandleEvent(int32_t event, const T& param);

    template<typename T>
    void HandleEvent(int32_t event, const T& param, bool checkValid);

    template<typename T, typename N>
    void HandleEvent(int32_t event, const T& firstParam, const N& secondParam);

    std::shared_ptr<NapiAsyncCallback> asyncCallback_;
    std::mutex lock_;
    std::list<napi_ref> callbacks_[EVENT_TYPE_MAX] {};
    std::shared_ptr<bool> isValid_ = std::make_shared<bool>(false);

    static constexpr size_t UNMASK_CHAR_NUM = 3;
};
} // namespace OHOS::AVSession
#endif // OHOS_NAPI_SESSIONLISTENER_H
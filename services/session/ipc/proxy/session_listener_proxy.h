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

#ifndef OHOS_SESSION_LISTENER_PROXY_H
#define OHOS_SESSION_LISTENER_PROXY_H

#include "isession_listener.h"
#include "iremote_proxy.h"

namespace OHOS::AVSession {
class SessionListenerProxy : public IRemoteProxy<ISessionListener> {
public:
    explicit SessionListenerProxy(const sptr<IRemoteObject>& impl);

    void OnSessionCreate(const AVSessionDescriptor& descriptor) override;

    void OnSessionRelease(const AVSessionDescriptor& descriptor) override;

    void OnTopSessionChange(const AVSessionDescriptor& descriptor) override;

    void OnAudioSessionChecked(const int32_t uid) override;

    void OnDeviceAvailable(const OutputDeviceInfo& castOutputDeviceInfo) override;

    void OnDeviceLogEvent(const DeviceLogEventCode eventId, const int64_t param) override;

    void OnDeviceOffline(const std::string& deviceId) override;

    void OnDeviceStateChange(const DeviceState& deviceState) override;

    void OnRemoteDistributedSessionChange(
        const std::vector<sptr<IRemoteObject>>& sessionControllers) override;
private:
    static inline BrokerDelegator<SessionListenerProxy> delegator_;
};
}
#endif // OHOS_SESSION_LISTENER_PROXY_H
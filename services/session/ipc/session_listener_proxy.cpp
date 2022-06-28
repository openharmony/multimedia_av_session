/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "session_listener_proxy.h"
#include "avsession_log.h"
#include "avsession_trace.h"

namespace OHOS::AVSession {
SessionListenerProxy::SessionListenerProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ISessionListener>(impl)
{
    SLOGD("construct");
}

void SessionListenerProxy::OnSessionCreate(const AVSessionDescriptor &descriptor)
{
    MessageParcel data;
    CHECK_AND_RETURN_LOG(data.WriteInterfaceToken(GetDescriptor()), "write interface token failed");
    descriptor.WriteToParcel(data);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    CHECK_AND_RETURN_LOG(!Remote()->SendRequest(LISTENER_CMD_ON_CREATE, data, reply, option), "send request fail");
}

void SessionListenerProxy::OnSessionRelease(const AVSessionDescriptor &descriptor)
{
    MessageParcel data;
    CHECK_AND_RETURN_LOG(data.WriteInterfaceToken(GetDescriptor()), "write interface token failed");
    descriptor.WriteToParcel(data);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    CHECK_AND_RETURN_LOG(!Remote()->SendRequest(LISTENER_CMD_ON_RELEASE, data, reply, option), "send request fail");
}

void SessionListenerProxy::OnTopSessionChanged(const AVSessionDescriptor& descriptor)
{
    MessageParcel data;
    CHECK_AND_RETURN_LOG(data.WriteInterfaceToken(GetDescriptor()), "write interface token failed");
    descriptor.WriteToParcel(data);
    MessageParcel reply;
    MessageOption option(MessageOption::TF_ASYNC);
    CHECK_AND_RETURN_LOG(!Remote()->SendRequest(LISTENER_CMD_TOP_CHANGED, data, reply, option), "send request fail");
}
}
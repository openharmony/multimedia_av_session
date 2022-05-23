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

#include "avsession_stub.h"
#include "avsession_log.h"
#include "avsession_errors.h"

namespace OHOS::AVSession {
bool AVSessionStub::CheckInterfaceToken(MessageParcel &data)
{
    auto localDescriptor = IAVSession::GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        SLOGE("interface token is not equal");
        return false;
    }
    return true;
}

int AVSessionStub::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply,
                                   MessageOption &option)
{
    if (!CheckInterfaceToken(data)) {
        return AVSESSION_ERROR;
    }
    if (code >= 0 && code < SESSION_CMD_MAX) {
        return (this->*handlers[code])(data, reply);
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int AVSessionStub::HandleGetSessionId(MessageParcel &data, MessageParcel &reply)
{
    reply.WriteInt32(GetSessionId());
    return ERR_NONE;
}

int AVSessionStub::HandleRegisterCallbackInner(MessageParcel &data, MessageParcel &reply)
{
    auto callback = data.ReadRemoteObject();
    RegisterCallbackInner(callback);
    reply.WriteInt32(AVSESSION_SUCCESS);
    return ERR_NONE;
}

int AVSessionStub::HandleRelease(MessageParcel &data, MessageParcel &reply)
{
    Release();
    reply.WriteInt32(AVSESSION_SUCCESS);
    return ERR_NONE;
}

int AVSessionStub::HandleGetAVPlaybackState(MessageParcel& data, MessageParcel& reply)
{
    AVPlaybackState avPlaybackState;
    avPlaybackState.SetState(data.ReadInt32());
    avPlaybackState.SetBufferedTime(data.ReadInt64());

    reply.WriteInt32(GetAVPlaybackState(avPlaybackState));
    return ERR_NONE;
}

int AVSessionStub::HandleSetAVMetaData(MessageParcel& data, MessageParcel& reply)
{
    const AVMetaData* avMetaData = data.ReadParcelable<AVMetaData>();
    reply.WriteInt32(SetAVMetaData(*avMetaData));
    return ERR_NONE;
}

int AVSessionStub::HandleSetLaunchAbility(MessageParcel& data, MessageParcel& reply)
{
    const AbilityRuntime::WantAgent::WantAgent* want = data.ReadParcelable<AbilityRuntime::WantAgent::WantAgent>();
    reply.WriteInt32(SetLaunchAbility(*want));
    return ERR_NONE;
}


int32_t AVSessionStub::SetLaunchAbility(const AbilityRuntime::WantAgent::WantAgent& ability)
{
    return 0;
}

std::shared_ptr<AVSessionController> AVSessionStub::GetController()
{
    return nullptr;
}

int32_t AVSessionStub::RegisterCallback(std::shared_ptr<AVSessionCallback>& callback)
{
    return 0;
}

int32_t AVSessionStub::Active()
{
    return 0;
}

int32_t AVSessionStub::Disactive()
{
    return 0;
}

bool AVSessionStub::IsActive()
{
    return false;
}

int32_t AVSessionStub::AddSupportCommand(const std::string& cmd)
{
    return 0;
}

int32_t AVSessionStub::Release()
{
    return 0;
}

int32_t AVSessionStub::GetSessionId()
{
    return 0;
}

int32_t AVSessionStub::GetAVMetaData(AVMetaData& meta)
{
    return 0;
}

int32_t AVSessionStub::SetAVMetaData(const AVMetaData& meta)
{
    return 0;
}

int32_t AVSessionStub::GetAVPlaybackState(AVPlaybackState& state)
{
    return 0;
}
}
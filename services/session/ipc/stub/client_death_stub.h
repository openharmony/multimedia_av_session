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

#ifndef OHOS_CLIENT_DEATH_STUB_H
#define OHOS_CLIENT_DEATH_STUB_H

#include "iclient_death.h"
#include "iremote_stub.h"
#include "avsession_log.h"

namespace OHOS::AVSession {
class ClientDeathStub : public IRemoteStub<IClientDeath> {
public:
    ~ClientDeathStub()
    {
        SLOGI("ClientDeathStub gone");
    }

    int OnRemoteRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override
    {
        return 0;
    };
};
} // namespace OHOS::AVSession
#endif // OHOS_CLIENT_DEATH_STUB_H
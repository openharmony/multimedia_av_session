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

#include "avsession_callback_proxy.h"
#include "avsession_log.h"

namespace OHOS::AVSession {
AVSessionCallbackProxy::AVSessionCallbackProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IAVSessionCallback>(impl)
{
    SLOGD("construct");
}
} // namespace OHOS::AVSession
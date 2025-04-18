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

#ifndef OHOS_REMOTE_SESSION_SINK_H
#define OHOS_REMOTE_SESSION_SINK_H

#include <string>

#include "avsession_info.h"
#include "avsession_item.h"
#include "avcontrol_command.h"

namespace OHOS::AVSession {
class RemoteSessionSink {
public:
    virtual int32_t CastSessionFromRemote(const sptr <AVSessionItem>& session, const std::string& sourceSessionId,
                                          const std::string& sourceDevice, const std::string& sinkDevice,
                                          const std::string& sourceCap) = 0;
    virtual int32_t CancelCastSession() = 0;
    virtual int32_t SetControlCommand(const AVControlCommand& command) = 0;
    virtual int32_t SetCommonCommand(const std::string& commonCommand, const AAFwk::WantParams& commandArgs) = 0;
    virtual ~RemoteSessionSink() = default;
};
} // namespace OHOS::AVSession
#endif // OHOS_REMOTE_SESSION_SINK_H
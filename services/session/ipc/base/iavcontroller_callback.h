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

#ifndef OHOS_IAVCONTROLLER_CALLBACK_H
#define OHOS_IAVCONTROLLER_CALLBACK_H

#include "avsession_info.h"
#include "iremote_broker.h"

namespace OHOS::AVSession {
class IAVControllerCallback : public AVControllerCallback, public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.avsession.IAVControllerCallback");

    enum {
        CONTROLLER_CMD_ON_SESSION_DESTROY,
        CONTROLLER_CMD_ON_PLAYBACK_STATE_CHANGE,
        CONTROLLER_CMD_ON_METADATA_CHANGE,
        CONTROLLER_CMD_ON_ACTIVE_STATE_CHANGE,
        CONTROLLER_CMD_ON_VALID_COMMAND_CHANGE,
        CONTROLLER_CMD_ON_OUTPUT_DEVICE_CHANGE,
        CONTROLLER_CMD_ON_SET_SESSION_EVENT,
        CONTROLLER_CMD_ON_QUEUE_ITEMS_CHANGE,
        CONTROLLER_CMD_ON_QUEUE_TITLE_CHANGE,
        CONTROLLER_CMD_ON_SET_EXTRAS_EVENT,
        CONTROLLER_CMD_ON_AVCALL_METADATA_CHANGE,
        CONTROLLER_CMD_ON_AVCALL_STATE_CHANGE,
        CONTROLLER_CMD_MAX,
    };
};
}
#endif // OHOS_IAVCONTROLLER_CALLBACK_H
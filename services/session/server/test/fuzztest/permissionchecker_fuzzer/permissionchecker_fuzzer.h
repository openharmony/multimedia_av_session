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

#ifndef OHOS_PERMISSIONCHECKER_FUZZER_H
#define OHOS_PERMISSIONCHECKER_FUZZER_H

#include <mutex>
#include <set>
#include "avsession_info.h"
#include "audio_adapter.h"
#include "app_manager_adapter.h"
#include "background_audio_controller.h"
#include "avsession_log.h"
#include "avsession_manager.h"

namespace OHOS::AVSession {
    void CheckPermissionTest001();
    void CheckPermissionTest002();
    void CheckPermissionTest003();
    void CheckSystemPermissionTest001();
    void CheckMediaResourcePermissionTest001();
    void CheckSystemPermissionByUidTest001(uint8_t* data);
}
#endif
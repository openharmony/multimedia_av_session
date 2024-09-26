/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OHOS_COLLABORATION_MANAGER_FUZZER_H
#define OHOS_COLLABORATION_MANAGER_FUZZER_H

namespace OHOS::AVSession {
    void CollaborationManagerRemoteRequest(uint8_t* data, size_t size);

    class CollaborationManagerFuzzer {
    public:
        CollaborationManagerFuzzer() = default;
        ~CollaborationManagerFuzzer() = default;
        void CollaborationManagerFuzzTest(uint8_t* data, size_t size);
    };
}

#endif

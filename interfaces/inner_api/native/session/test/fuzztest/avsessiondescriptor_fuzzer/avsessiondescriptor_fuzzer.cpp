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

#include "avsessiondescriptor_fuzzer.h"
#include "avsession_descriptor.h"
#include "parcel.h"

void OHOS::AVSession::AVSessionDescriptorTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size <= 0)) {
        return;
    }

    Parcel dataParcel;
    dataParcel.WriteBuffer(data, size);
    dataParcel.RewindRead(0);

    AVSessionDescriptor avSessionDescriptor;

    avSessionDescriptor.Unmarshalling(dataParcel);
    avSessionDescriptor.Marshalling(dataParcel);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AVSession::AVSessionDescriptorTest(data, size);
    return 0;
}
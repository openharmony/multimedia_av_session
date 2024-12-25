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

#include "securec.h"
#include "avsession_log.h"
#include "iremote_proxy.h"
#include "avcontroller_callback_proxy.h"
#include "iavcontroller_callback.h"
#include "iremote_proxy.h"
#include "avsession_log.h"
#include "avsession_errors.h"
#include "system_ability_definition.h"
#include "avsession_service.h"
#include "ability_connect_helper_fuzzer.h"

using namespace std;
using namespace OHOS;
using namespace OHOS::AVSession;

static const int32_t MAX_CODE_LEN  = 20;
static const int32_t MIN_SIZE_NUM = 10;
static const uint8_t *RAW_DATA = nullptr;
static size_t g_totalSize = 0;
static size_t g_sizePos;

/*
* describe: get data from FUZZ untrusted data(RAW_DATA) which size is according to sizeof(T)
* tips: only support basic type
*/
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (RAW_DATA == nullptr || objectSize > g_totalSize - g_sizePos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, RAW_DATA + g_sizePos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_sizePos += objectSize;
    return object;
}

std::string GetString()
{
    size_t objectSize = (GetData<int8_t>() % MAX_CODE_LEN) + 1;
    if (RAW_DATA == nullptr || objectSize > g_totalSize - g_sizePos) {
        return "OVER_SIZE";
    }
    char object[objectSize + 1];
    errno_t ret = memcpy_s(object, sizeof(object), RAW_DATA + g_sizePos, objectSize);
    if (ret != EOK) {
        return "";
    }
    g_sizePos += objectSize;
    std::string output(object);
    return output;
}

template<class T>
uint32_t GetArrLength(T& arr)
{
    if (arr == nullptr) {
        SLOGE("%{public}s: The array length is equal to 0", __func__);
        return 0;
    }
    return sizeof(arr) / sizeof(arr[0]);
}

typedef void (*TestFuncs[3])();

TestFuncs g_allFuncs = {
    AbilityConnectHelperFuzzTest,
    AbilityConnectionStubFuzzTest,
    AbilityConnectCallbackFuzzTest
};

bool FuzzTest(const uint8_t* rawData, size_t size)
{
    if (rawData == nullptr) {
        return false;
    }

    // initialize data
    RAW_DATA = rawData;
    g_totalSize = size;
    g_sizePos = 0;

    uint32_t code = GetData<uint32_t>();
    uint32_t len = GetArrLength(g_allFuncs);
    if (len > 0) {
        g_allFuncs[code % len]();
    } else {
        SLOGE("%{public}s: The len length is equal to 0", __func__);
    }

    return true;
}

void OHOS::AVSession::AbilityConnectHelperFuzzTest()
{
    std::string bundleName = GetString();
    std::string abilityName = GetString();

    AbilityConnectHelper::GetInstance().StartAbilityByCall(bundleName, abilityName);
}

void OHOS::AVSession::AbilityConnectionStubFuzzTest()
{
    MessageParcel Parcel;
    MessageParcel reply;
    MessageOption option;
    uint32_t code = GetData<uint32_t>();

    AbilityConnectCallback abilityConnectCallback;
    abilityConnectCallback.OnRemoteRequest(code, Parcel, reply, option);
}

void OHOS::AVSession::AbilityConnectCallbackFuzzTest()
{
    OHOS::AppExecFwk::ElementName elementName;
    int resultCode = GetData<int32_t>();
    AbilityConnectCallback abilityConnectCallback;
    sptr<IRemoteObject> remoteObject = nullptr;

    abilityConnectCallback.OnAbilityConnectDone(elementName, remoteObject, resultCode);
    abilityConnectCallback.OnAbilityDisconnectDone(elementName, resultCode);
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(uint8_t* data, size_t size)
{
    if (size < MIN_SIZE_NUM) {
        return 0;
    }
    /* Run your code on data */
    FuzzTest(data, size);
    return 0;
}
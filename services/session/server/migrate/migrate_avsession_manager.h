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

#ifndef MIGRATE_AVSESSION_MANAGER_H
#define MIGRATE_AVSESSION_MANAGER_H

#include <atomic>
#include <mutex>

#include "softbus_distributed_data_manager.h"
#include "migrate_avsession_server.h"
#include "migrate_avsession_proxy.h"

namespace OHOS::AVSession {
class MigrateAVSessionManager {
public:
    static MigrateAVSessionManager& GetInstance();

    void CreateLocalSessionStub(std::string scene, std::shared_ptr<MigrateAVSessionServer> server);

    void ReleaseLocalSessionStub(std::string scene);

    void CreateRemoteSessionProxy(std::string &networkId, std::string scene,
        std::shared_ptr<MigrateAVSessionProxy> proxy);
    
    void ReleaseRemoteSessionProxy(std::string &networkId, std::string scene);

    static constexpr const char* migrateSceneNext = "MigrateWithNext";
    static constexpr const int32_t MSG_HEAD_MODE_FOR_NEXT = 101;

private:
    MigrateAVSessionManager() = default;
    ~MigrateAVSessionManager() = default;

    void IncSoftBusRef();
    void DecSoftBusRef();

    std::map<std::string, std::shared_ptr<SoftbusSessionServer>> serverMap_;
    std::map<std::string, std::shared_ptr<SoftbusSessionProxy>> proxyMap_;
    std::shared_ptr<SoftbusDistributedDataManager> softBusDistributedDataMgr_ =
        std::make_shared<SoftbusDistributedDataManager>();
    std::atomic<int32_t> refs_ = 0;
    std::recursive_mutex migrateServerMapLock_;
};
}

#endif
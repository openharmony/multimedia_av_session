/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_AVSESSION_SERVICE_H
#define OHOS_AVSESSION_SERVICE_H

#include <atomic>
#include <mutex>
#include <map>

#include "iremote_stub.h"
#include "system_ability.h"
#include "audio_info.h"
#include "avsession_service_stub.h"
#include "avsession_item.h"
#include "avcontroller_item.h"
#include "session_container.h"
#include "iclient_death.h"
#include "isession_listener.h"
#include "focus_session_strategy.h"
#include "background_audio_controller.h"
#include "ability_manager_adapter.h"
#include "device_manager.h"
#include "dm_device_info.h"
#include "audio_adapter.h"
#include "remote_session_command_process.h"
#include "i_avsession_service_listener.h"
#include "avqueue_info.h"
#include "migrate/migrate_avsession_server.h"
#include "account_manager_adapter.h"
#include "app_manager_adapter.h"
#include "avsession_dynamic_loader.h"
#include "avsession_errors.h"
#include "avsession_log.h"
#include "avsession_info.h"
#include "file_ex.h"
#include "iservice_registry.h"
#include "key_event_adapter.h"
#include "nlohmann/json.hpp"
#include "session_stack.h"
#include "avsession_trace.h"
#include "avsession_dumper.h"
#include "command_send_limit.h"
#include "avsession_sysevent.h"
#include "json_utils.h"
#include "avsession_utils.h"
#include "avcontrol_command.h"
#include "avsession_event_handler.h"
#include "bundle_status_adapter.h"
#include "if_system_ability_manager.h"
#include "avsession_radar.h"

#include "common_event_manager.h"
#include "common_event_subscribe_info.h"
#include "common_event_subscriber.h"
#include "common_event_support.h"
#include "matching_skills.h"

#include "avsession_users_manager.h"

#ifdef BLUETOOTH_ENABLE
#include "bluetooth_host.h"
#endif

#ifdef CASTPLUS_CAST_ENGINE_ENABLE
#include "av_router.h"
#include "collaboration_manager.h"
#endif

namespace OHOS::AVSession {
class AVSessionDumper;

class ClientDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit ClientDeathRecipient(const std::function<void()>& callback);

    void OnRemoteDied(const wptr<IRemoteObject>& object) override;

private:
    std::function<void()> callback_;
};

class AVSessionInitDMCallback : public OHOS::DistributedHardware::DmInitCallback {
public:
    AVSessionInitDMCallback() = default;
    ~AVSessionInitDMCallback() override = default;
    void OnRemoteDied() override {};
};

#ifdef BLUETOOTH_ENABLE
class DetectBluetoothHostObserver : public OHOS::Bluetooth::BluetoothHostObserver {
public:
    DetectBluetoothHostObserver(AVSessionService *ptr);
    virtual ~DetectBluetoothHostObserver() = default;
    void OnStateChanged(const int transport, const int status) override;
    void OnDiscoveryStateChanged(int status) override
    {
        return;
    }
    
    void OnDiscoveryResult(const OHOS::Bluetooth::BluetoothRemoteDevice &device, int rssi,
        const std::string deviceName, int deviceClass) override
    {
        return;
    }

    void OnPairRequested(const OHOS::Bluetooth::BluetoothRemoteDevice &device) override
    {
        return;
    }

    void OnPairConfirmed(const OHOS::Bluetooth::BluetoothRemoteDevice &device, int reqType, int number) override
    {
        return;
    }

    void OnScanModeChanged(int mode) override
    {
        return;
    }

    void OnDeviceNameChanged(const std::string &deviceName) override
    {
        return;
    }

    void OnDeviceAddrChanged(const std::string &address) override
    {
        return;
    }

private:
    bool is2in1_ = false;
    bool lastEnabled_ = false;
    AVSessionService *servicePtr_ = nullptr;
};
#endif

class EventSubscriber : public EventFwk::CommonEventSubscriber {
public:
    EventSubscriber(const EventFwk::CommonEventSubscribeInfo &subscriberInfo, AVSessionService *ptr);
    virtual ~EventSubscriber() = default;
    void OnReceiveEvent(const EventFwk::CommonEventData &eventData) override;
private:
    AVSessionService *servicePtr_ = nullptr;
};

class AVSessionService : public SystemAbility, public AVSessionServiceStub, public IAVSessionServiceListener {
    DECLARE_SYSTEM_ABILITY(AVSessionService);

public:
    DISALLOW_COPY_AND_MOVE(AVSessionService);

    explicit AVSessionService(int32_t systemAbilityId, bool runOnCreate = true);

    ~AVSessionService() override;

    void OnDump() override;

    void OnStart() override;

    void OnStop() override;

    void PullMigrateStub();

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    sptr<IRemoteObject> CreateSessionInner(const std::string& tag, int32_t type,
                                           const AppExecFwk::ElementName& elementName) override;

    int32_t CreateSessionInner(const std::string& tag, int32_t type, const AppExecFwk::ElementName& elementName,
                               sptr<IRemoteObject>& object) override;

    int32_t GetAllSessionDescriptors(std::vector<AVSessionDescriptor>& descriptors) override;

    int32_t GetSessionDescriptorsBySessionId(const std::string& sessionId, AVSessionDescriptor& descriptor) override;

    int32_t GetHistoricalSessionDescriptors(int32_t maxSize, std::vector<AVSessionDescriptor>& descriptors) override;
    
    int32_t GetHistoricalAVQueueInfos(int32_t maxSize, int32_t maxAppSize,
                                      std::vector<AVQueueInfo>& avQueueInfos) override;
    
    int32_t StartAVPlayback(const std::string& bundleName, const std::string& assetId) override;

    int32_t CreateControllerInner(const std::string& sessionId, sptr<IRemoteObject>& object) override;

    bool IsAudioPlaybackAllowed(const int32_t uid, const int32_t pid) override;

#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    int32_t GetAVCastControllerInner(const std::string& sessionId, sptr<IRemoteObject>& object) override;
#endif

    int32_t RegisterSessionListener(const sptr<ISessionListener>& listener) override;

    int32_t RegisterSessionListenerForAllUsers(const sptr<ISessionListener>& listener) override;

    int32_t SendSystemAVKeyEvent(const MMI::KeyEvent& keyEvent) override;

    int32_t SendSystemControlCommand(const AVControlCommand& command) override;

    int32_t RegisterClientDeathObserver(const sptr<IClientDeath>& observer) override;

    void OnClientDied(pid_t pid);

    void HandleSessionRelease(std::string sessionId);

    void HandleCallStartEvent();

#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    __attribute__((no_sanitize("cfi"))) int32_t MirrorToStreamCast(sptr<AVSessionItem>& session);

    void SplitExtraInfo(std::string info);
#endif

    void HandleControllerRelease(AVControllerItem& controller);

    std::int32_t Dump(std::int32_t fd, const std::vector<std::u16string>& args) override;

    int32_t CastAudio(const SessionToken& token,
                      const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkAudioDescriptors) override;

    int32_t CastAudioForAll(const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkAudioDescriptors) override;

    void NotifyAudioSessionCheckTrigger(const int32_t uid)
    {
        return NotifyAudioSessionCheck(uid);
    }

    void NotifyBackgroundReportCheck(const int32_t uid, const int32_t pid,
        AudioStandard::StreamUsage streamUsage, AudioStandard::RendererState rendererState);

    void SuperLauncher(std::string deviceId, std::string serviceName,
        std::string extraInfo, const std::string& state);

#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    void ReleaseCastSession() override;

    void CreateSessionByCast(const int64_t castHandle) override;

    void NotifyDeviceAvailable(const OutputDeviceInfo& castOutputDeviceInfo) override;

    void NotifyDeviceLogEvent(const DeviceLogEventCode eventId, const int64_t param) override;

    void NotifyDeviceOffline(const std::string& deviceId) override;

    void NotifyMirrorToStreamCast();

    int32_t StartCast(const SessionToken& sessionToken, const OutputDeviceInfo& outputDeviceInfo) override;

    int32_t StopCast(const SessionToken& sessionToken) override;

    int32_t checkEnableCast(bool enable) override;
#endif

    int32_t Close(void) override;
    
    void AddAvQueueInfoToFile(AVSessionItem& session);

    void SetScreenOn(bool on);

    bool GetScreenOn();

    void SetScreenLocked(bool isLocked);

    bool GetScreenLocked();

    std::string GetAVQueueDir(int32_t userId = 0);

    std::string GetAVSortDir(int32_t userId = 0);

    void HandleUserEvent(const std::string &type, const int &userId);

    void HandleScreenStatusChange(std::string event);

    void RegisterBundleDeleteEventForHistory(int32_t userId = 0);

    void NotifyMigrateStop(const std::string &deviceId);

    void InitCastEngineService();

private:
    void CheckBrEnable();

    void NotifyProcessStatus(bool isStart);

    static SessionContainer& GetContainer();
    static AVSessionUsersManager& GetUsersManager();

    std::string AllocSessionId();

    bool AbilityHasSession(pid_t pid);

    sptr<AVControllerItem> GetPresentController(pid_t pid, const std::string& sessionId);

    void NotifySessionCreate(const AVSessionDescriptor& descriptor);
    void NotifySessionRelease(const AVSessionDescriptor& descriptor);
    void NotifyTopSessionChanged(const AVSessionDescriptor& descriptor);
    void NotifyAudioSessionCheck(const int32_t uid);
    void NotifySystemUI(const AVSessionDescriptor* historyDescriptor, bool isActiveSession);
    void NotifyDeviceChange();

    void AddClientDeathObserver(pid_t pid, const sptr<IClientDeath>& observer,
        const sptr<ClientDeathRecipient> recipient);
    void RemoveClientDeathObserver(pid_t pid);

    void AddSessionListener(pid_t pid, const sptr<ISessionListener>& listener);
    void AddSessionListenerForAllUsers(pid_t pid, const sptr<ISessionListener>& listener);
    void RemoveSessionListener(pid_t pid);

    void AddInnerSessionListener(SessionListener* listener);
    void RemoveInnerSessionListener(SessionListener* listener);

    sptr<AVSessionItem> SelectSessionByUid(const AudioStandard::AudioRendererChangeInfo& info);

    void OutputDeviceChangeListener(const AudioRendererChangeInfos& infos);

    sptr<AVSessionItem> CreateSessionInner(const std::string& tag, int32_t type, bool thirdPartyApp,
                                           const AppExecFwk::ElementName& elementName);

    int32_t CreateSessionInner(const std::string& tag, int32_t type, bool thirdPartyApp,
                               const AppExecFwk::ElementName& elementName, sptr<AVSessionItem>& sessionItem);

    bool IsParamInvalid(const std::string& tag, int32_t type, const AppExecFwk::ElementName& elementName);

    void ServiceCallback(sptr<AVSessionItem>& sessionItem);

    sptr<AVSessionItem> CreateNewSession(const std::string& tag, int32_t type, bool thirdPartyApp,
                                         const AppExecFwk::ElementName& elementName);

    sptr<AVControllerItem> CreateNewControllerForSession(pid_t pid, sptr<AVSessionItem>& session);

    int32_t CancelCastAudioForClientExit(pid_t pid, const sptr<AVSessionItem>& session);

    void ClearSessionForClientDiedNoLock(pid_t pid);

    void ClearControllerForClientDiedNoLock(pid_t pid);

    void InitKeyEvent();

    void InitAudio();

    void InitAMS();

    void InitDM();

    void InitBMS();

    void InitRadarBMS();

    void InitAccountMgr();

    void InitCommonEventService();

    void InitCollaboration();

    bool SelectFocusSession(const FocusSessionStrategy::FocusSessionChangeInfo& info);
    
    void RefreshFocusSessionSort(sptr<AVSessionItem> &session);

    void UpdateTopSession(const sptr<AVSessionItem>& newTopSession);

    void HandleFocusSession(const FocusSessionStrategy::FocusSessionChangeInfo& info);

    void HandleDeviceChange(const std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>> &desc);

    __attribute__((no_sanitize("cfi"))) std::shared_ptr<RemoteSessionCommandProcess> GetService(
        const std::string& deviceId);

    int32_t CastAudioProcess(const std::vector<AudioStandard::AudioDeviceDescriptor>& descriptors,
                             const std::string& sourceSessionInfo,
                             sptr<AVSessionItem>& session);

    int32_t CastAudioInner(const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkAudioDescriptors,
                           const std::string& sourceSessionInfo,
                           const sptr<AVSessionItem>& session);

    int32_t CancelCastAudioInner(const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkDevices,
                                 const std::string& sourceSessionInfo,
                                 const sptr<AVSessionItem>& session);

    int32_t RemoteCastAudioInner(const std::string& sourceSessionInfo, std::string& sinkSessionInfo);

    int32_t RemoteCancelCastAudioInner(const std::string& sessionInfo);

    int32_t ProcessCastAudioCommand(const RemoteServiceCommand command, const std::string& input,
                                    std::string& output) override;

    int32_t CastAudioForNewSession(const sptr<AVSessionItem>& session);

    bool IsLocalDevice(const std::string& networkId);

    int32_t GetLocalNetworkId(std::string& networkId);

    int32_t GetTrustedDeviceName(const std::string& networkId, std::string& deviceName);

    int32_t GetTrustedDevicesInfo(std::vector<OHOS::DistributedHardware::DmDeviceInfo>& deviceList);

    int32_t SetBasicInfo(std::string& sessionInfo);

    void SetDeviceInfo(const std::vector<AudioStandard::AudioDeviceDescriptor>& castAudioDescriptors,
                           sptr<AVSessionItem>& session);

    int32_t GetAudioDescriptor(const std::string deviceId,
                               std::vector<AudioStandard::AudioDeviceDescriptor>& audioDeviceDescriptors);

    bool GetAudioDescriptorByDeviceId(
        const std::vector<std::shared_ptr<AudioStandard::AudioDeviceDescriptor>>& descriptors,
        const std::string& deviceId,
        AudioStandard::AudioDeviceDescriptor& audioDescriptor);

    void GetDeviceInfo(const sptr<AVSessionItem>& session,
                           const std::vector<AudioStandard::AudioDeviceDescriptor>& descriptors,
                           std::vector<AudioStandard::AudioDeviceDescriptor>& castSinkDescriptors,
                           std::vector<AudioStandard::AudioDeviceDescriptor>& cancelSinkDescriptors);

    int32_t SelectOutputDevice(const int32_t uid, const AudioStandard::AudioDeviceDescriptor& descriptor);

    int32_t StartDefaultAbilityByCall(std::string& sessionId);

    int32_t StartAbilityByCall(const std::string& sessionIdNeeded, std::string& sessionId);

    void HandleEventHandlerCallBack();

    bool IsHistoricalSession(const std::string& sessionId);

    void DeleteHistoricalRecord(const std::string& bundleName, int32_t userId = 0);
    
    void DeleteAVQueueInfoRecord(const std::string& bundleName, int32_t userId = 0);

    const nlohmann::json& GetSubNode(const nlohmann::json& node, const std::string& name);

    void SaveSessionInfoInFile(const std::string& sessionId, const std::string& sessionType,
        const AppExecFwk::ElementName& elementName);

    bool CheckAndCreateDir(const std::string& filePath);

    bool CheckUserDirValid(int32_t userId = 0);

    bool LoadStringFromFileEx(const std::string& filePath, std::string& content);

    bool SaveStringToFileEx(const std::string& filePath, const std::string& content);

    bool CheckStringAndCleanFile(const std::string& filePath);

    void ClearClientResources(pid_t pid);
    
    bool SaveAvQueueInfo(std::string& oldContent, const std::string &bundleName,
        const AVMetaData& meta, const int32_t userId);

    int32_t GetHistoricalSessionDescriptorsFromFile(std::vector<AVSessionDescriptor>& descriptors);

    void ReportStartCastBegin(std::string func, const OutputDeviceInfo& outputDeviceInfo, int32_t uid);

    void ReportStartCastEnd(std::string func, const OutputDeviceInfo &outputDeviceInfo, int32_t uid, int ret);
    
    void HandleAppStateChange(int uid, int state);

    void UpdateFrontSession(sptr<AVSessionItem>& sessionItem, bool isAdd);

    std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> CreateWantAgent(
        const AVSessionDescriptor* histroyDescriptor);

    void DoMetadataImgClean(AVMetaData& data);

    void HandleSystemKeyColdStart(const AVControlCommand &command);

    bool SubscribeCommonEvent();

    bool UnSubscribeCommonEvent();

    void ReportSessionInfo(const sptr <AVSessionItem>& session, int res);

    bool CheckAncoAudio();

    int32_t ConvertKeyCodeToCommand(int keyCode);
    
    void RemoveExpired(std::list<std::chrono::system_clock::time_point> &list,
        const std::chrono::system_clock::time_point &now, int32_t time = 1);
    
    void LowQualityCheck(int32_t uid, int32_t pid, AudioStandard::StreamUsage streamUsage,
        AudioStandard::RendererState rendererState);

    void PlayStateCheck(int32_t uid, AudioStandard::StreamUsage streamUsage,
        AudioStandard::RendererState rState);

    std::shared_ptr<std::list<sptr<AVSessionItem>>> GetCurSessionListForFront();

    std::atomic<uint32_t> sessionSeqNum_ {};

    sptr<AVSessionItem> topSession_;
    std::map<pid_t, std::list<sptr<AVControllerItem>>> controllers_;
    std::map<pid_t, sptr<IClientDeath>> clientDeathObservers_;
    std::map<pid_t, sptr<ClientDeathRecipient>> clientDeathRecipients_;
    std::list<SessionListener*> innerSessionListeners_;
    std::map<std::string, std::shared_ptr<AbilityManagerAdapter>> abilityManager_;
    FocusSessionStrategy focusSessionStrategy_;
    BackgroundAudioController backgroundAudioController_;
    std::map<std::string, std::string> castAudioSessionMap_;
    bool isAllSessionCast_ {};
    std::string outputDeviceId_;
    std::unique_ptr<AVSessionDumper> dumpHelper_ {};
    friend class AVSessionDumper;
    std::shared_ptr<MigrateAVSessionServer> migrateAVSession_;
    std::shared_ptr<EventSubscriber> subscriber_;
    bool isScreenOn_ = false;
    bool isScreenLocked_ = true;
    std::list<std::chrono::system_clock::time_point> flowControlPublishTimestampList_;

    // The following locks are used in the defined order of priority
    std::recursive_mutex sessionServiceLock_;

    std::recursive_mutex sessionFrontLock_;

    std::recursive_mutex sessionListenersLock_;

    std::recursive_mutex migrateListenersLock_;

    std::recursive_mutex sessionFileLock_;

    std::recursive_mutex avQueueFileLock_;

    std::recursive_mutex abilityManagerLock_;

    std::recursive_mutex screenStateLock_;

    std::recursive_mutex clientDeathLock_;

    // DMSDP related locks
    std::recursive_mutex isAllSessionCastLock_;

    std::recursive_mutex outputDeviceIdLock_;

    std::recursive_mutex castAudioSessionMapLock_;

#ifdef BLUETOOTH_ENABLE
    OHOS::Bluetooth::BluetoothHost *bluetoothHost_ = nullptr;
    std::shared_ptr<DetectBluetoothHostObserver> bluetoothObserver;
#endif

#ifdef CASTPLUS_CAST_ENGINE_ENABLE
    std::map<std::string, std::string> castServiceNameMapState_;
    const std::string deviceStateConnection = "CONNECT_SUCC";
    const std::string deviceStateDisconnection = "IDLE";
    const std::string seperator = ",";
    int appState = -1;
    bool isSupportMirrorToStream_ = false;
    std::string castDeviceId_ = "0";
    std::string castDeviceName_ = " ";
    int32_t castDeviceType_ = 0;
    const int32_t beginAddPos = 3;
    const int32_t endDecPos = 4;
    const int32_t typeAddPos = 2;
#endif

    static constexpr const char *SORT_FILE_NAME = "sortinfo";
    static constexpr const char *DEFAULT_SESSION_ID = "default";
    static constexpr const char *DEFAULT_BUNDLE_NAME = "com.example.himusicdemo";
    static constexpr const char *DEFAULT_ABILITY_NAME = "MainAbility";
    static constexpr const int32_t SYSTEMUI_LIVEVIEW_TYPECODE_MDEDIACONTROLLER = 2;
    static constexpr const char *AVQUEUE_FILE_NAME = "avqueueinfo";

    const std::string MEDIA_CONTROL_BUNDLENAME = "com.ohos.mediacontroller";
    const std::string MEDIA_CONTROL_ABILITYNAME = "com.ohos.mediacontroller.avplayer.mainability";

    int32_t pressCount_ {};
    int32_t maxHistoryNums_ = 10;
    int uidForAppStateChange_ = 0;
    bool isFirstPress_ = true;
    bool isSourceInCast_ = false;
    bool isInCast_ = false;
    bool is2in1_ = false;

    void *migrateStubFuncHandle_ = nullptr;

    const int32_t ONE_CLICK = 1;
    const int32_t DOUBLE_CLICK = 2;
    const int32_t THREE_CLICK = 3;
    const int32_t unSetHistoryNum = 3;
    const int32_t CLICK_TIMEOUT = 500;
    const int32_t lowQualityTimeout = 1000;
    const int32_t errorStateTimeout = 3 * 1000;
    const int32_t defMaxHistoryNum = 10;
    const int32_t maxFileLength = 32 * 1024 * 1024;
    const int32_t maxAVQueueInfoLen = 99;
    const int32_t allocSpace = 2;
    const int32_t avSessionUid = 6700;
    const int32_t ancoUid = 1041;
    const int32_t saType = 1;
    const uint32_t MAX_NOTIFICATION_NUM = 3;
    const int32_t NOTIFICATION_CONTROL_TIME = 1000;
};
} // namespace OHOS::AVSession
#endif // OHOS_AVSESSION_SERVICE_H

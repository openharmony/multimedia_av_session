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

#ifndef OHOS_AVSESSION_INFO_H
#define OHOS_AVSESSION_INFO_H

#include <string>

#include "avmeta_data.h"
#include "avplayback_state.h"
#include "avcast_player_state.h"
#include "avmedia_description.h"
#include "avqueue_item.h"
#include "avsession_descriptor.h"
#include "key_event.h"
#include "want_params.h"
/**
 * @brief Listening events for the creation and destruction of sessions
 *     and the latest session changes.
 * @since 9
 */
namespace OHOS::AVSession {
using DeathCallback = std::function<void()>;

class SessionListener {
public:
    /**
     * @brief Listen for session creation events.
     *
     * @param descriptor Session related description callback.
     * @since 9
    */
    virtual void OnSessionCreate(const AVSessionDescriptor& descriptor) = 0;

    /**
     * @brief Listen for session release events.
     *
     * @param descriptor Session related description callback.
     * @since 9
    */
    virtual void OnSessionRelease(const AVSessionDescriptor& descriptor) = 0;

    /**
     * @brief Listen for changes in the latest session.
     *
     * @param descriptor Session related description callback.
     * @since 9
    */
    virtual void OnTopSessionChange(const AVSessionDescriptor& descriptor) = 0;

    /**
     * @brief Listen for AudioSession verification.
     *
     * @param uid Session related uid.
     * @since 9
    */
    virtual void OnAudioSessionChecked(const int32_t uid) {};

    /**
     * @brief Listen for the event of device discovery.
     *
     * @param { OutputDeviceInfo } castOutputDeviceInfo - Discovered device info.
     * @since 10
    */
    virtual void OnDeviceAvailable(const OutputDeviceInfo& castOutputDeviceInfo) {};

    /**
     * @brief Deconstruct SessionListener.
     * @since 9
    */
    virtual ~SessionListener() = default;
};

class IAVCastControllerProxyListener {
public:


    virtual void OnStateChange(const AVCastPlayerState& state) = 0;

    virtual void OnMediaItemChange(const AVQueueItem& avQueueItem) = 0;

    virtual void OnVolumeChange(const int32_t volume) = 0;

    virtual void OnLoopModeChange(const int32_t loopMode) = 0;

    virtual void OnPlaySpeedChange(const double playSpeed) = 0;

    virtual void OnPositionChange(const int32_t position) = 0;

    virtual void OnVideoSizeChange(const int32_t width, const int32_t height) = 0;
    
    virtual void OnError(const int32_t errorCode, const std::string& errorMsg) = 0;

    /**
     * @brief Deconstruct SessionListener.
     * @since 9
    */
    virtual ~IAVCastControllerProxyListener() = default;
};

class AVSessionCallback {
public:
    /**
     * @brief playback.
     * @since 9
    */
    virtual void OnPlay() = 0;

    /**
     * @brief Pause playback.
     * @since 9
    */
    virtual void OnPause() = 0;

    /**
     * @brief Stop playback.
     * @since 9
    */
    virtual void OnStop() = 0;

    /**
     * @brief play next.
     * @since 9
    */
    virtual void OnPlayNext() = 0;

    /**
     * @brief Play the previous song.
     * @since 9
    */
    virtual void OnPlayPrevious() = 0;

    /**
     * @brief Fast forward.
     * @since 9
    */
    virtual void OnFastForward() = 0;

    /**
     * @brief Fast rewind.
     * @since 9
    */
    virtual void OnRewind() = 0;

    /**
     * @brief Seek to the specified time.
     *
     * @param time Seek to the specified time
     * @since 9
    */
    virtual void OnSeek(int64_t time) = 0;

    /**
     * @brief Set playback speed.
     *
     * @param speed The playback speed to be set
     * @return Whether the playback speed is set successfully
     * @since 9
    */
    virtual void OnSetSpeed(double speed) = 0;

    /**
     * @brief Set cycle mode.
     *
     * @param mode Cycle mode to be set {@link LoopMode}
     * @return Whether the cycle mode is set successfully.
     * @since 9
    */
    virtual void OnSetLoopMode(int32_t loopMode) = 0;

    /**
     * @brief Whether to collect the current song.
     *
     * @param mediald current song id.
     * @since 9
    */
    virtual void OnToggleFavorite(const std::string& mediald) = 0;

    /**
     * @brief Listening for Media key events.
     *
     * @param keyEvent event callbacks.
     * @since 9
    */
    virtual void OnMediaKeyEvent(const MMI::KeyEvent& keyEvent) = 0;

    /**
     * @brief Monitor and play device change events.
     *
     * @param connectionState Event callback of device state.
     * @param outputDeviceInfo Event callback of device information.
     * @since 9
    */
    virtual void OnOutputDeviceChange(const int32_t connectionState, const OutputDeviceInfo& outputDeviceInfo) = 0;

    /**
     * @brief Listen for command events.
     *
     * @param commonCommand The name of the command to be sent.
    * @param commandArgs Command packets to be sent
     * @since 10
     */
    virtual void OnCommonCommand(const std::string& commonCommand, const AAFwk::WantParams& commandArgs) = 0;

    /**
     * @brief Listen to the change of item in the playlist to be played.
     *
     * @param itemId The serial number of the item to be played.
     * @since 10
     */
    virtual void OnSkipToQueueItem(int32_t itemId) = 0;
    
    /**
     * @brief Deconstruct AVSessionCallback.
     * @since 9
    */
    virtual ~AVSessionCallback() = default;
};

class AVControllerCallback {
public:
    /**
     * @brief Session Destroy.
     * @since 9
    */
    virtual void OnSessionDestroy() = 0;

    /**
     * @brief Listening session destruction playback status change event.
     *
     * @param state Session related description callback.
     * @since 9
    */
    virtual void OnPlaybackStateChange(const AVPlaybackState& state) = 0;

    /**
     * @brief Listen for metadata change events.
     *
     * @param data Data is the changed metadata.
     * @since 9
    */
    virtual void OnMetaDataChange(const AVMetaData& data) = 0;

    /**
     * @brief Listen to the activation status event of the session.
     *
     * @param isActive Whether the session is activated.
     * @since 9
    */
    virtual void OnActiveStateChange(bool isActive) = 0;

    /**
     * @brief Listen for valid command change events supported by the session.
     *
     * @param cmds Collection callback of valid commands.
     * @since 9
    */
    virtual void OnValidCommandChange(const std::vector<int32_t>& cmds) = 0;

    /**
     * @brief Monitor and play device change events.
     *
     * @param connectionState Event callback of device state.
     * @param outputDeviceInfo Device related information callback.
     * @since 9
    */
    virtual void OnOutputDeviceChange(const int32_t connectionState, const OutputDeviceInfo& outputDeviceInfo) = 0;

    /**
     * @brief Listen for changes in custom events of the session.
     *
     * @param event Changed session event name.
     * @param args Parameters of parts.
     * @since 9
    */
    virtual void OnSessionEventChange(const std::string& event, const AAFwk::WantParams& args) = 0;

    /**
     * @brief Session playlist change callback.
     *
     * @param items The changed queue items.
     * @since 10
     */
    virtual void OnQueueItemsChange(const std::vector<AVQueueItem>& items) = 0;

    /**
     * @brief Session title change callback.
     *
     * @param title The changed title.
     * @since 10
     */
    virtual void OnQueueTitleChange(const std::string& title) = 0;

    /**
     * @brief Session custom media packet change callback.
     *
     * @param extras The changed custom media packet.
     * @since 10
     */
    virtual void OnExtrasChange(const AAFwk::WantParams& extras) = 0;

    /**
     * @brief Deconstruct AVControllerCallback.
     * @since 9
    */
    virtual ~AVControllerCallback() = default;
};

class AVCastControllerCallback {
public:
    virtual void OnStateChange(const AVCastPlayerState& state) = 0;

    virtual void OnMediaItemChange(const AVQueueItem& avQueueItem) = 0;

    virtual void OnVolumeChange(const int32_t volume) = 0;

    virtual void OnLoopModeChange(const int32_t loopMode) = 0;

    virtual void OnPlaySpeedChange(const double playSpeed) = 0;

    virtual void OnPositionChange(const int32_t position) = 0;

    virtual void OnVideoSizeChange(const int32_t width, const int32_t height) = 0;

    virtual void OnError(const int32_t errorCode, const std::string& errorMsg) = 0;

    /**
     * @brief Deconstruct AVControllerCallback.
     * @since 9
    */
    virtual ~AVCastControllerCallback() = default;
};

class IAVCastSessionStateListener {
public:
    /**
     * @brief Listen to the change of cast state change.
     *
     * @param castHandle The combination of providerId and castId.
     * @since 9
    */
    virtual void OnCastStateChange(int32_t castState, OutputDeviceInfo outputDeviceInfo) = 0;

    /**
     * @brief Deconstruct IAVCastSessionStateListener.
     * @since 10
    */
    virtual ~IAVCastSessionStateListener() = default;
};


struct SessionToken {
    std::string sessionId;
    pid_t pid;
    pid_t uid;
};

enum SessionDataCategory {
    SESSION_DATA_CATEGORY_INVALID = -1,
    SESSION_DATA_META = 0,
    SESSION_DATA_PLAYBACK_STATE = 1,
    SESSION_DATA_CONTROL_COMMAND = 2,
    SESSION_DATA_SET_EVENT = 3,
    SESSION_DATA_QUEUE_ITEMS = 4,
    SESSION_DATA_QUEUE_TITLE = 5,
    SESSION_DATA_COMMON_COMMAND = 6,
    SESSION_DATA_EXTRAS = 7,
    SESSION_DATA_CATEGORY_MAX = 8,
};
} // namespace OHOS::AVSession
#endif // OHOS_AVSESSION_INFO_H

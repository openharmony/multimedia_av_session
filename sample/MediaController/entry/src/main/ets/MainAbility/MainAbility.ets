/*
* Copyright (C) 2023 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

import UIAbility from '@ohos.app.ability.UIAbility';
import hilog from '@ohos.hilog';
import window from '@ohos.window';
import Want from '@ohos.app.ability.Want';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want) {
    AppStorage.setOrCreate('context', this.context);
    hilog.info(0x0000, '[Sample_MediaController]', '%{public}s', 'Ability onCreate');
  }

  onDestroy() {
    hilog.info(0x0000, '[Sample_MediaController]', '%{public}s', 'Ability onDestroy');
  }

  async onWindowStageCreate(windowStage: window.WindowStage) {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, '[Sample_MediaController]', '%{public}s', 'Ability onWindowStageCreate');
    let currentWindow: window.Window = await windowStage.getMainWindow();
    await currentWindow.setWindowLayoutFullScreen(true);
    windowStage.loadContent('pages/PresentPage', (err, data) => {
      if (err.code) {
        hilog.error(0x0000, '[Sample_MediaController]', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, '[Sample_MediaController]', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
    });
  }

  onWindowStageDestroy() {
    // Main window is destroyed, release UI related resources
    hilog.info(0x0000, '[Sample_MediaController]', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground() {
    // Ability has brought to foreground
    hilog.info(0x0000, '[Sample_MediaController]', '%{public}s', 'Ability onForeground');
  }

  onBackground() {
    // Ability has back to background
    hilog.info(0x0000, '[Sample_MediaController]', '%{public}s', 'Ability onBackground');
  }
}

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

let ExtensionAbility = requireNapi('app.ability.ExtensionAbility');

class MediaControlExtensionAbility extends ExtensionAbility {
  onCreate(want) {
    console.log('onCreate, want:' + want.abilityName);
  }

  onRequest(want, startId) {
    console.log('onRequest, want:' + want.abilityName + ', startId:' + startId);
  }

  onLoadContent() {
    console.log('onLoadContent');
    return '';
  }

  onDestroy() {
    console.log('onDestroy');
  }
}

export default MediaControlExtensionAbility;
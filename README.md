# Arduino 版 ATOM Babies

※[UiFlow](https://flow.m5stack.com/) で実装する場合は[UiFlow 版 ATOM Babies](https://github.com/3110/atom-babies)を参照してください。

<div align="center">
<img src="https://i.gyazo.com/e46dc64eac0adfe1591c9fb7c9dc737b.jpg" width="480"/>
</div>

## ATOM Babies とは

ATOM Babies は M5Stack 社の<a href="https://shop.m5stack.com/collections/atom-series/products/atom-matrix-esp32-development-kit">ATOM Matrix</a>を顔とし，<a href="https://shop.m5stack.com/products/atom-mate">ATOM Mate</a>を利用して M5Stick C シリーズで使用する Hat を胴体にしたかわいいいきものです。Hat だけでなく，M5Stack 用に用意されているユニットも使用できるので，それらの機能を活用してさまざまな能力を持つ ATOM Babies をつくることができます。

ちなみに，ATOM Babies の名付け親は M5Stack 社 CEO の Jimmy Lai さんです。

<p align="center">
<img src="https://i.gyazo.com/338db268a5e14a0bea83b9184c8a8043.png" width="480"/>
</p>

## 環境設定

[Arduino IDE](https://docs.arduino.cc/software/ide-v1)と[PlatformIO IDE for VSCode](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide) 環境に対応しています。動作確認は Windows 11 上で実施しています。

### Arduino IDE

[https://github.com/3110/atom-babies-arduino/archive/refs/tags/v0.1.0.zip](https://github.com/3110/atom-babies-arduino/archive/refs/tags/v0.1.0.zip) をダウンロードし，メニューの「スケッチ」-「ライブラリをインクルード」-「.ZIP 形式のライブラリをインストール...」を選択して ZIP ファイルを読み込みます。読み込み後，念のために Arduino IDE を再起動してください。

サンプルはメニューの「ファイル」-「スケッチ例」にある「カスタムライブラリのスケッチ例」から「ATOM Babies」を選択し，「AllFaces」「Blink」「Bow」「Greeting」のいずれかを選びます。

### PlatformIO IDE for VScode

GitHub からコードをクローンし，VS Code で開きます。VS Code からコンパイル・ファームウェアの書き込みをする場合は，`platformio.ini`の`srd_dir`にコンパイルしたいサンプルのフォルダを指定し，PlatformIO:Upload ボタンを押します。

```platformio.ini
src_dir = examples/AllFaces
; src_dir = examples/Blink
; src_dir = examples/Bow
; src_dir = examples/Greeting
```

PlatformIO のターミナルからコンパイル・ファームウェアの書き込みをする場合は以下を実行します。

```bash
git clone https://github.com/3110/atom-babies-arduino
cd atom-babies-arduino
platformio run --target=upload
```

## 実装方法

顔の向きは USB-C の口を下にした向きを基準として以下のように定義しています。

<a href="https://gyazo.com/a210b4d6c098baf76d47a5af3ea772fb"><img src="https://i.gyazo.com/a210b4d6c098baf76d47a5af3ea772fb.png" alt="顔の向き" width="480"/></a>

顔の位置は顔の向きがどの方向でも以下のように定義しています。

<a href="https://gyazo.com/b02a5dcf99ae07560e1e334809e2e069"><img src="https://i.gyazo.com/b02a5dcf99ae07560e1e334809e2e069.png" alt="顔の位置" width="480"/></a>

通常の顔の向きで顔を表示するための最低限の実装は以下のようになります。
`M5.begin()`と`M5.update()`はそれぞれ`babies.begin()`と`babies.update()`の中で呼んでいるので，改めて呼ぶ必要はありません。

```c++
#include "AtomBabies.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;

void setup(void) {
    babies.begin();
    babies.display();
}

void loop(void) {
    babies.update();
}
```

### プラグインの実装

Hat や Unit の機能を使って実装するために，プラグインの機構を用意しました。
`plugins/AbstractAtomBabiesPlugin.h`をインクルードし，`AbstractAtomBabiesPlugin`を継承したクラスを実装してください。

実装例：`SamplePlugin.h`

```cpp
#pragma once

#include "plugins/AbstractAtomBabiesPlugin.h"

namespace M5Stack_AtomBabies {

class SamplePlugin : public AbstractAtomBabiesPlugin {
public:
    SamplePlugin(void) {
    }
    virtual ~SamplePlugin(void) {
    }

    // babies.setup()から呼び出される
    virtual bool begin(AtomBabies& babies) {
        return true;
    }

    // babies.update()から呼び出される
    virtual bool update(AtomBabies& babies) {
        return true;
    }

    virtual const char* getName(void) const {
        return "Sample";
    }
};

}  // namespace M5Stack_AtomBabies
```

実装したプラグインは`AtomBabies::begin()`を呼び出す前に`AtomBabies::addPlugin()`に渡します。追加できるプラグインの数は今のところ 10（`AtomBabies::MAX_PLUGINS`）で，追加した順に呼び出されます。

```cpp
#include "AtomBabies.h"
#include "SamplePlugin.h"

using namespace M5Stack_AtomBabies;

AtomBabies babies;
SamplePlugin sample;

void setup(void) {
    babies.addPlugin(sample);
    babies.begin(); // sample.begin(babies)が呼ばれる
}

void loop(void) {
    babies.update(); // sample.update(babies)が呼ばれる
}
```

具体的な実装例は`plugins/PIR.[h|cpp]`や，これらを使ったサンプル`Greeting`を参照してください。

## サンプルの説明

### すべての向きですべての顔を表示する（`AllFaces`）

ATOM Babies の顔の表示を一通り表示します。通常の向きから開始し，まばたきをした状態で始まります。ATOM Babies の顔（ボタン）を押すごとに右 → 逆さま → 左 → 通常と繰り返します。

### まばたきする（`Blink`）

ATOM Babies の顔（ボタン）を押すたびに，まばたきを始めたり止めたりします。
まばたきの設定はデフォルト（`AtomBabies.cpp`の`DEFAULT_BLINK_PARAM`を参照，2 回以下のランダムな回数でのまばたき，目を開けている時間が 500 ミリ秒，目を閉じている時間が 100 ミリ秒，次のまばたきを始めるまでの間隔が 1000 ミリ秒）です。

### おじぎをする（`Bow`）

ATOM Babies の顔（ボタン）を押すとおじぎをします。押すごとにおじぎをする回数が増えます。

### 数字を表示する（`Digits`)

ATOM Babies の顔（ボタン）を押すたびに 0 から順に数字が表示されます。2 桁以上の数字は各桁を順に表示します。

### 人が来たらあいさつをする（`Greeting`）

<p align="center">
<a href="https://youtu.be/hI7jYVK7nX0"><img src="https://img.youtube.com/vi/hI7jYVK7nX0/0.jpg" width="480"/></a><br>
ATOM Babies:人が来るとあいさつをする（YouTube）
</p>

人が目の前に来ると顔を出しておじぎをします。コンパイルする場合は環境を`env:greeting`に変更してください。

<p align="center">
<a href="https://i.gyazo.com/4491f8348d1d45264d212cc0f971b629.jpg"><img src="https://i.gyazo.com/4491f8348d1d45264d212cc0f971b629.jpg" width="480"/></a><br>
ATOM MatrixにATOM MateとPIR Hatを接続
</p>

[ATOM Mate](https://shop.m5stack.com/products/atom-mate)を使用して[PIR Hat](https://shop.m5stack.com/products/m5stickccompatible-hat-pir-sensor)を接続して利用します。
PIR Hat が胴体となるため，顔の向きは`OrientationUpsideDown`にします。

PIR は Passive pyroelectric infrared detector（受動焦電型赤外線検出器）の略で，人間が発する赤外線を検知し，目の前にいるかを判断します。
PIR Hat の仕様で，いったん検知した後，次に検知するまでに 2 秒間の遅れがあります。

### 頭をなでると喜ぶ（`Happy`）

<p align="center">
<a href="https://youtu.be/mfhnQ5W6wxA"><img src="https://img.youtube.com/vi/mfhnQ5W6wxA/0.jpg" width="480"/></a><br>
ATOM Babies:なでると喜ぶ（YouTube）
</p>

頭をなでると喜びます。試す場合は Hat を付けた状態で底面を固定しておいてください。

## 参考

- [Arduino IDE 1](https://docs.arduino.cc/software/ide-v1)
- [Visual Studio Code](https://azure.microsoft.com/ja-jp/products/visual-studio-code/)
- [PlatformIO](https://platformio.org/)
- [PlatformIO IDE for VSCode](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)

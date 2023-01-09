# signal-tower-server

## description
積層信号灯(パトライト)を遠隔で操作するためのサーバー

## how to use
ESP-IDF を使用する方法

1. [esp-idf](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)をインストール
2. 公式ドキュメントに従って環境構築を完了する
3. esp-idf のディレクトリ `esp/esp-idf/examples/get-started/sample_project` を参考にしてプロジェクトを作成
4. main.c を `signal-tower-server/main.c` に置き換える
5. プログラムをマイコン(ESP32-WROOM32d)に書き込む

API 仕様
- パトライトの各色(赤・黄・緑)の点灯を制御したいだけなので，GET メソッドを用いてクエリパラメータに点灯したい色を指定する
```
ip-address:80/light?color=red # 赤
ip-address:80/light?color=yellow # 黄
ip-address:80/light?color=green # 緑
```
- 3色以外の文字列を color に指定すると，全部のライトをOFFにする

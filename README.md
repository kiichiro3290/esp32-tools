
# ESP-IDF プロジェクト作成

## requirements
ESP-IDFの環境構築が済んでいること

### python 環境の構築
ESP-IDFの`idf.py`コマンドを使うためのpythonの環境構築<br>

pyenv を用いた環境構築<br>
pythonのバージョン管理にpyenvを用いる<br>
1. pyenv をインストール
```bash
$ brew install pyenv
```
2. 適当なバージョンのpythonをインストール
```bash
$ pyenv install 3.7.13
```
インストールが終了したら，globalに指定
```bash
$ pyenv global 3.7.13
```

3. インストールしたpythonに,ESP-IDFに必要なパッケージをインストール
pythonのバージョンを変更するたびに，実行する必要がある
```bash
$ cd [ESP_PATH]/esp/esp-idf // ESP-IDFのインストールディレクトリへ移動
$ python -m pip install -r requirements.txt
```

## プロジェクト作成
1. ディレクトリの作成
適当なディレクトリを作成する
```bash
$ mkdir sample-project
$ cd sample-project
```

2. パスを通す
ESP-IDFのディレクトリにある./export.shを実行してパスを通す<br>
.zshrcにaliasを作成しておくと便利
```.zshrc
alias get-idf = sh ${ESP_IDF_PATH}/export.sh 
```
パスを通すことで`idf.py`コマンドが実行できるようになる
```bash
$ get-idf
```

3. プロジェクトの作成
```bash
$ idf.py create-project hello-world
```
生成されるディレクトリの構成
 - hello-world/
    - main/
        - CMakeLists.txt
        - hello-world.c

4. ESP-IDFで定義してあるサンプルプログラムを利用したい
ESP-IDFではexamplesとして，すでにいくつかのプログラムが定義されている<br>
これらのプログラムを新しく作成したプロジェクトにincludeして利用したい<br>
 - CMakeLists.txtにincludeしたいサンプルのコードへのパスを追加する
```CMaleLists.txt
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/[PATH_TO_COMPONENT])
```








# TODO
 - CMake 周りを理解したい
 - CをCPPに置き換えしたい

## error fix
### includePathでエラーが出ているのを解消したい
内容
 - C言語のヘッダファイルが定義されている場所へのpathがVSCodeに認識されてなくて起きるエラー
原因
 - CMakeでビルドするときは,CMakeを通してヘッダファイルへのpathを認識してくれるが,VSCode側ではpathが認識されておらず,エラーが出てしまう
 - コンパイルには問題ないが，他のエラーを見逃してしまう可能性があるので，対処する
解決方法
 - 親ディレクトリで，`c_cpp_properties.json` ファイルを作成し，include_pathの設定を記述すればいける！
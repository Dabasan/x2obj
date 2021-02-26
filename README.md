# Model Converter

3Dモデルの形式を変換します。

## 概要

OBJやFBXといった様々なモデル形式を相互に変換することができます。
元々は、[X operations](https://hp.vector.co.jp/authors/VA022962/xops/)というWindows向けFPSゲームで使用されているX形式の3DモデルをOBJ形式に変換するために作成したプログラムです。

内部で[Assimp](https://github.com/assimp/assimp)というライブラリを使用しています。
入力モデルおよび出力モデルの形式は、Assimpが対応しているものなら何でも可能です。

## 使い方

### オプション一覧

|      オプション      |             意味             |
| :------------------: | :--------------------------: |
| -i, --inputFilepath  | 入力するモデルのファイルパス |
| -o, --outputFilepath | 出力するモデルのファイルパス |
|      -h, --help      |         ヘルプを表示         |
|    -v, --version     |     バージョン情報を表示     |

上の表に示した以外にも、Assimpで定義されている定数をオプションとして指定することができます。
詳しくは[postprocess.h File Reference](http://assimp.sourceforge.net/lib_html/postprocess_8h.html)を参照してください。

### 使用例

```
x2obj.exe -i ak47.x -o ak47.obj --aiProcessPreset_TargetRealtime_Quality
```

## プログラム情報

### 作者

駄場

### バージョン

0.1.0

### ライセンス

MIT


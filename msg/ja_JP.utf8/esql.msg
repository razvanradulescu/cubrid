
$
$ Copyright (C) 2008 Search Solution Corporation. All rights reserved by Search Solution. 
$
$   This program is free software; you can redistribute it and/or modify 
$   it under the terms of the GNU General Public License as published by 
$   the Free Software Foundation; either version 2 of the License, or
$   (at your option) any later version.
$
$  This program is distributed in the hope that it will be useful, 
$  but WITHOUT ANY WARRANTY; without even the implied warranty of 
$  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
$  GNU General Public License for more details. 
$
$  You should have received a copy of the GNU General Public License 
$  along with this program; if not, write to the Free Software 
$  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
$

$
$ $Id$
$
$ esql.msg
$

$set 1
1 "%1$s"はインテェジャーアーギュメントではありません。\n
2 "%1$s"はフロートアーギュメントではありません。\n
3 アーギュメント %1$s が抜いています。\n
4 [オプション] 
5 オプション: \n
6 キー %1$s に対するアーギュメントが抜いています。\n
7 わからないオプション　"%1$s"。\n
8 期待されてないポジショナルアーギュメント　"%1$s"。\n

$set 2
1 カーソル "%1$s"が再正義されています。
2  *\t : %1$s \n
3  *\n * カーソルテーブル:\n

$set 3
1 (set_class_bitのインターナルエラー): 正しくないストレージクラス (%1$d)
2 (add_spec_to_decl()のインターナルエラー): 正しくないチェーン (no specifier)
3 現在の文脈でストレージクラスの明細は許可されていません。
4 正しくないストレージクラスの組み合い
5 正しくないタイプ組み合い 
6 正しくないモディファイやーの組み合い
7 (pp_add_type_spec()のインターナルエラー): 予測されてないケース(%1$d)
8 (pp_add_type_adj()のインターナルエラー): 予測されてないケース(%1$d)
9 %1$s : 正しくない宣言

$set 4
1 %1$s
2 %1$s: ディレクトリーではありません。\n
3 %1$s: 正しくない設置ルート\n

$set 5
1 ?
2 正しいトークンか確認してください。
3 static カーソル "%1$s"にUSING説が使えません。
4 カーソル \"%1$s\" は正義されていません。
5 デスクリプターのポインータ
6 DB_OBJECTのポインータ
7 ストリング
8 "%1$s" のための指示バリアブルは許可されていません。
9 "%1$s" は BEGIN DECLARE sectionで宣言されていません。
52 %1$s ( "%2$s" )
53 Lexical エラー
54 "%1$s"でブンポエラー : %2$sが%3$sにありません。
55 "%1$s"でブンポエラー : %2$sがありません。

$set 6
1 サプレスーラインーディレクティブー
2 アウトレットーファイル
3 インクルードーファイル
4 ダンプースコープーインフォ
5 ダンプーmallocーインフォ
6 アナウンスーバージョン
7 イネイブルーuciートレース
8 ディスエイブルーvarcharーレングス
9 varchar2ースタイル
10 アンセーフーnull
11 インターナルーインディケータ
12 インプットーファイル

$set 7
1 出力ファイルに「#line」コマンドが見えないようにする。デフォルトは「off」
2 出力ファイル名。デフォルトは「<input-file>.c」または「stdout」
3 uci_プロトタイプを含めたincludeファイル。デフォルトは"esqlx.h"
4 スコープデバッギング情報を出力。デフォルトは「off」
5 最終mallocデバッギング情報を出力。デフォルトは「off」
6 バージョン情報を出力。デフォルトは「off」
7 uci_メソッドの実行情報の出力。デフォルトは「off」
8 VARCHARホストバリアブルのレングスが初期化されてない。デフォルトは「off」
9 違うVARCHARスタイルを使う。デフォルトは「off」
10 ignore -462(null indicator needed)エラー。デフォルトは「off」
11 -462(null indicator needed)エラーを防止するためにインターナルNULLインジケーターを使用。デフォルトは「off」
12 入力ファイル名。デフォルトは「stdin」

$set 8
1 REPEATEDオプションが無効になります。
2 %1$s バージョン %2$s\n

$set 9
1 (yy_pop_mode)インターナルエラー : 空のスタック
2 "%1$s"はこの文脈で許可されていません。

$set 10
1 インターナルエラー%1$s: %2$s\n
2 テーブルオーバーフロー

$set 11
1 インジケーターバリアブル"%1$s"はshort intタイプしか使えません。
2 ストラクチャーと共にインジケータバリアブルを使うことはできません。
3 不完全はストラクチャディフィニション"%1$s"
4 "%1$s"は有効は%2$sしか使えません。
5 %1$sは"%2$s"のために有効なタイプではありません。
6 予測されていないホストバリアブルタイプ: %1$s
7 (pp_addr_expr): NULLバリアブルのアドレス接近トライ
8 "%1$s"はポインタではありません。
9 "%1$s"はストラクチャーを示していない。
10 "%1$s"はストラクチャではありません。
11 %1$sはフィールド"%2$s"を持っていません。
12 "%1$s"は"%2$s"に逆リファレンスされる事はできません。

$set 12
1 メモリ不足

$set 13
1 メソッドの配列が正しくありません。
2 (pp_the_same_type)しらないルンククラス
3 (pp_get_sizeof): わからない名詞 (%1$d)

$set 14
1 (%1$sのインターナルエラー): インポッシブルケース

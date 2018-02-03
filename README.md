# Distributed-ThompthonSampling
分散型マルチエージェントシステム上でThompson抽出を実行することの検討
# What's this?
Thompson抽出という確率的Bandit用の方策を分散型マルチエージェントシステム上で協力して攻略するための方策のシミュレーション用プログラムです。
現時点ではプログラムしかアップロードしていません。理論的な部分については2018年3月頃にLaTeX,PDF形式でアップロードする予定です。
# 使い方
環境データの生成と実験結果の可視化にMathematicaが必要です。
ver11.2で動作を確認しています。
## コンパイル
次のものが必要です。
* g++ (7.2 で動作確認済)
* Qt (5.10.0 で動作確認済)

`simulation/algorithms/Type1/C++/Qt` と`simulation/algorithms/Type2/C++/Qt`にそれぞれType1とType2のQtプロジェクトがあります。
QtCreatorでインポートして各自の環境向けにコンパイルしてください。
尚、Windowsでの動作は確認していませんので悪しからず。
## 環境の設定
プログラムに入力する環境データはJSON形式で用意します。
`simulation/game-settings/envs`にサンプルの環境(env1)があります。
1つのフォルダが1つの環境に対応します。
`[環境名]`フォルダ内に`[環境名].json`の形で環境データファイルを置きます。
書式はサンプルを参考にしてください。
アームの期待値`mu`に関しては左から順にアーム1,2,...となっています。
__アーム1が最適アームであることを仮定しています__ のでご注意ください。
## 事前分布の設定
プログラムに入力する事前分布データはJSON形式で用意します。
先述の`[環境名]`フォルダ内に`priors`フォルダがあり、その中に事前分布データを置きます。
書式は例えば`prior1.json`を参考にしてください。
`alpha`と`beta`は左から順にアーム1,2,...となっています。
__現在は全てのプレイヤーで同じ事前分布を使う実装になっています。__
プレイヤー毎異なる事前分布を使うためにはプログラムの改良が必要です。
## グラフの設定
プログラムに入力するグラフデータはMathematicaによりJSON形式で生成します。
`simulation/game-settings/graphs`にデータファイル生成用のMathematicaノートブック(.nb ファイル)があります。
内容を見れば解ると思いますので説明しません。
既存のnbファイルをどれでも好きにコピーしてご自身の試したい環境を作ってください。
ノートブックを実行すると、`[ブック冒頭で定義されたグラフ名].json`の形でJSONファイルが保存されます。
## 実行
Type1もType2も呼び出し方は同じです。
下記のオプションを全て指定してください。
パスは相対指定可能です。
* --env : 環境データファイルのパス (例) --env "../../../../../game-settings/envs/env1/env1.json"
* --prior : 事前分布データファイルのパス (例) --prior "../../../../../game-settings/envs/env1/priors/prior1.json"
* --graph : グラフデータファイルのパス (例) --graph ../../../../../game-settings/graphs/Huge_WS.json"
* -T : プレイ時間 (例) --T 10000
* --rep : 実験を繰り返す回数(大きいほど結果のグラフが滑らかになる) (例) --rep 100
* --save : 実験結果の出力先フォルダのパス (例) --save "../../../results"
## 実行結果の可視化
Type1もType2も同じなのでType1で説明します。
`--save`オプションが`--save "../../../results"`であれば実行ファイルから3つ上がったところの`results`フォルダ`simulation/algorithms/Type1/results`内に実験結果が保存されます。
ファイル名はシミュレーションが完了した時刻に基づいて自動で決定されます。(書式`result_yyyy-MM-dd-hh:mm:ss:zzz.json`)
Type1の実行結果はMathematicaノートブック`simulation/algorithms/Type1/result_visualization.nb`を使って可視化します。
ノートブックを開いて「評価」->「ノートブックを評価」(Alt+V, then O)を実行してください。
ファイル選択ダイアログが開きますので、実験データ(JSONファイル)を選択します。
そうするとデータファイルが解析され、Regretのグラフが描画されます。
グラフは右クリックメニューよりベクタ画像形式でエクスポートできます。
# その他
`simulation/data-analysis.nb`は私が卒論で使用したデータ解析用のMathematicaノートブックです。
ひょっとすると何かの役に立つしれません。
# ライセンス
このプログラムをMITライセンスで公開します。  
(c) 2018 Motoki Kamimura  
Released under the MIT license  
http://opensource.org/licenses/mit-license.php

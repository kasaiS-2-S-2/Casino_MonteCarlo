# Casino_MonteCarlo
カジノの攻略法である、モンテカルロ法の検証プログラムです。（２倍、３倍配当のゲームに対応しています。）
## モンテカルロ法とは？
https://www.casino-winnersclub.com/monte-carlo.php
## 使い方
### 0. このプログラムを実行するには、C言語を実行できる環境が必要です。また、グラフを描く場合はgnuplotを使用するため、インストールが必要です。
C言語の環境構築：https://lyncs.hateblo.jp/entry/2018/03/11/032127#%E5%AF%BE%E8%B1%A1%E3%81%A8%E3%81%AA%E3%82%8B%E8%AA%AD%E8%80%85

gnuplotのインストール（Windows）：http://www.yamamo10.jp/yamamoto/comp/gnuplot/inst_win/index.php

gnuplotのインストール（Linux）：http://www.yamamo10.jp/yamamoto/comp/gnuplot/inst_linux/index.php
### 1. ターミナルで下記を実行し、リポジトリをクローンします。
git clone https://github.com/kasaiS-2-S-2/Casino_MonteCarlo.git
### 2. 上記を行うと新しいフォルダができるので、その中で下記コマンドを入力し、コンパイルします。
gcc -msse2 -DSFMT_MEXP=19937 SFMT.c MonteCarlo.c
### 3. 下記コマンドを実行し、プログラムを開始します。
./a.out

### 4〜. ここから検証に必要な情報を入力していきます。それぞれ入力した後はEnterを押すと次に進みます。数値はずべて半角で入力してください。数値のみ入力してください。

### 4. 数列、勝敗を表示しますか？（表示するなら１表示しないなら−１）:
-1 を入力すると、結果のみ表示されます。
### 5. グラフ書きますか？（書くなら１書かないなら−１）:
1を入力すると、3つのグラフを表示します。
### 6. 予算を入力してください（単位＄）：
### 7. 最高賭け金をいくらにするか入力してください（単位＄）(予め決めない場合は-1を入力)：
最高賭け金を決めた場合、bet額がその金額を超える場合は現時点での数列での試行を終了し、次の試行を開始します。

予め決めない場合は、モンテカルロ法が成功する（数列が無くなる）か、失敗する（持金が0になる）までモンテカルロ法を試行し続けます。
### 8. シミュレーションするゲームの倍率を入力してください(2か3)：
ゲームで勝った場合の配当の倍率を入力してください。（配当がbet額の2倍のゲームと3倍のゲームにのみ対応しています）

（例）ヨーロピアンルーレットの赤黒賭けならば、2を入力します。
### 9. 今回のゲームの勝率を入力してください(％で)：
検証するゲームの１勝負で勝つ確率を入力してください。

（例）ヨーロピアンルーレットの赤黒賭けならば、48.65%を入力します。
### 10.数列を何回繰り返すか、回数で入力してください(整数で)：
数列1回 = 数列（モンテカルロ法）が成功するか、失敗するかまでゲームを繰り返すこと を意味します。

数列の成功 = 数列が消えた場合を表しています。

数列の失敗 = 数列が消えず、bet額が持金を上回った場合を表しています。

※数列1回の試行が終わると、持金、bet額、数列が初期状態になってから次の試行へ移ります。

（例）100と入力した場合、数列（モンテカルロ法）が成功するか、失敗するまで試行し続けることを、100回繰り返します。
### 11. 数列に追加するデータを入力してください(-1入力で終了)(数字毎enterを入力)：
（例）数値を 1, 2, 3, -1 の順で入力した場合、試行する数列は「1 2 3」となります。
### 12. プログラムが実行され、結果が表示されます。

## 結果の見方
### 勝ちました or 負けました X Y Z 
これは、上記の4.で1を入力すると表示されます。

ゲーム毎の勝敗と、勝敗を受けての数列が表示されます。
### 全体収支：
行った試行全体の収支が表示されます。
### ペイアウト率：
行った試行全体のペイアウト率が表示されます。
※ペイアウト率は、(総払い出し金額 / 総bet金額) * 100 で算出される値です。

※100%を超えると収支がプラスになります。
### 数列X回中、Y回成功、Z回失敗した
上記10.で入力した回数の分数列（モンテカルロ法）を試行した結果が表示されます。
※数列の成功 = 数列が消えた場合を表しています。

※数列の失敗 = 数列が消えず、bet額が持金を上回った場合を表しています。
### 成功時には、平均X儲かった。
数列が成功したときの儲かった額の平均値が表示されます。
### X回に１回失敗した。その場合、平均でY負けた。
X:上記の10.で入力した、数列の試行回数の内、何回に１回失敗したかが表示されます。

Y:失敗した際の負け額の平均値が表示されます。
### 数列が消せた場合の最高賭け金：X、一番負けていた時の損益額：Y
X：数列が成功した際の全試行の中で、一度にbetした額の最高値が表示されます。

Y：数列が成功した際の全試行の中で、一番損の額が大きかった場面の損益額が表示されます。
### 消せなかった場合の最高賭け金：
数列が失敗した際の全試行の中で、一度にbetした額の最高値が表示されます。

### グラフの見方
例として、試行回数を100万回（内成功した回数は996039回）、数列を「1 2 3」、予算3000＄、払い出し倍率２倍、勝率48.65%（ヨーロピアンルーレットの赤黒）に設定したグラフを使います。

![image](https://user-images.githubusercontent.com/67869001/122716392-28bf6200-d2a5-11eb-9cd3-6c82334f46d1.png)

上記のグラフは、数列成功回数と成功した場合にかかったゲーム数の関係を表しています。


グラフを見てみると、かかったゲーム数が〜10回,〜20回の合計の数列成功回数が約95万回であったことが読み取れます。これにより数列が成功する場合には、その9.5割程が20ゲームまでに終わるということを示しています。つまり、負けが混み数列が消えぬまま20ゲーム目までいってしまった場合、それ以降に試行したとしてもその数列が消える（成功する）確率は約5%程で、このまま続けてもおそらく数列が失敗することが読み取れるので、損切りを考えた方が良いと考えることができます。

![image](https://user-images.githubusercontent.com/67869001/122718038-658c5880-d2a7-11eb-9c7d-0d19f35c0232.png)

次のグラフは、数列成功回数と成功した場合の最高bet額の関係を表しています。


グラフを見てみると、最高bet額が〜10＄,〜20＄,〜30＄の合計の数列成功回数が約93万回であったことが読み取れます。これにより数列が成功する場合には、その9.3割程がbet額が30＄以内に収まることを示しています。つまり、負けが混みbet額が30$までに膨れ上がった場合に、それ以降に試行したとしてもその数列が消える（成功する）確率は約7%程で、このまま続けてもおそらく数列が失敗をすることが読み取れるので、損切りを考えた方が良いと考えることができます。

![image](https://user-images.githubusercontent.com/67869001/122719086-ac2e8280-d2a8-11eb-8e0a-18657780815c.png)

３つめのグラフは、数列成功回数と成功した際の最高負け額の関係を示しています。

最高負け額  数列試行中に、一番損の額が大きくなった（予算に対して一番マイナス額が大きくなった）場面の損益額


グラフを見てみると、最高負け額が〜100$までの合計の数列成功回数が約95万回であったことが読み取れます。これにより数列が成功する場合には、その9.5割程が負け額が100＄以内に収まることを示しています。つまり、負けが混み負け額が100$までに膨れ上がった場合に、それ以降に試行したとしてもその数列が消える（成功する）確率は約5%程で、このまま続けてもおそらく数列が失敗をすることが読み取れるので、損切りを考えた方が良いと考えることができます。

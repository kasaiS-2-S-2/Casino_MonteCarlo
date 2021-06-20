# Casino_MonteCarlo
カジノの攻略法である、モンテカルロ法の検証プログラムです。
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

### 4〜. ここから検証に必要な情報を入力していきます。それぞれ入力した後はEnterを押すと次に進みます。

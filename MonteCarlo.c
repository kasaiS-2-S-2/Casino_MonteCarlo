/*
   SFMTを含むプログラムの実行の際のコンパイル
   gcc -msse2 -DSFMT_MEXP=19937 SFMT.c プログラム名
   （本当は、gccのあとに -03 っていう最適化？オプションがあるけれど、これいれるとコンパイル通らないからわかんないけど抜いた）
 */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include "SFMT.h"/*メルセンヌ・ツイスタをインクルード*/

/* 自己参照的構造体（Sequence型）を宣言 */
typedef struct node{
    int data;            /*　数列のデータ */
    struct node *next; /* 次のノードを指すポインタ */
}Sequence;/*数列の構造体*/

sfmt_t sfmt;/*メルセンヌ・ツイスタを使う際に必要な構造体への変数*/

/*プロトタイプ宣言*/
Sequence* create_node(int);
Sequence* add_end_list2(Sequence *, int);
void print_list(Sequence *);
int Bet(Sequence *, Sequence *, int);
int Maxbet(int, int);
int Gamble(double);
void IfWin(Sequence **, Sequence **, int);
void IfLose(Sequence *, Sequence **, int);
int Maxlose(int, int, int);
double Winrate(int *, int *, int);

int main(void)
{

    FILE *data1, *data2, *data3, *gp ,*gp2, *gp3;/*プログラム上でgnuでグラフを書く際に必要なぽいんた*/
    char *data_file1, *data_file2, *data_file3;/*グラフをつ来る際に、データｔｘｔの名前を保持するポインタ*/
    //char *data_png, *data_png2;/*png画像を作る際の画像名を保持するポインタ

    int budget=0, money=0, count=0;/*予算、持金、数列の繰り返し数*/
    double rate = 0.0;/*使用するゲームの勝率*/

    int flag1, flag2, flag3, showGameResultFlag;/*数列を作るノードが１つor２つorそれ以上かどうか, ゲームに勝ったかどうか, 数列、勝敗を表示するかどうか*/
    int bet=0, maxbet=0, maxlose=0, game=0, wincount=0, profit=0, collect1=0, i=1, j=0, losecount=0, winmaxlose=0, winmaxbet=0, losemaxbet=0, premaxbet=0, odds=0; /*bet額、最高賭け金、一番負けていた金額、数列が消えるまで、もしくは破産するまでのゲーム数、勝った回数、ゲームごとの利益、数列ごと利益、全体の利益、数列の繰り返しをカウントするための変数、その他カウントに必要な変数、数列失敗した回数、数列を消せた場合の一番負けていた金額、最高賭け金、数列を消せなかった場合の最高賭け金*、予め決めておく最高賭け金、プレイするゲームの倍率*/
    long int totallose=0, totalbet=0, totalpayout=0, collect2=0;/*数列消せなかった時の負け額の合計、賭けた金額の合計、払い戻し金額の合計*/

    double winrate = 0.0, payout = 0.0;/*実際の勝率*/

    int losemoney[15];/*決めた金額範囲での数列消せた時の最高負け額*/
    int maxmoney[15];/*決めた金額範囲での数列消せた時の最高賭け金*/
    int gamecount[15];/*決めた範囲内での数列終えるまでにかかったゲーム数の集計*/
    double losemoneyrate[15];/*その累積比率*/
    double maxmoneyrate[15];/*その累積比率*/
    double gamecountrate[15];/*その累積比率*/

    for(j=0; j<15; j++){/*配列の値を初期化*/
        losemoney[j] = 0;
        maxmoney[j] = 0;
        gamecount[j] = 0;
        losemoneyrate[j] = 0.0;
        maxmoneyrate[j] = 0.0;
        gamecountrate[j] = 0.0;
    }

    printf("数列、勝敗を表示しますか？（表示するなら１表示しないなら−１）:");
    scanf("%d", &showGameResultFlag);
    printf("グラフ書きますか？（書くなら１書かないなら−１）:");
    scanf("%d", &flag3);
    printf("予算を入力してください（単位＄）：");
    scanf("%d", &budget);
    printf("最高賭け金をいくらにするか入力してください（単位＄）(予め決めない場合は-1を入力)：");
    scanf("%d", &premaxbet);

    do{
        printf("シミュレーションするゲームの倍率を入力してください(2か3)：");
        scanf("%d", &odds);
    }while(odds != 2 && odds != 3);

    printf("今回のゲームの勝率を入力してください(％で)：");
    scanf("%lf", &rate);
    printf("数列を何回繰り返すか、回数で入力してください(整数で)：");
    scanf("%d", &count);

    money = budget;/*最初の持金＝予算*/

    if(premaxbet == -1){/*−１が入力されると、premaxbet＝予算にする*/
        premaxbet = budget;
    }

    sfmt_init_gen_rand(&sfmt, (unsigned)time(NULL));/*現在時間をseedとした乱数の生成*/
    //init_genrand((unsigned)time(NULL));/*現在時間をseedとした乱数の生成*/
    //srand((unsigned int)time(NULL));

    int item;
    Sequence *list = NULL; /* 数列の先頭 */
    Sequence *tail = NULL; /* 数列の末尾 */
    Sequence *list2 = NULL; /* 保存用の数列の先頭 */
    Sequence *tail2 = NULL; /* 保存用の数列の末尾 */
    Sequence *current = NULL;/*数列をたどるときに使うポインタ*/
    Sequence *remove_node = NULL;/*削除する数列の要素を指すポインタ*/
    Sequence *remove_node2 = NULL;/*削除する数列の要素を指すポインタ*/

    do{
        printf("数列に追加するデータを入力してください(-1入力で終了)(数字毎enterを入力)：");
        scanf("%d", &item);
        if(item != -1){
            tail = add_end_list2(tail, item);
            tail2 = add_end_list2(tail2, item);/* リストの末尾に追加 */
            if(list == NULL && list2 == NULL){/* 初回のデータ入力時 */
                list = tail;
                list2 = tail2;/* リストの先頭を記憶 */
            }
        }
    }while(item != -1);

    print_list(list); /* リストの表示 */

    while(list != NULL && tail != NULL && i <= count /*&& money >= 0*/){/*１つ目のループ：数列が存在し、入力した繰り返し数に達していなくて、持金が0になっていない限り続ける。ここの条件多分違う、百万回とかにすると無限ループになる*/

        /*ここいまんとこいらん（数列消すの失敗した時にはbreakするから）*/
        //if(money < budget){/*持金が予算より少ないかどうか*/
        //while(money < budget && collect2 >= 0){/*上の場合かつ全体の利益がマイナスにならない範囲で*/
        //   collect2--;
        //  money++;
        //}/*全体の利益から持金へ、持金が予算に到達するまで金を供給（毎回の数列をできるだけ、最初の予算から始めたい*/
        // }


        if(showGameResultFlag == 1) {
            printf("\n%d  回目", i);/*ゲームに勝ったかどうか *//////////////////////////////////////////////////////
            print_list(list);/*現状の数列の表示*/////////////////////////////////////////////////////////////////
        }

        money = budget;/*持金を予算に再設定*/

        while(list != NULL && tail != NULL){/*２つ目のループ：数列が存在する限り続ける*/

            /*倍率２倍の時*/
            if(odds == 2){

                if(list == tail){ /*数列が数字が１つかどうか*/
                    flag1 = 1;/*１つなら１を渡す*/
                    if(money >= tail->data){/*持金がそのデータより多いかどうか*/
                        bet = Bet(list, tail, flag1);/*そのデータのみ賭ける*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がそのデータより低ければ、ループを抜ける*/
                    }
                }else if(list->next == tail){/*数列の数字が２つの時*/
                    flag1 = 2;/*２つなら、２を渡す。*/
                    if(money >= (list->data + tail->data) ){/*数列が数字が１つかどうか*/
                        bet = Bet(list, tail, flag1);/*数列の先頭と、末尾のデータの和を賭ける。*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がデータの和より低ければ、ループを抜ける*/
                    }
                }else{
                    flag1 = 3;/*３つ以上なら、３を渡す。*/
                    if( money >= (list->data + tail->data) ){/*数列が数字が１つかどうか*/
                        bet = Bet(list, tail, flag1);/*数列の先頭と、末尾のデータの和を賭ける。*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がデータの和より低ければ、ループを抜ける*/
                    }
                }

                totalbet += bet;/*トータルの賭け金の計測*/

                maxbet = Maxbet(maxbet, bet);/*maxbet額の更新*/

                flag2 = Gamble(rate);/*ゲームの勝敗を決める*/

                if(flag2 == 1){/*勝ったら*/
                    totalpayout += bet*2;/*払い戻し金額の合計を計測*/
                    money += bet;/*持金 + 賭け金*/
                    IfWin(&list, &tail, flag1);/*勝った時の数列の処理*/
                    if(showGameResultFlag == 1) {
                        printf("勝ちました  ");/*ゲームに勝ったかどうか *//////////////////////////////////////////////////////
                        print_list(list);/*現状の数列の表示*/////////////////////////////////////////////////////////////////
                    }
                }else if(flag2 == 0){/*負けたら*/
                    money -= bet;/*持金 - 賭け金*/
                    IfLose(list, &tail, bet);/*負けた時の数列の処理*/
                    maxlose = Maxlose(budget, money, maxlose);/*一番負けていた額を更新*/
                    if(showGameResultFlag == 1) {
                        printf("負けました  ");/*ゲームに勝ったかどうか */////////////////////////////////////////////////////
                        print_list(list);/*現状の数列の表示*////////////////////////////////////////////////////////////
                    }
                }
                winrate = Winrate(&game, &wincount, flag2);/*数列が消える、または破産するまでの勝率を計算*/

                /*１ゲームごとに持金＞予算だったら利確する手続き*/
                //if(money > budget){/*持金が予算を上回っていたら*/
                //profit = money - budget;/*予算より上回っている分（ゲームごとの利益）を*/
                //money -= profit;/*持金から引いて、最初の予算にする（利益は別にとっとく）*/
                //collect1 += profit;/*今回の利益を数列ごとの収益へ*/
                //}

                /*倍率３倍の時*/
            }else if(odds == 3){

                if(list == tail){ /*数列が数字が１つかどうか*/
                    flag1 = 1;/*１つなら１を渡す*/
                    if(money >= tail->data){/*持金がそのデータより多いかどうか*/
                        bet = Bet(list, tail, flag1);/*そのデータのみ賭ける*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がそのデータより低ければ、ループを抜ける*/
                    }
                    totalbet += bet;/*トータルの賭け金の計測*/
                    maxbet = Maxbet(maxbet, bet);/*maxbet額の更新*/
                    flag2 = Gamble(rate);/*ゲームの勝敗を決める*/
                    if(flag2 == 1){/*勝ったら*/
                        totalpayout += bet*3;/*払い戻し金額の合計を計測*/
                        money += bet*2;/*持金 + 賭け金✕２*/
                        IfWin(&list, &tail, flag1);/*勝った時の数列の処理*/
                        if(showGameResultFlag == 1) {
                            printf("勝ちました  ");/*ゲームに勝ったかどうか *//////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*/////////////////////////////////////////////////////////////////
                        }
                    }else if(flag2 == 0){/*負けたら*/
                        money -= bet;/*持金 - 賭け金*/
                        IfLose(list, &tail, bet);/*負けた時の数列の処理*/
                        maxlose = Maxlose(budget, money, maxlose);/*一番負けていた額を更新*/
                        if(showGameResultFlag == 1) {
                            printf("負けました  ");/*ゲームに勝ったかどうか */////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*////////////////////////////////////////////////////////////
                        }
                    }

                }else if(list->next == tail){/*数列の数字が２つの時*/
                    flag1 = 2;/*２つなら、２を渡す。*/
                    if(money >= (list->data + tail->data) ){/*持金が、数列の両端の数字の和以上なら*/
                        bet = Bet(list, tail, flag1);/*数列の先頭と、末尾のデータの和を賭ける。*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がデータの和より低ければ、ループを抜ける*/
                    }
                    totalbet += bet;/*トータルの賭け金の計測*/
                    maxbet = Maxbet(maxbet, bet);/*maxbet額の更新*/
                    flag2 = Gamble(rate);/*ゲームの勝敗を決める*/
                    if(flag2 == 1){/*勝ったら*/
                        totalpayout += bet*3;/*払い戻し金額の合計を計測*/
                        money += bet*2;/*持金 + 賭け金✕２*/
                        IfWin(&list, &tail, flag1);/*勝った時の数列の処理*/
                        if(showGameResultFlag == 1) {
                            printf("勝ちました  ");/*ゲームに勝ったかどうか */ //////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*/////////////////////////////////////////////////////////
                        }
                    }else if(flag2 == 0){/*負けたら*/
                        money -= bet;/*持金 - 賭け金*/
                        IfLose(list, &tail, bet);/*負けた時の数列の処理*/
                        maxlose = Maxlose(budget, money, maxlose);/*一番負けていた額を更新*/
                        if(showGameResultFlag == 1) {
                            printf("負けました  ");/*ゲームに勝ったかどうか */////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*/////////////////////////////////////////////////////////
                        }
                    }

                }else if(list->next->next == tail){/*数列の数字が３つの時*/
                    flag1 = 3;/*３つ以上なら、３を渡す。*/
                    if( money >= (list->data + tail->data) ){/*持金が、数列の両端の数字の和以上なら*/
                        bet = Bet(list, tail, flag1);/*数列の先頭と、末尾のデータの和を賭ける。*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がデータの和より低ければ、ループを抜ける*/
                    }
                    totalbet += bet;/*トータルの賭け金の計測*/
                    maxbet = Maxbet(maxbet, bet);/*maxbet額の更新*/
                    flag2 = Gamble(rate);/*ゲームの勝敗を決める*/
                    if(flag2 == 1){/*勝ったら*/
                        totalpayout += bet*3;/*払い戻し金額の合計を計測*/
                        money += bet*2;/*持金 + 賭け金✕２*/
                        IfWin(&list, &tail, flag1);/*勝った時の数列の処理*/
                        flag1 = 1;/*ここでの数列の数字は１つのはずなので、２倍の時の１つしかないときの数列の処理をこの後すれば良い*/
                        IfWin(&list, &tail, flag1);
                        if(showGameResultFlag == 1) {
                            printf("勝ちました  ");/*ゲームに勝ったかどうか *////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*////////////////////////////////////////////////////////////////////
                        }
                    }else if(flag2 == 0){/*負けたら*/
                        money -= bet;/*持金 - 賭け金*/
                        IfLose(list, &tail, bet);/*負けた時の数列の処理*/
                        maxlose = Maxlose(budget, money, maxlose);/*一番負けていた額を更新*/
                        if(showGameResultFlag == 1) {
                            printf("負けました  ");/*ゲームに勝ったかどうか *//////////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*////////////////////////////////////////////////////////////////////
                        }
                    }

                }else if(list->next->next->next == tail){/*数列の数字が４つの時*/
                    flag1 = 3;/*３つ以上なら、３を渡す。*/
                    if( money >= (list->data + tail->data) ){/*持金が、数列の両端の数字の和以上なら*/
                        bet = Bet(list, tail, flag1);/*数列の先頭と、末尾のデータの和を賭ける。*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がデータの和より低ければ、ループを抜ける*/
                    }
                    totalbet += bet;/*トータルの賭け金の計測*/
                    maxbet = Maxbet(maxbet, bet);/*maxbet額の更新*/
                    flag2 = Gamble(rate);/*ゲームの勝敗を決める*/
                    if(flag2 == 1){/*勝ったら*/
                        totalpayout += bet*3;/*払い戻し金額の合計を計測*/
                        money += bet*2;/*持金 + 賭け金✕２*/
                        IfWin(&list, &tail, flag1);/*勝った時の数列の処理*/
                        flag1 = 2;/*ここでの数列の数字は２つのはずなので、２倍の時の２つしかないときの数列の処理をこの後すれば良い*/
                        IfWin(&list, &tail, flag1);
                        if(showGameResultFlag == 1) {
                            printf("勝ちました  ");/*ゲームに勝ったかどうか *///////////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*//////////////////////////////////////////////////////////////////////
                        }
                    }else if(flag2 == 0){/*負けたら*/
                        money -= bet;/*持金 - 賭け金*/
                        IfLose(list, &tail, bet);/*負けた時の数列の処理*/
                        maxlose = Maxlose(budget, money, maxlose);/*一番負けていた額を更新*/
                        if(showGameResultFlag == 1) {
                            printf("負けました  ");/*ゲームに勝ったかどうか *///////////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*/////////////////////////////////////////////////////////////////////
                        }
                    }

                }else{/*数列の数字が５つ以上の時*/
                    flag1 = 3;/*３つ以上なら、３を渡す。*/
                    if( money >= (list->data + tail->data) ){/*持金が、数列の両端の数字の和以上なら*/
                        bet = Bet(list, tail, flag1);/*数列の先頭と、末尾のデータの和を賭ける。*/
                        if(bet > premaxbet){
                            break;
                        }
                    }else{
                        break;/*持金がデータの和より低ければ、ループを抜ける*/
                    }
                    totalbet += bet;/*トータルの賭け金の計測*/
                    maxbet = Maxbet(maxbet, bet);/*maxbet額の更新*/
                    flag2 = Gamble(rate);/*ゲームの勝敗を決める*/
                    if(flag2 == 1){/*勝ったら*/
                        totalpayout += bet*3;/*払い戻し金額の合計を計測*/
                        money += bet*2;/*持金 + 賭け金✕２*/
                        IfWin(&list, &tail, flag1);/*勝った時の数列の処理*/
                        IfWin(&list, &tail, flag1);/*ここでの数列の数字は３つ以上のはずなので、２倍の時の３つ以上の数列の処理をこの後すれば良い*/
                        if(showGameResultFlag == 1) {
                            printf("勝ちました  ");/*ゲームに勝ったかどうか *///////////////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*///////////////////////////////////////////////////////////////////////////
                        }
                    }else if(flag2 == 0){/*負けたら*/
                        money -= bet;/*持金 - 賭け金*/
                        IfLose(list, &tail, bet);/*負けた時の数列の処理*/
                        maxlose = Maxlose(budget, money, maxlose);/*一番負けていた額を更新*/
                        if(showGameResultFlag == 1) {
                            printf("負けました  ");/*ゲームに勝ったかどうか */////////////////////////////////////////////////////
                            print_list(list);/*現状の数列の表示*/////////////////////////////////////////////////////////////
                        }
                    }
                }

                winrate = Winrate(&game, &wincount, flag2);/*数列が消える、または破産するまでの勝率を計算*/

                /*１ゲームごとに持金＞予算だったら利確する手続き*/
                //if(money > budget){/*持金が予算を上回っていたら*/
                //profit = money - budget;/*予算より上回っている分（ゲームごとの利益）を*/
                //money -= profit;/*持金から引いて、最初の予算にする（利益は別にとっとく）*/
                //collect1 += profit;/*今回の利益を数列ごとの収益へ*/
                //}

            }

        }/*２つ目のループ終わり*/

        /*１数列ごとに持金＞予算だったら利確する手続き*/
        if(money > budget){/*持金が予算を上回っていたら*/
            collect1 = money - budget;/*予算より上回っている分を利益として一時保存*/
            money -= collect1; 
        }

        /*数列ごとに持金＜予算だったらトータルの負け額を計測*/
        if(money < budget){
            totallose += (budget - money);/*予算からの負け額を追加*/
        }

        if(list == NULL && tail == NULL){/*数列を消せたかどうか*/

            if(winmaxbet < maxbet){
                winmaxbet = maxbet;/*数列を消せた場合の、最高賭け金*/
            }
            if(winmaxlose < maxlose){
                winmaxlose = maxlose;/*数列を消せた場合の、一番負けていた金額*/
            }

            /*最高賭け金、最高負け額、かかったゲーム数が０以上１０以下の時の処理*/

            if(maxlose >= 0 && maxlose <= 10){
                losemoney[0]++;
            }
            if(maxbet >= 0 && maxbet <= 10){
                maxmoney[0]++;
            }
            if(game >= 0 && game <= 10){
                gamecount[0]++;
            }

            /*最高賭け金、最高負け額、かかったゲーム数が１１以上５０以下の時の処理、１０刻みでカウント*/
            for(j=1; j<5; j++){
                if(maxlose > j*10 && maxlose <= j*10+10){
                    losemoney[j]++;
                }
                if(maxbet > j*10 && maxbet <= j*10+10){
                    maxmoney[j]++;
                }
                if(game > j*10 && game <= j*10+10){
                    gamecount[j]++;
                }

            }

            /*最高賭け金、最高負け額、かかったゲーム数が５１以上５００以下の時の処理、５０刻みでカウント*/
            for(j=5; j<14; j++){
                if(maxlose > (j-4)*50 && maxlose <= (j-4)*50+50){
                    losemoney[j]++;
                }
                if(maxbet > (j-4)*50 && maxbet <= (j-4)*50+50){
                    maxmoney[j]++;
                }
                if(game > (j-4)*50 && game <= (j-4)*50+50){
                    gamecount[j]++;
                }

            }
            /*最高賭け金、最高負け額、かかったゲーム数が５００より上の時の処理、それ以降は一箇所にカウント*/

            if(maxlose > 500){
                losemoney[14]++;
            }
            if(maxbet > 500){
                maxmoney[14]++; 
            }
            if(game > 500){
                gamecount[14]++;
            }

            //printf("成功\n");/*数列を消すことができた*/
        }else{

            while(list != NULL){/*余った数列（メモリ）を解放*/
                remove_node = list; /* remove_nodeは削除するノードを指す list自体が次のノードを示すから、list->nextは２つ後*/
                list = list->next;  /* listは削除後の先頭を指す */
                free(remove_node);  /* ノードの削除 */
            }
            tail = NULL;

            if(losemaxbet < maxbet){
                losemaxbet = maxbet;/*数列を消せなかった場合の、最高賭け金*/
            }

            losecount++;/*数列を失敗した回数を計測*/

            //printf("失敗\n");/*数列を消せず、破産した*/
        }

        /*これは、始めに打った数列を復元して、次の試行でまた使うためにやる*/
        current = list2;/*先頭からたどる*/
        do{
            tail = add_end_list2(tail, current->data); /* リストの末尾に追加 */
            current = current->next;/*次のノードへ*/
            if(list == NULL){                 /* 初回のデータ入力時 */
                list = tail;                    /* リストの先頭を記憶 */
            }
        }while(current != NULL);

        //printf("数列ごとの、、、 最高賭け金：%d 一番負けていた金額：%d 勝率：%.2f％ 収支：%d  %d回転目\n\n", maxbet, maxlose, winrate, (money + collect1)-budget, i);

        collect2 += collect1;/*数列ごとの利益を全体の利益として加算*/
        collect1 = 0;/*数列ごとの利益のカウントを一旦初期化*/

        maxbet=0;/*数列ごとの最高賭け金の初期化*/
        maxlose=0;/*数列ごとの最高負け額の初期化*/
        game=0;/*数列ごとのゲーム数の初期化*/
        wincount=0;/*数列ごとの勝った回数の初期化*/
        winrate = 0.0;/*数列ごとのゲームの勝率の初期化*/

        i++;

    } /*１つ目のループ終わり*/

    while(list != NULL && list2 != NULL){/*余分に作った数列（うまくやればもっとうまく書けそうだけどだるいからやだ）、保存用の数列（メモリ）を解放*/
        remove_node = list;
        remove_node2 = list2; /* remove_nodeは削除するノードを指す list2自体が次のノードを示すから、list2->nextは２つ後*/
        list = list->next;
        list2 = list2->next;  /* list2は削除後の先頭を指す */
        free(remove_node);  /* ノードの削除 */
        free(remove_node2);  /* ノードの削除 */
    }
    tail = NULL;
    tail2 = NULL;

    for(j=0; j<15; j++){/*数列ごとの最高賭け金、最高負け額、かかったゲーム数のそれぞれの累積比率を計算*/
        if(j == 0){

            losemoneyrate[j] = ( (double)losemoney[j]/(count - losecount) ) * 100 ;/*始めはただ代入*/
            maxmoneyrate[j] = ( (double)maxmoney[j]/(count - losecount) ) * 100 ;
            gamecountrate[j] = ( (double)gamecount[j]/(count - losecount) ) * 100 ;

        }else{
            losemoneyrate[j] = losemoneyrate[j-1] + ( (double)losemoney[j]/(count - losecount) ) * 100;/*2回目からはひとつ前のものを今回のものに足し合わせる。（累積）*/
            maxmoneyrate[j] = maxmoneyrate[j-1] + ( (double)maxmoney[j]/(count - losecount) ) * 100;
            gamecountrate[j] = gamecountrate[j-1] + ( (double)gamecount[j]/(count - losecount) ) * 100;
        }
    }

    printf("\n全体収支：%ld 儲かった\n\n", collect2 - totallose);/*全体利益ートータル負け額*/
    printf("ペイアウト率：%f％\n\n", ((double)totalpayout/totalbet)*100);
    printf("数列%d回中、%d回成功、%d回失敗した\n\n", i-1, count-losecount, losecount);
    if(losecount >= 1){
        printf("成功時には、平均%.1f儲かった。\n\n", (float)collect2/(float)(count-losecount));
        printf("%.1f回に1回失敗した。その場合、平均で%.1f負けた。\n\n", (float)count/losecount, (float)totallose/losecount);
    }
    printf("数列が消せた場合の最高賭け金：%d、一番負けていた時の損益額：-%d、\n消せなかった場合の最高賭け金：%d\n\n", winmaxbet, winmaxlose, losemaxbet);

    /*ファイルオープン*/
    data1 = fopen("data1.txt","w");
    data2 = fopen("data2.txt","w");
    data3 = fopen("data3.txt","w");

    /*配列のデータを別ファイルに書き込み*/
    for(j=0; j<5; j++){
        if(j == 0){/*０から１０の時*/
            fprintf(data1,"〜%d %d %f\n", j+10, losemoney[j], losemoneyrate[j]);
            fprintf(data2,"〜%d %d %f\n", j+10, maxmoney[j], maxmoneyrate[j]);
            fprintf(data3,"〜%d %d %f\n", j+10, gamecount[j], gamecountrate[j]);
        }else{/*１１から５０の時*/
            fprintf(data1,"〜%d %d %f\n", (j*10)+10, losemoney[j], losemoneyrate[j]);
            fprintf(data2,"〜%d %d %f\n", (j*10)+10, maxmoney[j], maxmoneyrate[j]);
            fprintf(data3,"〜%d %d %f\n", (j*10)+10, gamecount[j], gamecountrate[j]);
        }
    }

    for(j=5; j<14; j++){/*５１から５００の時*/
        fprintf(data1,"〜%d %d %f\n", ((j-4)*50)+50, losemoney[j], losemoneyrate[j]);
        fprintf(data2,"〜%d %d %f\n", ((j-4)*50)+50, maxmoney[j], maxmoneyrate[j]);
        fprintf(data3,"〜%d %d %f\n", ((j-4)*50)+50, gamecount[j], gamecountrate[j]);
    }

    /*５０１以上の時*/
    fprintf(data1,"%d〜 %d %f\n", 501, losemoney[14], losemoneyrate[14]);
    fprintf(data2,"%d〜 %d %f\n", 501, maxmoney[14], maxmoneyrate[14]);
    fprintf(data3,"%d〜 %d %f\n", 501, gamecount[14], gamecountrate[14]);
    /*ファイルクローズ（必ずする）*/
    fclose(data1);
    fclose(data2);
    fclose(data3);

    //グラフの作成
    if(flag3 == 1){

        data_file1 = "data1.txt";
        data_file2 = "data2.txt";
        data_file3 = "data3.txt";

        gp = popen("gnuplot -persist","w");
        gp2 = popen("gnuplot -persist", "w");
        gp3 = popen("gnuplot -persist", "w");

        fprintf(gp, "set style data histogram\n");
        fprintf(gp, "set style histogram cluster gap 1\n");
        fprintf(gp, "set xtics rotate by -90\n");
        fprintf(gp, "set y2tics\n");
        fprintf(gp, "set yrange [0:%d]\n", count-losecount);
        fprintf(gp, "set y2range [0:100]\n");
        fprintf(gp, "set xlabel \"最高負け額[$]\" \n");
        fprintf(gp, "set ylabel \"数列成功回数[回]\" \n");
        fprintf(gp, "set y2label \"累積比率[％]\" \n");

        fprintf(gp2, "set style data histogram\n");
        fprintf(gp2, "set style histogram cluster gap 1\n");
        fprintf(gp2, "set xtics rotate by -90\n");
        fprintf(gp2, "set y2tics\n");
        fprintf(gp2, "set yrange [0:%d]\n", count-losecount);
        fprintf(gp2, "set y2range [0:100]\n");
        fprintf(gp2, "set xlabel \"最高bet額[$]\" \n");
        fprintf(gp2, "set ylabel \"数列成功回数[回]\" \n");
        fprintf(gp2, "set y2label \"累積比率[％]\" \n");

        fprintf(gp3, "set style data histogram\n");
        fprintf(gp3, "set style histogram cluster gap 1\n");
        fprintf(gp3, "set xtics rotate by -90\n");
        fprintf(gp3, "set y2tics\n");
        fprintf(gp3, "set yrange [0:%d]\n", count-losecount);
        fprintf(gp3, "set y2range [0:100]\n");
        fprintf(gp3, "set xlabel \"かかったゲーム数[回]\" \n");
        fprintf(gp3, "set ylabel \"数列成功回数[回]\" \n");
        fprintf(gp3, "set y2label \"累積比率[％]\" \n");

        fprintf(gp, "plot \"%s\" using 2:xtic(1) axis x1y1 lw 2 title \"それぞれの金額の度数\"\n", data_file1);

        fprintf(gp, "replot \"%s\" using 3:xtic(1) axis x1y2 lw 2 title \"度数の累積比率\"\n",data_file1);


        fprintf(gp2, "plot \"%s\" using 2:xtic(1) axis x1y1 lw 2 title \"それぞれの金額の度数\"\n",data_file2);

        fprintf(gp2, "replot \"%s\" using 3:xtic(1) axis x1y2 lw 2 title \"度数の累積比率\"\n",data_file2);


        fprintf(gp3, "plot \"%s\" using 2:xtic(1) axis x1y1 lw 2 title \"それぞれの金額の度数\"\n",data_file3);

        fprintf(gp3, "replot \"%s\" using 3:xtic(1) axis x1y2 lw 2 title \"度数の累積比率\"\n",data_file3);


        pclose(gp);

        pclose(gp2);

        pclose(gp3);

    }
    return 0;
}

/* 
   1個の新しいノードを作成する関数 
   仮引数：データの値
   戻り値：作成したノードへのポインタ
 */
Sequence* create_node(int item)
{
    Sequence *new_node; /* 新しいノードへのポインタ */

    /* ノード1個分の領域を確保 */
    new_node = (Sequence *)calloc(1, sizeof(Sequence)); 

    new_node->data = item; /* データitemを格納 */
    new_node->next = NULL; /* 作成時点ではどこも指さない */

    return new_node;
}

/* 
   新しいノードをリストの末尾に追加する関数 (pro4 6thのrep5-3_2.cから引用) 
   仮引数：古いリストの末尾を指すポインタと追加するデータの値
   戻り値：新しいリストの末尾を指すポインタ
 */
Sequence* add_end_list2(Sequence *tail, int new_data)
{
    Sequence *new_node;
    new_node = create_node(new_data); /* 追加するノードを作成 */

    if(tail == NULL){
        tail = new_node; /* リストが空なら末尾ポインタにつなげる */
    }else{
        tail->next = new_node; /* 新しいノードを末尾につなげる */

        tail = tail->next;     /* tailが新しいノードを新しい末尾として繋げ直す。*/
    }

    return tail; /* 末尾を指すポインタを返す */
}

/*
   リストの要素をすべて表示する関数
   仮引数：リストの先頭を表すポインタ
   戻り値：なし
 */
void print_list(Sequence *list)
{
    while(list != NULL){
        printf("%d ", list->data);
        list = list->next; /* ポインタを次のノードへ移動 */
    }
    printf("\n");

    return;
}

/*
   掛け金を設定する関数
   仮引数：数列の先頭、末尾を指すポインタ、数列が数字１つか違うかを示すフラグ、今の持金を指すポインタ
   戻り値：決定したbet額
 */
int Bet(Sequence *list, Sequence *tail, int flag1)
{
    int bet;/*bet額*/

    if(flag1 == 1){/*数列が１つなら*/
        bet = tail->data;/*そのデータのみ賭ける*/
    }else if( (flag1 == 2) || (flag1 == 3) ){/*数列が複数なら*/
        bet = list->data + tail->data;/*数列の先頭と末尾の和を賭ける*/
    }

    return bet;
}

/*
   最高掛け金を更新する関数
   仮引数：今までの最高賭け金、今回のbet額
   戻り値：更新後の最高賭け金
 */
int Maxbet(int maxbet, int bet)
{
    if(bet > maxbet){/*今回の賭け金が今までよりも高ければ*/
        maxbet = bet;/*更新*/
    }

    return maxbet;
}

/*
   乱数生成器 メルセンヌ・ツイスタを用いて、入力された確率通りのゲームの勝敗の抽選を行う関数
   仮引数：入力したゲームの勝率
   戻り値：勝ったか負けたか
 */
int Gamble(double rate)
{
    int flag2;/*勝敗を示すフラグ*/

    if( sfmt_genrand_real2( &sfmt ) <= rate / 100 ){/*０から１の実数乱数を生成し、それが入力した確率より数値が低いかどうか*/

        //if( genrand_int32() % 1000 + 1 <= (int)(rate * 10)  ){
        //if(rand() % 1000 + 1 <= rate * 10){
        /*乱数割る1000の余り+1が入力した勝率✕10の値以下かどうか（入力した勝率で勝ったかどうか）*/
        flag2 = 1;/*勝ったら1を*/
    }else{
        flag2 = 0;/*負けたら0を*/
    }
    return flag2;/*1か0をmainに返す*/
    }

    /* 
       ゲームに勝った時に、数列の数字が１つだった場合は末尾のノードを、
       複数あったら、先頭と末尾のノードを削除する関数 
       仮引数：古いリストの先頭、末尾を指すポインタ、数列の数字が１つかどうかのフラグ
       戻り値：なし
     */
    void IfWin(Sequence **list, Sequence **tail, int flag1)
    {
        Sequence *remove_node1, *remove_node2, *current;/*削除するSequence型のノードへのポインタ、先頭から末尾をたどる際に使うノードへのポインタ*/

        if(flag1 == 1){/*数列の数字が１つの場合*/
            remove_node1 = *tail;/*末尾のみ削除の候補に*/
            *list = NULL;/*数列の要素がもうないハズなので、先頭、末尾を指すポインタをNULLに*/
            *tail = NULL;
            free(remove_node1);/*指定した領域を削除（解放）する。*/
        }else if(flag1 == 2){/*もし数列の要素が２つなら、*/
            remove_node1 = *list;
            remove_node2 = *tail;/*先頭、末尾を削除する候補に*/
            *list = NULL;/*数列の要素がもうないハズなので、先頭、末尾を指すポインタをNULLに*/
            *tail = NULL;
            free(remove_node1);
            free(remove_node2);/*指定した領域を削除（解放）する。*/
        }else if(flag1 == 3){/*もし数列の要素が２つより多いなら、*/
            remove_node1 = *list;
            remove_node2 = *tail;/*先頭と末尾を削除する候補に*/
            current = *list;              /* 先頭から辿る */
            while(current->next->next != NULL){/*current->nextだと、末尾の手前ではなく、末尾そのものを指してしまう。*/
                current = current->next;   /* ポインタを移動して末尾の手前を探す */
            }
            current->next = NULL;/* 削除後の末尾はNULLを保持 */
            *tail = current;/*末尾の手前を末尾に設定*/
            *list = (*list)->next;/*先頭を先頭の次のノードに設定 (*list)->next でなんでできたかわからん→かっこつけないと全体に*の影響がでるから*/
            free(remove_node1);
            free(remove_node2);           /* 指定したノードの削除（解放） */
        }
        //printf("勝ちました  ");/*ゲームに勝ったかどうか */
        //print_list(*list);/*現状の数列の表示*/

        return;
    }

    /*
       ゲームに負けた場合、数列の末尾に賭けていた金額を追加する関数
       仮引数：数列の末尾と、賭け金
       戻り値：なし
     */
    void IfLose(Sequence *list ,Sequence **tail, int bet)
    {
        *tail = add_end_list2(*tail, bet); /* 負けた場合はリストの末尾に賭けていた金額を数列の末尾に追加 */
        //printf("負けました  ");/*ゲームに勝ったかどうか */
        //print_list(list);/*現状の数列の表示*/

        return;
    }

    /*
       一番負けていた時の金額を更新する関数
       仮引数：予算、持金、一番負けていた時の金額
       戻り値：更新した一番負けていた場合の金額
     */
    int Maxlose(int budget, int money, int maxlose)
    {
        if(maxlose <  budget - money){
            maxlose = budget - money;
        }

        return maxlose;
    }

    /*
       数列が消えるか、破産するまでの勝率を計算する関数
       仮引数：ゲーム数を指すポインタ、勝った回数を指すポインタ、勝敗を表すフラグ
       戻り値：計算した勝率を返す。
     */    
    double Winrate(int *game, int *wincount, int flag2)
    {
        double winrate;/*計算する勝率*/

        if(flag2 == 1){/*ゲームに勝ったら*/
            (*game)++;/*ゲーム数を+1*/
            (*wincount)++;/*ゲームに勝った回数を+1*/
        }else if(flag2 == 0){/*ゲームに負けたら*/
            (*game)++;/*ゲーム数のみ+1*/
        }
        winrate = ( (double)*wincount / (double)*game ) * 100;/*勝率 = 勝った回数割るゲーム回数*/

        return winrate;
    }



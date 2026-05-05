#include <stdio.h>  //add status comitt push
#include <string.h> //
#include <stdlib.h>
#include <unistd.h>//即時入力＆待機時間用
#include <termios.h>//即時入力用
#include <time.h>//0で判定とペナルティ実装


#define BLACK 0
#define RED 1
#define KURO 30
#define AKA 31
#define SPACE 0
#define GYOU 1
#define KAIGYOU 2
//あとですべてmainの中に入れる
//trump_numに入れてしまうと後が面倒すぎるので違う変数作った方がよいかも
typedef struct{//すべて c.を付ける（メイン内）というかすべてでそうそろえる
    int trump_num[2][30];//処理的に箱は30個絶対にいる
    int trump_mark[2][30];
    int field_mark[2];
    int field_num[2];//出すところのカードの値
    int hand_card_mark_box[2][4];
    int hand_card_number_box[2][4];//これで中身を分かりやすくする
    int hand_card_total_number[2][4];//今手札に入っているカードが何番目か知るためのもの
    int card_sum_quantity[2];//カードの出した合計枚数を計るもの
    int total[2];
} card;

char getch(void);   
char *suto[5]={"\x1b[47;37m ","\x1b[47;30m♣","\x1b[47;30m♠","\x1b[47;31m♦","\x1b[47;31m♥",}; //マーク分け
int  player_color[2]={0};  //p1,player_color[1];//プレイヤー識別子(黒と赤の色分け)名前変えた方がいい
char player[10];
//int a=0,b=1,c.hand_card_total_number[2]=2,c.hand_card_total_number[3]=3,c.hand_card_total_number[4]=0,c.hand_card_total_number[5]=1,c.hand_card_total_number[2]=2,c.hand_card_total_number[3]=3;//カード識別子
//int c.field_mark[0],c.field_mark[1],c.field_num[0],c.field_num[1];//山札のカード
int aa=0,bb=0;
void show(int);//いらん
void field_card_display(card *c);
void card_display(card *c,int);
int hand_trump_card_change(card *c,int,int);//markとnumber
void hand_card_first_send(card *c);//最初の八枚を配る関数(box(略)関数に送る)
int field_number_judge(card *c);
void first_set_field(card *c);
void from_hand_to_field(card *c,int);
void key_judge(card *c,int,int);//キーの左からの数（例：２ならp1の二つ目なので２、９ならp2の三つ目なので3)(player,kazu)




//スピードは四枚並べるのでmarkは一つあれば使いまわせるわけではない
// なぜなら、赤、黒にはスートが二つずつありそれを判別できないため不可つまり八ついる
//なおmark numberを４×13にすればいけるがランダムが大変になるため不可いや、赤か黒か判別できればよいから５つか
//というか内部処理したら一つずつでよくね？
int main(void){
    card c = {0};//初期化
    for(int i = 0 ; i < 2 ; i++){
        for(int j = 0 ; j < 4 ; j++){
            c.hand_card_total_number[i][j] = j;
        }
    }
    for(int i=1;i<5;i++){//中身359.475あたりに使用
        for(int j=1;j<14;j++){
            if(aa<26){
                c.trump_num[BLACK][aa]=j;
                c.trump_mark[BLACK][aa++]=i;//0or1ならば黒側
            }else{
                c.trump_num[RED][bb]=j;
                c.trump_mark[RED][bb++]=i;
            }
        }
    }   
    srand((unsigned)time(NULL));
    for(int i=0;i<2;i++){//シャッフル
        for(int j=25;j>0;j--){
            int q,kari;
            q=rand()% (j+1);
            kari=c.trump_num[i][q];
            c.trump_num[i][q]=c.trump_num[i][j];
            c.trump_num[i][j]=kari;
            kari=c.trump_mark[i][q];
            c.trump_mark[i][q]=c.trump_mark[i][j];
            c.trump_mark[i][j]=kari;
        }
    }
    while(1){
        printf("プレイヤー1は赤or黒を入力してください");
        scanf("%s",player);
            if(strcmp(player,"赤")==0){
                player_color[0]=RED;
                player_color[1]=BLACK;
                break;
            }else if(strcmp(player,"黒")==0){
                player_color[0]=BLACK;
                player_color[1]=RED;
                break;
            }else{
                printf("正しく入力してください");
                continue;
            }
    }
    printf("プレイヤー1は上側で左のカードから1~4\n");
    printf("プレイヤー2は下側で左のカードから7~0でカードを出します\n");
    printf("カードを並べます\n");
    first_set_field(&c);
    hand_card_first_send(&c);
    sleep(1);
    card_display(&c,0);
    card_display(&c,1);
    printf("五秒後に開始します\n");
    for(int i=5;i>0;i--){
        printf("%d\n",i);
        sleep(1);
    }
    //これlとrいらんくね？(修正がめんどい)
    while(1){//このままだと空白が山札に出で0というカードとして認識してしまうのでそこを改善ifでわける
        //markの二つ目の配列がnumberでもmarkとnumberの数字は一致するよな仕様なので問題なし(なお分かりずらい)
        //markにするには逆に代入しなければならない
        card_display(&c,0);//p1
        field_card_display(&c);
        card_display(&c,1);//p2
        if(c.total[0]>28){
            printf("プレイヤー1の勝ち");
            return 0;
            }
        if(c.total[1]>28){
            printf("プレイヤー２の勝ち");
            return 0;
        }
        if(field_number_judge(&c)) {
            char k = getch();//p1==player_color[0]
            if(k=='1'){
                key_judge(&c,player_color[0],0);
            }
            if(k=='2'){
                key_judge(&c,player_color[0],1);
            }
            if(k=='3'){
                key_judge(&c,player_color[0],2);
            }
            if(k=='4'){
                key_judge(&c,player_color[0],3);
            }
            if(k=='7'){
                key_judge(&c,player_color[1],0);
            }
            if(k=='8'){
                key_judge(&c,player_color[1],1);
            }
            if(k=='9'){
                key_judge(&c,player_color[1],2);
            }
            if(k=='0'){
                key_judge(&c,player_color[1],3);
            }
            }else{ //出せないとき
                c.total[0] += 1;
                c.total[1] += 1;
                if(c.total[0]>25){//0~25は実態がある ここのifがないときに動いた
                    from_hand_to_field(&c,player_color[0]);
                }else if(c.total[1]>25){//0~25は実態がある
                            from_hand_to_field(&c,player_color[1]);
                }else{
                    c.field_mark[0]=c.trump_mark[0][c.total[0]];
                    c.field_mark[1]=c.trump_mark[1][c.total[1]];
                    c.field_num[0]=c.trump_num[0][c.total[0]];
                    c.field_num[1]=c.trump_num[1][c.total[1]];
                }
                printf("3秒後に山札のカードが出されます\n");//ここは全部上記の条件式を満たす場合通るようにする
                for(int i=3;i>0;i--){
                    printf("%d\n",i);
                    sleep(1);
                }

            }
        }
    }

void key_judge(card *c,int p, int n){
    if(c->hand_card_number_box[p][n] != 0){//ここにあらたな手札を加えるまでする
        for (int i = 0 ; i < 2 ; i ++){
            if( abs(c->field_num[i]-c->hand_card_number_box[p][n]) == 1 ){
                c->field_mark[i]=c->hand_card_mark_box[p][n];
                c->field_num[i]=c->hand_card_number_box[p][n];
                c->total[p] += 1;
                printf("アクセス: trump_num[%d][%d]\n", p, c->total[p]);
                c->hand_card_mark_box[p][n]=c->trump_mark[p][c->total[p]];
                c->hand_card_number_box[p][n]=c->trump_num[p][c->total[p]];
                return ;
            }
        }
    }else{
        //ペナルティ案
    }

}


void first_set_field(card *c){
    for(int i = 0 ; i < 2 ; i++){
        c->card_sum_quantity[i]=4;
        c->total[i]=4;
    }

}

void hand_card_first_send(card *c){
    for(int j = 0 ; j < 2 ; j++){
        for(int i = 0 ; i < 4 ; i++){
            c->hand_card_mark_box[j][i]=c->trump_mark[j][i];
            c->hand_card_number_box[j][i]=c->trump_num[j][i];
        }
    }
    for(int i = 0 ; i < 2 ; i++){
        c->field_mark[i]=c->trump_mark[i][4];
        c->field_num[i]=c->trump_num[i][4];
    }
}
//よく考えたら開いているところに入れるから関数８ついるね
void show(int n){
    switch(n){
        case 0:
        printf("\x1b[40;37m    ");
        break;
        case 1:
        printf("\x1b[47;37m |");
        break;
        case 2:
        printf("\x1b[40;37m \n");
        break;
        default:
        printf("このコードは何らかのバグorミスで表示されます\n何らかの問題が発生しました");
    }
}

int field_number_judge(card *c){
    int judge_insutition(int,int);
    for(int r=0; r<2; r++){//場のカード別
        for(int j=0; j<2; j++){//プレイヤー別
            for(int i=0; i<4; i++){//カード別
                if(judge_insutition(c->field_num[r],c->hand_card_number_box[j][i])){//iでよいような気もするが0or1のいずれかが入るため無理
                    return 1;
                }
            }
        }
    }
    return 0;
}

int judge_insutition(int a, int b){//for関数でこのa,bを動かせばよい
    if( abs(a - b) == 1 && b != 0){
        return 1 ;
    }else{
        return 0 ;
    }
}

void from_hand_to_field(card *c,int p){
    int random;
    while(1){
        random = rand() % 4;
        if(c->hand_card_number_box[p][random] != 0){
            printf("DEBUG: p=%d total=%d\n", p, c->total[p]);
            c->field_mark[p]=c->hand_card_mark_box[p][random];
            c->field_num[p]=c->hand_card_number_box[p][random];
            printf("アクセス: trump[%d][%d]\n", p, c->total[p]);
            c->hand_card_mark_box[p][random]=c->trump_mark[p][c->total[p]];
            c->hand_card_number_box[p][random]=c->trump_num[p][c->total[p]];
        break ;
        }
    }
}



void card_display(card *c, int p){//プレイヤーごとに分かれているためプレイヤー
    //を判定するための変数は必要ない
    int s;
    int co[4]={0};
    char *kara={"\x1b[47;37m "};
    char *change[4][13]={0};//文字列を入れるため
    int serach_mark[4]={0};
    int serach_number[4]={0};//四つの数字を覚えて代入する関数
    int kioku[4][13]={0};//カード表示用
    int color[2]={KURO,AKA};
    const char*t[14]={" ","A","2","3","4","5","6","7","8","9","X","J","Q","K"};
    int hyoji[13][13]={
                {0,0,0,0,0,0,1,0,0,0,0,0,0},
                {0,0,0,1,0,0,0,0,0,1,0,0,0},
                {0,0,0,1,0,0,1,0,0,1,0,0,0},
                {1,1,0,0,0,0,0,0,0,0,0,1,1},
                {1,1,0,0,0,0,1,0,0,0,0,1,1},
                {1,1,0,0,0,1,0,1,0,0,0,1,1},
                {1,1,0,0,0,1,1,1,0,0,0,1,1},
                {1,1,0,1,0,1,0,1,0,1,0,1,1},
                {0,0,1,1,1,1,1,1,1,1,1,0,0},
                {1,1,1,0,1,1,0,1,1,0,1,1,1},//デザイン問題
                {1,1,1,0,1,1,1,1,1,0,1,1,1},
                {1,1,1,1,1,1,0,1,1,1,1,1,1},
                {1,1,1,1,1,1,1,1,1,1,1,1,1},
            };//nullの場合すべてkiokuが0になるから
    for(int i=0; i < 4 ; i++){
        serach_mark[i]=c->hand_card_mark_box[p][i];//これは最初から使える
        serach_number[i]=c->hand_card_number_box[p][i];
    }

    for(int r=0; r<4; r++){
        for(int i=1;i<14;i++){
            if(serach_number[r]==i){//マークはプレイヤーで統一なのでmだがカードが四枚なのでこうなる
                for(int j=0;j<13;j++){
                    kioku[r][j]=hyoji[serach_number[r]-1][j];//rは何枚目のカードを記憶しているか、iは数字の番号jはマークの記憶用
                }
            }
        }
    }
    for(int j=0;j<4;j++){
        for(int i=0;i<13;i++){
            if(kioku[j][i] == 1){
                change[j][i]=suto[serach_mark[j]];
            }else{
                change[j][i]=kara;
            }
        }
    }
    for(int i=0;i<4;i++){//ここがmarkの中身見てる
        if(serach_mark[i]==1 || serach_mark[i]==2){
            co[i]=0;//黒
        }else if(serach_mark[i]==3 || serach_mark[i]==4){
            co[i]=1;//赤
        }
    }
    for(int i=0;i<4;i++){
        printf("\x1b[47;%dm ┌─────────────┐\x1b[0m",color[0]);
        printf("    ");
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm %s          \x1b[0m",color[co[i]],t[serach_number[i]]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm %s  %s   %s   \x1b[0m",color[co[i]],suto[serach_mark[i]],change[i][0],change[i][1]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s %s %s   \x1b[0m",color[co[i]],change[i][2],change[i][3],change[i][4]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s %s %s   \x1b[0m",color[co[i]],change[i][5],change[i][6],change[i][7]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s %s %s   \x1b[0m",color[co[i]],change[i][8],change[i][9],change[i][10]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s   %s  %s\x1b[0m",color[co[i]],change[i][11],change[i][12],suto[serach_mark[i]]);
        show(GYOU);
        show(SPACE);
    }   
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm           %s\x1b[0m",color[co[i]],t[serach_number[i]]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        printf("\x1b[47;%dm └─────────────┘\x1b[0m",color[0]);
        printf("    ");
    }
    printf("\n");
} 

void field_card_display(card *c){
    int s;
    int co[2]={0};
    char *kara={"\x1b[47;37m "};
    char *change[2][13]={0};
    int serach_mark[2]={0};
    int serach_number[2]={0};//四つの数字を覚えて代入する関数
    int kioku[2][13]={0};//カード表示用
    int color[2]={KURO,AKA};
    const char*t[14]={" ","A","2","3","4","5","6","7","8","9","X","J","Q","K"};
    int hyoji[13][13]={
                {0,0,0,0,0,0,1,0,0,0,0,0,0},
                {0,0,0,1,0,0,0,0,0,1,0,0,0},
                {0,0,0,1,0,0,1,0,0,1,0,0,0},
                {1,1,0,0,0,0,0,0,0,0,0,1,1},
                {1,1,0,0,0,0,1,0,0,0,0,1,1},
                {1,1,0,0,0,1,0,1,0,0,0,1,1},
                {1,1,0,0,0,1,1,1,0,0,0,1,1},
                {1,1,0,1,0,1,0,1,0,1,0,1,1},
                {0,0,1,1,1,1,1,1,1,1,1,0,0},
                {1,1,1,0,1,1,0,1,1,0,1,1,1},//デザイン問題
                {1,1,1,0,1,1,1,1,1,0,1,1,1},
                {1,1,1,1,1,1,0,1,1,1,1,1,1},
                {1,1,1,1,1,1,1,1,1,1,1,1,1},
            };//nullの場合すべてkiokuが0になるから
    for(int i = 0 ; i < 2 ; i++){
        serach_mark[i]=c->field_mark[i];//完了
        serach_number[i]=c->field_num[i];
    }

    for(int r=0; r<2; r++){
        for(int i=1;i<14;i++){
            if(serach_number[r]==i){//マークはプレイヤーで統一なのでmだがカードが四枚なのでこうなる
                for(int j=0;j<13;j++){
                    kioku[r][j]=hyoji[serach_number[r]-1][j];//rは何枚目のカードを記憶しているか、iは数字の番号jはマークの記憶用
                }
            }
        }
    }
    for(int j=0;j<2;j++){
        for(int i=0;i<13;i++){
            if(kioku[j][i] == 1){
                change[j][i]=suto[serach_mark[j]];
            }else{
                change[j][i]=kara;
            }
        }
    }
    for(int i=0;i<2;i++){
        if(serach_mark[i]==1 || serach_mark[i]==2){
            co[i]=0;//黒
        }else if(serach_mark[i]==3 || serach_mark[i]==4){
            co[i]=1;//赤
        }
    }
    printf("                    ");
    for(int i=0;i<2;i++){
        printf("\x1b[47;%dm ┌─────────────┐\x1b[0m",color[0]);
        printf("    ");
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm %s          \x1b[0m",color[co[i]],t[serach_number[i]]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm %s  %s   %s   \x1b[0m",color[co[i]],suto[serach_mark[i]],change[i][0],change[i][1]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s %s %s   \x1b[0m",color[co[i]],change[i][2],change[i][3],change[i][4]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s %s %s   \x1b[0m",color[co[i]],change[i][5],change[i][6],change[i][7]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s %s %s   \x1b[0m",color[co[i]],change[i][8],change[i][9],change[i][10]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm    %s   %s  %s\x1b[0m",color[co[i]],change[i][11],change[i][12],suto[serach_mark[i]]);
        show(GYOU);
        show(SPACE);
    }   
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm           %s\x1b[0m",color[co[i]],t[serach_number[i]]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        printf("\x1b[47;%dm └─────────────┘\x1b[0m",color[0]);
        printf("    ");
    }
    printf("\n");

}

char getch(void) {
    struct termios oldt, newt;
    char ch;

    tcgetattr(STDIN_FILENO, &oldt);      // 今の設定保存
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);    // 行入力＆エコーOFF
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    ch = getchar();                      // 1文字取得

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt); // 元に戻す
    return ch;
}

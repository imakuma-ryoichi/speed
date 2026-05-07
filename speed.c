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
#define PENALTY_TIME 3//s
#define DEFAULT_COOL_TIME 0.334//s
//あとですべてmainの中に入れる
//trump_numに入れてしまうと後が面倒すぎるので違う変数作った方がよいかも
typedef struct{//すべて c.を付ける（メイン内）呼び出し＆　作った関数内->
    int trump_num[2][30];//処理的に箱は30個絶対にいる
    int trump_mark[2][30];
    int field_mark[2];
    int field_num[2];//出すところのカードの値
    int hand_card_mark_box[2][4];
    int hand_card_number_box[2][4];//これで中身を分かりやすくする
    int hand_card_total_number[2][4];//今手札に入っているカードが何番目か知るためのもの
    int card_sum_quantity[2];//カードの出した合計枚数を計るもの
    int total[2];
    int player_color[2];
    int push_field_mark[2];
    int push_field_num[2]; 
    double penalty_sec[2];
    double cool_end[2];
} card;     //ミスしたかどうかを見てミスしたらクールタイムを伸ばすようにする

char getch(void);   

double now_time();
void judge(card *c);
void show(int);
void trump_def(card *c);
void player_def(card *c);
void field_card_display(card *c);
void card_display(card *c,int);
void hand_card_first_send(card *c);//最初の八枚を配る関数(box(略)関数に送る)
void first_set_field(card *c);
void from_hand_to_field(card *c,int);
void key_judge(card *c,int,int,int);//キーの左からの数（例：２ならp1の二つ目なので２、９ならp2の三つ目なので3)(player,kazu)
void push_key_judge(card *c,int);
void trump_shuffle(card *c);
void ready_sign();
int cool_time(card *c,char);//クールタイムの条件式(押したキー)
void win_judge(card *c);
int field_number_judge(card *c);
int hand_trump_card_change(card *c,int,int);//markとnumber
void field_equal_judge(card *c,int);//クールタイム中にfieldのカードが変わっていないかを見る

//スピードは四枚並べるのでmarkは一つあれば使いまわせるわけではない
int main(void){//最後にAとKをつなげる
    card c = {0};//初期化
    trump_def(&c);   
    trump_shuffle(&c);
    player_def(&c);//前のカード消すようにする
    first_set_field(&c);
    hand_card_first_send(&c);
    card_display(&c,0);
    card_display(&c,1);
    ready_sign();
    while(1){//このままだと空白が山札に出で0というカードとして認識してしまうのでそこを改善ifでわける
        system("clear");
        card_display(&c,0);//p1
        field_card_display(&c);
        card_display(&c,1);//p2
        win_judge(&c);
        judge(&c);
        }
    }


void field_equal_judge(card *c,int a){//これいらんくなった説ある
    for(int i = 0 ; i < 2 ; i++){
        if( c->push_field_mark[i] == c->field_mark[i] && 
            c->push_field_num[i] == c->field_num[i]){
            c->penalty_sec[a] = PENALTY_TIME ; //ペナルティを増やす        
            return ;
        }
    }
    return ;
}


int cool_time(card *c,char a){//judgeの中に入れる
    if( now_time() < c->cool_end[0] &&
        (a=='1'||a=='2'||a=='3'||a=='4')){
        return 1;
    }

    // player2 クールタイム
    if(now_time() < c->cool_end[1] &&
        (a=='7'||a=='8'||a=='9'||a=='0')){
        return 1;
    }
    return 0;
}

void trump_def(card *c){
    c->penalty_sec[0]= 0.1;
    c->penalty_sec[1]= 0.1;
    int aa=0,bb=0;
    for(int i=1;i<5;i++){//トランプカード設定
        for(int j=1;j<14;j++){
            if(aa<26){
                c->trump_num[BLACK][aa]=j;
                c->trump_mark[BLACK][aa++]=i;//0or1ならば黒側
            }else{
                c->trump_num[RED][bb]=j;
                c->trump_mark[RED][bb++]=i;
            }
        }
    }
}

void player_def(card *c){
    char player[10];
    while(1){
        printf("プレイヤー1は赤or黒を入力してください");
        scanf("%s",player);
            if(strcmp(player,"赤")==0){
                c->player_color[0]=RED;
                c->player_color[1]=BLACK;
                break;
            }else if(strcmp(player,"黒")==0){
                c->player_color[0]=BLACK;
                c->player_color[1]=RED;
                break;
            }else{
                printf("正しく入力してください");
                continue;
            }
    }
    printf("環境によってはうまく表示されないこともあります");
    printf("プレイヤー1は上側で左のカードから1~4\n");
    printf("プレイヤー2は下側で左のカードから7~0でカードを出します\n");
    printf("カードを並べます\n");
    printf("ペナルティ時間は%dsです\n",PENALTY_TIME);
}

void trump_shuffle(card *c){
    srand((unsigned)time(NULL));
    for(int i=0;i<2;i++){//シャッフル
        for(int j=25;j>0;j--){
            int q,kari;
            q=rand()% (j+1);
            kari=c->trump_num[i][q];
            c->trump_num[i][q]=c->trump_num[i][j];
            c->trump_num[i][j]=kari;
            kari=c->trump_mark[i][q];
            c->trump_mark[i][q]=c->trump_mark[i][j];
            c->trump_mark[i][j]=kari;
        }
    }
}

void ready_sign(){
    printf("五秒後に開始します\n");
    for(int i=5;i>0;i--){
        printf("===================%d===================\n",i);
        sleep(1);
    }
}

void win_judge(card *c){
    for(int i = 0; i < 2 ; i++){
            if(c->total[i]>28){
                printf("プレイヤー%dの勝ち",i+1);
                exit (0);
                }
        }
}

void judge(card *c){
    if(field_number_judge(c)) {
        char k = getch();//p1==c->player_color[0]
        push_key_judge(c,k);
    }else{ //出せないとき
        c->total[0] += 1;
        c->total[1] += 1;
        if(c->total[0]>25){//0~25は実態がある ここのifがないときに動いた
            from_hand_to_field(c,c->player_color[0]);
        }else if(c->total[1]>25){//0~25は実態がある
            from_hand_to_field(c,c->player_color[1]);
        }else{
            for(int i = 0 ; i < 2 ; i++){
                c->field_mark[i]=c->trump_mark[i][c->total[i]];
                c->field_num[i]=c->trump_num[i][c->total[i]];
                }
            }
            printf("3秒後に山札のカードが出されます\n");//ここは全部上記の条件式を満たす場合通るようにする
            for(int i=3;i>0;i--){
                printf("%d\n",i);
                sleep(1);
            }
        }
}

void key_judge(card *c,int p, int n, int player_num){
    if(c->hand_card_number_box[p][n] != 0){//ここにあらたな手札を加えるまでする
        for (int i = 0 ; i < 2 ; i ++){
            if( abs(c->field_num[i]-c->hand_card_number_box[p][n]) == 1 ){
                c->field_mark[i]=c->hand_card_mark_box[p][n];
                c->field_num[i]=c->hand_card_number_box[p][n];
                c->total[p] += 1;
                c->hand_card_mark_box[p][n]=c->trump_mark[p][c->total[p]];
                c->hand_card_number_box[p][n]=c->trump_num[p][c->total[p]];
                c->penalty_sec[player_num]=DEFAULT_COOL_TIME;
                return ;
            }
        }
        field_equal_judge(c,player_num);
        c->cool_end[player_num] =now_time() + c->penalty_sec[player_num];
    }else{
        field_equal_judge(c,player_num);
        c->cool_end[player_num] =now_time() + c->penalty_sec[player_num];
    }

}


void first_set_field(card *c){
    for(int i = 0 ; i < 2 ; i++){
        c->card_sum_quantity[i]=4;
        c->total[i]=4;
    }

}

void hand_card_first_send(card *c){
    for(int i = 0 ; i < 2 ; i++){
        for(int j = 0 ; j < 4 ; j++){
            c->hand_card_total_number[i][j] = j;
        }
    }
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
            c->field_mark[p]=c->hand_card_mark_box[p][random];
            c->field_num[p]=c->hand_card_number_box[p][random];
            c->hand_card_mark_box[p][random]=c->trump_mark[p][c->total[p]];
            c->hand_card_number_box[p][random]=c->trump_num[p][c->total[p]];
        break ;
        }
    }
}

void push_key_judge(card *c,int a){//もうここで&cで呼び出しているので次はアンパサンドはいらない
    if(cool_time(c,a)){
        return ;
    }
    switch(a){
        case '1':
        key_judge(c,c->player_color[0],0,0);//三つめはペナルティ案
        break;
        case '2':
        key_judge(c,c->player_color[0],1,0);
        break;
        case '3':
        key_judge(c,c->player_color[0],2,0);
        break;
        case '4':
        key_judge(c,c->player_color[0],3,0);
        break;
        case '7':
        key_judge(c,c->player_color[1],0,1);
        break;
        case '8':
        key_judge(c,c->player_color[1],1,1);
        break;
        case '9':
        key_judge(c,c->player_color[1],2,1);
        break;
        case '0':
        key_judge(c,c->player_color[1],3,1);
        break;
        default:
        //ここにしたらめんどいことになる（プレイヤー判別不能）
        break;
    }
}


void card_display(card *c, int p){//プレイヤーごとに分かれているためプレイヤー
    //を判定するための変数は必要ない
    int s;
    char *suto[5]={"\x1b[47;37m ","\x1b[47;30m♣","\x1b[47;30m♠","\x1b[47;31m♦","\x1b[47;31m♥",}; //マーク分け
    int co[4]={0};
    char *kara={"\x1b[47;37m "};
    char *change[4][13]={0};//文字列を入れるため
    int serach_mark[4]={0};
    int serach_number[4]={0};//四つの数字を覚えて代入する関数
    int kioku[4][13]={0};//カード表示用
    int color[2]={KURO,AKA};
    const char*num_up[14]={"  ","A ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ","10","J ","Q ","K "};
    const char*num_down[14]={"  "," A"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9","10"," J"," Q"," K"};
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
        printf("\x1b[47;%dm %s         \x1b[0m",color[co[i]],num_up[serach_number[i]]);
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
        printf("\x1b[47;%dm          %s\x1b[0m",color[co[i]],num_down[serach_number[i]]);
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
    char *suto[5]={"\x1b[47;37m ","\x1b[47;30m♣","\x1b[47;30m♠","\x1b[47;31m♦","\x1b[47;31m♥",}; //マーク分け
    int s;
    int co[2]={0};
    char *kara={"\x1b[47;37m "};
    char *change[2][13]={0};
    int serach_mark[2]={0};
    int serach_number[2]={0};//四つの数字を覚えて代入する関数
    int kioku[2][13]={0};//カード表示用
    int color[2]={KURO,AKA};
    const char*num_up[14]={"  ","A ","2 ","3 ","4 ","5 ","6 ","7 ","8 ","9 ","10","J ","Q ","K "};
    const char*num_down[14]={"  "," A"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9","10"," J"," Q"," K"};
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
        printf("\x1b[47;%dm %s         \x1b[0m",color[co[i]],num_up[serach_number[i]]);
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
        printf("\x1b[47;%dm          %s\x1b[0m",color[co[i]],num_down[serach_number[i]]);
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

double now_time(){
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec + ts.tv_nsec / 1000000000.0;
}
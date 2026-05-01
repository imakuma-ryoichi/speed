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

char getch(void);   
char *suto[5]={"\x1b[47;37m ","\x1b[47;30m♣","\x1b[47;30m♠","\x1b[47;31m♦","\x1b[47;31m♥",}; //マーク分け
int number[2][30]={0};//処理的に箱は
int mark[2][30]={0};
int nyuryoku[6]={1,4,7,0,1,4};
int  p1,p2;//プレイヤー識別子
char player[10];
int a=0,b=1,c=2,d=3,e=0,f=1,g=2,h=3;//カード識別子
int total1,total2;//カードの出した合計枚数を計るもの
int p1,p2,bordmark1,bordmark2,bordnumber1,bordnumber2;//山札のカード
int aa=0,bb=0;
void marker(int);
void show(int);
void yamahuda(int,int,int,int);
//スピードは四枚並べるのでmarkは一つあれば使いまわせるわけではない
// なぜなら、赤、黒にはスートが二つずつありそれを判別できないため不可つまり八ついる
//なおmark numberを４×13にすればいけるがランダムが大変になるため不可いや、赤か黒か判別できればよいから５つか
//というか内部処理したら一つずつでよくね？
int main(void){
    void card(int,int,int,int,int,int,int,int);
    for(int i=1;i<5;i++){//中身359.475あたりに使用
        for(int j=1;j<14;j++){
            if(aa<26){
                number[BLACK][aa]=j;
                mark[BLACK][aa++]=i;//0or1ならば黒側
            }else{
                number[RED][bb]=j;
                mark[RED][bb++]=i;
            }
        }
    }   
    srand((unsigned)time(NULL));
    for(int i=0;i<2;i++){//シャッフル
        for(int j=25;j>0;j--){
            int q,kari;
            q=rand()% (j+1);
            kari=number[i][q];
            number[i][q]=number[i][j];
            number[i][j]=kari;
            kari=mark[i][q];
            mark[i][q]=mark[i][j];
            mark[i][j]=kari;
        }
    }
    while(1){
        printf("プレイヤー1は赤or黒を入力してください");
        scanf("%s",player);
            if(strcmp(player,"赤")==0){
                p1=RED;
                p2=BLACK;
                break;
            }else if(strcmp(player,"黒")==0){
                p1=BLACK;
                p2=RED;
                break;
            }else{
                printf("正しく入力してください");
                continue;
            }
    }
    printf("プレイヤー1は上側で左のカードから1~4\n");
    printf("プレイヤー2は下側で左のカードから7~0でカードを出します\n");
    printf("カードを並べます\n");
    sleep(1);
    card(mark[p1][a],number[p1][a],mark[p1][b],number[p1][b],mark[p1][c],number[p1][c],mark[p1][d],number[p1][d]);
    card(mark[p2][e],number[p2][e],mark[p2][f],number[p2][f],mark[p2][g],number[p2][g],mark[p2][h],number[p2][h]);
    printf("五秒後に開始します\n");
    for(int i=5;i>0;i--){
        printf("%d\n",i);
        sleep(1);
    }
    p1=p1;
    p2=p2;//これlとrいらんくね？(修正がめんどい)
    bordmark1=4;
    bordmark2=4;
    bordnumber1=4;
    bordnumber2=4;
    total1=4,total2=4;
    while(1){//このままだと空白が山札に出で0というカードとして認識してしまうのでそこを改善ifでわける
        card(mark[p1][a],number[p1][a],mark[p1][b],number[p1][b],mark[p1][c],number[p1][c],mark[p1][d],number[p1][d]);
        yamahuda(mark[p1][bordmark1],number[p1][bordnumber1],mark[p2][bordmark2],number[p2][bordnumber2]);
        card(mark[p2][e],number[p2][e],mark[p2][f],number[p2][f],mark[p2][g],number[p2][g],mark[p2][h],number[p2][h]);
        if(total1>28){
            printf("プレイヤー1の勝ち");
            return 0;
            }
        if(total2>28){
            printf("プレイヤー２の勝ち");
            return 0;
        }
        if( (number[p1][bordnumber1]-1!=number[p1][a]|| number[p1][a]==0) &&
            (number[p1][bordnumber1]-1!=number[p1][b]|| number[p1][b]==0) &&
            (number[p1][bordnumber1]-1!=number[p1][c]|| number[p1][c]==0) &&
            (number[p1][bordnumber1]-1!=number[p1][d]|| number[p1][d]==0) &&
            (number[p1][bordnumber1]+1!=number[p1][a]|| number[p1][a]==0) &&
            (number[p1][bordnumber1]+1!=number[p1][b]|| number[p1][b]==0) &&
            (number[p1][bordnumber1]+1!=number[p1][c]|| number[p1][c]==0) &&
            (number[p1][bordnumber1]+1!=number[p1][d]|| number[p1][d]==0) &&
            (number[p1][bordnumber1]-1!=number[p2][e]|| number[p2][e]==0) &&
            (number[p1][bordnumber1]-1!=number[p2][f]|| number[p2][f]==0) &&
            (number[p1][bordnumber1]-1!=number[p2][g]|| number[p2][g]==0) &&
            (number[p1][bordnumber1]-1!=number[p2][h]|| number[p2][h]==0) &&
            (number[p1][bordnumber1]+1!=number[p2][e]|| number[p2][e]==0) &&
            (number[p1][bordnumber1]+1!=number[p2][f]|| number[p2][f]==0) &&
            (number[p1][bordnumber1]+1!=number[p2][g]|| number[p2][g]==0) &&
            (number[p1][bordnumber1]+1!=number[p2][h]|| number[p2][h]==0) &&
            (number[p2][bordnumber2]-1!=number[p1][a]|| number[p1][a]==0) &&
            (number[p2][bordnumber2]-1!=number[p1][b]|| number[p1][b]==0) &&
            (number[p2][bordnumber2]-1!=number[p1][c]|| number[p1][c]==0) &&
            (number[p2][bordnumber2]-1!=number[p1][d]|| number[p1][d]==0) &&
            (number[p2][bordnumber2]+1!=number[p1][a]|| number[p1][a]==0) &&
            (number[p2][bordnumber2]+1!=number[p1][b]|| number[p1][b]==0) &&
            (number[p2][bordnumber2]+1!=number[p1][c]|| number[p1][c]==0) &&
            (number[p2][bordnumber2]+1!=number[p1][d]|| number[p1][d]==0) &&
            (number[p2][bordnumber2]-1!=number[p2][e]|| number[p2][e]==0) &&
            (number[p2][bordnumber2]-1!=number[p2][f]|| number[p2][f]==0) &&
            (number[p2][bordnumber2]-1!=number[p2][g]|| number[p2][g]==0) &&
            (number[p2][bordnumber2]-1!=number[p2][h]|| number[p2][h]==0) &&
            (number[p2][bordnumber2]+1!=number[p2][e]|| number[p2][e]==0) &&
            (number[p2][bordnumber2]+1!=number[p2][f]|| number[p2][f]==0) &&
            (number[p2][bordnumber2]+1!=number[p2][g]|| number[p2][g]==0) &&
            (number[p2][bordnumber2]+1!=number[p2][h]|| number[p2][h]==0)) { 
                total1+=1;
                total2+=1;
                if(total1>25){//0~25は実態がある ここのifがないときに動いた
                    int random1;
                    while(1){//p1の場合
                        random1=rand()% 4;
                        if(random1==0 && a<26){
                            mark[p1][bordmark1]=mark[p1][a];
                            number[p1][bordnumber1]=number[p1][a];
                            mark[p1][a]=mark[p1][total1];
                            number[p1][a]=number[p1][total1];
                            break;
                        }else if(random1==1 && b<26){
                            mark[p1][bordmark1]=mark[p1][b];
                            number[p1][bordnumber1]=number[p1][b];
                            mark[p1][b]=mark[p1][total1];
                            number[p1][b]=number[p1][total1];
                            break;
                        }else if(random1==2 && c<26){
                            mark[p1][bordmark1]=mark[p1][c];
                            number[p1][bordnumber1]=number[p1][c];
                            mark[p1][c]=mark[p1][total1];
                            number[p1][c]=number[p1][total1];
                            break;
                        }else if(random1==3 && d<26){
                            mark[p1][bordmark1]=mark[p1][d];
                            number[p1][bordnumber1]=number[p1][d];
                            mark[p1][d]=mark[p1][total1];
                            number[p1][d]=number[p1][total1];
                            break;
                        }  
                    }
                }else if(total2>25){//0~25は実態がある
                        int random2;
                        while(1){//p2の場合 lをrにする
                            random2=rand()% 4;
                            if(random2==0 && e<26){
                                mark[p2][bordmark2]=mark[p2][e];
                                number[p2][bordnumber2]=number[p2][e];
                                mark[p2][e]=mark[p2][total2];
                                number[p2][e]=number[p2][total2];
                                break;
                            }else if(random2==1 && f<26){
                                mark[p2][bordmark2]=mark[p2][f];
                                number[p2][bordnumber2]=number[p2][f];
                                mark[p2][f]=mark[p2][total2];
                                number[p2][f]=number[p2][total2];
                                break;
                            }else if(random2==2 && g<26){
                                mark[p2][bordmark2]=mark[p2][g];
                                number[p2][bordnumber2]=number[p2][g];
                                mark[p2][g]=mark[p2][total2];
                                number[p2][g]=number[p2][total2];
                                break;
                            }else if(random2==3 && h<26){
                                mark[p2][bordmark2]=mark[p2][h];
                                number[p2][bordnumber2]=number[p2][h];
                                mark[p2][h]=mark[p2][total2];
                                number[p2][h]=number[p2][total2];
                                break;
                            }                 
                        }
                }else{
                    bordmark1=total1;
                    bordmark2=total2;
                    bordnumber1=total1;
                    bordnumber2=total2;
                }
                printf("3秒後に山札のカードが出されます\n");//ここは全部上記の条件式を満たす場合通るようにする
                for(int i=3;i>0;i--){
                    printf("%d\n",i);
                    sleep(1);
                }

            }else{
                char k = getch();
            if(k=='1'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p1][a] ||
                        number[p1][bordnumber1]+1==number[p1][a])  {
                        mark[p1][bordmark1]=mark[p1][a];
                        number[p1][bordmark1]=number[p1][a];
                        total1+=1;
                        a=total1;        
                    }else if(   number[p2][bordnumber2]-1==number[p1][a] ||
                                number[p2][bordnumber2]+1==number[p1][a])  {
                                mark[p2][bordmark2]=mark[p1][a];
                                number[p2][bordmark2]=number[p1][a];
                                total1+=1;
                                a=total1;
                }else{
                    //ペナルティ案
                }
            }else{
                //ペナルティ案
            }
            }
            if(k=='2'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p1][b] ||
                        number[p1][bordnumber1]+1==number[p1][b] ) {
                        mark[p1][bordmark1]=mark[p1][b];
                        number[p1][bordmark1]=number[p1][b];
                        total1+=1;
                        b=total1;        
                    }else if(   number[p2][bordnumber2]-1==number[p1][b] ||
                                number[p2][bordnumber2]+1==number[p1][b] ){
                                mark[p2][bordmark2]=mark[p1][b];
                                number[p2][bordmark2]=number[p1][b];
                                total1+=1;
                                b=total1;
                    }else{
                    //ペナルティ案
                    }
                }else{
                //ペナルティ案
            }
            }
            if(k=='3'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p1][c] ||
                        number[p1][bordnumber1]+1==number[p1][c] ){
                        mark[p1][bordmark1]=mark[p1][c];
                        number[p1][bordmark1]=number[p1][c];
                        total1+=1;
                        c=total1;        
                    }else if(   number[p2][bordnumber2]-1==number[p1][c] ||
                                number[p2][bordnumber2]+1==number[p1][c] ){
                                mark[p2][bordmark2]=mark[p1][c];
                                number[p2][bordmark2]=number[p1][c];
                                total1+=1;
                                c=total1;
                    }else{
                    //ペナルティ案
                    }
                }else{
                //ペナルティ案
            }
            }
            if(k=='4'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p1][d] ||
                        number[p1][bordnumber1]+1==number[p1][d] ){
                        mark[p1][bordmark1]=mark[p1][d];
                        number[p1][bordmark1]=number[p1][d];
                        total1+=1;
                        d=total1;        
                    }else if(   number[p2][bordnumber2]-1==number[p1][d] ||
                            number[p2][bordnumber2]+1==number[p1][d] ){
                                mark[p2][bordmark2]=mark[p1][d];
                                number[p2][bordmark2]=number[p1][d];
                                total1+=1;
                                d=total1;
                    }else{
                    //ペナルティ案
                    }
                }else{
                //ペナルティ案
                }
            }
            if(k=='7'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p2][e] ||
                        number[p1][bordnumber1]+1==number[p2][e] ){
                        mark[p1][bordmark1]=mark[p2][e];
                        number[p1][bordmark1]=number[p2][e];
                        total2+=1;
                        e=total2;        
                    }else if(   number[p2][bordnumber2]-1==number[p2][e] ||
                                number[p2][bordnumber2]+1==number[p2][e] ){
                                mark[p2][bordmark2]=mark[p2][e];
                                number[p2][bordmark2]=number[p2][e];
                                total2+=1;
                                e=total2;
                }else{
                    //ペナルティ案
                }
                }else{
                //ペナルティ案
            }
            }
            if(k=='8'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p2][f] ||
                        number[p1][bordnumber1]+1==number[p2][f] ){
                        mark[p1][bordmark1]=mark[p2][f];
                        number[p1][bordmark1]=number[p2][f];
                        total2+=1;
                        f=total2;        
                    }else if(   number[p2][bordnumber2]-1==number[p2][f] ||
                                number[p2][bordnumber2]+1==number[p2][f] ){
                                mark[p2][bordmark2]=mark[p2][f];
                                number[p2][bordmark2]=number[p2][f];
                                total2+=1;
                                f=total2;
                    }else{
                    //ペナルティ案
                    }
                }else{
                //ペナルティ案
            }
            }
            if(k=='9'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p2][g] ||
                        number[p1][bordnumber1]+1==number[p2][g] ){
                        mark[p1][bordmark1]=mark[p2][g];
                        number[p1][bordmark1]=number[p2][g];
                        total2+=1;
                        g=total2;        
                    }else if(   number[p2][bordnumber2]-1==number[p2][g] ||
                                number[p2][bordnumber2]+1==number[p2][g] ){
                                mark[p2][bordmark2]=mark[p2][g];
                                number[p2][bordmark2]=number[p2][g];
                                total2+=1;
                                g=total2;
                    }else{
                    //ペナルティ案
                    }
                    }else{
                //ペナルティ案
            }
            }
            if(k=='0'){
                if(number[p1][a]!=0){
                    if( number[p1][bordnumber1]-1==number[p2][h] ||
                        number[p1][bordnumber1]+1==number[p2][h] ){
                        mark[p1][bordmark1]=mark[p2][h];
                        number[p1][bordmark1]=number[p2][h];
                        total2+=1;
                        h=total2;        
                    }else if(   number[p2][bordnumber2]-1==number[p2][h] ||
                            number[p2][bordnumber2]+1==number[p2][h] ){
                                mark[p2][bordmark2]=mark[p2][h];
                                number[p2][bordmark2]=number[p2][h];
                                total2+=1;
                                h=total2;
                    }else{
                    //ペナルティ案
                    }
                }else{
                //ペナルティ案
            }
            }
            }
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


void card(int m1,int n1,int m2,int n2,int m3,int n3,int m4,int n4){//プレイヤーごとに分かれているためプレイヤー
    //を判定するための変数は必要ない
    int s;
    int co[4]={0};
    char *kara={"\x1b[47;37m "};
    char *change[4][13]={0};//文字列を入れるため
    int sirabemark[4]={0};
    int sirabenumber[4]={0};//四つの数字を覚えて代入する関数
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
    sirabemark[0]=m1;
    sirabemark[1]=m2;
    sirabemark[2]=m3;
    sirabemark[3]=m4;
    sirabenumber[0]=n1;
    sirabenumber[1]=n2;
    sirabenumber[2]=n3;
    sirabenumber[3]=n4;

    for(int p2=0;p2<4;p2++){
        for(int i=1;i<14;i++){
            if(sirabenumber[p2]==i){//マークはプレイヤーで統一なのでmだがカードが四枚なのでこうなる
                for(int j=0;j<13;j++){
                    kioku[p2][j]=hyoji[sirabenumber[p2]-1][j];//rは何枚目のカードを記憶しているか、iは数字の番号jはマークの記憶用
                }
            }
        }
    }
    for(int j=0;j<4;j++){
        for(int i=0;i<13;i++){
            if(kioku[j][i] == 1){
                change[j][i]=suto[sirabemark[j]];
            }else{
                change[j][i]=kara;
            }
        }
    }
    for(int i=0;i<4;i++){//ここがmarkの中身見てる
        if(sirabemark[i]==1 || sirabemark[i]==2){
            co[i]=0;//黒
        }else if(sirabemark[i]==3 || sirabemark[i]==4){
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
        printf("\x1b[47;%dm %s          \x1b[0m",color[co[i]],t[sirabenumber[i]]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm %s  %s   %s   \x1b[0m",color[co[i]],suto[sirabemark[i]],change[i][0],change[i][1]);
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
        printf("\x1b[47;%dm    %s   %s  %s\x1b[0m",color[co[i]],change[i][11],change[i][12],suto[sirabemark[i]]);
        show(GYOU);
        show(SPACE);
    }   
    show(KAIGYOU);
    for(int i=0;i<4;i++){
        show(GYOU);
        printf("\x1b[47;%dm           %s\x1b[0m",color[co[i]],t[sirabenumber[i]]);
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

void yamahuda(int m1,int n1,int m2,int n2){
            int s;
    int co[2]={0};
    char *kara={"\x1b[47;37m "};
    char *change[2][13]={0};
    int sirabemark[2]={0};
    int sirabenumber[2]={0};//四つの数字を覚えて代入する関数
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
    sirabemark[0]=m1;
    sirabemark[1]=m2;
    sirabenumber[0]=n1;
    sirabenumber[1]=n2;

    for(int p2=0;p2<2;p2++){
        for(int i=1;i<14;i++){
            if(sirabenumber[p2]==i){//マークはプレイヤーで統一なのでmだがカードが四枚なのでこうなる
                for(int j=0;j<13;j++){
                    kioku[p2][j]=hyoji[sirabenumber[p2]-1][j];//rは何枚目のカードを記憶しているか、iは数字の番号jはマークの記憶用
                }
            }
        }
    }
    for(int j=0;j<2;j++){
        for(int i=0;i<13;i++){
            if(kioku[j][i] == 1){
                change[j][i]=suto[sirabemark[j]];
            }else{
                change[j][i]=kara;
            }
        }
    }
    for(int i=0;i<2;i++){
        if(sirabemark[i]==1 || sirabemark[i]==2){
            co[i]=0;//黒
        }else if(sirabemark[i]==3 || sirabemark[i]==4){
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
        printf("\x1b[47;%dm %s          \x1b[0m",color[co[i]],t[sirabenumber[i]]);
        show(GYOU);
        show(SPACE);
    }
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm %s  %s   %s   \x1b[0m",color[co[i]],suto[sirabemark[i]],change[i][0],change[i][1]);
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
        printf("\x1b[47;%dm    %s   %s  %s\x1b[0m",color[co[i]],change[i][11],change[i][12],suto[sirabemark[i]]);
        show(GYOU);
        show(SPACE);
    }   
    show(KAIGYOU);
    printf("                    ");
    for(int i=0;i<2;i++){
        show(GYOU);
        printf("\x1b[47;%dm           %s\x1b[0m",color[co[i]],t[sirabenumber[i]]);
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

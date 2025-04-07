#include "aitest.h"
#include <iostream>
#include <QDebug>


int my_chess,enemy_chess;


//算法2定义的变量
int thisismapx,thisismapy;
int preaction_map_ai[21][21];
int maxdepth = 7;//搜索深度
int ai_player = 0;//暂时默认AI执白

void AI::init(int chess_map_detect_yor){//初始化

    my_chess=chess_map_detect_yor;
    enemy_chess=(my_chess+1)%2;

    for (int i=0;i<21;i++) {
        for (int j=0;j<21;j++) {
            map_ai[i][j]=-1;//棋子棋盘
            my_map_score[i][j]=0;//我方评分棋盘
            enemy_map_score[i][j]=0;//对方评分棋盘
            for (int k=0;k<4;k++){
            //map_certain_win[i][j][k]=0;//必胜判断棋盘（双活三、双活四、冲四活三、五子、单活四）
            //map_certain_lose[i][j][k]=0;//必输判断棋盘（双活三、双活四、冲四活三、五子、单活四）
            }
        }
    }
    //算法2初始化
    ai_player=chess_map_detect_yor;
}

void AI::aimap_valuecount(){//对棋盘进行赋分
    int enemy_chess_count=0,my_chess_count=0,no_chess_count=0,map_check_x=0,map_check_y=0;


    for (int map_detect_x=1;map_detect_x<=map_y_temp;map_detect_x++){
        for (int map_detect_y=1;map_detect_y<=map_x_temp;map_detect_y++){
            if (map_ai[map_detect_x][map_detect_y]==-1){
                for (int y=-1;y<=1;y++){
                    for (int x=-1;x<=1;x++){
                        enemy_chess_count=0;
                        my_chess_count=0;
                        no_chess_count=0;

                        if (x==0&&y==0) continue;
                        else{
                            //依据my_chess的颜色的棋子进行赋分
                            for (int i=1;i<=4;i++){
                                map_check_x=map_detect_x+i*y;
                                map_check_y=map_detect_y+i*x;
                                if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==my_chess){
                                    enemy_chess_count++;
                                }
                                else if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==-1){
                                    no_chess_count++;
                                    break;
                                }
                                else    break;
                            }

                            for (int i=1;i<=4;i++){
                                map_check_x=map_detect_x-i*y;
                                map_check_y=map_detect_y-i*x;
                                if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==my_chess){
                                    enemy_chess_count++;
                                }
                                else if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==-1){
                                    no_chess_count++;
                                    break;
                                }
                                else    break;
                            }

                            if (enemy_chess_count==1){//活二
                                my_map_score[map_detect_x][map_detect_y]+=10;
                            }
                            else if (enemy_chess_count==2){
                                if (no_chess_count==1){//死三
                                    my_map_score[map_detect_x][map_detect_y]+=30;
                                }
                                else if (no_chess_count==2){//活三
                                    my_map_score[map_detect_x][map_detect_y]+=40;
                                    //map_certain_win[map_detect_x][map_detect_y][0]+=1;
                                }
                            }
                            else if (enemy_chess_count == 3){
                                if (no_chess_count==1){//冲四
                                    my_map_score[map_detect_x][map_detect_y]+=50;
                                    //map_certain_win[map_detect_x][map_detect_y][1]+=1;
                                }
                                else if (no_chess_count==2){//活四
                                    my_map_score[map_detect_x][map_detect_y]+=250000;
                                    //map_certain_win[map_detect_x][map_detect_y][2]+=1;
                                }
                            }
                            else if (enemy_chess_count==4){//五子胜
                                my_map_score[map_detect_x][map_detect_y]+=600000;
                                //map_certain_win[map_detect_x][map_detect_y][3]+=1;
                            }


                            no_chess_count = 0;

                            //依据enemy_chess的颜色的棋子进行赋分
                            for (int i=1;i<=4;i++){
                                map_check_x=map_detect_x+i*y;
                                map_check_y=map_detect_y+i*x;
                                if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==enemy_chess){
                                    my_chess_count++;
                                }
                                else if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==-1){
                                    no_chess_count++;
                                    break;
                                }
                                else    break;
                            }

                            for (int i=1;i<=4;i++){
                                map_check_x=map_detect_x-i*y;
                                map_check_y=map_detect_y-i*x;
                                if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==enemy_chess){
                                    my_chess_count++;
                                }
                                else if (map_check_x>=1 && map_check_x<=map_y_temp && map_check_y>=1 && map_check_y<=map_x_temp && map_ai[map_check_x][map_check_y]==-1){
                                    no_chess_count++;
                                    break;
                                }
                                else    break;
                            }

                            if (my_chess_count==0)  enemy_map_score[map_detect_x][map_detect_y]+=5;
                            else if (my_chess_count==1){//活二
                                enemy_map_score[map_detect_x][map_detect_y]+=10;
                            }
                            else if (my_chess_count==2){
                                if (no_chess_count==1){//死三
                                    enemy_map_score[map_detect_x][map_detect_y]+=25;
                                }
                                else if (no_chess_count==2){//活三
                                    enemy_map_score[map_detect_x][map_detect_y]+=50;
                                    //map_certain_lose[map_detect_x][map_detect_y][0]+=1;
                                }
                            }
                            else if (my_chess_count==3){
                                if (no_chess_count==1){//冲四
                                    enemy_map_score[map_detect_x][map_detect_y]+=55;
                                    //map_certain_lose[map_detect_x][map_detect_y][1]+=1;
                                }
                                else if (no_chess_count==2){//活四
                                    enemy_map_score[map_detect_x][map_detect_y]+=200000;
                                    //map_certain_lose[map_detect_x][map_detect_y][2]+=1;
                                }
                            }
                            else if (my_chess_count>=4){//五子
                                enemy_map_score[map_detect_x][map_detect_y]+=500000;
                                //map_certain_lose[map_detect_x][map_detect_y][3]+=1;
                            }
                        }
                    }
                }
            }
        }
    }
}




int AI::method_fuzhi(){

    //计算棋盘评分
    aimap_valuecount();

    //测试评分用输出
    /*
    for (int i=1;i<map_y_temp;i++){
        for (int j=1;j<map_x_temp;j++){
            std::cout<<map_score[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    */

    //从评分中找出最大分数的位置
    int maxScore=0,maxScore_count=0;
    int des_max[400][2]={0};


    for (int map_detect_x=1;map_detect_x<=map_y_temp;map_detect_x++){
        for (int map_detect_y=1;map_detect_y<=map_x_temp;map_detect_y++){
            if (map_ai[map_detect_x][map_detect_y]==-1 && mustnot_chess[map_detect_x][map_detect_y]==0){//检测此处是否没有棋子+落子后是否会触发禁手

                //必胜、必输序列检测（旧版）
                /*
                if (map_certain_win[map_detect_x][map_detect_y][3]>=1){//我活五
                    my_map_score[map_detect_x][map_detect_y]+=900000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][3]>=1){//敌活五
                    enemy_map_score[map_detect_x][map_detect_y]+=800000000;
                }
                else if (map_certain_win[map_detect_x][map_detect_y][2]>=2){//我双活四
                    my_map_score[map_detect_x][map_detect_y]+=700000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][2]>=2){//敌双活四
                    my_map_score[map_detect_x][map_detect_y]+=650000000;
                }
                else if (map_certain_win[map_detect_x][map_detect_y][2]>=1 && map_certain_win[map_detect_x][map_detect_y][1]>=1){//我冲四活四
                    my_map_score[map_detect_x][map_detect_y]+=600000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][2]>=1 && map_certain_lose[map_detect_x][map_detect_y][1]>=1){//敌冲四活四
                    my_map_score[map_detect_x][map_detect_y]+=550000000;
                }
                else if (map_certain_win[map_detect_x][map_detect_y][1]>=2){//我双冲四
                    my_map_score[map_detect_x][map_detect_y]+=500000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][1]>=2){//敌双冲四
                    my_map_score[map_detect_x][map_detect_y]+=450000000;
                }
                else if (map_certain_win[map_detect_x][map_detect_y][2]>=1 && map_certain_win[map_detect_x][map_detect_y][0]>=1){//我活四活三
                    my_map_score[map_detect_x][map_detect_y]+=400000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][2]>=1 && map_certain_lose[map_detect_x][map_detect_y][0]>=1){//敌活四活三
                    my_map_score[map_detect_x][map_detect_y]+=350000000;
                }
                else if (map_certain_win[map_detect_x][map_detect_y][1]>=1 && map_certain_win[map_detect_x][map_detect_y][0]>=1){//我冲四活三
                    my_map_score[map_detect_x][map_detect_y]+=300000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][1]>=1 && map_certain_lose[map_detect_x][map_detect_y][0]>=1){//敌冲四活三
                    my_map_score[map_detect_x][map_detect_y]+=250000000;
                }
                else if (map_certain_win[map_detect_x][map_detect_y][1]+map_certain_win[map_detect_x][map_detect_y][2]>=1){//我四个
                    my_map_score[map_detect_x][map_detect_y]+=200000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][1]+map_certain_lose[map_detect_x][map_detect_y][2]>=1){//敌四个
                    my_map_score[map_detect_x][map_detect_y]+=150000000;
                }
                else if (map_certain_win[map_detect_x][map_detect_y][0]>=2){//我双活三
                    my_map_score[map_detect_x][map_detect_y]+=100000000;
                }
                else if (map_certain_lose[map_detect_x][map_detect_y][0]>=2){//敌双活三
                    my_map_score[map_detect_x][map_detect_y]+=50000000;
                }
                */

                //必胜、必输序列检测（新版）
                if (must_win_chess[map_detect_x][map_detect_y]==5){//我赢
                    my_map_score[map_detect_x][map_detect_y]+=600000;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==-5){//敌赢
                    enemy_map_score[map_detect_x][map_detect_y]+=500000;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==4){//双四
                    my_map_score[map_detect_x][map_detect_y]+=450000;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==-4){
                    enemy_map_score[map_detect_x][map_detect_y]+=400000;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==3){//四三
                    my_map_score[map_detect_x][map_detect_y]+=350000;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==-3){
                    enemy_map_score[map_detect_x][map_detect_y]+=300000;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==2){//活四，未调好
                    my_map_score[map_detect_x][map_detect_y]+=0;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==-2){
                    enemy_map_score[map_detect_x][map_detect_y]+=0;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==1){//双三
                    my_map_score[map_detect_x][map_detect_y]+=150000;
                }
                else if (must_win_chess[map_detect_x][map_detect_y]==-1){
                    enemy_map_score[map_detect_x][map_detect_y]+=100000;
                }


                if (enemy_map_score[map_detect_x][map_detect_y]+my_map_score[map_detect_x][map_detect_y]>maxScore){
                    maxScore=my_map_score[map_detect_x][map_detect_y]+enemy_map_score[map_detect_x][map_detect_y];
                    for (int i=0;i<maxScore_count;i++){
                        des_max[i][0]=0;
                        des_max[i][1]=0;
                    }
                    maxScore_count=1;
                    des_max[0][0]=map_detect_x;
                    des_max[0][1]=map_detect_y;
                }
                else if (my_map_score[map_detect_x][map_detect_y]+enemy_map_score[map_detect_x][map_detect_y]==maxScore){
                    des_max[maxScore_count][0]=map_detect_x;
                    des_max[maxScore_count][1]=map_detect_y;
                    maxScore_count++;
                }


            }
        }
    }

    if (maxScore_count>0){
        int temp_1005=rand()%maxScore_count;
        ai_find_x=des_max[temp_1005][0];
        ai_find_y=des_max[temp_1005][1];
        return 1;
    }
    else{
        if (my_chess==1){
            return 0;
        }
        else{
            for (int map_detect_x=1;map_detect_x<=map_y_temp;map_detect_x++){
                for (int map_detect_y=1;map_detect_y<=map_x_temp;map_detect_y++){
                    if (map_ai[map_detect_x][map_detect_y]==-1){
                        if (enemy_map_score[map_detect_x][map_detect_y]+my_map_score[map_detect_x][map_detect_y]>maxScore){
                            maxScore=my_map_score[map_detect_x][map_detect_y]+enemy_map_score[map_detect_x][map_detect_y];
                            for (int i=0;i<maxScore_count;i++){
                                des_max[i][0]=0;
                                des_max[i][1]=0;
                            }
                            maxScore_count=1;
                            des_max[0][0]=map_detect_x;
                            des_max[0][1]=map_detect_y;
                        }
                        else if (my_map_score[map_detect_x][map_detect_y]+enemy_map_score[map_detect_x][map_detect_y]==maxScore){
                            des_max[maxScore_count][0]=map_detect_x;
                            des_max[maxScore_count][1]=map_detect_y;
                            maxScore_count++;
                        }
                    }
                }
            }
            int temp_1005=rand()%maxScore_count;
            ai_find_x=des_max[temp_1005][0];
            ai_find_y=des_max[temp_1005][1];
            return 1;
        }
    }
}



//-------------------------------所有初始化函数和其他功能函数---------------------------------
void preaction_map_ai_copy() {//将预处理棋盘设定成当前棋盘
    for (int i = 0; i < thisismapx + 2; i++) {
        for (int j = 0; j < thisismapy + 2; j++) {
            preaction_map_ai[i][j] = map_ai[i][j];
        }
    }
}




//-----------------------------------------所有AI算法相关-------------------------------------------


bool in_search(int posx, int posy) {//检测指定点是否需要加入搜索（附近1格有子/2格内有2子）**作用于preaction_map_ai
    int chessnum = 0;
    for (int i = std::max(1, posx - 2); i <= std::min(thisismapx, posx + 2); i++) {
        for (int j = std::max(1, posy - 2); j <= std::min(thisismapy, posy + 2); j++) {
            if (preaction_map_ai[i][j] == 1 || preaction_map_ai[i][j] == 0)
                if (i != posx || j != posy) {
                    if (i == posx - 2 || i == posy + 2 || j == posy - 2 || j == posy + 2) {
                        chessnum++;
                        if (chessnum >= 2)return true;
                    }
                    else return true;
                }
        }
    }
    return false;
}

int sumscore(int posx, int posy) {//评估函数，作用于preaction_map_ai，记录含该点的所有结构总分，以黑棋为正分
    int score = 0;
    if (preaction_map_ai[posx][posy] == 0)score--;
    else if (preaction_map_ai[posx][posy] == 1)score++;
    return score;
}

int getscore_whiteplay(int posx, int posy) {//评估白棋落子的得分，评分方法为落子前后sumscore之差
    //注意一定要保证该点是空的！！
    //因为sumscore以黑棋为正，所以差个负号
    int score = sumscore(posx,posy);
    preaction_map_ai[posx][posy] = 0;
    score += -sumscore(posx, posy);
    //std::cout << "player=white posx=" << posx << " posy=" << posy << " sumscore=" << score << std::endl;
    //preaction_map_ai_display();
    preaction_map_ai[posx][posy] = -1;
    return score;
}

int getscore_blackplay(int posx, int posy) {//评估黑棋落子的得分，评分方法为落子前后sumscore之差
    //注意一定要保证该点是空的！！
    int score = -sumscore(posx, posy);
    preaction_map_ai[posx][posy] = 1;
    score += sumscore(posx, posy);
    //std::cout << "player=black posx=" << posx << " posy=" << posy << " sumscore=" << score << std::endl;
    //preaction_map_ai_display();
    preaction_map_ai[posx][posy] = -1;
    return score;
}

int getscore(int posx, int posy, int player) {
    if (player == 0)return getscore_whiteplay(posx, posy);
    else if (player == 1)return getscore_blackplay(posx, posy);
    return -99999999;
}

//极大-极小树
struct Tree {//记录点坐标和评分
    bool isActive;//是否激活
    int posx;
    int posy;
    int score;
};

int count = 0;

Tree search_sub_tree(int depth,int mscore,bool ismscoreActive) { //depth为奇数是极小树，偶数是极大树（从0开始）;
    //mscore为上级树传递下来的极（大/小）评分,ismscoreActive为上级树是否已成功激活（即有无mscore）
    Tree temptree{ false,0,0,0 };//创建临时本级树
    //count++;

    if(depth % 2 == 0) {//本级是极大树
        if (ismscoreActive) {//上级树已激活，可以进行剪枝
            if (depth >= maxdepth) {//搜索达到最大深度，返回最佳评分
                for (int i = 1; i <= thisismapx; i++)for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i, j)) {
                            int score = getscore(i, j, ai_player);
                            if (score >= mscore) {//本级树极大值超过上级极小树的极小值，进行剪枝
                                //std::cout << "剪枝成功,depth=" << depth << ",score=" << score << ",上级树score=" <<
                                //mscore << std::endl;
                                temptree.isActive = false;//剪枝：强制休眠本级树
                                return temptree;
                            }
                            if (!temptree.isActive) {//本级树未激活，则激活本级树
                                temptree.isActive = true;
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                            else if (score > temptree.score) {//本级树已激活，则比对评分
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                        }
            }
            else for (int i = 1; i <= thisismapx; i++) for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i, j)) {//向下继续深度搜索遍历所有子树
                            preaction_map_ai[i][j] = ai_player;//AI进行模拟落子
                            Tree temptreeII = search_sub_tree(depth + 1,temptree.score,temptree.isActive);//获取子树
                            if (temptreeII.isActive) {//子树成功激活
                                if (temptreeII.score >= mscore) {//本级树极大值超过上级极小树的极小值，进行剪枝
                                    preaction_map_ai[i][j] = -1;//记得恢复预处理棋盘！！
                                    //std::cout << "剪枝成功,depth=" << depth << ",score=" << temptreeII.score << ",上级树score=" <<
                                    //mscore << std::endl;
                                    temptree.isActive = false;//剪枝：强制休眠本级树
                                    return temptree;
                                }
                                if (!temptree.isActive) {//本级树未激活，则激活本级树
                                    temptree.isActive = true;
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                                else if (temptreeII.score > temptree.score) {//本级树已激活，则比对评分
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                            }
                            preaction_map_ai[i][j] = -1;//记得恢复预处理棋盘！！
                        }
        }

        else {//上级树未激活，无法剪枝

            if (depth >= maxdepth) {//搜索达到最大深度，返回最佳评分
                for (int i = 1; i <= thisismapx; i++)for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i,j)) {
                            int score = getscore(i, j, ai_player);
                            if (!temptree.isActive) {//本级树未激活，则激活本级树
                                temptree.isActive = true;
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                            else if (score > temptree.score) {//本级树已激活，则比对评分
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                        }
            }
            else for (int i = 1; i <= thisismapx; i++)for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i, j)) {//向下继续遍历所有子树
                            preaction_map_ai[i][j] = ai_player;//AI进行模拟落子
                            Tree temptreeII = search_sub_tree(depth + 1,temptree.score,temptree.isActive);//获取子树
                            if (temptreeII.isActive) {//子树成功激活
                                if (!temptree.isActive) {//本级树未激活，则激活本级树
                                    temptree.isActive = true;
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                                else if (temptreeII.score > temptree.score) {//本级树已激活，则比对评分
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                            }
                            preaction_map_ai[i][j] = -1;//记得恢复预处理棋盘！！
                        }
        }
    }

    else {//本级是极小树
        if (ismscoreActive) {//上级树已激活，可以进行剪枝
            if (depth >= maxdepth) {//搜索达到最大深度，返回最佳评分
                for (int i = 1; i <= thisismapx; i++)for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i, j)) {
                            int score = -getscore(i, j, 1 - ai_player);
                            if (score <= mscore) {//本级树极小值低于上级极大树的极大值，进行剪枝
                                //std::cout << "剪枝成功,depth=" << depth << ",score=" << score << ",上级树score=" <<
                                //mscore << std::endl;
                                temptree.isActive = false;//剪枝：强制休眠本级树
                                return temptree;
                            }
                            if (!temptree.isActive) {//本级树未激活，则激活本级树
                                temptree.isActive = true;
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                            else if (score > temptree.score) {//本级树已激活，则比对评分
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                        }
            }
            else for (int i = 1; i <= thisismapx; i++) for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i, j)) {//向下继续深度搜索遍历所有子树
                            preaction_map_ai[i][j] = 1 - ai_player;//AI进行模拟落子
                            Tree temptreeII = search_sub_tree(depth + 1, temptree.score, temptree.isActive);//获取子树
                            if (temptreeII.isActive) {//子树成功激活
                                if (temptreeII.score <= mscore) {//本级树极小值低于上级极大树的极大值，进行剪枝
                                    preaction_map_ai[i][j] = -1;//记得恢复预处理棋盘！！
                                    //std::cout << "剪枝成功,depth=" << depth << ",score=" << temptreeII.score << ",上级树score=" <<
                                    //mscore << std::endl;
                                    temptree.isActive = false;//剪枝：强制休眠本级树
                                    return temptree;
                                }
                                if (!temptree.isActive) {//本级树未激活，则激活本级树
                                    temptree.isActive = true;
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                                else if (temptreeII.score > temptree.score) {//本级树已激活，则比对评分
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                            }
                            preaction_map_ai[i][j] = -1;//记得恢复预处理棋盘！！
                        }
        }

        else {//上级树未激活，无法剪枝
            if (depth >= maxdepth) {//搜索达到最大深度，返回最佳评分
                for (int i = 1; i <= thisismapx; i++)for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i, j)) {
                            int score = -getscore(i, j, 1 - ai_player);
                            if (!temptree.isActive) {//本级树未激活，则激活本级树
                                temptree.isActive = true;
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                            else if (score > temptree.score) {//本级树已激活，则比对评分
                                temptree.posx = i;
                                temptree.posy = j;
                                temptree.score = score;
                            }
                        }
            }
            else for (int i = 1; i <= thisismapx; i++)for (int j = 1; j <= thisismapy; j++)
                        if (preaction_map_ai[i][j] == -1 && in_search(i, j)) {//向下继续遍历所有子树
                            preaction_map_ai[i][j] = 1 - ai_player;//AI进行模拟落子
                            Tree temptreeII = search_sub_tree(depth + 1, temptree.score, temptree.isActive);//获取子树
                            if (temptreeII.isActive) {//子树成功激活
                                if (!temptree.isActive) {//本级树未激活，则激活本级树
                                    temptree.isActive = true;
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                                else if (temptreeII.score > temptree.score) {//本级树已激活，则比对评分
                                    temptree.posx = temptreeII.posx;
                                    temptree.posy = temptreeII.posy;
                                    temptree.score = temptreeII.score;
                                }
                            }
                            preaction_map_ai[i][j] = -1;//记得恢复预处理棋盘！！
                        }
        }
    }
    return temptree;
}

int ai_point[2] = { 0,0 };//落子位置

bool ai_play() {//AI尝试获取落子位置,成功则落子位置被记录并返回true，否则返回false
    preaction_map_ai_copy();
    Tree temptree = search_sub_tree(0, 0, false);
    if (temptree.isActive) {
        ai_point[0] = temptree.posx;
        ai_point[1] = temptree.posy;
        return true;
    }
    else return false;
}


int AI::method_boyishu(){
    thisismapx=map_x_temp;
    thisismapy=map_y_temp;
    if (ai_mode==11){
        maxdepth=3;
    }
    else if (ai_mode==12){
        maxdepth=5;
    }
    else if (ai_mode==13){
        maxdepth=7;
    }
    else{
        qDebug()<<"ai难度系数未找到";
        maxdepth=7;
    }

    int temp_mhx_tqltql;
    temp_mhx_tqltql=ai_play();
    if (temp_mhx_tqltql){
        ai_find_x=ai_point[0];
        ai_find_y=ai_point[1];
        return 1;
    }
    return 0;
}


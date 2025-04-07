#include "board.h"
#include "ui_board.h"
#include "hiddensource.h"
#include <filesystem>
#include "read_save_select.h"
#include "aitest.h"
#include "board_review.h"
#include <QPainter>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QInputDialog>
#include <QObject>
#include <math.h>
#include <QFile>
#include <QWidget>
#include <time.h>
#include <QSoundEffect>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>
#include <QPixmap>
#include <functional>
#include <windows.h>

int read_save_flag=0;

int row_z=-1,col_z=-1,flag_sbzz=0;//鼠标追踪相关参数

int flag_gaming_double=0,flag_musicmode_double=-1;
int single_whofirst=0;//单人模式谁先手 0表示玩家先（玩家执黑）1表示机器先
int flag_esapce_closechange=0;//打开读档界面的标识变量
bool ruleLC=true;//长连禁手
bool ruleFF=true;//四四禁手
bool ruleTT=true;//三三禁手

size_t operater_password=5690257477338839213;//管理员密码掩码
size_t yuanshen_qidong=1023947710135759026;
size_t caijiuduolian=9697797889013647504;

int mode_must_blind=0;//必须盲棋模式

int mode_blind_chess=0;

int ai_renshu=0;

int win_method;



int flag_just_open_board=0;

int hidden_close=0;

int ad_mode=0;

int flag_idea_paint=0,flag_idea_paint_chess_color=-1;

std::string backgroundmusic_str,savepath_double;
QString backgroundmusic_qstr;


int map[21][21];
/*棋盘最大储存范围为（1，1）-（19，19)

*/
int Chess_process[400][2]={0};//第0步是空棋盘状态，用于检测，棋局第一步即存在Chess_process[1]中
int Chess_count=0,flag_close_save=0;
float Volume_user=0.8;
std::fstream f;

//判定部分定义↓


//棋子类型：-1=empty,1=black,0=white,2=wall
//棋盘为17*17二维数组

//棋盘信息

int preaction_map[21][21] = { {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
                             {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2} }; //预处理棋盘


//所有判定序列
const int null[1] = { 0 };//空
const int lc[6] = { 1,1,1,1,1,1 };//长连
const int win[2][5] = { { 0,0,0,0,0 },{1,1,1,1,1} };//五连
const int tc_I[2][7] = { {4,-1,1,1,1,-1,-1},{-1,-1,1,1,1,-1,4} };//活三种类1 *同一方向上只统计1次
const int tc_I_origin[3] = {1,1,1};//活三种类1的原始形式
const int tc_II[2][6]={ {-1, 1, -1, 1, 1,-1},{-1,1,1,-1,1,-1} };//活三种类2
const int tc_II_origin[2][4] = { { 1, -1, 1, 1},{1,1,-1,1} };//活三种类2的原始形式
const int fc_I[2][6] = { {4,-1,1,1,1,1},{1,1,1,1,-1,4} };//活四种类1 *同一方向上只统计1次
const int fc_I_origin[4] = {1,1,1,1};//活四种类1的原始形式
const int fc_II[3][7] = { {4,1,1,1,-1,1,4},{4,1,1,-1,1,1,4},{4,1,-1,1,1,1,4} };//活四种类2
const int fc_II_origin[3][5] = { {1,1,1,-1,1},{1,1,-1,1,1},{1,-1,1,1,1} };//活四种类2的原始形式

/*
判定法则:用于判断棋子与序列是否匹配
因为-1是空位置，索引需要从0开始，所以使用其中的值需要+1,即：match[chess+1][cs_num+1]
cs_num=-1,0,1,2时仅匹配到对应chess返回true;
cs_num=3时一定返回true;
cs_num=4时除黑子外,即匹配到白子，空，墙均返回true；
cs_num=5时除白子外，即匹配到黑子，空，墙均返回true；
*/
const bool match[4][7] = { {true,false,false,false,true,true,true},//chess=-1,空位置;
                          {false,true,false,false,true,true,false},//chess=0,白子;
                          {false,false,true,false,true,false,true},//chess=1,黑子;
                          {false,false,false,true,true,true,true} };//chess=2,墙;

/*
玩家信息:
Player:white, black 执白棋玩家为玩家0，执黑棋玩家为玩家1
state:玩家是否胜利，0=否，1=是，2=黑子违反禁手规则 #default=0
win_posx(y):若玩家胜利，储存五连子的起始坐标 #default=-1
win_direct:若玩家胜利，储存五连子的方向 #default=-1
win_case:五连子序列(一般也不会去改它...) #default=win[0] or win[1]
win_case_length:五连子序列长度（废话） #default=5
fm_type:违反的禁手规则,LC=长连禁手,TT=三三禁手,FF=四四禁手 #default="NULL"
fm_posx(y)(_II):禁手序列起始坐标(TT,FF需要储存两个) #default=-1
fm_direct(_II):禁手序列方向(TT,FF需要储存两个) #default=-1
fm_case(_II):禁手序列(TT,FF需要储存两个) #default=null
fm_case(_II)_length:禁手序列长度(TT,FF需要储存两个) #default=1
*/
struct player_white{
    int state = 0;
    int win_posx = -1;
    int win_posy = -1;
    int win_direct = -1;
    const int* win_case = win[0];
    int win_case_length = 5;
}white = { 0,-1,-1,-1,win[0],5 };
struct player_black{
    int state = 0;
    int win_posx = -1;
    int win_posy = -1;
    int win_direct = -1;
    const int* win_case = win[1];
    int win_case_length = 5;
    std::string fm_type = "NULL";
    int fm_posx = -1;
    int fm_posy = -1;
    int fm_direct = -1;
    const int* fm_case = null;
    int fm_case_length = 1;
    int fm_posx_II = -1;
    int fm_posy_II = -1;
    int fm_direct_II = -1;
    const int* fm_case_II= null;
    int fm_case_II_length = 1;
}black = {0,-1,-1,-1,win[1],5,"NULL",-1,-1,-1,null,1,-1,-1,-1,null,1};

int tempinfo[3] = { -1,-1,-1 };//用于临时储存find函数比对成功的direct,posx,posy信息

int fm_point[400][2] = { {0,0} };//记录禁手点
std::string fm_point_type[400] = {};//禁手点对应的禁手规则
int fm_point_length = 0;//禁手点序列长度


void Board::PlayMusic(){
    if (!player){
        player = new QMediaPlayer;
        player->setSource(backgroundmusic_qstr);
    }
    player->play();
}


void Board::onStateChanged(QAudio::State state){
    if (state==QAudio::StoppedState){
        player->setPosition(0);
        player->play();
    }
}





Board::Board(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Board)
{

    setWindowIcon(QIcon(":/icon/favicon.ico"));
    flag_just_open_board=1;
    ui->setupUi(this);
    flag_idea_paint=0;
    ui->chess_undo->setFont(QFont("华文楷体",16));
    ui->chess_false->setFont(QFont("华文楷体",16));
    ui->save_and_exit->setFont(QFont("华文楷体",16));
    ui->read_save->setFont(QFont("华文楷体",16));
    ui->gamemode->setFont(QFont("华文楷体",16));
    ui->rule_pro->setFont(QFont("华文楷体",16));
    ui->change_whofirst->setFont(QFont("华文楷体",16));
    ui->AutoBattle->setFont(QFont("华文楷体",14));
    ui->blindchess->setFont(QFont("华文楷体",14));
    ui->label->setFont(QFont("华文楷体",16));

    read_save_flag=0;

    ad_mode=0;

    ruleLC=ruleLC_save;
    ruleTT=ruleTT_save;
    ruleFF=ruleFF_save;

    hidden_close=0;

    single_whofirst=0;

    mode_blind_chess=0;

    win_method=-1;


    for (int i=0;i<21;i++){
        for (int j=0;j<21;j++){
            map[i][j]=-1;
        }
    }

    Chess_count=0;

    f.open("flag_gaming.txt",std::ios::out);
    f<<flag_board_gaming_mode<<std::endl;
    f.close();
    map_x_temp=map_x;
    map_y_temp=map_y;
    //qDebug()<<"写入对局状态";
    map_default();
    if ((flag_board_gaming_mode<=2) && (flag_gaming_double==0 || flag_gaming_double==5)){

        flag_gaming_double=1;
        //写入临时保存棋局的开头两行
        map_x_temp=map_x;map_y_temp=map_y;
        time_t chess_start_time;
        time(&chess_start_time);
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out);
        else {
            //qDebug()<<"ERROR20";
        }
        f<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
        if (flag_board_gaming_mode==0)  f<<flag_board_gaming_mode+single_whofirst+20<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else if (flag_board_gaming_mode==1)  f<<flag_board_gaming_mode<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else {
            //qDebug()<<"ERROR502";
        }
        f<<map_x_temp<<" "<<map_y_temp<<std::endl;//棋盘大小
        f.close();
    }
    else if(flag_board_gaming_mode==3 || flag_board_gaming_mode==4){//若上次棋局意外退出，加载未保存的棋局
        read_save_flag=1;
        ui->read_save->setText("提示");
        /* 临时棋局记录文件结构：
         * 第一行：游戏开始时间
         * 第二行：游戏模式（0为单人，1为双人,20表示单人玩家执黑，21表示单人玩家执白）禁手信息（依次为LC、FF、TT）
         * 第三行：棋盘大小map_x map_y;
         * 第四行及以后格式："坐标 列坐标" （行列坐标从1开始计数）
         * 悔棋用-20 -20表示
         * -114 -514表示棋局结束
         * 结束后第一行：胜负模式 0为白连五 1为黑连五 21长连禁手 22四四禁手 23三三禁手 3平局
         * 结束后第二行和第三行：胜负序列标识 格式如下
         * 起始点x坐标 起始点y坐标 序列方向（0右 1下 2右下 3左下） 序列长度
         * 注：1为黑棋，0为白棋，count从1开始计数（即表示真实步数）*/
        flag_board_gaming_mode-=3;
        flag_gaming_double=1;
        f.open("flag_gaming.txt",std::ios::out);
        f<<flag_board_gaming_mode<<std::endl;
        f.close();
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::in);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::in);
        else{
            //qDebug()<<"ERROR21";
        }
        int temp_1001;
        std::string str_temp;
        std::getline(f,str_temp);//读取掉时间行
        f>>temp_1001;//读掉游戏模式
        f>>ruleLC>>ruleFF>>ruleTT;
        f>>map_x_temp>>map_y_temp;
        map_default();
        while(f>>Chess_process[Chess_count+1][0]>>Chess_process[Chess_count+1][1]){
            //qDebug()<<Chess_process[Chess_count+1][0]<<" "<<Chess_process[Chess_count+1][1];
            if (Chess_process[Chess_count+1][0]==-20 && Chess_process[Chess_count+1][1]==-20){//-20 -20表示悔棋
                map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=-1;
                Chess_process[Chess_count][0]=0;
                Chess_process[Chess_count][1]=0;
                Chess_count--;
            }
            else{
                Chess_count++;
                map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=Chess_count%2;
            }
        }
        f.close();
        if (Chess_count==0) ui->read_save->setText("读取存档");
        if (temp_1001==20){
            ui->gamemode->setText("人机对弈(玩家执黑)");
            if (Chess_count==0) ui->change_whofirst->show();
            single_whofirst=0;
        }
        else if (temp_1001==21){
            ui->gamemode->setText("人机对弈(玩家执白)");
            if (Chess_count<=1){
                ui->read_save->setText("读取存档");
                ui->change_whofirst->show();
            }
            single_whofirst=1;
        }
        if (flag_board_gaming_mode==0)  setWindowTitle("人机对弈");
    }
    else if (flag_board_gaming_mode==5 || flag_board_gaming_mode==6){//读档
        read_save_flag=1;
        ui->read_save->setText("提示");
        flag_gaming_double=1;
        flag_board_gaming_mode-=5;
        f.open("flag_gaming.txt",std::ios::out);
        f<<flag_board_gaming_mode<<std::endl;
        f.close();
        if (flag_board_gaming_mode==0)  setWindowTitle("人机对弈");
        //qDebug()<<flag_board_gaming_mode;

        time_t chess_start_time;
        time(&chess_start_time);
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out);
        else{
            //qDebug()<<"ERROR22";
        }
        f<<asctime(localtime(&chess_start_time))<<std::endl;//时间行（输出时间中内含换行）
        f.close();

        //qDebug()<<savepath_double;
        //qDebug()<<flag_gaming_double<<" "<<flag_board_gaming_mode;
        if (flag_board_gaming_mode==1)  f.open("history/unfinished/double/"+savepath_double+".txt",std::ios::in);
        else if (flag_board_gaming_mode==0)  f.open("history/unfinished/single/"+savepath_double+".txt",std::ios::in);
        else{

            //qDebug()<<"ERROR23";
        }
        int temp_1001;
        std::string str_temp;
        std::getline(f,str_temp);//读取掉时间行
        f>>temp_1001;//读掉游戏模式
        f>>ruleLC>>ruleFF>>ruleTT;
        f>>map_x_temp>>map_y_temp;
        map_default();
        while(f>>Chess_process[Chess_count+1][0]>>Chess_process[Chess_count+1][1]){
            //qDebug()<<Chess_process[Chess_count+1][0]<<" "<<Chess_process[Chess_count+1][1];
            if (Chess_process[Chess_count+1][0]==-20 && Chess_process[Chess_count+1][1]==-20){//-20 -20表示悔棋
                map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=-1;
                Chess_process[Chess_count][0]=0;
                Chess_process[Chess_count][1]=0;
                Chess_count--;
            }
            else{
                Chess_count++;
                map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=Chess_count%2;
            }
        }
        f.close();
        if (Chess_count==0) ui->read_save->setText("读取存档");
        if (temp_1001==20){
            ui->gamemode->setText("人机对弈(玩家执黑)");
            single_whofirst=0;
        }
        else if (temp_1001==21){
            ui->gamemode->setText("人机对弈(玩家执白)");
            if (Chess_count<=1){
                ui->read_save->setText("读取存档");
            }
            single_whofirst=1;
        }
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out|std::ios::app);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
        else{

            //qDebug()<<"ERROR24";
        }
        if (flag_board_gaming_mode==0)  f<<flag_board_gaming_mode+single_whofirst+20<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else if (flag_board_gaming_mode==1)  f<<flag_board_gaming_mode<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else{

            //qDebug()<<"ERROR55";
        }
        f<<map_x_temp<<" "<<map_y_temp<<std::endl;

        for (int i=1;i<=Chess_count;i++){
            f<<Chess_process[i][0]<<" "<<Chess_process[i][1]<<std::endl;
        }
        f.close();
        if (flag_board_gaming_mode==1){
            std::filesystem::remove("history/unfinished/double/"+savepath_double+".txt");
            std::filesystem::remove("history/unfinished/double/"+savepath_double+".png");
        }
        else if (flag_board_gaming_mode==0){
            std::filesystem::remove("history/unfinished/single/"+savepath_double+".txt");
            std::filesystem::remove("history/unfinished/single/"+savepath_double+".png");
        }
        else{
            //qDebug()<<"ERROR25";
        }
    }
    else{

        //qDebug()<<"ERROR#2";
    }


    //qDebug()<<flag_board_gaming_mode;
    //播放背景音乐
    f.open("music_background.txt",std::ios::in);
    f>>flag_musicmode_double;
    getline(f,backgroundmusic_str);
    getline(f,backgroundmusic_str);
    f.close();
    //qDebug()<<flag_musicmode_double<<" "<<backgroundmusic_str;

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(backgroungmusic_volume);


    int len_temp_1;
    len_temp_1=backgroundmusic_str.length();
    backgroundmusic_str.erase(len_temp_1,1);
    backgroundmusic_qstr=QString::fromStdString(backgroundmusic_str);
    if (backgroundmusic_qstr.isEmpty()){//检查音乐文件是否缺省
        flag_musicmode_double=-1;
    }
    if (flag_musicmode_double==1){
        player->setSource(backgroundmusic_qstr);
    }
    else if (flag_musicmode_double==0){
        player->setSource(QUrl("qrc:/music/background_music/"+backgroundmusic_qstr+".mp3"));
    }
    else{
        player->setSource(QUrl("qrc:/music/background_music/Wake.mp3"));
    }
    //QObject::connect(player,QMediaPlayer::statechanged,Board::onStateChanged);
    player->setLoops(QMediaPlayer::Infinite);
    PlayMusic();

    //调整窗口大小和按钮位置+按钮ui设计
    this->resize(map_x_temp*40+310,std::max(map_y_temp*40+40,400));
    int button_start_x,button_start_y;
    button_start_x=map_x_temp*40+50;
    button_start_y=25;
    ui->gamemode->setGeometry(QRect(button_start_x,button_start_y,231,31));
    ui->AutoBattle->setGeometry(QRect(button_start_x+10,button_start_y+280,151,33));
    ui->blindchess->setGeometry(QRect(button_start_x+120,button_start_y+280,151,33));
    ui->background_music_volume->setGeometry(QRect(button_start_x+90,button_start_y+320,160,32));
    ui->change_whofirst->setGeometry(QRect(button_start_x+190,button_start_y,61,32));
    ui->label->setGeometry(QRect(button_start_x,button_start_y+320,100,29));
    ui->rule_pro->setGeometry(QRect(button_start_x,button_start_y+40,271,31));
    button_start_x+=1;
    button_start_y+=80;
    ui->chess_false->setGeometry(QRect(button_start_x,button_start_y+1,229,32));
    ui->chess_undo->setGeometry(QRect(button_start_x,button_start_y+51,229,32));
    ui->read_save->setGeometry(QRect(button_start_x,button_start_y+102,229,32));
    ui->save_and_exit->setGeometry(QRect(button_start_x,button_start_y+152,229,32));
    ui->ui_button_1->setGeometry(QRect(button_start_x,button_start_y+1-2,229,36));
    ui->ui_button_2->setGeometry(QRect(button_start_x,button_start_y+51-2,229,36));
    ui->ui_button_3->setGeometry(QRect(button_start_x,button_start_y+102-2,229,36));
    ui->ui_button_4->setGeometry(QRect(button_start_x,button_start_y+152-2,229,36));
    //ui->verticalLayout->addSpacing();


    //禁止修改窗口大小
    this->setMinimumSize(map_x_temp*40+310,std::max(map_y_temp*40+40,400));
    this->setMaximumSize(map_x_temp*40+310,std::max(map_y_temp*40+40,400));


    //修改窗口信息
    if (flag_board_gaming_mode==0){
        if (Chess_count>0)  ui->change_whofirst->hide();
        setWindowTitle("人机对弈");
        if (single_whofirst==0) ui->gamemode->setText("人机对弈(玩家执黑)");
        else if (single_whofirst==1) ui->gamemode->setText("人机对弈(玩家执白)");
        else ui->gamemode->setText("人机对弈");
    }
    else ui->change_whofirst->hide();
    if (ruleFF==0 && ruleTT==0 && ruleLC==0)    ui->rule_pro->setText("禁手信息：无");
    else{
        std::string temp_201="禁手信息:";
        if (ruleLC==1)  temp_201+="长连 ";
        if (ruleFF==1)  temp_201+="四四 ";
        if (ruleTT==1)  temp_201+="三三 ";
        ui->rule_pro->setText(QString::fromStdString(temp_201));
    }

    int temp_114514;
    temp_114514=100*backgroungmusic_volume;
    //qDebug()<<"背景音乐初始值："<<temp_114514;
    ui->background_music_volume->setValue(temp_114514);


    flag_just_open_board=0;
    //qDebug()<<flag_board_gaming_mode;


    if (mode_must_blind==1){
        mode_blind_chess=1;
        ui->blindchess->setChecked(1);

        //可以自定义图标的qMessageBox
        QMessageBox msgBox;
        msgBox.setWindowTitle("盲棋裁判");
        msgBox.setText("当前版本为盲棋专业版，只能使用盲棋模式！\n 如需解锁完全专业版，请联系管理员！");
        QPixmap mIcon(":/pictures/messgagebox_funny.png"); //图标路径
        mIcon=mIcon.scaled(60,60,Qt::KeepAspectRatio,Qt::SmoothTransformation);
        if (!mIcon.isNull()){
            msgBox.setIconPixmap(mIcon); //设置自定义图标
        }
        else {
            msgBox.setIcon(QMessageBox::Warning); //图标加载失败时的备选
        }
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.exec();
    }

}




void Board::closeEvent(QCloseEvent *event){//关闭窗口的多种事件响应
    if (mainwindow_exit==1 || hidden_close==1){
        return;
    }
    if (flag_gaming_double==1 || (flag_gaming_double==5 && flag_esapce_closechange==1)){//棋局未在进行中，直接关闭
        if (flag_esapce_closechange==1){
            //qDebug()<<flag_board_gaming_mode;
            flag_esapce_closechange=0;
            player->stop();
        }
        else if (flag_close_save!=1){//右上角叉号退出
            QSoundEffect sound1;
            sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
            sound1.play();
            QMessageBox choose(QMessageBox::Question,QString(tr("请确认")),QString(tr("当前对局仍在进行中，且尚未保存，确认要退出吗？")),QMessageBox::Yes| QMessageBox::No);
            choose.button(QMessageBox::Yes)->setText(tr("不保存直接退出"));
            choose.button(QMessageBox::No)->setText(tr("取消"));
            int temp_15=choose.exec();
            if (temp_15 == QMessageBox::Yes){
                flag_close_save=0;
                flag_gaming_double=0;
                flag_board_gaming_mode=-1;
                std::fstream f;
                f.open("flag_gaming.txt",std::ios::out);
                f<<"-1"<<std::endl;
                f.close();
                player->stop();
                event->accept();
            }
            else{
                flag_close_save=0;
                event->ignore();
            }
        }
        else{//保存并退出
            QSoundEffect sound1;
            sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
            sound1.play();
            QMessageBox choose(QMessageBox::Question,QString(tr("请确认")),QString(tr("当前对局尚未结束，确认要保存当前进度并退出吗？")),QMessageBox::Yes| QMessageBox::No);
            choose.button(QMessageBox::Yes)->setText(tr("保存并退出"));
            choose.button(QMessageBox::No)->setText(tr("取消"));
            int temp_15=choose.exec();
            if (temp_15 == QMessageBox::Yes){
                write_in_double(0,0);
                flag_close_save=0;
                flag_gaming_double=0;
                flag_board_gaming_mode=-1;
                std::fstream f;
                f.open("flag_gaming.txt",std::ios::out);
                f<<"-1"<<std::endl;
                f.close();
                player->stop();
                event->accept();
            }
            else{
                flag_close_save=0;
                event->ignore();
            }
        }
    }
    else{
        flag_board_gaming_mode=-1;
        flag_gaming_double=0;
        flag_close_save=0;
        player->stop();
    }
}


Board::~Board()
{
    delete ui;
}


int Chess_r_l=18;
int Chess_r_s=4;

void Board::paintEvent(QPaintEvent *){//绘制五子棋棋盘

    QPainter painter(this);

    painter.drawPixmap(0,-10,2400,1124,QPixmap(":/pictures/board_background.png"));//画背景图片


    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);


    ////qDebug()<<map_x_temp<<" "<<map_y_temp;
    //画背景
    QPen Line,Point,Square;
    QBrush Square_brush;
    Square.setColor(Qt::yellow);
    painter.setPen(Square);
    Square_brush.setColor(Qt::yellow);
    Square_brush.setStyle(Qt::SolidPattern);
    painter.setBrush(Square_brush);
    QRect Background(20,20,map_x_temp*40,map_y_temp*40);
    painter.drawRect(Background);
    /*for (int i=0;i<21;i++){
        for (int j=0;j<21;j++){
            std::cout<<map[i][j];
        }
        std::cout<<std::endl;
    }*/
    //画特征点
    if (map_x_temp==15 && map_y_temp==15){
        Point.setWidth(3);
        Point.setBrush(Qt::black);
        painter.setPen(Point);
        int r=3;
        QRect Circle_1(160-r,160-r,2*r,2*r);
        painter.drawEllipse(Circle_1);
        QRect Circle_2(160-r,480-r,2*r,2*r);
        painter.drawEllipse(Circle_2);
        QRect Circle_3(480-r,160-r,2*r,2*r);
        painter.drawEllipse(Circle_3);
        QRect Circle_4(320-r,320-r,2*r,2*r);
        painter.drawEllipse(Circle_4);
        QRect Circle_5(480-r,480-r,2*r,2*r);
        painter.drawEllipse(Circle_5);
    }
    //画网格
    Line.setWidth(2);
    Line.setBrush(Qt::black);
    painter.setPen(Line);
    for (int i=1;i<=map_x_temp;i++){
        QLine Line_2(i*40,40,i*40,map_y_temp*40);
        painter.drawLine(Line_2);
    }
    for (int i=1;i<=map_y_temp;i++){
        QLine Line_1(40,i*40,map_x_temp*40,i*40);
        painter.drawLine(Line_1);
    }
    Line.setWidth(4);
    Line.setBrush(Qt::darkBlue);
    painter.setPen(Line);
    QLine Line_3(18,18,18,map_y_temp*40+20);
    painter.drawLine(Line_3);
    QLine Line_4(18,18,map_x_temp*40+20,18);
    painter.drawLine(Line_4);
    QLine Line_5(18,map_y_temp*40+20,map_x_temp*40+20,map_y_temp*40+20);
    painter.drawLine(Line_5);
    QLine Line_6(map_x_temp*40+20,18,map_x_temp*40+20,map_y_temp*40+20);
    painter.drawLine(Line_6);


    if (mode_blind_chess==1){//盲棋模式绘制坐标提示
        Line.setColor(Qt::red);
        painter.setPen(Line);
        for (int i=1;i<=map_y_temp;i++){
            painter.drawText(20,i*40-Chess_r_l/2,Chess_r_l,Chess_r_l,Qt::AlignCenter,QString::fromStdString(std::to_string(i)));
        }
        for (int i=1;i<=map_x_temp;i++){
            painter.drawText(i*40-Chess_r_l/2,20,Chess_r_l,Chess_r_l,Qt::AlignCenter,QString::fromStdString(std::to_string(i)));
        }

    }

    if (mode_blind_chess==1 && flag_gaming_double==1){//盲棋模式不画棋子
        return ;
    }

    drawChess();

}




void Board::drawChess(){//绘制棋子


    /*QPen Chess_Pen_white;
    Chess_Pen_white.setWidth(12);
    Chess_Pen_white.setBrush(Qt::white);
    QPen Chess_Pen_black;
    Chess_Pen_black.setWidth(12);
    Chess_Pen_black.setBrush(Qt::black);
    QBrush Square;*/
    QPainter painter(this);

    QPen Chess_Pen;


    if (flag_idea_paint==1){//绘制提示棋子位置
        QPen Chess_Pen_white;
        Chess_Pen_white.setWidth(12);
        Chess_Pen_white.setBrush(Qt::white);
        QPen Chess_Pen_black;
        Chess_Pen_black.setWidth(12);
        Chess_Pen_black.setBrush(Qt::black);
        QBrush Square;
        if (flag_idea_paint_chess_color==0){//白棋
            ////qDebug()<<"FLAG10";
            painter.setPen(Chess_Pen_white);
            Square.setColor(Qt::white);
            painter.setBrush(Square);
            QRect chess_white_large(ai_find_y*40-Chess_r_l,ai_find_x*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
            painter.drawEllipse(chess_white_large);
        }
        else if (flag_idea_paint_chess_color==1){//黑棋
            ////qDebug()<<"FLAG11";
            Square.setColor(Qt::black);
            painter.setPen(Chess_Pen_black);
            painter.setBrush(Square);
            QRect chess_black_large(ai_find_y*40-Chess_r_l,ai_find_x*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
            painter.drawEllipse(chess_black_large);
        }
        else{
            //qDebug()<<"ERROR510";
        }
    }


    Chess_Pen.setWidth(1);
    painter.setPen(Chess_Pen);
    /*
    for(int i=1;i<=map_y_temp;i++){
        for(int j=1;j<=map_x_temp;j++){
            if (map[i][j]==0){//白棋
                QRect chess_white_large(j*40-Chess_r_l,i*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
                painter.drawPixmap(chess_white_large,QPixmap(":/pictures/chess_white.png"));
            }
            else if (map[i][j]==1){//黑棋
                QRect chess_black_large(j*40-Chess_r_l,i*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
                painter.drawPixmap(chess_black_large,QPixmap(":/pictures/chess_black.png"));
            }
        }
    }
    */
    for(int i=1;i<=Chess_count;i++){
        if (i%2==0){//白棋
            QRect chess_white_large(Chess_process[i][1]*40-Chess_r_l,Chess_process[i][0]*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
            painter.drawPixmap(chess_white_large,QPixmap(":/pictures/chess_white.png"));
        }
        else if (i%2==1){//黑棋
            QRect chess_black_large(Chess_process[i][1]*40-Chess_r_l,Chess_process[i][0]*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
            painter.drawPixmap(chess_black_large,QPixmap(":/pictures/chess_black.png"));
        }
    }


    //鼠标追踪-预落子位置绘制
    if (flag_sbzz && flag_gaming_double==1){
        //qDebug()<<row_z<<" "<<col_z;
        if (map[row_z][col_z]==-1){
            if ((Chess_count+1)%2==0){//白棋
                QRect chess_white_large(col_z*40-Chess_r_l,row_z*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
                painter.drawPixmap(chess_white_large,QPixmap(":/pictures/white_transparent.png"));
            }
            else if ((Chess_count+1)%2==1){//黑棋
                QRect chess_black_large(col_z*40-Chess_r_l,row_z*40-Chess_r_l,2*Chess_r_l,2*Chess_r_l);
                painter.drawPixmap(chess_black_large,QPixmap(":/pictures/black_transparent.png"));
            }
        }

    }


    if (Chess_count>0){//为最后落下的棋子绘制标识
        Chess_Pen.setWidth(6);
        Chess_Pen.setColor(Qt::red);
        painter.setPen(Chess_Pen);
        painter.drawPoint(Chess_process[Chess_count][1]*40,Chess_process[Chess_count][0]*40);
    }




    //赢棋后绘制提示线
    if (win_method==0){//白连五
        //qDebug()<<"FLAG601";
        //qDebug()<<"白子获胜序列起点："<<white.win_posx<<" "<<white.win_posy<<" "<<white.win_direct;

        int des_d_x,des_d_y;

        des_d_x=white.win_posx;
        des_d_y=white.win_posy;

        if (white.win_direct==0 || white.win_direct==2 || white.win_direct==3)    des_d_y+=4;
        if (white.win_direct==1 || white.win_direct==2 || white.win_direct==3)    des_d_x+=4;
        if (white.win_direct==3)   des_d_y-=8;
        //qDebug()<<"白子获胜序列终点："<<des_d_x<<" "<<des_d_y;
        QPen winpaint;
        winpaint.setWidth(10);
        winpaint.setColor(Qt::green);
        painter.setPen(winpaint);
        QLine Line_11(white.win_posy*40,white.win_posx*40,des_d_y*40,des_d_x*40);
        painter.drawLine(Line_11);
    }
    else if (win_method==1){//黑连五

        int des_d_x,des_d_y;
        des_d_x=black.win_posx;
        des_d_y=black.win_posy;

        if (black.win_direct==0 || black.win_direct==2 || black.win_direct==3)    des_d_y+=4;
        if (black.win_direct==1 || black.win_direct==2 || black.win_direct==3)    des_d_x+=4;
        if (black.win_direct==3)   des_d_y-=8;

        QPen winpaint;
        winpaint.setWidth(10);
        winpaint.setColor(Qt::green);
        painter.setPen(winpaint);
        QLine Line_11(black.win_posy*40,black.win_posx*40,des_d_y*40,des_d_x*40);
        painter.drawLine(Line_11);
    }
    else if (win_method==21){//长连禁手
        int des_d_x,des_d_y;
        des_d_x=black.fm_posx;
        des_d_y=black.fm_posy;

        if (black.fm_direct==0 || black.fm_direct==2 || black.fm_direct==3)    des_d_y+=black.fm_case_length-1;
        if (black.fm_direct==1 || black.fm_direct==2 || black.fm_direct==3)    des_d_x+=black.fm_case_length-1;
        if (black.fm_direct==3)   des_d_y-=2*black.fm_case_length-2;

        QPen winpaint;
        winpaint.setWidth(10);
        winpaint.setColor(Qt::red);
        painter.setPen(winpaint);
        QLine Line_11(black.fm_posy*40,black.fm_posx*40,des_d_y*40,des_d_x*40);
        painter.drawLine(Line_11);
    }
    else if (win_method==22){//四四禁手
        int des_d_x,des_d_y;
        des_d_x=black.fm_posx;
        des_d_y=black.fm_posy;

        if (black.fm_direct==0 || black.fm_direct==2 || black.fm_direct==3)    des_d_y+=black.fm_case_length-1;
        if (black.fm_direct==1 || black.fm_direct==2 || black.fm_direct==3)    des_d_x+=black.fm_case_length-1;
        if (black.fm_direct==3)   des_d_y-=2*black.fm_case_length-2;

        QPen winpaint;
        winpaint.setWidth(10);
        winpaint.setColor(Qt::red);
        painter.setPen(winpaint);
        QLine Line_11(black.fm_posy*40,black.fm_posx*40,des_d_y*40,des_d_x*40);
        painter.drawLine(Line_11);

        des_d_x=black.fm_posx_II;
        des_d_y=black.fm_posy_II;

        if (black.fm_direct_II==0 || black.fm_direct_II==2 || black.fm_direct_II==3)    des_d_y+=black.fm_case_II_length-1;
        if (black.fm_direct_II==1 || black.fm_direct_II==2 || black.fm_direct_II==3)    des_d_x+=black.fm_case_II_length-1;
        if (black.fm_direct_II==3)   des_d_y-=2*black.fm_case_II_length-2;

        winpaint.setWidth(10);
        winpaint.setColor(Qt::red);
        painter.setPen(winpaint);
        QLine Line_12(black.fm_posy_II*40,black.fm_posx_II*40,des_d_y*40,des_d_x*40);
        painter.drawLine(Line_12);
    }
    else if (win_method==23){//三三禁手
        int des_d_x,des_d_y;
        des_d_x=black.fm_posx;
        des_d_y=black.fm_posy;

        if (black.fm_direct==0 || black.fm_direct==2 || black.fm_direct==3)    des_d_y+=black.fm_case_length-1;
        if (black.fm_direct==1 || black.fm_direct==2 || black.fm_direct==3)    des_d_x+=black.fm_case_length-1;
        if (black.fm_direct==3)   des_d_y-=2*black.fm_case_length-2;

        QPen winpaint;
        winpaint.setWidth(10);
        winpaint.setColor(Qt::red);
        painter.setPen(winpaint);
        QLine Line_11(black.fm_posy*40,black.fm_posx*40,des_d_y*40,des_d_x*40);
        painter.drawLine(Line_11);

        des_d_x=black.fm_posx_II;
        des_d_y=black.fm_posy_II;

        if (black.fm_direct_II==0 || black.fm_direct_II==2 || black.fm_direct_II==3)    des_d_y+=black.fm_case_II_length-1;
        if (black.fm_direct_II==1 || black.fm_direct_II==2 || black.fm_direct_II==3)    des_d_x+=black.fm_case_II_length-1;
        if (black.fm_direct_II==3)   des_d_y-=2*black.fm_case_II_length-2;

        winpaint.setWidth(10);
        winpaint.setColor(Qt::red);
        painter.setPen(winpaint);
        QLine Line_12(black.fm_posy_II*40,black.fm_posx_II*40,des_d_y*40,des_d_x*40);
        painter.drawLine(Line_12);
    }

}


void Board::mouseMoveEvent(QMouseEvent *mouse_des){//鼠标追踪事件
    if (flag_gaming_double==1){
        int temp__1,temp__2;
        temp__1=row_z;
        temp__2=col_z;
        if (judge_if_can_drop(mouse_des->pos())){
            if (flag_sbzz==0 || row_z!=temp__1 || col_z!=temp__2){
                flag_sbzz=1;
                update();
            }
        }
        else{
            if (flag_sbzz){
                flag_sbzz=0;
                update();
            }
        }
    }

}


bool Board::judge_if_can_drop(QPoint loc){
    int distance_x,distance_y;
    QPoint des;
    for(int i=std::max(1,loc.y()/40);i<=std::min(map_y_temp,loc.y()/40+1);i++){
        for(int j=std::max(1,loc.x()/40);j<=std::min(map_x_temp,loc.x()/40+1);j++){
            des.rx()=j*40;
            des.ry()=i*40;
            distance_x=des.x()-loc.x();
            distance_y=des.y()-loc.y();
            if(distance_x*distance_x + distance_y*distance_y < (Chess_r_l-1)*(Chess_r_l-1)){
                row_z=i;
                col_z=j;
                return true;
            }
        }
    }
    return false;
}


void Board::mousePressEvent(QMouseEvent *ev){//鼠标左键点击时确认点击位置
    if (ev->button() != Qt::LeftButton){
        return;
    }
    QPoint loc= ev->pos();

    int row=-1,col=-1,distance_x,distance_y,temp_flag=0;
    QPoint des;
    for(int i=std::max(1,loc.y()/40);i<=std::min(map_y_temp,loc.y()/40+1);i++){
        for(int j=std::max(1,loc.x()/40);j<=std::min(map_x_temp,loc.x()/40+1);j++){
            des.rx()=j*40;
            des.ry()=i*40;
            distance_x=des.x()-loc.x();
            distance_y=des.y()-loc.y();
            if(distance_x*distance_x + distance_y*distance_y < (Chess_r_l-1)*(Chess_r_l-1)){
                row=i;
                col=j;
                temp_flag=1;
                break;
            }
        }
        if(temp_flag==1){
            break;
        }
    }

    //qDebug()<<"鼠标点击坐标位置： "<<row<<" "<<col;
    Chess_drop(row,col);
}


bool Board::Chess_drop(int row,int col){//在对应位置下子（含判断能否下子）
    if (row>=1 && row<=map_y_temp && col>=1 && col<=map_x_temp && map[row][col]==-1 && flag_gaming_double==1){
        flag_idea_paint=0;
        ui->read_save->setText("提示");
        ui->change_whofirst->hide();
        Chess_count++;
        map[row][col]=Chess_count%2;
        Chess_process[Chess_count][0]=row;
        Chess_process[Chess_count][1]=col;

        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out|std::ios::app);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
        else{

            //qDebug()<<"ERROR27 "<<flag_board_gaming_mode;
        }
        f<<Chess_process[Chess_count][0]<<" "<<Chess_process[Chess_count][1]<<std::endl;
        f.close();
        sound.stop();
        sound.setSource(QUrl("qrc:/music/ChessDrop.wav"));
        sound.play();
        //qDebug()<<"Flag3 "<<Chess_process[Chess_count][0]<<" "<<Chess_process[Chess_count][1];
        update();
        if (mode_blind_chess==1 && flag_gaming_double==1){
            QString Title="盲棋裁判";

            QString Info="您的";
            if (Chess_count%2==0){
                Info+="白棋";
            }
            else if (Chess_count%2==1){
                Info+="黑棋";
            }
            Info+="落子位置: "+QString::fromStdString(std::to_string(row))+" 行 "+QString::fromStdString(std::to_string(col))+" 列。";
            QMessageBox::warning(this,Title,Info);
        }
        if (winner_judging_double(Chess_count%2,Chess_process[Chess_count][0],Chess_process[Chess_count][1])!=-1){
            IfWin_double(winner_judging_double(Chess_count%2,Chess_process[Chess_count][0],Chess_process[Chess_count][1]));
        }
        //单人模式下机器自动落子
        if (flag_board_gaming_mode==0 && flag_gaming_double!=5){//flag_gaming_double检测作用为判断棋局时候已经结束
            ai_chess_drop(single_whofirst);
        }
        return true;
    }
    //盲棋模式如果落子位置有棋子给予提示
    else if (mode_blind_chess==1 && flag_gaming_double==1 && row>=1 && row<=map_y_temp && col>=1 && col<=map_x_temp && map[row][col]!=-1){
        QString Title="盲棋裁判";
        QString Info="此处已有棋子，不得落子，请重新思考！";
        QMessageBox::warning(this,Title,Info);
        return false;
    }
    else return false;
}


void Board::ai_chess_drop(int chess_color){//ai下棋

    ai_find_chess_drop_position(chess_color);
    //qDebug()<<"ai落子开始";
    if (ai_renshu==1 && Chess_count<map_x_temp*map_y_temp){
        //qDebug()<<"ai认输！";
        IfWin_double(4-chess_color);
        ai_renshu=0;
        return;
    }
    Chess_count++;
    map[ai_find_x][ai_find_y]=chess_color;
    Chess_process[Chess_count][0]=ai_find_x;
    Chess_process[Chess_count][1]=ai_find_y;

    if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out|std::ios::app);
    else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
    else{

        //qDebug()<<"ERROR41";
    }
    f<<Chess_process[Chess_count][0]<<" "<<Chess_process[Chess_count][1]<<std::endl;
    f.close();
    //qDebug()<<"Flag3 "<<Chess_process[Chess_count][0]<<" "<<Chess_process[Chess_count][1];
    update();

    sound.stop();
    sound.setSource(QUrl("qrc:/music/ChessDrop.wav"));
    sound.play();
    if (mode_blind_chess==1 && flag_gaming_double==1){
        QString Title="盲棋裁判";
        QString Info="AI的";
        if (Chess_count%2==0){
            Info+="白棋";
        }
        else if (Chess_count%2==1){
            Info+="黑棋";
        }
        Info+="落子位置: "+QString::fromStdString(std::to_string(ai_find_x))+" 行 "+QString::fromStdString(std::to_string(ai_find_y))+" 列。";
        QMessageBox::warning(this,Title,Info);
    }
    if (winner_judging_double(Chess_count%2,Chess_process[Chess_count][0],Chess_process[Chess_count][1])!=-1){
        IfWin_double(winner_judging_double(Chess_count%2,Chess_process[Chess_count][0],Chess_process[Chess_count][1]));
    }
}



void Board::IfWin_double(int winningflag){
    /*判断胜负
     * 0 1 2由winner_judging函数返回，分别表示白胜、黑胜、黑违反禁手规则
     * 3为白认输 4为黑认输 由按钮触发
     */
    int temp_flag_2=0;
    win_method=winningflag;
    if (winningflag==0){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==1){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="黑方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==2){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="黑方违反禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==21){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="黑方违反长连禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==22){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="黑方违反四四禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==23){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="黑方违反三三禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==3){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="黑方认输，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==4){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="白方认输，黑方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if(winningflag==5){
        flag_gaming_double=5;
        QString Title="提示";
        QString Info="平局！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    if (temp_flag_2==1){
        ui->read_save->setText("读取存档");
        ui->chess_undo->setText("重新开始");
        ui->chess_false->setText("复盘");
        ui->AutoBattle->setChecked(0);
        read_save_flag=0;
        f.open("flag_gaming.txt",std::ios::out);
        f<<"-1"<<std::endl;
        f.close();
        //qDebug()<<"写入对局状态0";


        write_in_double(winningflag,1);
        update();
    }
}


std::string game_name;

void Board::write_in_double(int winningflag,bool iffinished){

    if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::in);
    else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::in);
    else{
        //qDebug()<<"ERROR28";
    }
    std::getline(f,game_name);
    f.close();
    //qDebug()<<game_name;
    int temp_4;
    temp_4=game_name.length();
    game_name.erase(temp_4,1);
    //qDebug()<<game_name<<" "<<temp_4;
    for (int i=0;i<temp_4;i++){//去除windows文件名中不合法的":"
        if (game_name[i]==':'){
            game_name[i]='_';
        }
    }
    //qDebug()<<game_name;
    if (iffinished){//结束保存历史记录
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out|std::ios::app);
        else if (flag_board_gaming_mode==0) f.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
        else{
            //qDebug()<<"ERROR29";
        }
        f<<"-114"<<" "<<"-514"<<std::endl;//-114 -514表示对局结束，停止读取下棋记录
        f<<winningflag<<std::endl;
        if (winningflag==0){//白连五
            f<<white.win_posx<<" "<<white.win_posy<<" "<<white.win_direct<<" "<<"5"<<std::endl;
            f<<"-1"<<" "<<"-1"<<" "<<"-1"<<" "<<"-1"<<std::endl;
        }
        else if (winningflag==1){//黑连五
            f<<black.win_posx<<" "<<black.win_posy<<" "<<black.win_direct<<" "<<"5"<<std::endl;
            f<<"-1"<<" "<<"-1"<<" "<<"-1"<<" "<<"-1"<<std::endl;
        }
        else if (winningflag==21){//长连禁手
            f<<black.fm_posx<<" "<<black.fm_posy<<" "<<black.fm_direct<<" "<<black.fm_case_length<<std::endl;
            f<<"-1"<<" "<<"-1"<<" "<<"-1"<<" "<<"-1"<<std::endl;
        }
        else if (winningflag==22 || winningflag==23){//四四和三三禁手
            f<<black.fm_posx<<" "<<black.fm_posy<<" "<<black.fm_direct<<" "<<black.fm_case_length<<std::endl;
            f<<black.fm_posx_II<<" "<<black.fm_posy_II<<" "<<black.fm_direct_II<<" "<<black.fm_case_II_length<<std::endl;
        }
        else{
            f<<"-1"<<" "<<"-1"<<" "<<"-1"<<" "<<"-1"<<std::endl;
            f<<"-1"<<" "<<"-1"<<" "<<"-1"<<" "<<"-1"<<std::endl;
            //qDebug()<<"ERROR114-1 黑白棋胜负类型未找到 未写入禁手点序列（以空白占位替代）";
        }
        f.close();

        if (flag_board_gaming_mode==1){
            std::filesystem::copy("GameRecord_temp_1.txt","history/"+game_name+".txt");
        }
        else if (flag_board_gaming_mode==0){
            std::filesystem::copy("GameRecord_temp_0.txt","history/"+game_name+".txt");
        }
        else{
            //qDebug()<<"ERROR30";
        }
        update();
        QPixmap board_grab=this->grab(QRect(20,20,map_x_temp*40-3,map_y_temp*40-3));
        board_grab.save(QString::fromStdString("history/"+game_name+".png"));
    }
    else{//未结束存档
        mode_blind_chess=0;
        update();
        QPixmap board_grab=this->grab(QRect(20,20,map_x_temp*40-3,map_y_temp*40-3));
        if (flag_board_gaming_mode==1){
            std::filesystem::copy("GameRecord_temp_1.txt","history/unfinished/double/"+game_name+".txt");
            board_grab.save(QString::fromStdString("history/unfinished/double/"+game_name+".png"));
        }
        else if (flag_board_gaming_mode==0){
            std::filesystem::copy("GameRecord_temp_0.txt","history/unfinished/single/"+game_name+".txt");
            board_grab.save(QString::fromStdString("history/unfinished/single/"+game_name+".png"));
        }
        else {
            //qDebug()<<"ERROR31";
        }
    }
}

void Board::on_chess_undo_clicked()//悔棋操作和重新开始操作
{
    QSoundEffect sound1;
    sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
    sound1.play();

    if (flag_gaming_double==1){//悔棋操作
        QMessageBox choose(QMessageBox::Question,QString(tr("提示")),QString(tr("古人云 “落子无悔”  \n您确定要悔棋吗？")),QMessageBox::Yes| QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("悔棋"));
        choose.button(QMessageBox::No)->setText(tr("取消"));
        int temp_15=choose.exec();
        if (temp_15 == QMessageBox::Yes){
            if (flag_board_gaming_mode==1){
                if (Chess_count<1){
                    QString Title="悔棋失败";
                    QString Info="目前棋盘上没有你的棋子，无法进行悔棋！";
                    QMessageBox::warning(this,Title,Info);
                }
                else{
                    map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=-1;
                    Chess_process[Chess_count][0]=0;
                    Chess_process[Chess_count][1]=0;
                    Chess_count--;
                    f.open("GameRecord_temp_1.txt",std::ios::out|std::ios::app);
                    f<<"-20 -20"<<std::endl;//临时文件中-20 -20表示悔棋
                    f.close();
                    flag_idea_paint=0;
                    update();
                }
            }
            else if (flag_board_gaming_mode==0){
                if (Chess_count<single_whofirst+1){
                    QString Title="悔棋失败";
                    QString Info="目前棋盘上没有你的棋子，无法进行悔棋！";
                    QMessageBox::warning(this,Title,Info);
                }
                else{
                    map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=-1;
                    Chess_process[Chess_count][0]=0;
                    Chess_process[Chess_count][1]=0;
                    Chess_count--;
                    map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=-1;
                    Chess_process[Chess_count][0]=0;
                    Chess_process[Chess_count][1]=0;
                    Chess_count--;
                    f.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
                    f<<"-20 -20"<<std::endl;//临时文件中-20 -20表示悔棋
                    f<<"-20 -20"<<std::endl;
                    f.close();
                    flag_idea_paint=0;
                    update();
                }
            }
        }
    }
    else if(flag_gaming_double==0 || flag_gaming_double==5){//重新开始操作

        ad_mode=0;


        ui->AutoBattle->setChecked(0);

        for (int i=0;i<21;i++){
            for (int j=0;j<21;j++){
                map[i][j]=-1;
            }
        }

        for (int i=0;i<400;i++){
            Chess_process[i][0]=0;
            Chess_process[i][1]=0;
        }
        Chess_count=0;
        flag_gaming_double=1;
        win_method=-1;
        f.open("flag_gaming.txt",std::ios::out);
        f<<flag_board_gaming_mode<<std::endl;
        f.close();
        //qDebug()<<"写入对局状态";



        //写入临时保存棋局的开头两行
        time_t chess_start_time;
        time(&chess_start_time);
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out);
        //elseqDebug()<<"ERROR33";
        f<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
        if (flag_board_gaming_mode==0)  f<<flag_board_gaming_mode+single_whofirst+20<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else if (flag_board_gaming_mode==1)  f<<flag_board_gaming_mode<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        //elseqDebug()<<"ERROR59";
        f<<map_x_temp<<" "<<map_y_temp<<std::endl;
        f.close();
        if(single_whofirst==1){
            Chess_count=1;
            Chess_process[Chess_count][0]=(map_y_temp+1)/2;
            Chess_process[Chess_count][1]=(map_x_temp+1)/2;
            map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=1;
            update();

            //重新写入文档
            time_t chess_start_time;
            time(&chess_start_time);
            if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out);
            else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out);
            //elseqDebug()<<"ERROR20";
            f<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
            if (flag_board_gaming_mode==0)  f<<flag_board_gaming_mode+single_whofirst+20<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
            else if (flag_board_gaming_mode==1)  f<<flag_board_gaming_mode<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
            //elseqDebug()<<"ERROR502";
            f<<map_x_temp<<" "<<map_y_temp<<std::endl;//棋盘大小
            f<<Chess_process[Chess_count][0]<<" "<<Chess_process[Chess_count][1]<<std::endl;
            f.close();
        }
        ui->chess_undo->setText("悔棋");
        ui->chess_false->setText("认输");
        if (flag_board_gaming_mode==0)  ui->change_whofirst->show();
        update();
    }
}


void Board::on_chess_false_clicked()//认输和复盘
{
    QSoundEffect sound1;
    sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
    sound1.play();

    if (flag_gaming_double==1){//认输
        QMessageBox choose(QMessageBox::Question,QString(tr("提示")),QString(tr("您确定要认输吗？")),QMessageBox::Yes| QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("认输"));
        choose.button(QMessageBox::No)->setText(tr("取消"));
        int temp_15=choose.exec();
        if (temp_15 == QMessageBox::Yes){
            flag_idea_paint=0;
            update();
            IfWin_double(Chess_count%2+3);

        }


    }
    else if (flag_gaming_double==0 || flag_gaming_double==5){//复盘
        if (flag_review==0){
            review_file_name=game_name;
            Board_Review *Board_ReviewWindow = new Board_Review;
            Board_ReviewWindow->setAttribute(Qt::WA_QuitOnClose,false);
            Board_ReviewWindow->show();
        }
        else{
            QString Title="警告";
            QString Info="当前已有一局打开的历史记录，请先关闭已打开的历史记录！";
            QMessageBox::warning(this,Title,Info);
        }
    }
}


void Board::on_save_and_exit_clicked()//存盘操作
{

        flag_close_save=1;
        close();
}



void Board::on_read_save_clicked()//读取存档和提示
{

    if (((Chess_count>0 && (single_whofirst==0 || read_save_flag==1))|| (Chess_count>1 && single_whofirst==1)) && flag_gaming_double==1 ){
            if (mode_blind_chess==1){
                QSoundEffect sound1;
                sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
                sound1.play();
                QString Title="盲棋裁判";
                QString Info="盲棋模式不得使用提示！";
                QMessageBox::warning(this,Title,Info);
                return ;
            }
            //qDebug()<<"给予提示！";
            ai_idea_show((Chess_count+1)%2);
        }
        else{

            flag_gaming_double=5;
            flag_read_save=flag_board_gaming_mode;
            flag_esapce_closechange=1;
            close();
            //qDebug()<<flag_board_gaming_mode<<" "<<flag_read_save<<" "<<savepath_double;
            Read_save_select *ReadselectWindow = new Read_save_select;
            ReadselectWindow->setAttribute(Qt::WA_QuitOnClose,false);
            ReadselectWindow->show();
        }

}


void Board::ai_idea_show(int chess_color){
    if (flag_idea_paint==0){

        int temp_mhx_tql;
        temp_mhx_tql=ai_mode;
        ai_mode=0;
        ai_find_chess_drop_position(chess_color);
        ai_mode=temp_mhx_tql;
        if (ai_renshu==0){
            flag_idea_paint=1;
            flag_idea_paint_chess_color=chess_color;
            update();
        }
        else if (ai_renshu==1){
            QString Title="提示";
            QString Info="你已经没有地方可以下棋了！赶快认输吧！";
            QMessageBox::warning(this,Title,Info);
        }
        else{
            //qDebug()<<"ERROR9990";
        }
    }
    else{
        QString Title="提示";
        QString Info="落子提示已经给出！";
        QMessageBox::warning(this,Title,Info);
    }

}



//-------------------------------所有初始化函数和其他功能函数---------------------------------
void preaction_map_copy() {//将预处理棋盘设定成当前棋盘
    for (int i = 0; i < 19 + 2; i++) for(int j=0;j<19+2;j++) {
            preaction_map[i][j]=2;
        }
    for (int i = 0; i < map_y_temp + 2; i++) {
        for (int j = 0; j < map_x_temp + 2; j++) {
            preaction_map[i][j] = map[i][j];
        }
    }
}

void fm_point_default() {
    for (int i = 0; i < 400; i++) {
        fm_point[i][0] = 0;
        fm_point[i][1] = 0;
    }
}

void Board::map_default() {//初始化棋盘
    for (int i = 0; i < 19 + 2; i++) for(int j=0;j<19+2;j++) {
        map[i][j]=2;
    }
    for (int i = 1; i <= map_y_temp; i++) {
        for (int j = 1; j <= map_x_temp; j++) {
            map[i][j] = -1;
        }
    }
    preaction_map_copy();
}

void player_default() {//初始化玩家信息
    white = player_white{ 0, -1, -1, -1, win[0], 5 };
    black = player_black{ 0,-1,-1,-1,win[1],5,"NULL",-1,-1,-1,null,1,-1,-1,-1,null,1 };
}

//-----------------------------------------所有禁手相关函数-------------------------------------------
bool find(const int cs[],int length,int direct,int posx,int posy){
    /*
    function:在包含指定点(pos)的所有指定方向(direct)的序列里，检测是否出现指定序列(cs)，顺序为行坐标增加方向
    参数：序列，序列长度，检测方向(0=水平，1=竖直，2=左上-右下，3=右上-左下)，点坐标
    以下墙也算作一种“棋子”
    以下统一i,x为行坐标，j,y为列坐标，即board[i(x)][j(y)]
    */
    if (direct == 0) {//行检测
        for (int j = std::max(posy - length + 1, 0); j <= std::min(posy , 19 + 2 - length); j++) {
            for (int k = 0; k < length; k++) {
                if (!match[map[posx][j + k]+1][cs[k]+1]) break;
                if (k == length - 1) {
                    tempinfo[0] = 0;
                    tempinfo[1] = posx;
                    tempinfo[2] = j;
                    return true;
                }
            }
        }
    }
    else if (direct == 1) {//列检测
        for (int i = std::max(posx - length + 1, 0); i <= std::min(posx, 19 + 2 - length); i++) {
            for (int k = 0; k < length; k++) {
                if (!match[map[i + k][posy]+1][cs[k]+1]) break;
                if (k == length - 1) {
                    tempinfo[0] = 1;
                    tempinfo[1] = i;
                    tempinfo[2] = posy;
                    return true;
                }
            }
        }
    }
    else if (direct == 2) {//斜对角线检测
        //定义对角线序数s=x-y,可得对角线上棋子个数为boardlen-|s|
        int s=posx-posy;
        if(19+2-std::max(-s,s)>=length){
            for (int m = std::max(0,std::max(s,posx+1-length)); m <= std::min(19+2-length,std::min(19+2-length+s,posx)); m++) {
                for (int k = 0; k < length; k++) {
                    int i = m + k;
                    int j = i - s;
                    if (!match[map[i][j]+1][cs[k]+1]) break;
                    if (k == length - 1) {
                        tempinfo[0] = 2;
                        tempinfo[1] = m;
                        tempinfo[2] = m-s;
                        return true;
                    }
                }
            }
        }
    }
    else if (direct == 3) {//反对角线检测
        //定义对角线序数s=x+y,可得对角线上棋子个数为boardlen+2-|s-boardlen-1|
        int s = posx + posy;
        if(19+2-std::max(s-19-1,19+1-s)>=length){
            for (int m = std::max(0,std::max(s-19-1, posx+1-length)); m <= std::min(19+2-length,std::min(s-length+1, posx)); m++) {
                for (int k = 0; k < length; k++) {
                    int i = m + k;
                    int j = s - i;
                    if (!match[map[i][j]+1][cs[k]+1]) break;
                    if (k == length - 1) {
                        tempinfo[0] = 3;
                        tempinfo[1] = m;
                        tempinfo[2] = s-m;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool p_find(const int cs[], int length, int direct, int posx, int posy) {//作用于preaction_board的find函数
    if (direct == 0) {//行检测
        for (int j = std::max(posy - length + 1, 0); j <= std::min(posy, 19 + 2 - length); j++) {
            for (int k = 0; k < length; k++) {
                if (!match[preaction_map[posx][j + k] + 1][cs[k] + 1]) break;
                if (k == length - 1) {
                    return true;
                }
            }
        }
    }
    else if (direct == 1) {//列检测
        for (int i = std::max(posx - length + 1, 0); i <= std::min(posx, 19 + 2 - length); i++) {
            for (int k = 0; k < length; k++) {
                if (!match[preaction_map[i + k][posy] + 1][cs[k] + 1]) break;
                if (k == length - 1) {
                    return true;
                }
            }
        }
    }
    else if (direct == 2) {//斜对角线检测
        //定义对角线序数s=x-y,可得对角线上棋子个数为boardlen-|s|
        int s = posx - posy;
        if (19 + 2 - std::max(-s, s) >= length) {
            for (int m = std::max(0, std::max(s, posx + 1 - length)); m <= std::min(19 + 2 - length, std::min(19 + 2 - length + s, posx)); m++) {
                for (int k = 0; k < length; k++) {
                    int i = m + k;
                    int j = i - s;
                    if (!match[preaction_map[i][j] + 1][cs[k] + 1]) break;
                    if (k == length - 1) {
                        return true;
                    }
                }
            }
        }
    }
    else if (direct == 3) {//反对角线检测
        //定义对角线序数s=x+y,可得对角线上棋子个数为boardlen+2-|s-boardlen-1|
        int s = posx + posy;
        if(19+2 - std::max(s - 19 - 1, 19 + 1 - s) >= length) {
            for (int m = std::max(0, std::max(s - 19 - 1, posx + 1 - length)); m <= std::min(19 + 2 - length, std::min(s - length + 1, posx)); m++) {
                for (int k = 0; k < length; k++) {
                    int i = m + k;
                    int j = s - i;
                    if (!match[preaction_map[i][j] + 1][cs[k] + 1]) break;
                    if (k == length - 1) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool LC(int posx,int posy) { //function:检测是否满足长连禁手,同时记录长连禁手信息并修改玩家状态
    for (int direct = 0; direct <= 3; direct++)
        if (find(lc, 6, direct, posx, posy)) {
            black.state = 2;
            black.fm_type = "LC";
            black.fm_direct = tempinfo[0];
            black.fm_posx = tempinfo[1];
            black.fm_posy = tempinfo[2];
            black.fm_case = lc;
            black.fm_case_length = 6;
            return true;
        }
    return false;
}

bool p_LC(int posx, int posy) { //作用于preaction_board的LC函数
    for (int direct = 0; direct <= 3; direct++)
        if (p_find(lc, 6, direct, posx, posy)) {
            return true;
        }
    return false;
}

bool player_win(int player, int posx, int posy) { //function:检测对应玩家落子后是否胜利,同时记录获胜信息并修改玩家状态
    //应先检测是否满足长连禁手
    for (int direct = 0; direct <= 3; direct++) {
        if (find(win[player], 5, direct, posx, posy)) {
            if (player == 0) {
                white.state = 1;
                white.win_direct = tempinfo[0];
                white.win_posx = tempinfo[1];
                white.win_posy = tempinfo[2];
            }
            else {
                black.state = 1;
                black.win_direct = tempinfo[0];
                black.win_posx = tempinfo[1];
                black.win_posy = tempinfo[2];
            }
            return true;
        }
    }
    return false;
}

bool p_black_win(int posx, int posy) { //作用于preaction_board的对黑子的player_win函数
    //应先检测是否满足长连禁手
    for (int direct = 0; direct <= 3; direct++) {
        if (p_find(win[1], 5, direct, posx, posy)) {
            return true;
        }
    }
    return false;
}

bool FF(int posx, int posy) { //function:检测是否满足四四禁手,同时记录四四禁手信息并修改玩家状态
    int fc_num = 0;
    for (int direct = 0; direct <= 3; direct++) {
        for (int i = 0; i < 2; i++) {
            if (find(fc_I[i], 6, direct, posx, posy)) {
                fc_num++;
                if (fc_num == 1) {
                    black.fm_direct = tempinfo[0];
                    if (i == 0) {
                        if (direct == 0) {
                            black.fm_posx = tempinfo[1];
                            black.fm_posy = tempinfo[2] + 2;
                        }
                        else if (direct == 1) {
                            black.fm_posx = tempinfo[1] + 2;
                            black.fm_posy = tempinfo[2];
                        }
                        else if (direct == 2) {
                            black.fm_posx = tempinfo[1] + 2;
                            black.fm_posy = tempinfo[2] + 2;
                        }
                        else if (direct == 3) {
                            black.fm_posx = tempinfo[1] + 2;
                            black.fm_posy = tempinfo[2] - 2;
                        }
                    }
                    else if (i == 1) {
                        if (direct == 0) {
                            black.fm_posx = tempinfo[1];
                            black.fm_posy = tempinfo[2];
                        }
                        else if (direct == 1) {
                            black.fm_posx = tempinfo[1];
                            black.fm_posy = tempinfo[2];
                        }
                        else if (direct == 2) {
                            black.fm_posx = tempinfo[1];
                            black.fm_posy = tempinfo[2];
                        }
                        else if (direct == 3) {
                            black.fm_posx = tempinfo[1];
                            black.fm_posy = tempinfo[2];
                        }
                    }
                    black.fm_case = fc_I_origin;
                    black.fm_case_length = 4;
                }
                if (fc_num >= 2) {
                    black.state = 2;
                    black.fm_type = "FF";
                    black.fm_direct_II = tempinfo[0];
                    if (i == 0) {
                        if (direct == 0) {
                            black.fm_posx_II = tempinfo[1];
                            black.fm_posy_II = tempinfo[2] + 2;
                        }
                        else if (direct == 1) {
                            black.fm_posx_II = tempinfo[1] + 2;
                            black.fm_posy_II = tempinfo[2];
                        }
                        else if (direct == 2) {
                            black.fm_posx_II = tempinfo[1] + 2;
                            black.fm_posy_II = tempinfo[2] + 2;
                        }
                        else if (direct == 3) {
                            black.fm_posx_II = tempinfo[1] + 2;
                            black.fm_posy_II = tempinfo[2] - 2;
                        }
                    }
                    else if(i == 1){
                        if (direct == 0) {
                            black.fm_posx_II = tempinfo[1];
                            black.fm_posy_II = tempinfo[2];
                        }
                        else if (direct == 1) {
                            black.fm_posx_II = tempinfo[1];
                            black.fm_posy_II = tempinfo[2];
                        }
                        else if (direct == 2) {
                            black.fm_posx_II = tempinfo[1];
                            black.fm_posy_II = tempinfo[2];
                        }
                        else if (direct == 3) {
                            black.fm_posx_II = tempinfo[1];
                            black.fm_posy_II = tempinfo[2];
                        }
                    }
                    black.fm_case_II = fc_I_origin;
                    black.fm_case_II_length = 4;
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (find(fc_II[i], 7, direct, posx, posy)) {
                fc_num++;
                if (fc_num == 1) {
                    black.fm_direct = tempinfo[0];
                    if (direct == 0) {
                        black.fm_posx = tempinfo[1];
                        black.fm_posy = tempinfo[2] + 1;
                    }
                    else if (direct == 1) {
                        black.fm_posx = tempinfo[1] + 1;
                        black.fm_posy = tempinfo[2];
                    }
                    else if (direct == 2) {
                        black.fm_posx = tempinfo[1] + 1;
                        black.fm_posy = tempinfo[2] + 1;
                    }
                    else if (direct == 3) {
                        black.fm_posx = tempinfo[1] + 1;
                        black.fm_posy = tempinfo[2] - 1;
                    }
                    black.fm_case = fc_II_origin[i];
                    black.fm_case_length = 5;
                }
                if (fc_num >= 2) {
                    black.state = 2;
                    black.fm_type = "FF";
                    black.fm_direct_II = tempinfo[0];
                    if (direct == 0) {
                        black.fm_posx_II = tempinfo[1];
                        black.fm_posy_II = tempinfo[2] + 1;
                    }
                    else if (direct == 1) {
                        black.fm_posx_II = tempinfo[1] + 1;
                        black.fm_posy_II = tempinfo[2];
                    }
                    else if (direct == 2) {
                        black.fm_posx_II = tempinfo[1] + 1;
                        black.fm_posy_II = tempinfo[2] + 1;
                    }
                    else if (direct == 3) {
                        black.fm_posx_II = tempinfo[1] + 1;
                        black.fm_posy_II = tempinfo[2] - 1;
                    }
                    black.fm_case_II = fc_II_origin[i];
                    black.fm_case_II_length = 5;
                    return true;
                }
            }
        }
    }
    return false;
}

bool p_FF(int posx, int posy) { //作用于preaction_board的FF函数
    int fc_num = 0;
    for (int direct = 0; direct <= 3; direct++) {
        for (int i = 0; i < 2; i++) {
            if (p_find(fc_I[i], 6, direct, posx, posy)) {
                fc_num++;
                if (fc_num >= 2) {
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (p_find(fc_II[i], 7, direct, posx, posy)) {
                fc_num++;
                if (fc_num >= 2) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool TT(int posx, int posy) { //function:检测是否满足三三禁手,同时记录三三禁手信息并修改玩家状态
    int tc_num = 0;
    for (int direct = 0; direct <= 3; direct++) {
        for (int i = 0; i < 2; i++) {
            if (find(tc_I[i], 7, direct, posx, posy)) {
                tc_num++;
                if (tc_num == 1) {
                    black.fm_direct = tempinfo[0];
                    if (direct == 0) {
                        black.fm_posx = tempinfo[1];
                        black.fm_posy = tempinfo[2] + 2;
                    }
                    else if (direct == 1) {
                        black.fm_posx = tempinfo[1] + 2;
                        black.fm_posy = tempinfo[2];
                    }
                    else if (direct == 2) {
                        black.fm_posx = tempinfo[1]+2;
                        black.fm_posy = tempinfo[2]+2;
                    }
                    else if (direct == 3) {
                        black.fm_posx = tempinfo[1]+2;
                        black.fm_posy = tempinfo[2]-2;
                    }
                    black.fm_case = tc_I_origin;
                    black.fm_case_length = 3;
                }
                if (tc_num >= 2) {
                    black.state = 2;
                    black.fm_type = "TT";
                    black.fm_direct_II = tempinfo[0];
                    if (direct == 0) {
                        black.fm_posx_II = tempinfo[1];
                        black.fm_posy_II = tempinfo[2] + 2;
                    }
                    else if (direct == 1) {
                        black.fm_posx_II = tempinfo[1] + 2;
                        black.fm_posy_II = tempinfo[2];
                    }
                    else if (direct == 2) {
                        black.fm_posx_II = tempinfo[1] + 2;
                        black.fm_posy_II = tempinfo[2] + 2;
                    }
                    else if (direct == 3) {
                        black.fm_posx_II = tempinfo[1] + 2;
                        black.fm_posy_II = tempinfo[2] - 2;
                    }
                    black.fm_case_II = tc_I_origin;
                    black.fm_case_II_length = 3;
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (find(tc_II[i], 6, direct, posx, posy)) {
                tc_num++;
                if (tc_num == 1) {
                    black.fm_direct = tempinfo[0];
                    if (direct == 0) {
                        black.fm_posx = tempinfo[1];
                        black.fm_posy = tempinfo[2] + 1;
                    }
                    else if (direct == 1) {
                        black.fm_posx = tempinfo[1] + 1;
                        black.fm_posy = tempinfo[2];
                    }
                    else if (direct == 2) {
                        black.fm_posx = tempinfo[1] + 1;
                        black.fm_posy = tempinfo[2] + 1;
                    }
                    else if (direct == 3) {
                        black.fm_posx = tempinfo[1] + 1;
                        black.fm_posy = tempinfo[2] - 1;
                    }
                    black.fm_case = tc_II_origin[i];
                    black.fm_case_length = 4;
                }
                if (tc_num >= 2) {
                    black.state = 2;
                    black.fm_type = "TT";
                    black.fm_direct_II = tempinfo[0];
                    if (direct == 0) {
                        black.fm_posx_II = tempinfo[1];
                        black.fm_posy_II = tempinfo[2] + 1;
                    }
                    else if (direct == 1) {
                        black.fm_posx_II = tempinfo[1] + 1;
                        black.fm_posy_II = tempinfo[2];
                    }
                    else if (direct == 2) {
                        black.fm_posx_II = tempinfo[1] + 1;
                        black.fm_posy_II = tempinfo[2] + 1;
                    }
                    else if (direct == 3) {
                        black.fm_posx_II = tempinfo[1] + 1;
                        black.fm_posy_II = tempinfo[2] - 1;
                    }
                    black.fm_case_II = tc_II_origin[i];
                    black.fm_case_II_length = 4;
                    return true;
                }
            }
        }
    }
    return false;
}

bool p_TT(int posx, int posy) { //作用于preaction_board的TT函数
    int tc_num = 0;
    for (int direct = 0; direct <= 3; direct++) {
        for (int i = 0; i < 2; i++) {
            if (p_find(tc_I[i], 7, direct, posx, posy)) {
                tc_num++;
                if (tc_num >= 2) {
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (p_find(tc_II[i], 6, direct, posx, posy)) {
                tc_num++;
                if (tc_num >= 2) {
                    return true;
                }
            }
        }
    }
    return false;
}

int event_get_after_action(int player, int posx, int posy) {//玩家落子后，检测发生的事件
    //返回值为0：未获胜；返回值为1：对应玩家获胜；返回值为2：黑方违反禁手规则；返回值为-1：发生异常错误（玩家不是0/1）
    if (player == 0) {
        if (player_win(0, posx, posy)) {
            return 1;
        }
        return 0;
    }
    else if (player == 1) {
        if (ruleLC) {
            if (LC(posx, posy)) {
                return 21;
            }
        }
        if (player_win(1, posx, posy)) {
            return 1;
        }
        if (ruleFF) {
            if (FF(posx, posy)) {
                return 22;
            }
        }
        if (ruleTT){
            if (TT(posx, posy)) {
                return 23;
            }
        }
        return 0;
    }
    return -1;
}

void get_fm_point() {
    preaction_map_copy();
    int count = 0;
    bool flag = true;
    for (int i = 1; i <= map_y_temp; i++) {
        for (int j = 1; j <= map_x_temp; j++) {
            if (preaction_map[i][j] == -1) {
                preaction_map[i][j] = 1;
                flag = true;
                if (ruleLC&&flag) {
                    if (p_LC(i, j)) {
                        fm_point[count][0] = i;
                        fm_point[count][1] = j;
                        fm_point_type[count] = "LC";
                        count++;
                        flag = false;
                    }
                }
                if (p_black_win(i, j))flag = false;
                if (ruleFF&&flag) {
                    if (p_FF(i, j)) {
                        fm_point[count][0] = i;
                        fm_point[count][1] = j;
                        fm_point_type[count] = "FF";
                        count++;
                        flag = false;
                    }
                }
                if (ruleTT&&flag) {
                    if (p_TT(i, j)) {
                        fm_point[count][0] = i;
                        fm_point[count][1] = j;
                        fm_point_type[count] = "TT";
                        count++;
                        flag = false;
                    }
                }
                preaction_map[i][j] = -1;
            }
        }
    }
    fm_point_length = count;
}

int mustwin_point[100][3];


bool p_FT(int row,int col){//预检测四三序列
    int tc_num=0,fc_num=0;
    for (int direct=0;direct<=3;direct++){
        for (int i=0;i<2;i++){//活三检测
            if (p_find(tc_I[i],7,direct,row,col)){
                tc_num++;
                if (tc_num>=1 && fc_num>=1) {
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (p_find(tc_II[i], 6, direct, row, col)) {
                tc_num++;
                if (tc_num>=1 && fc_num>=1) {
                    return true;
                }
            }
        }

        for (int i = 0; i < 2; i++) {//活四检测
            if (p_find(fc_I[i], 6, direct, row, col)) {
                fc_num++;
                if (tc_num>=1 && fc_num>=1) {
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (p_find(fc_II[i], 7, direct, row, col)) {
                fc_num++;
                if (tc_num>=1 && fc_num>=1) {
                    return true;
                }
            }
        }
    }
    return false;
}


bool p_F(int row,int col){//预检测活四序列
    for (int direct=0;direct<=3;direct++){
        for (int i = 0; i < 2; i++) {
            if (p_find(fc_I[i], 6, direct, row, col)) {
                return true;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (p_find(fc_II[i], 7, direct, row, col)) {
                    return true;
            }
        }
    }
    return false;
}


int must_win_judge(bool is_player_my,int count){//此处只能检测黑棋，如为白棋则在must_win_get中已转置
    for (int i = 1; i <= map_y_temp; i++) {
        for (int j = 1; j <= map_x_temp; j++) {
            if (preaction_map[i][j] == -1) {
                preaction_map[i][j] = 1;
                if (player_win(1,i,j)){//直接获胜
                    mustwin_point[count][0]=i;
                    mustwin_point[count][1]=j;
                    if (is_player_my==0)  mustwin_point[count][2]=-5;
                    else if (is_player_my==1)   mustwin_point[count][2]=5;
                    count++;
                }
                else if (p_FF(i, j)){//双活四
                    mustwin_point[count][0]=i;
                    mustwin_point[count][1]=j;
                    if (is_player_my==0)  mustwin_point[count][2]=-4;
                    else if (is_player_my==1)   mustwin_point[count][2]=4;
                    count++;
                }
                else if (p_FT(i,j)){//四三
                    mustwin_point[count][0]=i;
                    mustwin_point[count][1]=j;
                    if (is_player_my==0)  mustwin_point[count][2]=-3;
                    else if (is_player_my==1)   mustwin_point[count][2]=3;
                    count++;
                }
                else if(p_F(i,j)){//四（此处无法区分是否为两端活四，后续在ai文件中进行进一步辨别）
                    mustwin_point[count][0]=i;
                    mustwin_point[count][1]=j;
                    if (is_player_my==0)  mustwin_point[count][2]=-2;
                    else if (is_player_my==1)   mustwin_point[count][2]=2;
                    count++;
                }
                else if (p_TT(i,j)){//双三
                    mustwin_point[count][0]=i;
                    mustwin_point[count][1]=j;
                    if (is_player_my==0)  mustwin_point[count][2]=-1;
                    else if (is_player_my==1)   mustwin_point[count][2]=1;
                    count++;
                }
                preaction_map[i][j] = -1;
            }
        }
    }
    return count;
}


//AI-必胜序列预检测
int must_win_get(){//四四、四三、三三
    /* 数组中第三位元素表示状态，如下
     * （己方为正，对方为负）
     * 直接获胜返回值为5
     * 四四返回值为4
     * 四三返回值为3
     * 四个返回值为2
     * 函数返回值为表示有多少个必胜序列
     * 三三返回值为1*/
    preaction_map_copy();
    int count=0;
    int my;
    my=(Chess_count+1)%2;
    if (my==1){
        count=must_win_judge(1,count);
        for (int i=1;i<=map_y_temp;i++) {
            for (int j=1;j<=map_x_temp;j++) {
                if (preaction_map[i][j]!=-1){
                    preaction_map[i][j]=1-preaction_map[i][j];
                }
            }
        }
        count=must_win_judge(0,count);
    }
    else if (my==0){
        count=must_win_judge(0,count);
        for (int i=1;i<=map_y_temp;i++) {
            for (int j=1;j<=map_x_temp;j++) {
                if (preaction_map[i][j]!=-1){
                    preaction_map[i][j]=1-preaction_map[i][j];
                }
            }
        }
        count=must_win_judge(1,count);
    }
    else    return -1;
    return count;
}



int Board::winner_judging_double(int player, int posx, int posy){//输赢+禁手判断函数
    //map_default();
    ////qDebug()<<"输赢检测debug"<<Chess_count<<" "<<map_x_temp*map_y_temp;
    int temp_12;
    temp_12=event_get_after_action(player,posx,posy);
    //get_fm_point();
    //for(int i=0;i<fm_point_length;i++){
        //qDebug()<<'('<<fm_point[i][0]<<','<<fm_point[i][1]<<')'<<' '<<fm_point_type[i];
    //}
    //qDebug()<<"sum="<<fm_point_length;

    if (temp_12==1)    return player;
    else if (temp_12==2 || temp_12==21 || temp_12==22 || temp_12==23)    return temp_12;
    else if (Chess_count >= map_x_temp*map_y_temp)   return 5;
    else if (temp_12==0)    return -1;
    else {
        //qDebug()<<"ERROR #11";
        return -1;
    }

}



void Board::on_change_whofirst_clicked()//切换先后手
{
    QSoundEffect sound1;
    sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
    sound1.play();

    if (Chess_count!=0){
        map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=-1;
        Chess_process[Chess_count][0]=0;
        Chess_process[Chess_count][1]=0;
        Chess_count=0;
        update();
    }
    single_whofirst=(single_whofirst+1)%2;
    if (single_whofirst==0){
        ui->gamemode->setText("人机对弈(玩家执黑)");

        //重新写入文档
        time_t chess_start_time;
        time(&chess_start_time);
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out);
        //else qDebug()<<"ERROR20";
        f<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
        if (flag_board_gaming_mode==0)  f<<flag_board_gaming_mode+single_whofirst+20<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else if (flag_board_gaming_mode==1)  f<<flag_board_gaming_mode<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        //else qDebug()<<"ERROR502";
        f<<map_x_temp<<" "<<map_y_temp<<std::endl;//棋盘大小
        f.close();
    }
    else if (single_whofirst==1){
        ui->gamemode->setText("人机对弈(玩家执白)");
        if (Chess_count!=0){
            //qDebug()<<"ERROR501";
        }

        sound.stop();
        sound.setSource(QUrl("qrc:/music/ChessDrop.wav"));
        sound.play();
        Chess_count=1;
        Chess_process[Chess_count][0]=(map_y_temp+1)/2;
        Chess_process[Chess_count][1]=(map_x_temp+1)/2;
        map[Chess_process[Chess_count][0]][Chess_process[Chess_count][1]]=1;
        update();

        //重新写入文档
        time_t chess_start_time;
        time(&chess_start_time);
        if (flag_board_gaming_mode==1)  f.open("GameRecord_temp_1.txt",std::ios::out);
        else if (flag_board_gaming_mode==0)  f.open("GameRecord_temp_0.txt",std::ios::out);
        else{
            //qDebug()<<"ERROR20";
        }
        f<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
        if (flag_board_gaming_mode==0)  f<<flag_board_gaming_mode+single_whofirst+20<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else if (flag_board_gaming_mode==1)  f<<flag_board_gaming_mode<<" "<<ruleLC<<" "<<ruleFF<<" "<<ruleTT<<std::endl;
        else{
            //qDebug()<<"ERROR502";
        }
        f<<map_x_temp<<" "<<map_y_temp<<std::endl;//棋盘大小
        f<<Chess_process[Chess_count][0]<<" "<<Chess_process[Chess_count][1]<<std::endl;
        f.close();
        if (mode_blind_chess==1){//盲棋模式给出落子点
            QString Title="盲棋裁判";
            QString Info="AI的黑棋落子位置: "+QString::fromStdString(std::to_string((map_y_temp+1)/2))+" 行 "+QString::fromStdString(std::to_string((map_x_temp+1)/2))+ " 列。";
            QMessageBox::warning(this,Title,Info);
        }
    }
    else ui->gamemode->setText("人机对弈");
    //qDebug()<<single_whofirst<<" "<<Chess_count;
}



void Board::on_AutoBattle_clicked(bool checked)//开发者模式（自动战斗）
{
    if (mode_blind_chess==1 && checked==1){
        QSoundEffect sound1;
        sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
        sound1.play();

        QString Title="自动战斗";
        QString Info="盲棋模式下不支持自动战斗！";
        QMessageBox::warning(this,Title,Info);
        ad_mode=0;
        ui->AutoBattle->setChecked(0);
        return ;
    }
    if (flag_gaming_double==5){
        QString Title="自动战斗已终止";
        QString Info="当前棋局已经结束！";
        QMessageBox::warning(this,Title,Info);
        ad_mode=0;
        ui->AutoBattle->setChecked(0);
        return ;
    }
    if (checked==1){
        QSoundEffect sound1;
        sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
        sound1.play();

        std::string password_input;
        QString pass_input=QInputDialog::getText(this,"自动战斗","此选项为开发者调试使用！\n如需开启请验证管理员密码",QLineEdit::Normal);
        password_input=pass_input.toStdString();
        std::hash<std::string> hasher;
        size_t hashed=hasher(password_input);
        //qDebug()<<hashed;
        if (hashed==operater_password){
            QString Title="自动战斗，启动！";
            QString Info="密码输入正确，管理员身份验证成功！\n即将开启自动战斗，请做好准备！";
            QMessageBox::warning(this,Title,Info);
            ui->change_whofirst->hide();
            if (ad_mode==0){
                ad_mode=1;
                autobattle_qidong();
            }
        }
        else if (hashed==yuanshen_qidong && flag_caidan_open==0){
            hidden_close=1;
            close();
            hidden_close=0;
            player->stop();
            flag_caidan_open_mode=flag_board_gaming_mode;
            hiddensource *hiddensourceWindow = new hiddensource;
            hiddensourceWindow->setAttribute(Qt::WA_QuitOnClose,false);
            hiddensourceWindow->show();

            return ;
        }
        else if(!pass_input.isEmpty()){
            QSoundEffect sound1;
            sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
            sound1.play();

            //qDebug()<<hashed;
            QString Title="开启失败";
            QString Info="密码输入错误！";
            QMessageBox::warning(this,Title,Info);
            ad_mode=0;
            ui->AutoBattle->setChecked(0);
        }
        else{
            ad_mode=0;
            ui->AutoBattle->setChecked(0);
        }
    }
    else if (checked==0){
        ad_mode=0;
    }
}



void Board::autobattle_qidong(){
    while (flag_gaming_double==1 && ad_mode==1){
        ai_chess_drop((Chess_count+1)%2);
        if (flag_board_gaming_mode==0 && flag_gaming_double==1){
            ai_chess_drop((Chess_count+1)%2);
        }
        if(ad_mode==0)  break;
    }
}

int value[21][21];//赋值棋盘

AI ai_test3;

void Board::ai_find_chess_drop_position(int chess_color){//获取ai给出的落子位置
    qDebug()<<ai_mode;
    ai_renshu=0;
    //初始化
    ai_test3.init(chess_color);
    for (int i=1;i<=map_y_temp;i++){
        for (int j=1;j<=map_x_temp;j++){
            map_ai[i][j]=map[i][j];
            mustnot_chess[i][j]=0;
            must_win_chess[i][j]=0;
        }
    }
    int temp_0=must_win_get();
    //qDebug()<<temp_0;
    for (int i=0;i<temp_0;i++){//必胜序列导入
        if ((abs(mustwin_point[i][2])>abs(must_win_chess[mustwin_point[i][0]][mustwin_point[i][1]])) ||
            (abs(mustwin_point[i][2])==abs(must_win_chess[mustwin_point[i][0]][mustwin_point[i][1]]) && (mustwin_point[i][2]>0 && mustwin_point[i][2]!=2))){
            must_win_chess[mustwin_point[i][0]][mustwin_point[i][1]]=mustwin_point[i][2];
        }
    }
    //添加可能的禁手点
    get_fm_point();
    for(int i=0;i<fm_point_length;i++){
        mustnot_chess[fm_point[i][0]][fm_point[i][1]]=1;
    }

    if (ai_mode==0){//赋值算法（0）


        //ai运算
        int temp_1006;
        temp_1006=ai_test3.method_fuzhi();
        if (temp_1006==1){
            qDebug()<<"赋值算法结果已给出！";
        }
        else{
            qDebug()<<"赋值算法未找到可落子位置！";
            ai_renshu=1;
        }
    }
    //此处要接入剩余ai！
    else if (ai_mode==11 || ai_mode==12 || ai_mode==13){//博弈树算法
        int temp_1006;
        temp_1006=ai_test3.method_boyishu();
        if (temp_1006==1){
            qDebug()<<"博弈树算法结果已给出！";
        }
        else{
            qDebug()<<"博弈树算法未找到可落子位置！";
            ai_renshu=1;
        }
    }
    else//随机下棋
    while(1){//测试用函数
        ai_find_x=rand()%map_y_temp+1;
        ai_find_y=rand()%map_x_temp+1;
        if (ai_find_x>=1 && ai_find_x<=map_y_temp && ai_find_y>=1 && ai_find_y<=map_x_temp && map[ai_find_x][ai_find_y]==-1){
            break;
        }
    }
    //qDebug()<<"ai给出的落子位置 "<<ai_find_x<<" "<<ai_find_y;
}

void Board::on_background_music_volume_valueChanged(int value)
{
    if (flag_just_open_board==0){
    backgroungmusic_volume=value/100.0;
    //qDebug()<<"背景音乐大小变为 "<<backgroungmusic_volume;
    audioOutput->setVolume(backgroungmusic_volume);
    }
    else if (flag_just_open_board==1) return;
    //elseqDebug()<<"ERROR509";
}


void Board::on_blindchess_clicked(bool checked)//盲棋模式
{
    if (mode_must_blind==1){
        if (checked==0){
            std::string password_input;
            QString pass_input=QInputDialog::getText(this,"盲棋裁判","当前为盲棋专业模式，无法关闭盲棋！\n如需关闭请验证管理员密码 ",QLineEdit::Normal);
            password_input=pass_input.toStdString();
            std::hash<std::string> hasher;
            size_t hashed=hasher(password_input);
            if (hashed==caijiuduolian){
                QString Title="盲棋裁判";
                QString Info="密码输入正确！ \n盲棋模式已关闭";
                QMessageBox::warning(this,Title,Info);
                mode_blind_chess=0;
                update();
            }
            else if(!password_input.empty()){
                QString Title="盲棋裁判";
                QString Info="密码错误，无法关闭盲棋模式";
                QMessageBox::warning(this,Title,Info);
                ui->blindchess->setChecked(1);
            }
            else{
                ui->blindchess->setChecked(1);
            }
        }
        else{
            mode_blind_chess=1;
            update();
        }
        return ;
    }
    if (checked==1 && mode_must_blind==0){
        QSoundEffect sound1;
        sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
        sound1.play();

        QMessageBox choose(QMessageBox::Question,QString(tr("盲棋裁判")),QString(tr("开启盲棋功能后，棋盘将由盲棋裁判接管，不再显示棋子 \n您确定要开启盲棋功能吗？")),QMessageBox::Yes| QMessageBox::No);
        choose.icon();
        choose.button(QMessageBox::Yes)->setText(tr("开启"));
        choose.button(QMessageBox::No)->setText(tr("不开启"));
        int temp_1=choose.exec();
        if(temp_1== QMessageBox::Yes){
            mode_blind_chess=1;
            QString Title="盲棋裁判";
            QString Info="您已开启盲棋模式！ \n棋盘上将不再显示棋子！";
            QMessageBox::warning(this,Title,Info);
            update();
        }
        else{
            ui->blindchess->setChecked(0);
            mode_blind_chess=0;
        }
    }
    else{
        mode_blind_chess=0;
        update();
    }
}


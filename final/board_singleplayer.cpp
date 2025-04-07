#include "board_singleplayer.h"
#include "ui_board_singleplayer.h"
#include "board_review.h"
#include "read_save_select.h"
#include <QPainter>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QFile>
#include <fstream>
#include <time.h>
#include <iostream>

int flag_gaming_single,flag_musicmode_single=-1;
std::string backgroundmusic_str_single,savepath_single;
QString backgroundmusic_qstr_single;

int map_S[17][17];//棋盘储存范围为（1，1）-（15，15），即从1开始计数
int Chess_process_S[260][2]={0};//第0步是空棋盘状态，用于检测，棋局第一步即存在Chess_process_S[1]中
int Chess_count_S=0,flag_colsesave_S;
float Volume_user_S=0.8;
std::fstream f_S;



void Board_SinglePlayer::PlayMusic(){
    if (!player){
        player = new QMediaPlayer;
        player->setSource(backgroundmusic_qstr_single);
    }
    player->play();
}


void Board_SinglePlayer::onStateChanged(QAudio::State state){
    if (state==QAudio::StoppedState){
        player->setPosition(0);
        player->play();
    }
}



Board_SinglePlayer::Board_SinglePlayer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Board_SinglePlayer)
{
    ui->setupUi(this);

    ui->chess_undo->setFont(QFont("华文楷体",16));
    ui->chess_false->setFont(QFont("华文楷体",16));
    ui->save_and_exit->setFont(QFont("华文楷体",16));
    ui->read_save->setFont(QFont("华文楷体",16));

    for (int i=1;i<=15;i++){
        for (int j=1;j<=15;j++){
            map_S[i][j]=-1;
        }
    }

    Chess_count_S=0;
    if (flag_gaming_single==0 || flag_gaming_single==5){
        flag_gaming_single=1;
        f_S.open("flag_gaming.txt",std::ios::out);
        f_S<<"1"<<" "<<"0"<<std::endl;
        f_S.close();

        //写入临时保存棋局的开头两行
        time_t chess_start_time;
        time(&chess_start_time);
        f_S.open("GameRecord_temp_0.txt",std::ios::out);
        f_S<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
        f_S<<"0"<<std::endl;
        f_S.close();
    }
    else if (flag_gaming_single==1){//若上次棋局意外退出，加载未保存的棋局
        /* 临时棋局记录文件结构：
         * 第一行：游戏开始时间
         * 第二行：游戏模式（0为单人，1为双人）
         * 第二行及以后格式："count 行坐标 列坐标" （行列坐标从1开始计数）
         * 1为黑棋，0为白棋，count从1开始计数（即表示真实步数）*/
        flag_gaming_single=1;
        f_S.open("flag_gaming.txt",std::ios::out);
        f_S<<"1"<<" "<<"0"<<std::endl;
        f_S.close();
        f_S.open("GameRecord_temp_0.txt",std::ios::in);
        std::string str_temp;
        std::getline(f_S,str_temp);//读取掉时间行
        std::getline(f_S,str_temp);//读取掉游戏模式行
        while(f_S>>Chess_process_S[Chess_count_S+1][0]>>Chess_process_S[Chess_count_S+1][1]){
            //qDebug()<<Chess_process_S[Chess_count_S+1][0]<<" "<<Chess_process_S[Chess_count_S+1][1];
            if (Chess_process_S[Chess_count_S+1][0]==-20 && Chess_process_S[Chess_count_S+1][1]==-20){//-20 -20表示悔棋
                map_S[Chess_process_S[Chess_count_S][0]][Chess_process_S[Chess_count_S][1]]=-1;
                Chess_process_S[Chess_count_S][0]=0;
                Chess_process_S[Chess_count_S][1]=0;
                Chess_count_S--;
            }
            else{
                Chess_count_S++;
                map_S[Chess_process_S[Chess_count_S][0]][Chess_process_S[Chess_count_S][1]]=Chess_count_S%2;
            }
        }
        f_S.close();
    }
    else if (flag_gaming_single==2){
        flag_gaming_single=1;
        f_S.open("flag_gaming.txt",std::ios::out);
        f_S<<"1"<<" "<<"0"<<std::endl;
        f_S.close();

        time_t chess_start_time;
        time(&chess_start_time);
        f_S.open("GameRecord_temp_1.txt",std::ios::out);
        f_S<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
        f_S<<"1"<<std::endl;
        f_S.close();
        f_S.open("history/unfinished/single/"+savepath_single+".txt",std::ios::in);
        //qDebug()<<"history/unfinished/single/"+savepath_single+".txt";
        std::string str_temp;
        std::getline(f_S,str_temp);//读取掉时间行
        std::getline(f_S,str_temp);//读取掉游戏模式行
        while(f_S>>Chess_process_S[Chess_count_S+1][0]>>Chess_process_S[Chess_count_S+1][1]){
            //qDebug()<<Chess_process_S[Chess_count_S+1][0]<<" "<<Chess_process_S[Chess_count_S+1][1];
            if (Chess_process_S[Chess_count_S+1][0]==-20 && Chess_process_S[Chess_count_S+1][1]==-20){//-20 -20表示悔棋
                map_S[Chess_process_S[Chess_count_S][0]][Chess_process_S[Chess_count_S][1]]=-1;
                Chess_process_S[Chess_count_S][0]=0;
                Chess_process_S[Chess_count_S][1]=0;
                Chess_count_S--;
            }
            else{
                Chess_count_S++;
                map_S[Chess_process_S[Chess_count_S][0]][Chess_process_S[Chess_count_S][1]]=Chess_count_S%2;
            }
        }
        f_S.close();
        f_S.open("GameRecord_temp_1.txt",std::ios::out|std::ios::app);
        for (int i=1;i<=Chess_count_S;i++){
            f_S<<Chess_process_S[i][0]<<" "<<Chess_process_S[i][1]<<std::endl;
        }
        f_S.close();
        std::filesystem::remove("history/unfinished/double/"+savepath_single+".txt");
    }
    //else qDebug()<<"ERROR#2";

    f_S.open("music_background.txt",std::ios::in);//加载背景音乐
    f_S>>flag_musicmode_single;
    getline(f_S,backgroundmusic_str_single);
    getline(f_S,backgroundmusic_str_single);
    f_S.close();
    //qDebug()<<flag_musicmode_single<<" "<<backgroundmusic_str_single;

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.8);

    int len_temp_2;
    len_temp_2=backgroundmusic_str_single.length();
    backgroundmusic_str_single.erase(len_temp_2,1);
    backgroundmusic_qstr_single=QString::fromStdString(backgroundmusic_str_single);
    if (backgroundmusic_qstr_single.isEmpty()){//检查音乐文件是否缺省
        flag_musicmode_single=-1;
    }
    if (flag_musicmode_single==1){
        player->setSource(backgroundmusic_qstr_single);
    }
    else if (flag_musicmode_single==0){
        player->setSource(QUrl("qrc:/music/background_music/"+backgroundmusic_qstr_single+".mp3"));
    }
    else{
        player->setSource(QUrl("qrc:/music/background_music/bgm1.mp3"));
    }
    //QObject::connect(player,QMediaPlayer::statechanged,Board::onStateChanged);
    player->setLoops(QMediaPlayer::Infinite);//设置音乐循环播放
    PlayMusic();
}


void Board_SinglePlayer::closeEvent(QCloseEvent *event){//右上角叉号弹出提示
    if (flag_gaming_single==1){
        if (flag_colsesave_S!=1){
            QMessageBox choose(QMessageBox::Question,QString(tr("请确认")),QString(tr("当前对局仍在进行中，且尚未保存，确认要退出吗？")),QMessageBox::Yes| QMessageBox::No);
            choose.button(QMessageBox::Yes)->setText(tr("不保存直接退出"));
            choose.button(QMessageBox::No)->setText(tr("取消"));
            int temp_15=choose.exec();
            if (temp_15 == QMessageBox::Yes){
                flag_colsesave_S==0;
                flag_gaming_single=0;
                std::fstream f;
                f.open("flag_gaming.txt",std::ios::out);
                f<<"0"<<" "<<"0"<<std::endl;
                f.close();
                player->stop();
                event->accept();
            }
            else{
                flag_colsesave_S=0;
                event->ignore();
            }
        }
        else{
            QMessageBox choose(QMessageBox::Question,QString(tr("请确认")),QString(tr("当前对局尚未结束，确认要保存当前进度并退出吗？")),QMessageBox::Yes| QMessageBox::No);
            choose.button(QMessageBox::Yes)->setText(tr("保存并退出"));
            choose.button(QMessageBox::No)->setText(tr("取消"));
            int temp_15=choose.exec();
            if (temp_15 == QMessageBox::Yes){
                write_in_double(0,0);
                flag_colsesave_S==0;
                flag_gaming_single=0;
                std::fstream f;
                f.open("flag_gaming.txt",std::ios::out);
                f<<"0"<<" "<<"0"<<std::endl;
                f.close();
                player->stop();
                event->accept();
            }
            else{
                flag_colsesave_S=0;
                event->ignore();
            }
        }
    }
    else{
        flag_gaming_single=0;
        flag_colsesave_S==0;
        player->stop();
    }

}
Board_SinglePlayer::~Board_SinglePlayer()
{
    delete ui;
}

void Board_SinglePlayer::paintEvent(QPaintEvent *){//绘制五子棋棋盘
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    //画背景
    QPen Line,Point,Square;
    QBrush Square_brush;
    Square.setColor(Qt::yellow);
    painter.setPen(Square);
    Square_brush.setColor(Qt::yellow);
    Square_brush.setStyle(Qt::SolidPattern);
    painter.setBrush(Square_brush);
    QRect Background(20,20,600,600);
    painter.drawRect(Background);

    //画特征点
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

    //画网格
    Line.setWidth(2);
    Line.setBrush(Qt::black);
    painter.setPen(Line);
    for (int i=1;i<16;i++){
        QLine Line_1(40,i*40,600,i*40);
        painter.drawLine(Line_1);
        QLine Line_2(i*40,40,i*40,600);
        painter.drawLine(Line_2);
    }
    Line.setWidth(4);
    Line.setBrush(Qt::darkBlue);
    painter.setPen(Line);
    QLine Line_3(18,18,18,622);
    painter.drawLine(Line_3);
    QLine Line_4(18,18,622,18);
    painter.drawLine(Line_4);
    QLine Line_5(18,622,622,622);
    painter.drawLine(Line_5);
    QLine Line_6(622,18,622,622);
    painter.drawLine(Line_6);

    drawChess();
    ////qDebug()<< "FLAG_1 "<<Chess_count_S;

}

int Chess_r_l_S=18;
int Chess_r_s_S=4;

void Board_SinglePlayer::drawChess(){//绘制棋子
    /*QPen Chess_Pen_white;
    Chess_Pen_white.setWidth(12);
    Chess_Pen_white.setBrush(Qt::white);
    QPen Chess_Pen_black;
    Chess_Pen_black.setWidth(12);
    Chess_Pen_black.setBrush(Qt::black);
    QBrush Square;*/
    QPainter painter(this);
    QPen Chess_Pen;
    Chess_Pen.setWidth(1);
    painter.setPen(Chess_Pen);
    for(int i=1;i<=15;i++){
        for(int j=1;j<=15;j++){
            if (map_S[i][j]==0){//白棋
                /*painter.setPen(Chess_Pen_white);
                Square.setColor(Qt::white);
                painter.setBrush(Square);*/
                QRect chess_white_large(i*40-Chess_r_l_S,j*40-Chess_r_l_S,2*Chess_r_l_S,2*Chess_r_l_S);
                /*painter.drawEllipse(chess_white_large);
                QRect chess_white_small(i*40-Chess_r_s_S,j*40-Chess_r_s_S,2*Chess_r_s_S,2*Chess_r_s_S);
                painter.drawRect(chess_white_small);*/
                painter.drawPixmap(chess_white_large,QPixmap(":/pictures/chess_white.png"));
            }
            else if (map_S[i][j]==1){//黑棋
                /*Square.setColor(Qt::black);
                painter.setPen(Chess_Pen_black);
                painter.setBrush(Square);*/
                QRect chess_black_large(i*40-Chess_r_l_S,j*40-Chess_r_l_S,2*Chess_r_l_S,2*Chess_r_l_S);
                /*painter.drawEllipse(chess_black_large);
                QRect chess_black_small(i*40-Chess_r_s_S,j*40-Chess_r_s_S,2*Chess_r_s_S,2*Chess_r_s_S);
                painter.drawRect(chess_black_small);*/
                painter.drawPixmap(chess_black_large,QPixmap(":/pictures/chess_black.png"));
            }
        }
    }
    if (Chess_count_S>0){//为最后落下的棋子绘制标识
        Chess_Pen.setWidth(6);
        Chess_Pen.setColor(Qt::red);
        painter.setPen(Chess_Pen);
        painter.drawPoint(Chess_process_S[Chess_count_S][0]*40,Chess_process_S[Chess_count_S][1]*40);
    }
}


void Board_SinglePlayer::mousePressEvent(QMouseEvent *ev){//鼠标左键点击时确认点击位置
    if (ev->button() != Qt::LeftButton){
        return;
    }
    QPoint loc= ev->pos();

    int row=-1,col=-1,distance_x,distance_y,temp_flag=0;
    QPoint des;
    for(int i=1;i<=15;i++){
        for(int j=1;j<=15;j++){
            des.rx()=j*40;
            des.ry()=i*40;
            distance_x=des.x()-loc.x();
            distance_y=des.y()-loc.y();
            if(distance_x*distance_x + distance_y*distance_y < (Chess_r_l_S-1)*(Chess_r_l_S-1)){
                row=j;
                col=i;
                temp_flag=1;
                break;
            }
        }
        if(temp_flag==1){
            break;
        }
    }

    ////qDebug()<<"Flag2 "<<row<<" "<<col;
    Chess_drop(row,col);
}


bool Board_SinglePlayer::Chess_drop(int row,int col){//在对应位置下子（含判断能否下子）
    if (row>=1 && row<=15 && col>=1 && col<=15 && map_S[row][col]==-1 && flag_gaming_single==1){
        ui->read_save->hide();//隐藏读取存档按钮
        Chess_count_S++;
        map_S[row][col]=Chess_count_S%2;
        Chess_process_S[Chess_count_S][0]=row;
        Chess_process_S[Chess_count_S][1]=col;
        f_S.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
        f_S<<Chess_process_S[Chess_count_S][0]<<" "<<Chess_process_S[Chess_count_S][1]<<std::endl;
        f_S.close();
        ////qDebug()<<"Flag3 "<<Chess_process_S[Chess_count_S][0]<<" "<<Chess_process_S[Chess_count_S][1];
        update();
        if (winner_judging_single(Chess_count_S%2,Chess_process_S[Chess_count_S][0],Chess_process_S[Chess_count_S][1])!=-1){
            IfWin_single(winner_judging_single(Chess_count_S%2,Chess_process_S[Chess_count_S][0],Chess_process_S[Chess_count_S][1]));
        }
        else{
            Chess_drop_ai(row,col);
            update();
            if (winner_judging_single(Chess_count_S%2,Chess_process_S[Chess_count_S][0],Chess_process_S[Chess_count_S][1])!=-1){
                IfWin_single(winner_judging_single(Chess_count_S%2,Chess_process_S[Chess_count_S][0],Chess_process_S[Chess_count_S][1]));
            }
        }

        return true;
    }
    else return false;
}


bool Board_SinglePlayer::Chess_drop_ai(int row,int col){
    //随机生成落子位置
    /*
    while(1){
        row=rand()%15+1;
        col=rand()%15+1;
        if (row>=1 && row<=15 && col>=1 && col<=15 && map_S[row][col]==-1){
            break;
        }
    }
    */
    //调用AItest中ai获取落子位置
    //

    /*if(pos.m_col != -1){
        //执行落子操作
        Chess_count_S++;
        map_S[row][col]=Chess_count_S%2;
        Chess_process_S[Chess_count_S][0]=row;
        Chess_process_S[Chess_count_S][1]=col;
        f_S.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
        f_S<<Chess_process_S[Chess_count_S][0]<<" "<<Chess_process_S[Chess_count_S][1]<<std::endl;
        f_S.close();
        update();
        return true;
    }else {
        //qDebug()<<"AI error!";
        return false;
    }
*/


}



void Board_SinglePlayer::IfWin_single(int winningflag){
    /*判断胜负
     * 0 1 2由winner_judging函数返回，分别表示白胜、黑胜、黑违反禁手规则
     * 3为白认输 4为黑认输 由按钮触发
     */
    int temp_flag_2=0;
    if (winningflag==0){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==1){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="黑方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==2){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="黑方违反禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==21){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="黑方违反长连禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==22){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="黑方违反四四禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==23){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="黑方违反三三禁手规则，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==3){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="黑方认输，白方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==4){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="白方认输，黑方获得胜利！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    else if (winningflag==5){
        flag_gaming_single=5;
        ui->read_save->show();
        QString Title="提示";
        QString Info="平局！";
        QMessageBox::warning(this,Title,Info);
        temp_flag_2=1;
    }
    if (temp_flag_2==1){
        ui->chess_undo->setText("重新开始");
        ui->chess_false->setText("复盘");
        f_S.open("flag_gaming.txt",std::ios::out);
        f_S<<"0"<<" "<<"0"<<std::endl;
        f_S.close();
        //qDebug()<<"写入对局状态0";
        write_in_double(winningflag,1);
    }

}



std::string game_name_S;

void Board_SinglePlayer::write_in_double(int winningflag,int iffinished){

    f_S.open("GameRecord_temp_0.txt",std::ios::in);
    std::getline(f_S,game_name_S);
    f_S.close();
    int temp_4;
    temp_4=game_name_S.length();
    game_name_S.erase(temp_4,1);
    //qDebug()<<game_name_S<<" "<<temp_4;
    for (int i=0;i<temp_4;i++){//去除windows文件名中不合法的":"
        if (game_name_S[i]==':'){
            game_name_S[i]='_';
        }
    }
    //qDebug()<<game_name_S;
    if (iffinished){
        f_S.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
        f_S<<"-114"<<" "<<"-514"<<std::endl;//-114 -514表示对局结束，停止读取下棋记录
        f_S<<winningflag<<std::endl;
        f_S.close();
        std::filesystem::copy("GameRecord_temp_0.txt","history/"+game_name_S+".txt");
    }
    else    std::filesystem::copy("GameRecord_temp_0.txt","history/unfinished/single/"+game_name_S+".txt");
}


void Board_SinglePlayer::on_chess_undo_clicked()//悔棋操作和重新开始操作
{
    if (flag_gaming_single==1){//悔棋操作
        QMessageBox choose(QMessageBox::Question,QString(tr("提示")),QString(tr("古人云 “落子无悔”  \n您确定要悔棋吗？")),QMessageBox::Yes| QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("悔棋"));
        choose.button(QMessageBox::No)->setText(tr("取消"));
        int temp_15=choose.exec();
        if (temp_15 == QMessageBox::Yes){
            if (Chess_count_S<2){//单人模式悔棋一悔悔两步
                QString Title="悔棋失败";
                QString Info="目前棋盘上没有你的棋子，无法进行悔棋！";
                QMessageBox::warning(this,Title,Info);
            }
            else{
                map_S[Chess_process_S[Chess_count_S][0]][Chess_process_S[Chess_count_S][1]]=-1;
                Chess_process_S[Chess_count_S][0]=0;
                Chess_process_S[Chess_count_S][1]=0;
                Chess_count_S--;
                map_S[Chess_process_S[Chess_count_S][0]][Chess_process_S[Chess_count_S][1]]=-1;
                Chess_process_S[Chess_count_S][0]=0;
                Chess_process_S[Chess_count_S][1]=0;
                Chess_count_S--;
                f_S.open("GameRecord_temp_0.txt",std::ios::out|std::ios::app);
                f_S<<"-20 -20"<<std::endl;
                f_S<<"-20 -20"<<std::endl;
                f_S.close();
                update();
            }
        }
    }
    else if (flag_gaming_single==0 || flag_gaming_single==5){//重新开始操作
        for (int i=0;i<17;i++){
            for (int j=0;j<17;j++){
                map_S[i][j]=-1;
            }
        }
        for (int i=0;i<230;i++){
            Chess_process_S[i][0]=0;
            Chess_process_S[i][1]=0;
        }
        Chess_count_S=0;
        flag_gaming_single=1;
        f_S.open("flag_gaming.txt",std::ios::out);
        f_S<<"0"<<" "<<"1"<<std::endl;
        f_S.close();
        //qDebug()<<"写入对局状态";

        //写入临时保存棋局的开头两行
        time_t chess_start_time;
        time(&chess_start_time);
        f_S.open("GameRecord_temp_1.txt",std::ios::out);
        f_S<<asctime(localtime(&chess_start_time));//时间行（输出时间中内含换行）
        f_S<<"1"<<std::endl;
        f_S.close();
        ui->chess_undo->setText("悔棋");
        ui->chess_false->setText("认输");
        update();
    }
}


void Board_SinglePlayer::on_chess_false_clicked()//认输+复盘操作
{
    if (flag_gaming_single==1){//认输
        QMessageBox choose(QMessageBox::Question,QString(tr("提示")),QString(tr("您确定要认输吗？")),QMessageBox::Yes| QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("认输"));
        choose.button(QMessageBox::No)->setText(tr("取消"));
        int temp_15=choose.exec();
        if (temp_15 == QMessageBox::Yes){
            IfWin_single(Chess_count_S%2+3);//
        }
    }
    else if (flag_gaming_single==0 || flag_gaming_single==5){//复盘
        if (flag_review==0){
            review_file_name=game_name_S;
            Board_Review *Board_ReviewWindow = new Board_Review;
            Board_ReviewWindow->show();
        }
        else{
            QString Title="警告";
            QString Info="当前已有一局打开的历史记录，请先关闭已打开的历史记录！";
            QMessageBox::warning(this,Title,Info);
        }
    }
}


void Board_SinglePlayer::on_save_and_exit_clicked()
{
    flag_colsesave_S=1;
    close();
}


void Board_SinglePlayer::on_read_save_clicked()
{
    if (Chess_count_S>0 && flag_gaming_single==1){
        QString Title="提示";
        QString Info="当前对局已经开始，请先结束当前的对局！";
        QMessageBox::warning(this,Title,Info);
    }
    else{
        flag_gaming_single=5;
        flag_read_save=0;
        close();
        Read_save_select *ReadselectWindow = new Read_save_select;
        ReadselectWindow->show();
    }
}


//游戏信息:禁手规则是否开启
bool ruleLC_S_S = true;
bool ruleFF_S = true;
bool ruleTT_S = true;

//所有判定序列
const int null_S[1] = { 0 };//空
const int lc_S[6] = { 1,1,1,1,1,1 };//长连
const int win_S[2][5] = { { 0,0,0,0,0 },{1,1,1,1,1} };//五连
const int tc_S_I[2][7] = { {4,-1,1,1,1,-1,-1},{-1,-1,1,1,1,-1,4} };//活三种类1 *同一方向上只统计1次
const int tc_S_I_origin[3] = {1,1,1};//活三种类1的原始形式
const int tc_S_II[2][6]={ {-1, 1, -1, 1, 1,-1},{-1,1,1,-1,1,-1} };//活三种类2
const int tc_S_II_origin[2][4] = { { 1, -1, 1, 1},{1,1,-1,1} };//活三种类2的原始形式
const int fc_S_I[2][6] = { {4,-1,1,1,1,1},{1,1,1,1,-1,4} };//活四种类1 *同一方向上只统计1次
const int fc_S_I_origin[4] = {1,1,1,1};//活四种类1的原始形式
const int fc_S_II[3][7] = { {4,1,1,1,-1,1,4},{4,1,1,-1,1,1,4},{4,1,-1,1,1,1,4} };//活四种类2
const int fc_S_II_origin[3][5] = { {1,1,1,-1,1},{1,1,-1,1,1},{1,-1,1,1,1} };//活四种类2的原始形式

/*
判定法则:用于判断棋子与序列是否匹配
因为-1是空位置，索引需要从0开始，所以使用其中的值需要+1,即：match_S[chess+1][cs_num+1]
cs_num=-1,0,1,2时仅匹配到对应chess返回true;
cs_num=3时一定返回true;
cs_num=4时除黑子外,即匹配到白子，空，墙均返回true；
cs_num=5时除白子外，即匹配到黑子，空，墙均返回true；
*/
const bool match_S[4][7] = { {true,false,false,false,true,true,true},//chess=-1,空位置;
                            {false,true,false,false,true,true,false},//chess=0,白子;
                            {false,false,true,false,true,false,true},//chess=1,黑子;
                            {false,false,false,true,true,true,true} };//chess=2,墙;

/*
玩家信息:
Player:white_S, black_S 执白棋玩家为玩家0，执黑棋玩家为玩家1
state:玩家是否胜利，0=否，1=是，2=黑子违反禁手规则 #default=0
win_S_posx(y):若玩家胜利，储存五连子的起始坐标 #default=-1
win_S_direct:若玩家胜利，储存五连子的方向 #default=-1
win_S_case:五连子序列(一般也不会去改它...) #default=win_S[0] or win_S[1]
win_S_case_length:五连子序列长度（废话） #default=5
fm_type:违反的禁手规则,LC_S=长连禁手,TT=三三禁手,FF=四四禁手 #default="NULL_S"
fm_posx(y)(_II):禁手序列起始坐标(TT,FF需要储存两个) #default=-1
fm_direct(_II):禁手序列方向(TT,FF需要储存两个) #default=-1
fm_case(_II):禁手序列(TT,FF需要储存两个) #default=null_S
fm_case(_II)_length:禁手序列长度(TT,FF需要储存两个) #default=1
*/
struct player_white_S_S{
    int state = 0;
    int win_S_posx = -1;
    int win_S_posy = -1;
    int win_S_direct = -1;
    const int* win_S_case = win_S[0];
    int win_S_case_length = 5;
}white_S = { 0,-1,-1,-1,win_S[0],5 };
struct player_black_S_S{
    int state = 0;
    int win_S_posx = -1;
    int win_S_posy = -1;
    int win_S_direct = -1;
    const int* win_S_case = win_S[1];
    int win_S_case_length = 5;
    std::string fm_type = "NULL_S";
    int fm_posx = -1;
    int fm_posy = -1;
    int fm_direct = -1;
    const int* fm_case = null_S;
    int fm_case_length = 1;
    int fm_posx_II = -1;
    int fm_posy_II = -1;
    int fm_direct_II = -1;
    const int* fm_case_II= null_S;
    int fm_case_II_length = 1;
}black_S = {0,-1,-1,-1,win_S[1],5,"NULL_S",-1,-1,-1,null_S,1,-1,-1,-1,null_S,1};

int tempinfo_S[3] = { -1,-1,-1 };//用于临时储存find_S函数比对成功的direct,posx,posy信息

bool find_S(const int cs[],int length,int direct,int posx,int posy){
    /*
    function:在包含指定点(pos)的所有指定方向(direct)的序列里，检测是否出现指定序列(cs)，顺序为行坐标增加方向
    参数：序列，序列长度，检测方向(0=水平，1=竖直，2=左上-右下，3=右上-左下)，点坐标
    以下墙也算作一种“棋子”
    以下统一i,x为行坐标，j,y为列坐标，即board[i(x)][j(y)]
    */
    if (direct == 0) {//行检测
        for (int j = std::max(posy - length + 1, 0); j <= std::min(posy , 17 - length); j++) {
            for (int k = 0; k < length; k++) {
                if (!match_S[map_S[posx][j + k]+1][cs[k]+1]) break;
                if (k == length - 1) {
                    tempinfo_S[0] = 0;
                    tempinfo_S[1] = posx;
                    tempinfo_S[2] = j;
                    return true;
                }
            }
        }
    }
    else if (direct == 1) {//列检测
        for (int i = std::max(posx - length + 1, 0); i <= std::min(posx, 17 - length); i++) {
            for (int k = 0; k < length; k++) {
                if (!match_S[map_S[i + k][posy]+1][cs[k]+1]) break;
                if (k == length - 1) {
                    tempinfo_S[0] = 1;
                    tempinfo_S[1] = i;
                    tempinfo_S[2] = posy;
                    return true;
                }
            }
        }
    }
    else if (direct == 2) {//斜对角线检测
        //定义对角线序数s=x-y,-16<=s<=16，可得对角线上棋子个数为17-|s|
        int s=posx-posy;
        if(17-std::max(-s,s)>=length){
            for (int m = std::max(0,std::max(s,posx+1-length)); m <= std::min(17-length,std::min(17-length+s,posx)); m++) {
                for (int k = 0; k < length; k++) {
                    int i = m + k;
                    int j = i - s;
                    if (!match_S[map_S[i][j]+1][cs[k]+1]) break;
                    if (k == length - 1) {
                        tempinfo_S[0] = 2;
                        tempinfo_S[1] = m;
                        tempinfo_S[2] = m-s;
                        return true;
                    }
                }
            }
        }
    }
    else if (direct == 3) {//反对角线检测
        //定义对角线序数s=x+y,2<=s<=30，可得对角线上棋子个数为17-|s-16|
        int s = posx + posy;
        if(17-std::max(s-16,16-s)>=length){
            for (int m = std::max(0,std::max(s-16, posx+1-length)); m <= std::min(17-length,std::min(s-length+1, posx)); m++) {
                for (int k = 0; k < length; k++) {
                    int i = m + k;
                    int j = s - i;
                    if (!match_S[map_S[i][j]+1][cs[k]+1]) break;
                    if (k == length - 1) {
                        tempinfo_S[0] = 3;
                        tempinfo_S[1] = m;
                        tempinfo_S[2] = s-m;
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool LC_S(int posx,int posy) { //function:检测是否满足长连禁手,同时记录长连禁手信息并修改玩家状态
    for (int direct = 0; direct <= 3; direct++)
        if (find_S(lc_S, 6, direct, posx, posy)) {
            black_S.state = 2;
            black_S.fm_type = "LC_S";
            black_S.fm_direct = tempinfo_S[0];
            black_S.fm_posx = tempinfo_S[1];
            black_S.fm_posy = tempinfo_S[2];
            black_S.fm_case = lc_S;
            black_S.fm_case_length = 6;
            return true;
        }
    return false;
}

bool player_win_S(int player, int posx, int posy) { //function:检测对应玩家落子后是否胜利,同时记录获胜信息并修改玩家状态
    //应先检测是否满足长连禁手
    for (int direct = 0; direct <= 3; direct++) {
        if (find_S(win_S[player], 5, direct, posx, posy)) {
            if (player == 0) {
                white_S.state = 1;
                white_S.win_S_direct = tempinfo_S[0];
                white_S.win_S_posx = tempinfo_S[1];
                white_S.win_S_posy = tempinfo_S[2];
            }
            else {
                black_S.state = 1;
                black_S.win_S_direct = tempinfo_S[0];
                black_S.win_S_posx = tempinfo_S[1];
                black_S.win_S_posy = tempinfo_S[2];
            }
            return true;
        }
    }
    return false;
}

bool FF_S(int posx, int posy) { //function:检测是否满足四四禁手,同时记录四四禁手信息并修改玩家状态
    int fc_S_num = 0;
    for (int direct = 0; direct <= 3; direct++) {
        for (int i = 0; i < 2; i++) {
            if (find_S(fc_S_I[i], 6, direct, posx, posy)) {
                fc_S_num++;
                if (fc_S_num == 1) {
                    black_S.fm_direct = tempinfo_S[0];
                    if (i == 0) {
                        if (direct == 0) {
                            black_S.fm_posx = tempinfo_S[1];
                            black_S.fm_posy = tempinfo_S[2] + 2;
                        }
                        else if (direct == 1) {
                            black_S.fm_posx = tempinfo_S[1] + 2;
                            black_S.fm_posy = tempinfo_S[2];
                        }
                        else if (direct == 2) {
                            black_S.fm_posx = tempinfo_S[1] + 2;
                            black_S.fm_posy = tempinfo_S[2] + 2;
                        }
                        else if (direct == 3) {
                            black_S.fm_posx = tempinfo_S[1] + 2;
                            black_S.fm_posy = tempinfo_S[2] - 2;
                        }
                    }
                    else if (i == 1) {
                        if (direct == 0) {
                            black_S.fm_posx = tempinfo_S[1];
                            black_S.fm_posy = tempinfo_S[2];
                        }
                        else if (direct == 1) {
                            black_S.fm_posx = tempinfo_S[1];
                            black_S.fm_posy = tempinfo_S[2];
                        }
                        else if (direct == 2) {
                            black_S.fm_posx = tempinfo_S[1];
                            black_S.fm_posy = tempinfo_S[2];
                        }
                        else if (direct == 3) {
                            black_S.fm_posx = tempinfo_S[1];
                            black_S.fm_posy = tempinfo_S[2];
                        }
                    }
                    black_S.fm_case = fc_S_I_origin;
                    black_S.fm_case_length = 4;
                }
                if (fc_S_num >= 2) {
                    black_S.state = 2;
                    black_S.fm_type = "FF";
                    black_S.fm_direct_II = tempinfo_S[0];
                    if (i == 0) {
                        if (direct == 0) {
                            black_S.fm_posx_II = tempinfo_S[1];
                            black_S.fm_posy_II = tempinfo_S[2] + 2;
                        }
                        else if (direct == 1) {
                            black_S.fm_posx_II = tempinfo_S[1] + 2;
                            black_S.fm_posy_II = tempinfo_S[2];
                        }
                        else if (direct == 2) {
                            black_S.fm_posx_II = tempinfo_S[1] + 2;
                            black_S.fm_posy_II = tempinfo_S[2] + 2;
                        }
                        else if (direct == 3) {
                            black_S.fm_posx_II = tempinfo_S[1] + 2;
                            black_S.fm_posy_II = tempinfo_S[2] - 2;
                        }
                    }
                    else if(i == 1){
                        if (direct == 0) {
                            black_S.fm_posx_II = tempinfo_S[1];
                            black_S.fm_posy_II = tempinfo_S[2];
                        }
                        else if (direct == 1) {
                            black_S.fm_posx_II = tempinfo_S[1];
                            black_S.fm_posy_II = tempinfo_S[2];
                        }
                        else if (direct == 2) {
                            black_S.fm_posx_II = tempinfo_S[1];
                            black_S.fm_posy_II = tempinfo_S[2];
                        }
                        else if (direct == 3) {
                            black_S.fm_posx_II = tempinfo_S[1];
                            black_S.fm_posy_II = tempinfo_S[2];
                        }
                    }
                    black_S.fm_case_II = fc_S_I_origin;
                    black_S.fm_case_II_length = 4;
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (find_S(fc_S_II[i], 7, direct, posx, posy)) {
                fc_S_num++;
                if (fc_S_num == 1) {
                    black_S.fm_direct = tempinfo_S[0];
                    if (direct == 0) {
                        black_S.fm_posx = tempinfo_S[1];
                        black_S.fm_posy = tempinfo_S[2] + 1;
                    }
                    else if (direct == 1) {
                        black_S.fm_posx = tempinfo_S[1] + 1;
                        black_S.fm_posy = tempinfo_S[2];
                    }
                    else if (direct == 2) {
                        black_S.fm_posx = tempinfo_S[1] + 1;
                        black_S.fm_posy = tempinfo_S[2] + 1;
                    }
                    else if (direct == 3) {
                        black_S.fm_posx = tempinfo_S[1] + 1;
                        black_S.fm_posy = tempinfo_S[2] - 1;
                    }
                    black_S.fm_case = fc_S_II_origin[i];
                    black_S.fm_case_length = 5;
                }
                if (fc_S_num >= 2) {
                    black_S.state = 2;
                    black_S.fm_type = "FF";
                    black_S.fm_direct_II = tempinfo_S[0];
                    if (direct == 0) {
                        black_S.fm_posx_II = tempinfo_S[1];
                        black_S.fm_posy_II = tempinfo_S[2] + 1;
                    }
                    else if (direct == 1) {
                        black_S.fm_posx_II = tempinfo_S[1] + 1;
                        black_S.fm_posy_II = tempinfo_S[2];
                    }
                    else if (direct == 2) {
                        black_S.fm_posx_II = tempinfo_S[1] + 1;
                        black_S.fm_posy_II = tempinfo_S[2] + 1;
                    }
                    else if (direct == 3) {
                        black_S.fm_posx_II = tempinfo_S[1] + 1;
                        black_S.fm_posy_II = tempinfo_S[2] - 1;
                    }
                    black_S.fm_case_II = fc_S_II_origin[i];
                    black_S.fm_case_II_length = 5;
                    return true;
                }
            }
        }
    }
    return false;
}

bool TT_S(int posx, int posy) { //function:检测是否满足三三禁手,同时记录三三禁手信息并修改玩家状态
    int tc_S_num = 0;
    for (int direct = 0; direct <= 3; direct++) {
        for (int i = 0; i < 2; i++) {
            if (find_S(tc_S_I[i], 7, direct, posx, posy)) {
                tc_S_num++;
                if (tc_S_num == 1) {
                    black_S.fm_direct = tempinfo_S[0];
                    if (direct == 0) {
                        black_S.fm_posx = tempinfo_S[1];
                        black_S.fm_posy = tempinfo_S[2] + 2;
                    }
                    else if (direct == 1) {
                        black_S.fm_posx = tempinfo_S[1] + 2;
                        black_S.fm_posy = tempinfo_S[2];
                    }
                    else if (direct == 2) {
                        black_S.fm_posx = tempinfo_S[1]+2;
                        black_S.fm_posy = tempinfo_S[2]+2;
                    }
                    else if (direct == 3) {
                        black_S.fm_posx = tempinfo_S[1]+2;
                        black_S.fm_posy = tempinfo_S[2]-2;
                    }
                    black_S.fm_case = tc_S_I_origin;
                    black_S.fm_case_length = 3;
                }
                if (tc_S_num >= 2) {
                    black_S.state = 2;
                    black_S.fm_type = "TT";
                    black_S.fm_direct_II = tempinfo_S[0];
                    if (direct == 0) {
                        black_S.fm_posx_II = tempinfo_S[1];
                        black_S.fm_posy_II = tempinfo_S[2] + 2;
                    }
                    else if (direct == 1) {
                        black_S.fm_posx_II = tempinfo_S[1] + 2;
                        black_S.fm_posy_II = tempinfo_S[2];
                    }
                    else if (direct == 2) {
                        black_S.fm_posx_II = tempinfo_S[1] + 2;
                        black_S.fm_posy_II = tempinfo_S[2] + 2;
                    }
                    else if (direct == 3) {
                        black_S.fm_posx_II = tempinfo_S[1] + 2;
                        black_S.fm_posy_II = tempinfo_S[2] - 2;
                    }
                    black_S.fm_case_II = tc_S_I_origin;
                    black_S.fm_case_II_length = 3;
                    return true;
                }
                if (i == 0)i++;
            }
        }
        for (int i = 0; i < 2; i++) {
            if (find_S(tc_S_II[i], 6, direct, posx, posy)) {
                tc_S_num++;
                if (tc_S_num == 1) {
                    black_S.fm_direct = tempinfo_S[0];
                    if (direct == 0) {
                        black_S.fm_posx = tempinfo_S[1];
                        black_S.fm_posy = tempinfo_S[2] + 1;
                    }
                    else if (direct == 1) {
                        black_S.fm_posx = tempinfo_S[1] + 1;
                        black_S.fm_posy = tempinfo_S[2];
                    }
                    else if (direct == 2) {
                        black_S.fm_posx = tempinfo_S[1] + 1;
                        black_S.fm_posy = tempinfo_S[2] + 1;
                    }
                    else if (direct == 3) {
                        black_S.fm_posx = tempinfo_S[1] + 1;
                        black_S.fm_posy = tempinfo_S[2] - 1;
                    }
                    black_S.fm_case = tc_S_II_origin[i];
                    black_S.fm_case_length = 4;
                }
                if (tc_S_num >= 2) {
                    black_S.state = 2;
                    black_S.fm_type = "TT";
                    black_S.fm_direct_II = tempinfo_S[0];
                    if (direct == 0) {
                        black_S.fm_posx_II = tempinfo_S[1];
                        black_S.fm_posy_II = tempinfo_S[2] + 1;
                    }
                    else if (direct == 1) {
                        black_S.fm_posx_II = tempinfo_S[1] + 1;
                        black_S.fm_posy_II = tempinfo_S[2];
                    }
                    else if (direct == 2) {
                        black_S.fm_posx_II = tempinfo_S[1] + 1;
                        black_S.fm_posy_II = tempinfo_S[2] + 1;
                    }
                    else if (direct == 3) {
                        black_S.fm_posx_II = tempinfo_S[1] + 1;
                        black_S.fm_posy_II = tempinfo_S[2] - 1;
                    }
                    black_S.fm_case_II = tc_S_II_origin[i];
                    black_S.fm_case_II_length = 4;
                    return true;
                }
            }
        }
    }
    return false;
}

int event_get_after_action_S(int player, int posx, int posy) {//玩家落子后，检测发生的事件
    //返回值为0：未获胜；返回值为1：对应玩家获胜；返回值为2：黑方违反禁手规则；返回值为-1：发生异常错误（玩家不是0/1）
    if (player == 0) {
        if (player_win_S(0, posx, posy)) {
            return 1;
        }
        return 0;
    }
    else if (player == 1) {
        if (ruleLC_S_S) {
            if (LC_S(posx, posy)) {
                return 21;
            }
        }
        if (player_win_S(1, posx, posy)) {
            return 1;
        }
        if (ruleFF_S) {
            if (FF_S(posx, posy)) {
                return 22;
            }
        }
        if (ruleTT_S){
            if (TT_S(posx, posy)) {
                return 23;
            }
        }
        return 0;
    }
    return -1;
}



void player_default_S() {//初始化玩家信息
    white_S = player_white_S_S{ 0, -1, -1, -1, win_S[0], 5 };
    black_S = player_black_S_S{ 0,-1,-1,-1,win_S[1],5,"NULL_S",-1,-1,-1,null_S,1,-1,-1,-1,null_S,1 };

}

int Board_SinglePlayer::winner_judging_single(int player, int posx, int posy){//输赢+禁手判断函数（后续需要接入正确的判断函数）
    int temp_12;
    temp_12=event_get_after_action_S(player,posx,posy);
    if (temp_12==0)    return -1;
    else if (temp_12==1)    return player;
    else if (temp_12==2 || temp_12==21 || temp_12==22 || temp_12==23)    return temp_12;
    else if (Chess_count_S>=225)    return 5;
    else {
        //qDebug()<<"ERROR #11";
        return -1;
    }

}



#include "board_review.h"
#include "ui_board_review.h"
#include <QPainter>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QObject>
#include <QFile>
#include <fstream>
#include <iostream>
#include <string>

std::string review_file_name;
int flag_review=0;
int map_x_review=15,map_y_review=15;
int win_line_x[2]={-1,-1},win_line_y[2]={-1,-1},win_line_direct[2]={-1,-1},win_line_length[2]={-1,-1};
int win_line_show=1;

bool ruleLC_review=true;//长连禁手
bool ruleFF_review=true;//四四禁手
bool ruleTT_review=true;//三三禁手

int gamemode_review,Chess_process_review[400][2]={{0}},Chess_count_review,Chess_review_step,Chess_winmode_review;
std::string Chesstime_review;std::fstream f_r;
bool flag_draw_num=1;

Board_Review::Board_Review(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Board_Review)
{
    //初始化禁手序列
    win_line_x[0]=-1;win_line_y[0]=-1;win_line_direct[0]=-1;win_line_length[0]=-1;
    win_line_x[1]=-1;win_line_y[1]=-1;win_line_direct[1]=-1;win_line_length[1]=-1;

    ui->setupUi(this);
    ui->previous->setFont(QFont("华文楷体",16));
    ui->next->setFont(QFont("华文楷体",16));
    ui->step_show->setFont(QFont("华文楷体",16));
    ui->who_win->setFont(QFont("华文楷体",16));
    ui->how_win->setFont(QFont("华文楷体",16));
    ui->chess_information->setFont(QFont("华文楷体",16));
    ui->chess_mode->setFont(QFont("华文楷体",16));
    ui->show_step_do->setFont(QFont("华文楷体",16));
    ui->shezhi_review->setFont(QFont("华文楷体",16));
    ui->rule_show->setFont(QFont("华文楷体",16));
    ui->show_winning_line->setFont(QFont("华文楷体",16));
    ui->show_step_do->setChecked(1);
    flag_draw_num=1;
    ui->show_winning_line->setChecked(1);
    win_line_show=1;
    f_r.open("history/"+review_file_name+".txt",std::ios::in);
    getline(f_r,Chesstime_review);
    f_r>>gamemode_review>>ruleLC_review>>ruleFF_review>>ruleTT_review;
    Chess_count_review=0;
    f_r>>map_x_review>>map_y_review;
    while(f_r>>Chess_process_review[Chess_count_review+1][0]>>Chess_process_review[Chess_count_review+1][1]){
        if (Chess_process_review[Chess_count_review+1][0]==-114 && Chess_process_review[Chess_count_review+1][1]==-514){
            break;
        }
        else if (Chess_process_review[Chess_count_review+1][0]==-20 && Chess_process_review[Chess_count_review+1][1]==-20){
            Chess_process_review[Chess_count_review][0]=0;
            Chess_process_review[Chess_count_review][1]=0;
            Chess_count_review--;
        }
        else{
            Chess_count_review++;
        }
    }
    f_r>>Chess_winmode_review;
    f_r>>win_line_x[0]>>win_line_y[0]>>win_line_direct[0]>>win_line_length[0];
    f_r>>win_line_x[1]>>win_line_y[1]>>win_line_direct[1]>>win_line_length[1];
    f_r.close();
    Chess_review_step=Chess_count_review;
    std::string temp_5;
    temp_5= "第 "+std::to_string(Chess_review_step)+" / "+std::to_string(Chess_count_review)+" 步";
    QString temp_6;
    temp_6=QString::fromStdString(temp_5);
    ui->step_show->setText(temp_6);
    if (gamemode_review==1){
        QString temp_7;
        temp_7=QString::fromStdString("对局复盘-双人对弈- "+Chesstime_review+" 场");
        setWindowTitle(temp_7);
        temp_7=QString::fromStdString("时间："+Chesstime_review);
        ui->chess_information->setText(temp_7);
        ui->chess_mode->setText("双人对弈");
    }
    else if (gamemode_review==0 || gamemode_review==20 || gamemode_review==21){
        QString temp_7;
        temp_7=QString::fromStdString("对局复盘-人机对弈- "+Chesstime_review+" 场");
        setWindowTitle(temp_7);
        temp_7=QString::fromStdString("时间："+Chesstime_review);
        ui->chess_information->setText(temp_7);
        if (gamemode_review==0) ui->chess_mode->setText("人机对弈");
        else if (gamemode_review==20)   ui->chess_mode->setText("人机对弈（玩家执黑）");
        else if (gamemode_review==21)   ui->chess_mode->setText("人机对弈（玩家执白）");
        //else qDebug()<<"ERROR57";
    }

    if (Chess_winmode_review==0){
        ui->who_win->setText("获胜方：白方");
        ui->how_win->setText("获胜原因：白方连成五个棋子");
    }
    else if (Chess_winmode_review==1){
        ui->who_win->setText("获胜方：黑方");
        ui->how_win->setText("获胜原因：黑方连成五个棋子");
    }
    else if (Chess_winmode_review==2){
        ui->who_win->setText("获胜方：白方");
        ui->how_win->setText("获胜原因：黑方违反禁手规则");
    }
    else if (Chess_winmode_review==21){
        ui->who_win->setText("获胜方：白方");
        ui->how_win->setText("获胜原因：黑方违反长连禁手规则");
    }
    else if (Chess_winmode_review==22){
        ui->who_win->setText("获胜方：白方");
        ui->how_win->setText("获胜原因：黑方违反四四禁手规则");
    }
    else if (Chess_winmode_review==23){
        ui->who_win->setText("获胜方：白方");
        ui->how_win->setText("获胜原因：黑方违反三三禁手规则");
    }
    else if (Chess_winmode_review==3){
        ui->who_win->setText("获胜方：白方");
        ui->how_win->setText("获胜原因：黑方认输");
        ui->show_winning_line->hide();//隐藏判定线选项
    }
    else if (Chess_winmode_review==4){
        ui->who_win->setText("获胜方：黑方");
        ui->how_win->setText("获胜原因：白方认输");
        ui->show_winning_line->hide();//隐藏判定线选项
    }
    else if (Chess_winmode_review==5){
        ui->who_win->setText("平局，无人获胜");
        ui->how_win->setText("填满棋盘时没有人违反规则或连成五子");
        ui->show_winning_line->hide();//隐藏判定线选项
    }
    flag_review=1;

    if (ruleFF_review==0 && ruleTT_review==0 && ruleLC_review==0)    ui->rule_show->setText("禁手信息：无");
    else{
        std::string temp_201="禁手信息：";
        if (ruleLC_review==1)  temp_201+="长连 ";
        if (ruleFF_review==1)  temp_201+="四四 ";
        if (ruleTT_review==1)  temp_201+="三三 ";
        ui->rule_show->setText(QString::fromStdString(temp_201));
    }


    //调整窗口大小和按钮位置
    if (Chess_winmode_review==5)    this->resize(map_x_review*40+410,std::max(map_y_review*40+40,400));
    else this->resize(map_x_review*40+375,std::max(map_y_review*40+40,400));
    int button_start_x,button_start_y;
    button_start_x=map_x_review*40+40;
    button_start_y=35;
    ui->chess_mode->setGeometry(QRect(button_start_x,button_start_y,301,19));
    ui->rule_show->setGeometry(QRect(button_start_x,button_start_y+30,301,19));
    ui->chess_information->setGeometry(QRect(button_start_x,button_start_y+60,301,19));
    ui->who_win->setGeometry(QRect(button_start_x,button_start_y+90,301,19));
    ui->how_win->setGeometry(QRect(button_start_x,button_start_y+120,401,19));
    ui->shezhi_review->setGeometry(QRect(button_start_x,button_start_y+210,351,19));
    ui->show_step_do->setGeometry(QRect(button_start_x,button_start_y+240,211,23));
    ui->show_winning_line->setGeometry(QRect(button_start_x,button_start_y+270,211,23));
    button_start_y+=161;
    ui->previous->setGeometry(QRect(button_start_x+1,button_start_y,95,32));
    ui->step_show->setGeometry(QRect(button_start_x+103,button_start_y,125,32));
    ui->next->setGeometry(QRect(button_start_x+232,button_start_y,95,32));


    //禁止修改窗口大小
    if (Chess_winmode_review==5){
        this->setMinimumSize(map_x_review*40+410,std::max(map_y_review*40+40,400));
        this->setMaximumSize(map_x_review*40+410,std::max(map_y_review*40+40,400));
    }
    else{
        this->setMinimumSize(map_x_review*40+375,std::max(map_y_review*40+40,400));
        this->setMaximumSize(map_x_review*40+375,std::max(map_y_review*40+40,400));
    }

    setWindowIcon(QIcon(":/icon/favicon.ico"));

}

Board_Review::~Board_Review()
{
    delete ui;
}

void Board_Review::closeEvent(QCloseEvent *event){
    flag_review=0;
}



void Board_Review::paintEvent(QPaintEvent *){//绘制五子棋棋盘
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    painter.drawPixmap(0,-10,2400,1124,QPixmap(":/pictures/board_background.png"));//画背景图片

    ////qDebug()<<map_x_review<<" "<<map_y_review;
    //画背景
    QPen Line,Point,Square;
    QBrush Square_brush;
    Square.setColor(Qt::yellow);
    painter.setPen(Square);
    Square_brush.setColor(Qt::yellow);
    Square_brush.setStyle(Qt::SolidPattern);
    painter.setBrush(Square_brush);
    QRect Background(20,20,map_x_review*40,map_y_review*40);
    painter.drawRect(Background);
    /*for (int i=0;i<21;i++){
        for (int j=0;j<21;j++){
            std::cout<<map[i][j];
        }
        std::cout<<std::endl;
    }*/
    //画特征点
    if (map_x_review==15 && map_y_review==15){
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
    for (int i=1;i<=map_x_review;i++){
        QLine Line_2(i*40,40,i*40,map_y_review*40);
        painter.drawLine(Line_2);
    }
    for (int i=1;i<=map_y_review;i++){
        QLine Line_1(40,i*40,map_x_review*40,i*40);
        painter.drawLine(Line_1);
    }
    Line.setWidth(4);
    Line.setBrush(Qt::darkBlue);
    painter.setPen(Line);
    QLine Line_3(18,18,18,map_y_review*40+20);
    painter.drawLine(Line_3);
    QLine Line_4(18,18,map_x_review*40+20,18);
    painter.drawLine(Line_4);
    QLine Line_5(18,map_y_review*40+20,map_x_review*40+20,map_y_review*40+20);
    painter.drawLine(Line_5);
    QLine Line_6(map_x_review*40+20,18,map_x_review*40+20,map_y_review*40+20);
    painter.drawLine(Line_6);

    drawChess_review();
    ////qDebug()<< "FLAG_1 "<<Chess_count;

}

int Chess_r_l_r=18;

void Board_Review::drawChess_review(){//绘制棋子

    QPainter painter(this);
    QPen Chess_Pen_r;
    Chess_Pen_r.setColor(Qt::red);
    Chess_Pen_r.setWidth(1);
    painter.setPen(Chess_Pen_r);
    painter.setFont(QFont("华文楷体",16,QFont::Bold));
    for(int i=1;i<=Chess_review_step;i++){
        if (i%2==0){//白棋

            QRect chess_white_large(Chess_process_review[i][1]*40-Chess_r_l_r,Chess_process_review[i][0]*40-Chess_r_l_r,2*Chess_r_l_r,2*Chess_r_l_r);

            painter.drawPixmap(chess_white_large,QPixmap(":/pictures/chess_white.png"));
            if (flag_draw_num==1){
                painter.drawText(Chess_process_review[i][1]*40-Chess_r_l_r,Chess_process_review[i][0]*40-Chess_r_l_r,2*Chess_r_l_r,2*Chess_r_l_r,Qt::AlignCenter,QString::fromStdString(std::to_string(i)));
            }
        }
        else if (i%2==1){//黑棋
            QRect chess_black_large(Chess_process_review[i][1]*40-Chess_r_l_r,Chess_process_review[i][0]*40-Chess_r_l_r,2*Chess_r_l_r,2*Chess_r_l_r);
            painter.drawPixmap(chess_black_large,QPixmap(":/pictures/chess_black.png"));
            if (flag_draw_num==1){
                painter.drawText(Chess_process_review[i][1]*40-Chess_r_l_r,Chess_process_review[i][0]*40-Chess_r_l_r,2*Chess_r_l_r,2*Chess_r_l_r,Qt::AlignCenter,QString::fromStdString(std::to_string(i)));
            }
        }
    }
    if (Chess_review_step>0 && flag_draw_num==0){//为最后落下的棋子绘制标识
        Chess_Pen_r.setWidth(6);
        Chess_Pen_r.setColor(Qt::red);
        painter.setPen(Chess_Pen_r);
        painter.drawPoint(Chess_process_review[Chess_review_step][1]*40,Chess_process_review[Chess_review_step][0]*40);
    }

    //赢棋后绘制提示线
    if (win_line_show==1){
        if (Chess_winmode_review==0){//白连五
            //qDebug()<<"FLAG601";
            //qDebug()<<"白子获胜序列起点："<<win_line_x[0]<<" "<<win_line_y[0]<<" "<<win_line_direct[0];

            int des_d_x,des_d_y;

            des_d_x=win_line_x[0];
            des_d_y=win_line_y[0];

            if (win_line_direct[0]==0 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_y+=4;
            if (win_line_direct[0]==1 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_x+=4;
            if (win_line_direct[0]==3)   des_d_y-=8;
            //qDebug()<<"白子获胜序列终点："<<des_d_x<<" "<<des_d_y;
            QPen winpaint;
            winpaint.setWidth(10);
            winpaint.setColor(Qt::green);
            painter.setPen(winpaint);
            QLine Line_11(win_line_y[0]*40,win_line_x[0]*40,des_d_y*40,des_d_x*40);
            painter.drawLine(Line_11);
        }
        else if (Chess_winmode_review==1){//黑连五

            int des_d_x,des_d_y;
            des_d_x=win_line_x[0];
            des_d_y=win_line_y[0];

            if (win_line_direct[0]==0 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_y+=4;
            if (win_line_direct[0]==1 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_x+=4;
            if (win_line_direct[0]==3)   des_d_y-=8;

            QPen winpaint;
            winpaint.setWidth(10);
            winpaint.setColor(Qt::green);
            painter.setPen(winpaint);
            QLine Line_11(win_line_y[0]*40,win_line_x[0]*40,des_d_y*40,des_d_x*40);
            painter.drawLine(Line_11);
        }
        else if (Chess_winmode_review==21){//长连禁手
            int des_d_x,des_d_y;
            des_d_x=win_line_x[0];
            des_d_y=win_line_y[0];

            if (win_line_direct[0]==0 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_y+=win_line_length[0]-1;
            if (win_line_direct[0]==1 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_x+=win_line_length[0]-1;
            if (win_line_direct[0]==3)   des_d_y-=2*win_line_length[0]-2;

            QPen winpaint;
            winpaint.setWidth(10);
            winpaint.setColor(Qt::red);
            painter.setPen(winpaint);
            QLine Line_11(win_line_y[0]*40,win_line_x[0]*40,des_d_y*40,des_d_x*40);
            painter.drawLine(Line_11);
        }
        else if (Chess_winmode_review==22){//四四禁手
            int des_d_x,des_d_y;
            des_d_x=win_line_x[0];
            des_d_y=win_line_y[0];

            if (win_line_direct[0]==0 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_y+=win_line_length[0]-1;
            if (win_line_direct[0]==1 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_x+=win_line_length[0]-1;
            if (win_line_direct[0]==3)   des_d_y-=2*win_line_length[0]-2;

            QPen winpaint;
            winpaint.setWidth(10);
            winpaint.setColor(Qt::red);
            painter.setPen(winpaint);
            QLine Line_11(win_line_y[0]*40,win_line_x[0]*40,des_d_y*40,des_d_x*40);
            painter.drawLine(Line_11);

            des_d_x=win_line_x[1];
            des_d_y=win_line_y[1];

            if (win_line_direct[1]==0 || win_line_direct[1]==2 || win_line_direct[1]==3)    des_d_y+=win_line_length[1]-1;
            if (win_line_direct[1]==1 || win_line_direct[1]==2 || win_line_direct[1]==3)    des_d_x+=win_line_length[1]-1;
            if (win_line_direct[1]==3)   des_d_y-=2*win_line_length[1]-2;

            winpaint.setWidth(10);
            winpaint.setColor(Qt::red);
            painter.setPen(winpaint);
            QLine Line_12(win_line_y[1]*40,win_line_x[1]*40,des_d_y*40,des_d_x*40);
            painter.drawLine(Line_12);
        }
        else if (Chess_winmode_review==23){//三三禁手
            int des_d_x,des_d_y;
            des_d_x=win_line_x[0];
            des_d_y=win_line_y[0];

            if (win_line_direct[0]==0 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_y+=win_line_length[0]-1;
            if (win_line_direct[0]==1 || win_line_direct[0]==2 || win_line_direct[0]==3)    des_d_x+=win_line_length[0]-1;
            if (win_line_direct[0]==3)   des_d_y-=2*win_line_length[0]-2;

            QPen winpaint;
            winpaint.setWidth(10);
            winpaint.setColor(Qt::red);
            painter.setPen(winpaint);
            QLine Line_11(win_line_y[0]*40,win_line_x[0]*40,des_d_y*40,des_d_x*40);
            painter.drawLine(Line_11);

            des_d_x=win_line_x[1];
            des_d_y=win_line_y[1];

            if (win_line_direct[1]==0 || win_line_direct[1]==2 || win_line_direct[1]==3)    des_d_y+=win_line_length[1]-1;
            if (win_line_direct[1]==1 || win_line_direct[1]==2 || win_line_direct[1]==3)    des_d_x+=win_line_length[1]-1;
            if (win_line_direct[1]==3)   des_d_y-=2*win_line_length[1]-2;

            winpaint.setWidth(10);
            winpaint.setColor(Qt::red);
            painter.setPen(winpaint);
            QLine Line_12(win_line_y[1]*40,win_line_x[1]*40,des_d_y*40,des_d_x*40);
            painter.drawLine(Line_12);
        }
    }
}


void Board_Review::on_previous_clicked()
{
    if (Chess_review_step>0){
        Chess_review_step--;
        std::string temp_5;
        temp_5= "第 "+std::to_string(Chess_review_step)+" / "+std::to_string(Chess_count_review)+" 步";
        QString temp_6;
        temp_6=QString::fromStdString(temp_5);
        ui->step_show->setText(temp_6);
        update();
    }
    else{
        QString Title="操作失败";
        QString Info="当前已是第0步！";
        QMessageBox::warning(this,Title,Info);
    }
}


void Board_Review::on_next_clicked()
{
    if (Chess_review_step<Chess_count_review){
        Chess_review_step++;
        std::string temp_5;
        temp_5= "第 "+std::to_string(Chess_review_step)+" / "+std::to_string(Chess_count_review)+" 步";
        QString temp_6;
        temp_6=QString::fromStdString(temp_5);
        ui->step_show->setText(temp_6);
        update();
    }
    else{
        QString Title="操作失败";
        QString Info="当前已是最后一步！";
        QMessageBox::warning(this,Title,Info);
    }
}


void Board_Review::on_show_step_do_clicked(bool checked)
{
    flag_draw_num=checked;
    update();
}


void Board_Review::on_show_winning_line_clicked(bool checked)
{
    win_line_show=checked;
    update();
}


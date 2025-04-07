#include "mainwindow.h"
#include "board.h"
#include "ui_mainwindow.h"
#include "board_singleplayer.h"
#include "board_review.h"

#include "history.h"
#include <QMessageBox>
#include <QCloseEvent>
#include <windows.h>
#include "rule.h"
#include "shezhi.h"
#include <QString>
#include <QDir>
#include <fstream>
#include <iostream>
#include <filesystem>




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icon/favicon.ico"));

    ui->exit->setFont(QFont("华文楷体",16));
    ui->rule->setFont(QFont("华文楷体",16));
    ui->doubleplayer->setFont(QFont("华文楷体",16));
    ui->shezhi->setFont(QFont("华文楷体",16));
    ui->singleplayer->setFont(QFont("华文楷体",16));
    ui->history->setFont(QFont("华文楷体",16));

    std::fstream f;
    f.open("flag_gaming.txt",std::ios::in);
    f>>flag_board_gaming_mode;
    f.close();
    //qDebug()<<flag_board_gaming_mode;

    f.open("jinshou_info.txt",std::ios::in);
    f>>ruleLC_save>>ruleFF_save>>ruleTT_save;
    f.close();

    f.open("board_size.txt",std::ios::in);
    f>>map_x>>map_y;
    f.close();

    f.open("ai_mode.txt",std::ios::in);
    f>>ai_mode;
    f.close();

    std::filesystem::path currentPath = std::filesystem::current_path();
    currentPath+="/history/";
    if (std::filesystem::exists(currentPath)){
        //qDebug()<<"history文件夹已经存在";
    }
    else{
        std::filesystem::create_directory(currentPath);
    }
    currentPath+="unfinished/";
    if (std::filesystem::exists(currentPath)){
        //qDebug()<<"unfinished文件夹已经存在";
    }
    else{
        std::filesystem::create_directory(currentPath);
    }
    currentPath+="double/";
    if (std::filesystem::exists(currentPath)){
        //qDebug()<<"double文件夹已经存在";
    }
    else{
        std::filesystem::create_directory(currentPath);
    }
    currentPath=currentPath.parent_path();currentPath=currentPath.parent_path();
    //qDebug()<<currentPath;
    currentPath+="/single/";
    //qDebug()<<currentPath;
    if (std::filesystem::exists(currentPath)){
        //qDebug()<<"single文件夹已经存在";
    }
    else{
        std::filesystem::create_directory(currentPath);
    }


    if (flag_board_gaming_mode==1){//恢复关闭时未保存的棋局
        QMessageBox choose(QMessageBox::Question,QString(tr("提示")),QString(tr("检测到上次游戏意外退出，是否恢复未保存的棋局？ \n未保存的棋局：双人游戏")),QMessageBox::Yes| QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("恢复"));
        choose.button(QMessageBox::No)->setText(tr("不恢复"));
        int temp_1=choose.exec();
        if(temp_1== QMessageBox::Yes){
            flag_board_gaming_mode=4;
            //qDebug()<<"恢复双人棋局";//添加打开棋盘并恢复的内容

            Board *BoardWindow =new Board;
            BoardWindow->setAttribute(Qt::WA_QuitOnClose,false);
            BoardWindow->show();
        }
        else{
            flag_board_gaming_mode=-1;
            std::fstream f;
            f.open("flag_gaming.txt",std::ios::out);
            f<<"-1"<<std::endl;
            f.close();
        }
    }
    else if (flag_board_gaming_mode==0){
        QMessageBox choose(QMessageBox::Question,QString(tr("提示")),QString(tr("上次游戏意外退出，是否恢复未保存的棋局？ \n未保存的棋局：单人游戏")),QMessageBox::Yes| QMessageBox::No);
        choose.button(QMessageBox::Yes)->setText(tr("恢复"));
        choose.button(QMessageBox::No)->setText(tr("不恢复"));
        int temp_1=choose.exec();
        if(temp_1== QMessageBox::Yes){
            flag_board_gaming_mode=3;
            //qDebug()<<"恢复单人棋局";//添加打开棋盘并恢复的内容
            Board *BoardWindow =new Board;
            BoardWindow->setAttribute(Qt::WA_QuitOnClose,false);
            BoardWindow->show();
        }
        else{
            flag_board_gaming_mode=-1;
            std::fstream f;
            f.open("flag_gaming.txt",std::ios::out);
            f<<"-1"<<std::endl;
            f.close();
        }
    }


    //禁止修改窗口大小
    this->setMinimumSize(541,514);
    this->setMaximumSize(541,514);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_rule_clicked()//规则说明
{

    Rule *RuleWindow = new Rule;
    RuleWindow->setAttribute(Qt::WA_QuitOnClose,false);
    RuleWindow->show();
}

void MainWindow::closeEvent(QCloseEvent *event){//关闭时（右上角叉号+退出）弹出提示

    QSoundEffect sound1;
    sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
    sound1.play();
    QString Title ="请确认";
    QString Info="是否确认关闭游戏？";
    QMessageBox :: StandardButton   defaultBtn=QMessageBox::NoButton;
    QMessageBox :: StandardButton   result;
    result = QMessageBox::question(this,Title,Info,QMessageBox::Yes | QMessageBox ::No, defaultBtn);

    if (result == QMessageBox::Yes){

        mainwindow_exit=1;
        event->accept();
    }
    else{

        event->ignore();
    }
}


void MainWindow::on_doubleplayer_clicked()//双人游戏
{
    QSoundEffect sound1;
    sound1.setSource(QUrl("qrc:/music/menuconfirm.wav"));
    sound1.play();
    if (if_settings_open==1){
        QString Title="警告";
        QString Info="请先完成设置再进行游戏！";
        QMessageBox::warning(this,Title,Info);
    }
    else if (flag_board_gaming_mode==-1){
        flag_board_gaming_mode=1;
        Board *BoardWindow =new Board;
        BoardWindow->setAttribute(Qt::WA_QuitOnClose,false);
        BoardWindow->show();
    }
    else if (flag_board_gaming_mode==0){
        QString Title="警告";
        QString Info="您有一局单人游戏正在进行中，请先结束正在进行的游戏！";
        QMessageBox::warning(this,Title,Info);
    }
    else{
        QString Title="警告";
        QString Info="您有一局双人游戏正在进行中，请先结束正在进行的游戏！";
        QMessageBox::warning(this,Title,Info);
    }
}




void MainWindow::on_shezhi_clicked()//设置
{

    if (if_settings_open==1){
        QString Title="警告";
        QString Info="您已经打开设置界面了";
        QMessageBox::warning(this,Title,Info);
    }
    else if (flag_board_gaming_mode==-1){
        if_settings_open=1;
        shezhi *shezhiWindow = new shezhi;
        shezhiWindow->setAttribute(Qt::WA_QuitOnClose,false);
        shezhiWindow->show();
    }
    else if (flag_board_gaming_mode==0){
        QString Title="警告";
        QString Info="您有一局单人游戏正在进行中，请先结束正在进行的游戏！";
        QMessageBox::warning(this,Title,Info);
    }
    else{
        QString Title="警告";
        QString Info="您有一局双人游戏正在进行中，请先结束正在进行的游戏！";
        QMessageBox::warning(this,Title,Info);
    }
}


void MainWindow::on_singleplayer_clicked()//单人游戏
{


    if (if_settings_open==1){
        QString Title="警告";
        QString Info="请先完成设置再进行游戏！";
        QMessageBox::warning(this,Title,Info);
    }
    else if (flag_board_gaming_mode==-1){
        flag_board_gaming_mode=0;
        Board *BoardWindow =new Board;
        BoardWindow->setAttribute(Qt::WA_QuitOnClose,false);
        BoardWindow->show();
    }
    else if (flag_board_gaming_mode==0){
        QString Title="警告";
        QString Info="您有一局单人游戏正在进行中，请先结束正在进行的游戏！";
        QMessageBox::warning(this,Title,Info);
    }
    else{
        QString Title="警告";
        QString Info="您有一局双人游戏正在进行中，请先结束正在进行的游戏！";
        QMessageBox::warning(this,Title,Info);
    }
}


void MainWindow::on_history_clicked()
{

    History *HistoryWindow = new History;
    HistoryWindow->setAttribute(Qt::WA_QuitOnClose,false);
    HistoryWindow->show();
}


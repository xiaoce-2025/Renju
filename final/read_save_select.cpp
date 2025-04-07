#include "read_save_select.h"
#include "ui_read_save_select.h"
#include "board.h"
#include "basic_variable.h"
#include <QFileDialog>
#include <QMessageBox>
#include <string>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QDir>

int flag_read_save=-1;//0表示单人游戏调用，1表示双人游戏调用
int temp_10001=0;

Read_save_select::Read_save_select(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Read_save_select)
{
    ui->setupUi(this);
    ui->HistoryList->setFont(QFont("华文楷体",16));
    ui->pushButton->setFont(QFont("华文楷体",16));
    ui->label->setFont(QFont("华文楷体",16));
    ui->pushButton_2->setFont(QFont("华文楷体",16));
    if (flag_read_save==0){
        QDir dir("history/unfinished/single/");
        QStringList filters;
        filters << "*.txt";
        dir.setNameFilters(filters);
        QStringList fileList = dir.entryList(filters, QDir::Files);
        std::string temp_9;
        temp_10001=0;
        foreach(QString temp_8,fileList){
            temp_9=temp_8.toStdString();
            if (temp_9.length()>=24)   temp_9.erase(24,4);
            fileList[temp_10001]=QString::fromStdString(temp_9);
            temp_10001++;
        }
        //qDebug()<<fileList;
        ui->HistoryList->addItems(fileList);
        setWindowTitle("选择一个未完成的单人游戏");
    }
    else if (flag_read_save==1){
        QDir dir("history/unfinished/double/");
        QStringList filters;
        filters << "*.txt";
        dir.setNameFilters(filters);
        QStringList fileList = dir.entryList(filters, QDir::Files);
        std::string temp_9;
        temp_10001=0;
        foreach(QString temp_8,fileList){
            temp_9=temp_8.toStdString();
            if (temp_9.length()>=24)   temp_9.erase(24,4);
            fileList[temp_10001]=QString::fromStdString(temp_9);
            temp_10001++;
        }
        //qDebug()<<fileList;
        ui->HistoryList->addItems(fileList);
        setWindowTitle("选择一个未完成的双人游戏");
    }

    setWindowIcon(QIcon(":/icon/favicon.ico"));

    //禁止修改窗口大小
    this->setMinimumSize(727,458);
    this->setMaximumSize(727,458);

}

Read_save_select::~Read_save_select()
{
    delete ui;
}

void Read_save_select::on_pushButton_clicked()
{
    if (temp_10001==0){
        QString Title="警告";
        QString Info="当前没有可以打开的历史记录";
        QMessageBox::warning(this,Title,Info);
        return;
    }
    if (flag_read_save==0){
        //if (flag_gaming_single==0 && flag_gaming_double==0){
            flag_board_gaming_mode=5;
        //qDebug()<<"FL4 "<<flag_board_gaming_mode;
        //Board *BoardWindow =new Board;
        //BoardWindow->show();
        close();
        /*}
        else if (flag_gaming_double==0){
            QString Title="警告";
            QString Info="您有一局单人游戏正在进行中，请先结束正在进行的游戏！";
            QMessageBox::warning(this,Title,Info);
        }
        else{
            QString Title="警告";
            QString Info="您有一局双人游戏正在进行中，请先结束正在进行的游戏！";
            QMessageBox::warning(this,Title,Info);
        }*/
    }
    else if (flag_read_save==1){
        //if (flag_gaming_double==0 && flag_gaming_single==0){

            flag_board_gaming_mode=6;

        //qDebug()<<"FL5 "<<flag_board_gaming_mode;
            //Board *BoardWindow =new Board;
            //BoardWindow->show();
            close();
        /*}
        else if (flag_gaming_double==0){
            QString Title="警告";
            QString Info="您有一局单人游戏正在进行中，请先结束正在进行的游戏！";
            QMessageBox::warning(this,Title,Info);
        }
        else{
            QString Title="警告";
            QString Info="您有一局双人游戏正在进行中，请先结束正在进行的游戏！";
            QMessageBox::warning(this,Title,Info);
        }*/
    }
}


void Read_save_select::on_HistoryList_currentTextChanged(const QString &arg1)
{
    QPixmap board_preview_pic;
    if (flag_read_save==0){
        savepath_double=arg1.toStdString();
        board_preview_pic.load(QString::fromStdString("history/unfinished/single/"+savepath_double+".png"));
        if (savepath_double.length()>=24)   savepath_double.erase(24,4);
    }
    else if (flag_read_save==1){
        savepath_double=arg1.toStdString();
        board_preview_pic.load(QString::fromStdString("history/unfinished/double/"+savepath_double+".png"));
        if (savepath_double.length()>=24)   savepath_double.erase(24,4);
    }


    board_preview_pic=board_preview_pic.scaled(241,241,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->board_preview->setPixmap(board_preview_pic);
}

void Read_save_select::closeEvent(QCloseEvent *event){
    Board *BoardWindow =new Board;
    BoardWindow->setAttribute(Qt::WA_QuitOnClose,false);
    BoardWindow->show();
}


void Read_save_select::on_pushButton_2_clicked()
{
    close();
}


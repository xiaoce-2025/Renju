#include "history.h"
#include "ui_history.h"
#include "board_review.h"
#include "basic_variable.h"
#include <QFileDialog>
#include <string>
#include <QComboBox>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QDir>

int temp_flag_111=-1;
int temp_10=0;
History::History(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::History)
{
    temp_flag_111=-1;
    ui->setupUi(this);
    ui->HistoryList->setFont(QFont("华文楷体",16));
    ui->pushButton->setFont(QFont("华文楷体",16));
    ui->label->setFont(QFont("华文楷体",16));
    ui->save_delete->setFont(QFont("华文楷体",16));
    ui->pushButton_2->setFont(QFont("华文楷体",16));
    QDir dir("history/");
    QStringList filters;
    filters << "*.txt";
    dir.setNameFilters(filters);
    QStringList fileList = dir.entryList(filters, QDir::Files);
    std::string temp_9;
    temp_10=0;
    foreach(QString temp_8,fileList){
        temp_9=temp_8.toStdString();
        if (temp_9.length()>=24)    temp_9.erase(24,4);
        fileList[temp_10]=QString::fromStdString(temp_9);
        temp_10++;
    }
    //qDebug()<<fileList;
    ui->save_delete->hide();
    ui->HistoryList->addItems(fileList);

    //禁止修改窗口大小
    this->setMinimumSize(620,460);
    this->setMaximumSize(620,460);

    setWindowIcon(QIcon(":/icon/favicon.ico"));
}

History::~History()
{
    delete ui;
}



void History::on_pushButton_clicked()
{
    if (flag_review==0){
        if (temp_10!=0){
        Board_Review *Board_ReviewWindow = new Board_Review;
        Board_ReviewWindow->setAttribute(Qt::WA_QuitOnClose,false);
        Board_ReviewWindow->show();
        close();
        }
        else{
            QString Title="警告";
            QString Info="当前没有可以打开的历史记录";
            QMessageBox::warning(this,Title,Info);
        }
    }
    else{
        QString Title="警告";
        QString Info="当前已有一局打开的历史记录，请先关闭已打开的历史记录！";
        QMessageBox::warning(this,Title,Info);
    }
}


void History::on_HistoryList_currentTextChanged(const QString &arg1)
{
    ui->save_delete->show();
    review_file_name=arg1.toStdString();
    QPixmap board_preview_pic;
    board_preview_pic.load(QString::fromStdString("history/"+review_file_name+".png"));
    board_preview_pic=board_preview_pic.scaled(241,241,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    ui->board_preview->setPixmap(board_preview_pic);
    //qDebug()<<review_file_name;
}


void History::on_save_delete_clicked()
{
    if (flag_review!=0){
        QString Title="无法执行删除操作";
        QString Info="当前已有一局打开的历史记录，请先关闭已打开的历史记录再执行删除操作！";
        QMessageBox::warning(this,Title,Info);
        return ;
    }
    if (flag_board_gaming_mode!=-1){
        QString Title="无法执行删除操作";
        QString Info="当前有对局正在进行中，请关闭对局界面再执行删除操作！";
        QMessageBox::warning(this,Title,Info);
        return ;
    }
    QMessageBox choose(QMessageBox::Question,QString(tr("提示")),QString(tr("您确定要删除该次棋局记录吗？ \n棋局记录一经删除不可恢复，请谨慎操作 ")),QMessageBox::Yes| QMessageBox::No);
    choose.button(QMessageBox::Yes)->setText(tr("删除"));
    choose.button(QMessageBox::No)->setText(tr("取消"));
    int temp_15=choose.exec();
    if (temp_15 == QMessageBox::Yes){
        std::filesystem::remove("history/"+review_file_name+".txt");
        std::filesystem::remove("history/"+review_file_name+".png");
        ui->HistoryList->removeItem(temp_flag_111);
        temp_10--;
    }

    if (temp_10==0){
        ui->save_delete->hide();
    }
}


void History::on_HistoryList_currentIndexChanged(int index)
{
    temp_flag_111=index;
}


void History::on_pushButton_2_clicked()
{
    close();
}


#include "shezhi.h"
#include "ui_shezhi.h"
#include "basic_variable.h"
#include <QDir>
#include <QUrl>
#include <QFileDialog>
#include <QString>
#include <QMediaPlayer>
#include <fstream>
#include <iostream>
#include <QDebug>
#include <QMessageBox>

QString background_music_path;
std::string temp_1000011="0";
bool rule_TT=1,rule_LC=1,rule_FF=1;
int flag_background_music_1=-1;
int choose_x=15,choose_y=15;
int flag_just_open=0;
int ai_mode_choose=0;

shezhi::shezhi(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::shezhi)
{
    temp_1000011="0";
    ui->setupUi(this);
    ui->background_music_list->hide();
    ui->music_name->hide();
    ui->reset->setFont(QFont("华文楷体",12));
    ui->cancel->setFont(QFont("华文楷体",12));
    ui->confirm->setFont(QFont("华文楷体",12));
    ui->label->setFont(QFont("华文楷体",12));
    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    audioOutput->setVolume(0.2);

    flag_just_open=1;

    ui->select_x->setCurrentIndex(map_x-1);
    ui->select_y->setCurrentIndex(map_y-1);

    std::fstream f;
    QString temp_101;
    std::string temp_102;
    int temp_103;
    f.open("music_background.txt",std::ios::in);
    f>>temp_103;
    getline(f,temp_102);
    getline(f,temp_102);
    f.close();


    int len_temp_1;
    len_temp_1=temp_102.length();
    temp_102.erase(len_temp_1,1);
    temp_101=QString::fromStdString(temp_102);
    background_music_path=temp_101;

    if (temp_103==0){//默认背景音乐

        ui->music_settled->click();
        temp_1000011=temp_102;
        ui->background_music_list->setCurrentText(temp_101);
    }
    else if (temp_103==1){//自定义背景音乐

        ui->user_music->click();
        ui->music_name->setText(temp_101);
    }
    if (temp_102=="Wake"){
        player->setSource(QUrl("qrc:/music/background_music/Wake.mp3"));
        player->play();
    }

    rule_FF=ruleFF_save;
    rule_TT=ruleTT_save;
    rule_LC=ruleLC_save;

    if (rule_FF==0 && rule_TT==0 && rule_LC==0){
        ui->jinshou_false->click();
        ui->LC->hide();
        ui->FF->hide();
        ui->TT->hide();
    }
    else{
        ui->jinshou_true->click();
        rule_FF=ruleFF_save;
        rule_TT=ruleTT_save;
        rule_LC=ruleLC_save;
        ui->LC->setChecked(rule_LC);
        ui->FF->setChecked(rule_FF);
        ui->TT->setChecked(rule_TT);
    }
    flag_just_open=0;

    ai_mode_choose=ai_mode;
    if(ai_mode_choose==0){
        ui->radioButton->setChecked(1);
    }
    else if(ai_mode_choose==11){
        ui->radioButton_2->setChecked(1);
    }
    else if(ai_mode_choose==12){
        ui->radioButton_3->setChecked(1);
    }
    else if(ai_mode_choose==13){
        ui->radioButton_4->setChecked(1);
    }


    //禁止修改窗口大小
    this->setMinimumSize(549,495);
    this->setMaximumSize(549,495);

    setWindowIcon(QIcon(":/icon/favicon.ico"));
}

shezhi::~shezhi()
{
    delete ui;
}


void shezhi::closeEvent(QCloseEvent *event){//关闭时关闭背景音乐预览
    if_settings_open=0;
    player->stop();
}

int reset_music=0;


void shezhi::on_music_settled_clicked()//默认背景音乐
{
    flag_background_music_1=0;
    player->stop();
    ui->music_name->hide();
    ui->background_music_list->show();
    if (temp_1000011=="0" || reset_music==1){
        player->setSource(QUrl("qrc:/music/background_music/Wake.mp3"));
        reset_music=0;
    }
    else if (reset_music==0){
        player->setSource(QUrl("qrc:/music/background_music/"+QString::fromStdString(temp_1000011)+".mp3"));
    }
    player->play();
}


void shezhi::on_background_music_list_currentTextChanged(const QString &arg1)//选择并预览默认背景音乐
{
    player->setSource(QUrl("qrc:/music/background_music/"+arg1+".mp3"));
    flag_background_music_1=0;
    background_music_path=arg1;
    //qDebug()<<arg1;
    player->play();
}




void shezhi::on_user_music_clicked()//自定义背景音乐
{
    ui->background_music_list->hide();
    if (flag_just_open==0){
        player->stop();

        QString curPath = QDir :: currentPath();
        QString dlgTitle = "选择一个文件" ;
        QString filter="音频文件(*.mp3)";
        background_music_path = QFileDialog ::getOpenFileName(this,dlgTitle,curPath,filter);
        if (!background_music_path.isEmpty()){
            ui->music_name->setText(background_music_path);
            ui->music_name->show();
            flag_background_music_1=1;
        }
    }
    else if (flag_just_open==1){
        ui->music_name->setText(background_music_path);
        ui->music_name->show();
        flag_background_music_1=1;
    }
    else    //qDebug()<<"ERROR61";
    //在设置界面对背景音乐进行预览
    player->setSource(background_music_path);
    player->play();
}

void setting_save(){//将设置内容写入本地文档
    std::fstream f;
    f.open("music_background.txt",std::ios::out);
    f<<flag_background_music_1<<std::endl;
    f<<background_music_path.toStdString()<<std::endl;
    f.close();

    map_x=choose_x;
    map_y=choose_y;
    f.open("board_size.txt",std::ios::out);
    f<<choose_x<<" "<<choose_y<<std::endl;
    f.close();

    ruleFF_save=rule_FF;
    ruleTT_save=rule_TT;
    ruleLC_save=rule_LC;
    f.open("jinshou_info.txt",std::ios::out);
    f<<rule_LC<<" "<<rule_FF<<" "<<rule_TT<<std::endl;
    f.close();
    ai_mode=ai_mode_choose;
    f.open("ai_mode.txt",std::ios::out);
    f<<ai_mode<<std::endl;
    f.close();
}



void shezhi::on_reset_clicked()//恢复默认设置
{
    reset_music=1;
    ui->jinshou_true->click();
    ui->select_x->setCurrentIndex(14);
    ui->select_y->setCurrentIndex(14);
    ui->music_settled->click();
    ui->background_music_list->setCurrentIndex(0);
    background_music_path=QString::fromStdString("Wake");
    rule_FF=1;
    rule_TT=1;
    rule_LC=1;
    choose_y=15;
    choose_x=15;
    ui->radioButton->setChecked(1);
    ai_mode_choose=0;
}


void shezhi::on_confirm_clicked()//将相关内容写入本地文档并关闭窗口
{
    setting_save();
    player->stop();
    close();
}






void shezhi::on_select_y_currentIndexChanged(int index)
{
    choose_y=index+1;
    //qDebug()<<choose_y;
}


void shezhi::on_select_x_currentIndexChanged(int index)
{
    choose_x=index+1;
    //qDebug()<<choose_x;
}


void shezhi::on_jinshou_true_clicked()
{
    ui->LC->show();
    ui->LC->setChecked(1);

    ui->TT->show();
    ui->TT->setChecked(1);

    ui->FF->show();
    ui->FF->setChecked(1);

    rule_FF=1;
    rule_TT=1;
    rule_LC=1;
}


void shezhi::on_jinshou_false_clicked()
{
    ui->LC->hide();
    ui->TT->hide();
    ui->FF->hide();
    ui->radioButton->setChecked(1);
    rule_FF=0;
    rule_TT=0;
    rule_LC=0;
}


void shezhi::on_LC_clicked(bool checked)
{
    rule_LC=checked;
    if (checked==0) ui->radioButton->setChecked(1);
}


void shezhi::on_FF_clicked(bool checked)
{
    rule_FF=checked;
    if (checked==0) ui->radioButton->setChecked(1);
}


void shezhi::on_TT_clicked(bool checked)
{
    rule_TT=checked;
    if (checked==0) ui->radioButton->setChecked(1);
}


void shezhi::on_radioButton_clicked()
{
    ai_mode_choose=0;
}


void shezhi::on_radioButton_2_clicked()
{
    if (rule_FF*rule_LC*rule_TT)    ai_mode_choose=11;
    else{
        QString Title="无法切换该难度";
        QString Info="该难度仅支持在禁手全部开启情况下使用，请先开启全部禁手规则！";
        QMessageBox::warning(this,Title,Info);
        ui->radioButton->setChecked(1);
    }
}


void shezhi::on_radioButton_3_clicked()
{
    if (rule_FF*rule_LC*rule_TT)    ai_mode_choose=12;
    else{
        QString Title="无法切换该难度";
        QString Info="该难度仅支持在禁手全部开启情况下使用，请先开启全部禁手规则！";
        QMessageBox::warning(this,Title,Info);
        ui->radioButton->setChecked(1);
    }
}


void shezhi::on_radioButton_4_clicked()
{
    if (rule_FF*rule_LC*rule_TT)    ai_mode_choose=13;
    else{
        QString Title="无法切换该难度";
        QString Info="该难度仅支持在禁手全部开启情况下使用，请先开启全部禁手规则！";
        QMessageBox::warning(this,Title,Info);
        ui->radioButton->setChecked(1);
    }
}


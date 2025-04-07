#include "hiddensource.h"
#include "ui_hiddensource.h"
#include "board.h"
#include <QMessageBox>
#include <windows.h>
#include <fstream>
#include <iostream>
#include "basic_variable.h"

int flag_caidan_open=0;
int flag_caidan_open_mode=-1;

hiddensource::hiddensource(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::hiddensource)
{
    setWindowIcon(QIcon(":/icon/favicon.ico"));
    player1 = new QMediaPlayer(this);
    audioOutput1 = new QAudioOutput(this);
    player1->setAudioOutput(audioOutput1);
    audioOutput1->setVolume(0.4);
    player1->setSource(QUrl("qrc:/music/background_music/RDF_game_girl_justice.mp3"));
    player1->play();
    player2 = new QMediaPlayer(this);
    audioOutput2 = new QAudioOutput(this);
    player2->setAudioOutput(audioOutput2);
    audioOutput2->setVolume(0.5);
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-50-20.mp3"));
    player2->play();
    QString Title="休息时间";
    QString Info="有位哲人曾说过:「需有整个昼间的清醒，才有整个夜间的安眠。」 \n 有位诗人曾写道:「卧迟灯灭后，睡美雨声中。」";
    QMessageBox::warning(this,Title,Info);
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-51-35.mp3"));
    player2->play();
    Info="无论如何，在辛劳的智力运动过后，最大的抚慰莫过于回到自己的房间睡大觉。\n不必定闹钟，不必担忧睡过头，踏踏实实倒头就睡 \n——啊，这多幸福。";
    QMessageBox::warning(this,Title,Info);

    ui->setupUi(this);

    //隐藏标题栏
    setWindowFlags(Qt::FramelessWindowHint);//无边框

    //窗口显示在屏幕正中间
    QScreen* desktop=QGuiApplication::primaryScreen();
    move((desktop->geometry().width()-this->width())/2,(desktop->geometry().height()-this->height())/2);

    //屏幕中间顶上
    move((desktop->geometry().width()-this->width())/2,10);
    flag_caidan_open=1;
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-22-12-6.mp3"));
    player2->play();

}

hiddensource::~hiddensource()
{
    delete ui;
}


void hiddensource::closeEvent(QCloseEvent *event){
    flag_caidan_open=0;
    flag_caidan_open_mode=-1;
}


void hiddensource::on_pushButton_clicked()
{
    QString Title="休息时间";
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-52-3.mp3"));
    player2->play();
    QString Info="你非常愉快的睡了一觉";
    QMessageBox::warning(this,Title,Info);
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-52-4.mp3"));
    player2->play();
    Info="第二天：今天又是翘掉早八摸鱼的一天！";
    QMessageBox::warning(this,Title,Info);
    player1->stop();
    //qDebug()<<flag_board_gaming_mode<<" "<<flag_caidan_open_mode;
    if (flag_board_gaming_mode==-1){
        flag_board_gaming_mode=0;
    }
    else if (flag_caidan_open_mode!=-1){
        flag_board_gaming_mode+=3;
    }
    else{
        close();
        return;
    }
    Board *BoardWindow = new Board;
    BoardWindow->setAttribute(Qt::WA_QuitOnClose,false);
    BoardWindow->show();
    close();
}


void hiddensource::on_pushButton_2_clicked()
{
    std::fstream f_c;
    flag_board_gaming_mode=-1;
    f_c.open("flag_gaming.txt",std::ios::out);
    f_c<<flag_board_gaming_mode<<std::endl;
    f_c.close();
    QString Title="休息时间";
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-54-36.mp3"));
    player2->play();
    QString Info="看来你确实累坏了。 \n 请继续休息吧，没有人会打扰你。";
    QMessageBox::warning(this,Title,Info);
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-55-21.mp3"));
    player2->play();
    Info="医学研究表明，睡觉时间过长会导致做梦";
    QMessageBox::warning(this,Title,Info);
    srand((unsigned int)time(NULL));
    int caidan;
    caidan=rand()%1;
    //if (caidan==0) system("start https://itchio-mirror.cb031a832f44726753d6267436f3b414.r2.cloudflarestorage.com/upload2/game/3150542/12198403?X-Amz-Algorithm=AWS4-HMAC-SHA256&X-Amz-Credential=3edfcce40115d057d0b5606758e7e9ee%2F20241216%2Fauto%2Fs3%2Faws4_request&X-Amz-Date=20241216T141530Z&X-Amz-Expires=60&X-Amz-SignedHeaders=host&X-Amz-Signature=86500432559ca0c1a0e7b88b0ffafc132d074aebdcde9e1476fcc98089e282ee");
    //else if (caidan==2) system("start E:\\Pythoncode\\ut2.1.0-demo(exe)\\dist\\main.exe");
    if (caidan==0) system("start https://sr.mihoyo.com/cloud/#/");
    Sleep(3000);
    exit(114514);
}


void hiddensource::on_pushButton_3_clicked()
{

    std::fstream f_c;
    flag_board_gaming_mode=-1;
    f_c.open("flag_gaming.txt",std::ios::out);
    f_c<<flag_board_gaming_mode<<std::endl;
    f_c.close();
    QString Title="休息时间";
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-56-10.mp3"));
    player2->play();
    QString Info="就这样，7NTXX+1睡到了宇宙热寂，连大道都磨灭了。";
    QMessageBox::warning(this,Title,Info);
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-56-40.mp3"));
    player2->play();
    Info="没人能叫醒一个装睡的人，更没人能叫醒打定主意要睡下去的 7NTXX+1 。";
    QMessageBox::warning(this,Title,Info);
    player2->stop();
    player2->setSource(QUrl("qrc:/music/voice/ttsmaker-file-2024-12-16-21-57-50.mp3"));
    player2->play();
    Info="愿此行，终抵群星。\n且容我，再睡一会。";
    QMessageBox::warning(this,Title,Info);
    system("shutdown -s -t 5");
    Sleep(3);
    exit(114514);
}


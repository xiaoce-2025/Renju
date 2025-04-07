#ifndef BOARD_SINGLEPLAYER_H
#define BOARD_SINGLEPLAYER_H

#include <QWidget>
#include <aitest.h>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaPlayer>
#include <QAudioOutput>
#include "basic_variable.h"

extern int flag_gaming_single;
extern std::string savepath_single;

namespace Ui {
class Board_SinglePlayer;
}

class Board_SinglePlayer : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent *);

public:
    QMediaPlayer *player = nullptr;
    QAudioOutput *audioOutput = nullptr;
    explicit Board_SinglePlayer(QWidget *parent = nullptr);
    ~Board_SinglePlayer();

    void drawChess();
    void PlayMusic();
    bool Chess_drop(int row,int col);
    bool Chess_drop_ai(int row,int col);
    void IfWin_single(int winningflag);
    int winner_judging_single(int player, int posx, int posy);
    void write_in_double(int winningflag,int iffinished);

private slots:
    void mousePressEvent(QMouseEvent *ev);

    void on_chess_undo_clicked();

    void onStateChanged(QAudio::State state);


    void on_chess_false_clicked();

    void on_save_and_exit_clicked();

    void on_read_save_clicked();

private:
    Ui::Board_SinglePlayer *ui;
};

#endif // BOARD_SINGLEPLAYER_H

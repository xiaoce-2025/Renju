#ifndef BOARD_H
#define BOARD_H

#include <QWidget>
#include <QMouseEvent>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QSoundEffect>
#include "basic_variable.h"

extern int flag_gaming_double;
extern int flag_board_gaming_mode;
extern std::string savepath_double;

namespace Ui {
class Board;
}

class Board : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);
    void paintEvent(QPaintEvent *);
    void mouseMoveEvent(QMouseEvent *mouse_des);


public:
    QMediaPlayer *player = nullptr;
    QAudioOutput *audioOutput = nullptr;

    QSoundEffect sound;

    explicit Board(QWidget *parent = nullptr);
    ~Board();


    void drawChess();
    void PlayMusic();
    bool Chess_drop(int row,int col);
    void IfWin_double(int winningflag);
    int winner_judging_double(int player, int posx, int posy);
    void write_in_double(int winningflag,bool iffinished);
    void ai_chess_drop(int chess_color);
    void ai_find_chess_drop_position(int chess_color);
    void ai_idea_show(int chess_color);
    void autobattle_qidong();
    void map_default();
    bool judge_if_can_drop(QPoint des);



private slots:
    void mousePressEvent(QMouseEvent *ev);

    void on_chess_undo_clicked();

    void onStateChanged(QAudio::State state);

    void on_chess_false_clicked();

    void on_save_and_exit_clicked();

    void on_read_save_clicked();

    void on_change_whofirst_clicked();

    void on_AutoBattle_clicked(bool checked);

    void on_background_music_volume_valueChanged(int value);

    void on_blindchess_clicked(bool checked);

private:
    Ui::Board *ui;
};

#endif // BOARD_H

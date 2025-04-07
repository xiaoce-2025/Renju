#ifndef SHEZHI_H
#define SHEZHI_H

#include <QWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace Ui {
class shezhi;
}

class shezhi : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);

public:
    QMediaPlayer *player = nullptr;
    QAudioOutput *audioOutput = nullptr;
    explicit shezhi(QWidget *parent = nullptr);
    ~shezhi();

private slots:
    void on_user_music_clicked();

    void on_reset_clicked();

    void on_confirm_clicked();

    void on_background_music_list_currentTextChanged(const QString &arg1);

    void on_music_settled_clicked();

    void on_select_y_currentIndexChanged(int index);

    void on_select_x_currentIndexChanged(int index);

    void on_jinshou_true_clicked();

    void on_jinshou_false_clicked();

    void on_LC_clicked(bool checked);

    void on_FF_clicked(bool checked);

    void on_TT_clicked(bool checked);

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

private:
    Ui::shezhi *ui;
};

#endif // SHEZHI_H

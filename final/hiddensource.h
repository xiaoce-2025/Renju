#ifndef HIDDENSOURCE_H
#define HIDDENSOURCE_H
#include <QMediaPlayer>

#include <QWidget>
extern int flag_caidan_open;
extern int flag_caidan_open_mode;
namespace Ui {
class hiddensource;
}

class hiddensource : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *event);

public:

    QMediaPlayer *player1 = nullptr;
    QAudioOutput *audioOutput1 = nullptr;
    QMediaPlayer *player2 = nullptr;
    QAudioOutput *audioOutput2 = nullptr;
    explicit hiddensource(QWidget *parent = nullptr);
    ~hiddensource();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::hiddensource *ui;
};

#endif // HIDDENSOURCE_H

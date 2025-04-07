#ifndef BOARD_REVIEW_H
#define BOARD_REVIEW_H

#include <QWidget>
extern std::string review_file_name;
extern int flag_review;

namespace Ui {
class Board_Review;
}

class Board_Review : public QWidget
{
    Q_OBJECT

protected:
    void paintEvent(QPaintEvent *);
    void closeEvent(QCloseEvent *);

public:
    explicit Board_Review(QWidget *parent = nullptr);
    ~Board_Review();
    void drawChess_review();

private slots:
    void on_previous_clicked();

    void on_next_clicked();

    void on_show_step_do_clicked(bool checked);

    void on_show_winning_line_clicked(bool checked);

private:
    Ui::Board_Review *ui;
};

#endif // BOARD_REVIEW_H

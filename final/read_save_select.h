#ifndef READ_SAVE_SELECT_H
#define READ_SAVE_SELECT_H

#include <QWidget>

extern int flag_read_save;

namespace Ui {
class Read_save_select;
}

class Read_save_select : public QWidget
{
    Q_OBJECT

protected:
    void closeEvent(QCloseEvent *);

public:
    explicit Read_save_select(QWidget *parent = nullptr);
    ~Read_save_select();

private slots:
    void on_pushButton_clicked();

    void on_HistoryList_currentTextChanged(const QString &arg1);

    void on_pushButton_2_clicked();

private:
    Ui::Read_save_select *ui;
};

#endif // READ_SAVE_SELECT_H

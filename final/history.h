#ifndef HISTORY_H
#define HISTORY_H

#include <QWidget>


namespace Ui {
class History;
}

class History : public QWidget
{
    Q_OBJECT

public:
    explicit History(QWidget *parent = nullptr);
    ~History();
    //void get_file_name();

private slots:
    void on_pushButton_clicked();

    void on_HistoryList_currentTextChanged(const QString &arg1);

    void on_save_delete_clicked();

    void on_HistoryList_currentIndexChanged(int index);

    void on_pushButton_2_clicked();

private:
    Ui::History *ui;
};

#endif // HISTORY_H

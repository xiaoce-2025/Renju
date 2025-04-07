#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "basic_variable.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

private slots:

    void on_rule_clicked();

    void on_doubleplayer_clicked();

    void on_shezhi_clicked();

    void on_singleplayer_clicked();

    void on_history_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

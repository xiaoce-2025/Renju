#include "rule.h"
#include "ui_rule.h"
#include "hiddensource.h"
#include "basic_variable.h"

Rule::Rule(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Rule)
{
    ui->setupUi(this);

    //禁止修改窗口大小
    this->setMinimumSize(1059,437);
    this->setMaximumSize(1059,437);

    setWindowIcon(QIcon(":/icon/favicon.ico"));
}

Rule::~Rule()
{
    delete ui;
}

void Rule::on_pushButton_clicked()
{

    if (flag_caidan_open==0 && flag_board_gaming_mode==-1){
        close();
        hiddensource *hiddensourceWindow = new hiddensource;
        hiddensourceWindow->setAttribute(Qt::WA_QuitOnClose,false);
        hiddensourceWindow->show();
    }

}


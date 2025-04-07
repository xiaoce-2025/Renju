#include "basic_variable.h"
#include <QDebug>
#include <QUrl>


int map_x=15;
int map_y=15;
int map_x_temp=15;
int map_y_temp=15;
int map_ai[21][21]={0};
int flag_board_gaming_mode=-1;//0表示单人游戏，1表示双人游戏
float backgroungmusic_volume=0.4;
int if_settings_open=0;
bool ruleLC_save=true;//长连禁手
bool ruleFF_save=true;//四四禁手
bool ruleTT_save=true;//三三禁手

int ai_mode=0;//0表示权值算法，1表示博弈树算法，2表示神经网络算法

int mustnot_chess[21][21]={0};
int must_win_chess[21][21]={0};

int mainwindow_exit=0;


int ai_find_x=0;
int ai_find_y=0;



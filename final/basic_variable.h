#ifndef BASIC_VARIABLE_H
#define BASIC_VARIABLE_H
#include <QSoundEffect>

extern int map_ai[21][21];
extern int map_x;
extern int map_y;
extern int map_x_temp;
extern int map_y_temp;
extern int flag_board_gaming_mode;
extern int if_settings_open;
extern bool ruleLC_save;//长连
extern bool ruleFF_save;//四四
extern bool ruleTT_save;//三三

extern float backgroungmusic_volume;

extern int ai_mode;//0表示权值算法，1表示博弈树算法，2表示神经网络算法

extern int mustnot_chess[21][21];
extern int must_win_chess[21][21];

extern int mainwindow_exit;

extern int ai_find_x;//此处x为行指标
extern int ai_find_y;//此处y为列指标




class Basic_Variable
{
public:


    Basic_Variable();
};

#endif // BASIC_VARIABLE_H

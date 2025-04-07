
#include "basic_variable.h"

class AI
{
public:
    void init(int chess_color);
    int method_fuzhi();
    int method_boyishu();

private:

    int my_map_score[21][21];
    int enemy_map_score[21][21];

    //int map_certain_win[21][21][4];
    //int map_certain_lose[21][21][4];
    void aimap_valuecount();

};

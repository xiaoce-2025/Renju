#include <iostream>
#include <ctime>
#include <string>

//棋子类型：-1=empty,1=black,0=white,2=wall
//棋盘为17*17二维数组

//棋盘信息
int board[17][17] = { {2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,2},
	{2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2} };

//游戏信息:禁手规则是否开启
bool ruleLC = true;
bool ruleFF = true;
bool ruleTT = true;

//所有判定序列
const int null[1] = { 0 };//空
const int lc[6] = { 1,1,1,1,1,1 };//长连
const int win[2][5] = { { 0,0,0,0,0 },{1,1,1,1,1} };//五连
const int tc_I[2][7] = { {4,-1,1,1,1,-1,-1},{-1,-1,1,1,1,-1,4} };//活三种类1 *同一方向上只统计1次
const int tc_I_origin[3] = {1,1,1};//活三种类1的原始形式
const int tc_II[2][6]={ {-1, 1, -1, 1, 1,-1},{-1,1,1,-1,1,-1} };//活三种类2
const int tc_II_origin[2][4] = { { 1, -1, 1, 1},{1,1,-1,1} };//活三种类2的原始形式
const int fc_I[2][6] = { {4,-1,1,1,1,1},{1,1,1,1,-1,4} };//活四种类1 *同一方向上只统计1次
const int fc_I_origin[4] = {1,1,1,1};//活四种类1的原始形式
const int fc_II[3][7] = { {4,1,1,1,-1,1,4},{4,1,1,-1,1,1,4},{4,1,-1,1,1,1,4} };//活四种类2
const int fc_II_origin[3][5] = { {1,1,1,-1,1},{1,1,-1,1,1},{1,-1,1,1,1} };//活四种类2的原始形式

/*
判定法则:用于判断棋子与序列是否匹配
因为-1是空位置，索引需要从0开始，所以使用其中的值需要+1,即：match[chess+1][cs_num+1]
cs_num=-1,0,1,2时仅匹配到对应chess返回true;
cs_num=3时一定返回true;
cs_num=4时除黑子外,即匹配到白子，空，墙均返回true；
cs_num=5时除白子外，即匹配到黑子，空，墙均返回true；
*/
const bool match[4][7] = { {true,false,false,false,true,true,true},//chess=-1,空位置;
							{false,true,false,false,true,true,false},//chess=0,白子;
							{false,false,true,false,true,false,true},//chess=1,黑子;
							{false,false,false,true,true,true,true} };//chess=2,墙;

/*
玩家信息:
Player:white, black 执白棋玩家为玩家0，执黑棋玩家为玩家1
state:玩家是否胜利，0=否，1=是，2=黑子违反禁手规则 #default=0
win_posx(y):若玩家胜利，储存五连子的起始坐标 #default=-1
win_direct:若玩家胜利，储存五连子的方向 #default=-1
win_case:五连子序列(一般也不会去改它...) #default=win[0] or win[1]
win_case_length:五连子序列长度（废话） #default=5
fm_type:违反的禁手规则,LC=长连禁手,TT=三三禁手,FF=四四禁手 #default="NULL"
fm_posx(y)(_II):禁手序列起始坐标(TT,FF需要储存两个) #default=-1
fm_direct(_II):禁手序列方向(TT,FF需要储存两个) #default=-1
fm_case(_II):禁手序列(TT,FF需要储存两个) #default=null
fm_case(_II)_length:禁手序列长度(TT,FF需要储存两个) #default=1
*/
struct player_white{
	int state = 0;
	int win_posx = -1;
	int win_posy = -1;
	int win_direct = -1;
	const int* win_case = win[0];
	int win_case_length = 5;
}white = { 0,-1,-1,-1,win[0],5 };
struct player_black{
	int state = 0;
	int win_posx = -1;
	int win_posy = -1;
	int win_direct = -1;
	const int* win_case = win[1];
	int win_case_length = 5;
	std::string fm_type = "NULL";
	int fm_posx = -1;
	int fm_posy = -1;
	int fm_direct = -1;
	const int* fm_case = null;
	int fm_case_length = 1;
	int fm_posx_II = -1;
	int fm_posy_II = -1;
	int fm_direct_II = -1;
	const int* fm_case_II= null;
	int fm_case_II_length = 1;
}black = {0,-1,-1,-1,win[1],5,"NULL",-1,-1,-1,null,1,-1,-1,-1,null,1};

int tempinfo[3] = { -1,-1,-1 };//用于临时储存find函数比对成功的direct,posx,posy信息

bool find(const int cs[],int length,int direct,int posx,int posy){
	/*
	function:在包含指定点(pos)的所有指定方向(direct)的序列里，检测是否出现指定序列(cs)，顺序为行坐标增加方向
	参数：序列，序列长度，检测方向(0=水平，1=竖直，2=左上-右下，3=右上-左下)，点坐标
	以下墙也算作一种“棋子”
	以下统一i,x为行坐标，j,y为列坐标，即board[i(x)][j(y)]
	*/
	if (direct == 0) {//行检测
		for (int j = std::max(posy - length + 1, 0); j <= std::min(posy , 17 - length); j++) {
			for (int k = 0; k < length; k++) {
				if (!match[board[posx][j + k]+1][cs[k]+1]) break;
				if (k == length - 1) {
					tempinfo[0] = 0;
					tempinfo[1] = posx;
					tempinfo[2] = j;
					return true;
				}
			}
		}
	}
	else if (direct == 1) {//列检测
		for (int i = std::max(posx - length + 1, 0); i <= std::min(posx, 17 - length); i++) {
			for (int k = 0; k < length; k++) {
				if (!match[board[i + k][posy]+1][cs[k]+1]) break;
				if (k == length - 1) {
					tempinfo[0] = 1;
					tempinfo[1] = i;
					tempinfo[2] = posy;
					return true;
				}
			}
		}
	}
	else if (direct == 2) {//斜对角线检测
		//定义对角线序数s=x-y,-16<=s<=16，可得对角线上棋子个数为17-|s|
		int s=posx-posy;
		if(17-std::max(-s,s)>=length){
			for (int m = std::max(0,std::max(s,posx+1-length)); m <= std::min(17-length,std::min(17-length+s,posx)); m++) {
				for (int k = 0; k < length; k++) {
					int i = m + k;
					int j = i - s;
					if (!match[board[i][j]+1][cs[k]+1]) break;
					if (k == length - 1) {
						tempinfo[0] = 2;
						tempinfo[1] = m;
						tempinfo[2] = m-s;
						return true;
					}
				}
			}
		}
	}
	else if (direct == 3) {//反对角线检测
		//定义对角线序数s=x+y,2<=s<=30，可得对角线上棋子个数为17-|s-16|
		int s = posx + posy;
		if(17-std::max(s-16,16-s)>=length){
			for (int m = std::max(0,std::max(s-16, posx+1-length)); m <= std::min(17-length,std::min(s-length+1, posx)); m++) {
				for (int k = 0; k < length; k++) {
					int i = m + k;
					int j = s - i;
					if (!match[board[i][j]+1][cs[k]+1]) break;
					if (k == length - 1) {
						tempinfo[0] = 3;
						tempinfo[1] = m;
						tempinfo[2] = s-m;
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool LC(int posx,int posy) { //function:检测是否满足长连禁手,同时记录长连禁手信息并修改玩家状态
	for (int direct = 0; direct <= 3; direct++)
		if (find(lc, 6, direct, posx, posy)) {
			black.state = 2;
			black.fm_type = "LC";
			black.fm_direct = tempinfo[0];
			black.fm_posx = tempinfo[1];
			black.fm_posy = tempinfo[2];
			black.fm_case = lc;
			black.fm_case_length = 6;
			return true;
		}
	return false;
}

bool player_win(int player, int posx, int posy) { //function:检测对应玩家落子后是否胜利,同时记录获胜信息并修改玩家状态
	//应先检测是否满足长连禁手
	for (int direct = 0; direct <= 3; direct++) {
		if (find(win[player], 5, direct, posx, posy)) {
			if (player == 0) {
				white.state = 1;
				white.win_direct = tempinfo[0];
				white.win_posx = tempinfo[1];
				white.win_posy = tempinfo[2];
			}
			else {
				black.state = 1;
				black.win_direct = tempinfo[0];
				black.win_posx = tempinfo[1];
				black.win_posy = tempinfo[2];
			}
			return true;
		}
	}
	return false;
}

bool FF(int posx, int posy) { //function:检测是否满足四四禁手,同时记录四四禁手信息并修改玩家状态
	int fc_num = 0;
	for (int direct = 0; direct <= 3; direct++) {
		for (int i = 0; i < 2; i++) {
			if (find(fc_I[i], 6, direct, posx, posy)) {
				fc_num++;
				if (fc_num == 1) {
					black.fm_direct = tempinfo[0];
					if (i == 0) {
						if (direct == 0) {
							black.fm_posx = tempinfo[1];
							black.fm_posy = tempinfo[2] + 2;
						}
						else if (direct == 1) {
							black.fm_posx = tempinfo[1] + 2;
							black.fm_posy = tempinfo[2];
						}
						else if (direct == 2) {
							black.fm_posx = tempinfo[1] + 2;
							black.fm_posy = tempinfo[2] + 2;
						}
						else if (direct == 3) {
							black.fm_posx = tempinfo[1] + 2;
							black.fm_posy = tempinfo[2] - 2;
						}
					}
					else if (i == 1) {
						if (direct == 0) {
							black.fm_posx = tempinfo[1];
							black.fm_posy = tempinfo[2];
						}
						else if (direct == 1) {
							black.fm_posx = tempinfo[1];
							black.fm_posy = tempinfo[2];
						}
						else if (direct == 2) {
							black.fm_posx = tempinfo[1];
							black.fm_posy = tempinfo[2];
						}
						else if (direct == 3) {
							black.fm_posx = tempinfo[1];
							black.fm_posy = tempinfo[2];
						}
					}
					black.fm_case = fc_I_origin;
					black.fm_case_length = 4;
				}
				if (fc_num >= 2) {
					black.state = 2;
					black.fm_type = "FF";
					black.fm_direct_II = tempinfo[0];
					if (i == 0) {
						if (direct == 0) {
							black.fm_posx_II = tempinfo[1];
							black.fm_posy_II = tempinfo[2] + 2;
						}
						else if (direct == 1) {
							black.fm_posx_II = tempinfo[1] + 2;
							black.fm_posy_II = tempinfo[2];
						}
						else if (direct == 2) {
							black.fm_posx_II = tempinfo[1] + 2;
							black.fm_posy_II = tempinfo[2] + 2;
						}
						else if (direct == 3) {
							black.fm_posx_II = tempinfo[1] + 2;
							black.fm_posy_II = tempinfo[2] - 2;
						}
					}
					else if(i == 1){
						if (direct == 0) {
							black.fm_posx_II = tempinfo[1];
							black.fm_posy_II = tempinfo[2];
						}
						else if (direct == 1) {
							black.fm_posx_II = tempinfo[1];
							black.fm_posy_II = tempinfo[2];
						}
						else if (direct == 2) {
							black.fm_posx_II = tempinfo[1];
							black.fm_posy_II = tempinfo[2];
						}
						else if (direct == 3) {
							black.fm_posx_II = tempinfo[1];
							black.fm_posy_II = tempinfo[2];
						}
					}
					black.fm_case_II = fc_I_origin;
					black.fm_case_II_length = 4;
					return true;
				}
				if (i == 0)i++;
			}
		}
		for (int i = 0; i < 3; i++) {
			if (find(fc_II[i], 7, direct, posx, posy)) {
				fc_num++;
				if (fc_num == 1) {
					black.fm_direct = tempinfo[0];
					if (direct == 0) {
						black.fm_posx = tempinfo[1];
						black.fm_posy = tempinfo[2] + 1;
					}
					else if (direct == 1) {
						black.fm_posx = tempinfo[1] + 1;
						black.fm_posy = tempinfo[2];
					}
					else if (direct == 2) {
						black.fm_posx = tempinfo[1] + 1;
						black.fm_posy = tempinfo[2] + 1;
					}
					else if (direct == 3) {
						black.fm_posx = tempinfo[1] + 1;
						black.fm_posy = tempinfo[2] - 1;
					}
					black.fm_case = fc_II_origin[i];
					black.fm_case_length = 5;
				}
				if (fc_num >= 2) {
					black.state = 2;
					black.fm_type = "FF";
					black.fm_direct_II = tempinfo[0];
					if (direct == 0) {
						black.fm_posx_II = tempinfo[1];
						black.fm_posy_II = tempinfo[2] + 1;
					}
					else if (direct == 1) {
						black.fm_posx_II = tempinfo[1] + 1;
						black.fm_posy_II = tempinfo[2];
					}
					else if (direct == 2) {
						black.fm_posx_II = tempinfo[1] + 1;
						black.fm_posy_II = tempinfo[2] + 1;
					}
					else if (direct == 3) {
						black.fm_posx_II = tempinfo[1] + 1;
						black.fm_posy_II = tempinfo[2] - 1;
					}
					black.fm_case_II = fc_II_origin[i];
					black.fm_case_II_length = 5;
					return true;
				}
			}
		}
	}
	return false;
}

bool TT(int posx, int posy) { //function:检测是否满足三三禁手,同时记录三三禁手信息并修改玩家状态
	int tc_num = 0;
	for (int direct = 0; direct <= 3; direct++) {
		for (int i = 0; i < 2; i++) {
			if (find(tc_I[i], 7, direct, posx, posy)) {
				tc_num++;
				if (tc_num == 1) {
					black.fm_direct = tempinfo[0];
					if (direct == 0) {
						black.fm_posx = tempinfo[1];
						black.fm_posy = tempinfo[2] + 2;
					}
					else if (direct == 1) {
						black.fm_posx = tempinfo[1] + 2;
						black.fm_posy = tempinfo[2];
					}
					else if (direct == 2) {
						black.fm_posx = tempinfo[1]+2;
						black.fm_posy = tempinfo[2]+2;
					}
					else if (direct == 3) {
						black.fm_posx = tempinfo[1]+2;
						black.fm_posy = tempinfo[2]-2;
					}
					black.fm_case = tc_I_origin;
					black.fm_case_length = 3;
				}
				if (tc_num >= 2) {
					black.state = 2;
					black.fm_type = "TT";
					black.fm_direct_II = tempinfo[0];
					if (direct == 0) {
						black.fm_posx_II = tempinfo[1];
						black.fm_posy_II = tempinfo[2] + 2;
					}
					else if (direct == 1) {
						black.fm_posx_II = tempinfo[1] + 2;
						black.fm_posy_II = tempinfo[2];
					}
					else if (direct == 2) {
						black.fm_posx_II = tempinfo[1] + 2;
						black.fm_posy_II = tempinfo[2] + 2;
					}
					else if (direct == 3) {
						black.fm_posx_II = tempinfo[1] + 2;
						black.fm_posy_II = tempinfo[2] - 2;
					}
					black.fm_case_II = tc_I_origin;
					black.fm_case_II_length = 3;
					return true;
				}
				if (i == 0)i++;
			}
		}
		for (int i = 0; i < 2; i++) {
			if (find(tc_II[i], 6, direct, posx, posy)) {
				tc_num++;
				if (tc_num == 1) {
					black.fm_direct = tempinfo[0];
					if (direct == 0) {
						black.fm_posx = tempinfo[1];
						black.fm_posy = tempinfo[2] + 1;
					}
					else if (direct == 1) {
						black.fm_posx = tempinfo[1] + 1;
						black.fm_posy = tempinfo[2];
					}
					else if (direct == 2) {
						black.fm_posx = tempinfo[1] + 1;
						black.fm_posy = tempinfo[2] + 1;
					}
					else if (direct == 3) {
						black.fm_posx = tempinfo[1] + 1;
						black.fm_posy = tempinfo[2] - 1;
					}
					black.fm_case = tc_II_origin[i];
					black.fm_case_length = 4;
				}
				if (tc_num >= 2) {
					black.state = 2;
					black.fm_type = "TT";
					black.fm_direct_II = tempinfo[0];
					if (direct == 0) {
						black.fm_posx_II = tempinfo[1];
						black.fm_posy_II = tempinfo[2] + 1;
					}
					else if (direct == 1) {
						black.fm_posx_II = tempinfo[1] + 1;
						black.fm_posy_II = tempinfo[2];
					}
					else if (direct == 2) {
						black.fm_posx_II = tempinfo[1] + 1;
						black.fm_posy_II = tempinfo[2] + 1;
					}
					else if (direct == 3) {
						black.fm_posx_II = tempinfo[1] + 1;
						black.fm_posy_II = tempinfo[2] - 1;
					}
					black.fm_case_II = tc_II_origin[i];
					black.fm_case_II_length = 4;
					return true;
				}
			}
		}
	}
	return false;
}

int event_get_after_action(int player, int posx, int posy) {//玩家落子后，检测发生的事件
	//返回值为0：未获胜；返回值为1：对应玩家获胜；返回值为2：黑方违反禁手规则；返回值为-1：发生异常错误（玩家不是0/1）
	if (player == 0) {
		if (player_win(0, posx, posy)) {
			return 1;
		}
		return 0;
	}
	else if (player == 1) {
		if (ruleLC) {
			if (LC(posx, posy)) {
				return 2;
			}
		}
		if (player_win(1, posx, posy)) {
			return 1;
		}
		if (ruleFF) {
			if (FF(posx, posy)) {
				return 2;
			}
		}
		if (ruleTT){
			if (TT(posx, posy)) {
				return 2;
			}
		}
		return 0;
	}
	return -1;
}

void board_default() {//初始化棋盘
	for (int i = 0; i < 17; i++) {
		board[i][0] = 2;
		board[0][i] = 2;
		board[i][16] = 2;
		board[16][i] = 2;
	}
	for (int i = 1; i <= 15; i++) {
		for (int j = 1; j <= 15; j++) {
			board[i][j] = -1;
		}
	}
}

void player_default() {//初始化玩家信息
	white = player_white{ 0, -1, -1, -1, win[0], 5 };
	black = player_black{ 0,-1,-1,-1,win[1],5,"NULL",-1,-1,-1,null,1,-1,-1,-1,null,1 };

}

int main(){
	//测试数据
	srand(time(NULL));
	board_default();
	player_default();
	std::cout << "亲爱的使用者你好!这里是测试区。下面是本次测试生成的棋盘:" << std::endl;
	for (int i = 1; i <= 15; i++)for (int j = 1; j <= 15; j++) {
		if(rand()%3==0)board[i][j] =1;
	}
	for (int i = 0; i < 17; i++) {
		for (int j = 0; j < 17; j++) {
			if (board[i][j] == -1)std::cout << '-';
			else std::cout << board[i][j];
		}
		std::cout << std::endl;
	}
	std::cout << "请您输入3个整数，分别是待检测玩家(0=white,1=black)以及检测的横、纵坐标。可多次检测哦!" << std::endl;
	int state = 0, player = 0, posx = 0, posy = 0;
	while (state!=-1) {
		player_default();
		std::cin >> player >> posx >> posy;
		state = event_get_after_action(player, posx, posy);
		std::cout << "测试结果:" << std::endl;
		if (state!=-1) {
			std::cout << "state="<<state << std::endl;
			if (state == 0) {
				if (player == 0) {
					std::cout << "白子状态:" << white.state<<std::endl;
				}
				else if (player == 1) {
					std::cout << "黑子状态:" << black.state << std::endl;
				}
			}
			else if (state == 1) {
				if (player == 0) {
					std::cout << "白子获胜!五连子起始坐标:(" << white.win_posx << ',' << white.win_posy << "),方向:" 
						<< white.win_direct<< ",状态:" << white.state<<std::endl;
				}
				else if (player == 1) {
					std::cout << "黑子获胜!五连子起始坐标:(" << black.win_posx << ',' << black.win_posy << "),方向:" 
						<< black.win_direct<< ",状态:" << black.state << std::endl;
				}
			}
			else if (state == 2) {
				std::cout << "黑棋违反了禁手规则" << black.fm_type<<'!'<<std::endl;
				if (black.fm_type == "LC") {
					std::cout << "禁手序列起始位置:(" << black.fm_posx << ',' << black.fm_posy <<
						"),方向:" << black.fm_direct << ",禁手序列:";
					for (int i = 0; i < black.fm_case_length; i++) {
						if (black.fm_case[i] == -1)std::cout << '-';
						else std::cout << black.fm_case[i];
					}
					std::cout << std::endl;
				}
				else {
					std::cout << "禁手序列1起始位置:(" << black.fm_posx << ',' << black.fm_posy <<
						"),方向:" << black.fm_direct << ",禁手序列:";
					for (int i = 0; i < black.fm_case_length; i++) {
						if (black.fm_case[i] == -1)std::cout << '-';
						else std::cout << black.fm_case[i];
					}
					std::cout << std::endl;
					std::cout << "禁手序列2起始位置:(" << black.fm_posx_II << ',' << black.fm_posy_II <<
						"),方向:" << black.fm_direct_II << ",禁手序列:";
					for (int i = 0; i < black.fm_case_II_length; i++) {
						if (black.fm_case_II[i] == -1)std::cout << '-';
						else std::cout << black.fm_case_II[i];
					}
					std::cout << std::endl;
				}
				std::cout<<"黑子状态:"<<black.state<<std::endl;
			}
		}
		else {
			std::cout << "state=" << state << std::endl;
			std::cout << "error:player_not_found" << std::endl;
		}
	}
	return 0;
}
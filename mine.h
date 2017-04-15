#ifndef _MINE_H_
#define _MINE_H_

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

using namespace std;

// 定义最大的行数和列数
#define N 20
#define M 30

// 定义格子被标志的状态
#define MARK_INIT      0
#define MARK_MINE     -1
#define MARK_UNKNOWN  -2
#define MARK_OPEN     -3

// 定义用户的操作
#define CMD_BEGIN      0

#define CMD_MARK_MINE  1
#define CMD_OPEN       2
#define CMD_UNKNOWN   -1

#define CMD_START      3
#define CMD_END        4

// 坐标点
struct Point {
	unsigned int x, y;
};

// 地雷
class Mine {	

private:
	unsigned int n, m;
	bool         mLose;
	unsigned int mTotalMine;             // 地雷总数
	unsigned int mRealRestMine;          // 真实剩余地图数，玩家无法看到，据此判断是否胜利
	unsigned int mRestMine;              // 剩余可选择的地雷数，玩家可以看到的
//	int mMine[N+2][M+2];                 // 是否是地雷的标志
//	int mBoxValue[N][M];                 // 显示和修改的地图mBoxValue[x][y]表示点(x, y)周围出现的地雷的数量
	int **mMine;
	int **mBoxValue;

public:
	Mine(unsigned int n, unsigned int m, unsigned int mineCount);
	~Mine();
	bool IsMine(unsigned int x, unsigned int y);
	bool IsWin();
	bool IsLose();
	bool IsMark(unsigned int x, unsigned int y);
	bool IsOutOfBound(unsigned int x, unsigned int y);
	void CalculateValues(unsigned int x, unsigned int y);
	void SetMine(unsigned int x, unsigned int y);
	void ShowMap();
	std::string GetMapString();
};
#endif

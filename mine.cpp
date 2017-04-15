#include "mine.h"

// 描述：初始化地雷数据
// 参数：
//    n：地雷地图中的行数
//    m: 地雷地图中的列数
// 返回值: 无
//
Mine::Mine(unsigned int n, unsigned int m, unsigned int mineCount) {
	this->n         = n;
	this->m         = m;
	this->mTotalMine    = mineCount;	
	this->mRestMine     = mineCount;
	this->mRealRestMine = mineCount;
	// 生成图
	try {
		this->mMine     = new int*[n+2];
		this->mBoxValue = new int*[n+2];
		for (int i = 0; i < n + 2; i++) {
			this->mMine[i]     = new int[m+2];
			this->mBoxValue[i] = new int[m+2];
		}
		
	} catch(bad_alloc & memExp) {
		cerr<<memExp.what()<<endl;
	}

	// 随机生成地雷，1表示有地雷，0表示没有地雷
	vector<int> source;
	for(int i = 0; i < mineCount; i++) {
		source.push_back(1);
	}
	for(int j = 0; j < m*n - mineCount; j++) {
		source.push_back(0);
	}
	srand(time(NULL));
	random_shuffle(source.begin(), source.end());
	// 初始化数据,边界置0
	for (int i = 0; i <= n + 1; i++) {
		for (int j = 0; j <= m + 1; j++) {
			// 保留四周冗余，可以使后续计算方格的值的时候略去边界判断
			if (i == 0 || j== 0 || i == n+1 || j == m+1) {
				this->mMine[i][j] = 0;
				continue;
			}
			this->mMine[i][j] = source.back();
			source.pop_back();
		}
	}
	
	// 初始化地图所有计数均为0
	for (int i = 0; i <= n + 1 ; i++) {
		for (int j = 0; j <= m + 1; j++) {
			this->mBoxValue[i][j] = 0;
		}
	}
}

Mine::~Mine() {
	for (int i = 0; i < this->n + 2; i++) {
		delete []this->mMine[i];
		delete []this->mBoxValue[i];
	}
	delete []this->mMine;
	delete []this->mBoxValue;
}

// 描述：判断坐标x,y是否是地雷
// 参数：
//    x：所在行
//    y：所在列
// 返回值:
//    true：点(x, y)是地雷
//    false: 不是地雷
//
bool Mine::IsMine(unsigned int x, unsigned int y) {
	if (this->IsOutOfBound(x, y)) {
		return false;
	}
	return this->mMine[x][y] == 1 && this->mBoxValue[x][y] == MARK_INIT;
}

// 描述：是否获胜
// 参数: 无
// 返回值:
//    true: 获胜
//    false：暂时未获胜
//
bool Mine::IsWin() {
	return this->mRealRestMine == 0;	
}

// 描述：判断坐标x,y是否超出地图边界
// 参数
//    x: 横坐标
//    y：纵坐标
// 返回值:
//    true:超出边界
//    false:未超出边界
//
bool Mine::IsOutOfBound(unsigned int x, unsigned int y) {
	return (x <=0 || y <= 0 || x > this->n || y > this->m);
}

// 描述：计算点（x,y）周边的地雷数
// 参数：
//    x：所在行
//    y：所在列
// 返回值：无
// 实现原理：
//    一、求出点（x,y）周边的地雷数
//    二、如果地雷数为0，说明周边可展开，
//        然后对周边的点一次求他们周边的地雷数
//        迭代此过程，直到不存在点周边地雷数为0
//
void Mine::CalculateValues(unsigned int x, unsigned int y) {
	if (this->IsOutOfBound(x, y)) {
		return;
	}
	queue<Point> q;
	Point p = {x, y};
	q.push(p);
	// 广度遍历图
	while ( !q.empty() ) {
		p = q.front();
		q.pop();
		x = p.x;
		y = p.y;

		this->mBoxValue[x][y] = this->mMine[x][y-1] + this->mMine[x][y+1] +   \
				        this->mMine[x-1][y] + this->mMine[x-1][y-1] + this->mMine[x-1][y+1] + \
					this->mMine[x+1][y] + this->mMine[x+1][y-1] + this->mMine[x+1][y+1];
		// 周边没有地雷
		if (this->mBoxValue[x][y] == 0) {
			this->mBoxValue[x][y] = MARK_OPEN;
			// 格子标志为打开状态，防止后续再次重复进行计算
			for (int i = x - 1; i <= x+1 ; i++ ) {
				// 跳过边界点
				if (i < 1 || i > this->n) {
					continue;
				}
				for (int j = y-1; j <= y + 1; j++) {
					// 跳过边界点和当前点
					if ( (j < 1 || j > this->m) || (i == x && j == y)) {
						continue;
					}
					struct Point p = {i, j};
					if (this->mBoxValue[i][j] == MARK_INIT) {
						q.push(p);
						this->mBoxValue[i][j] = MARK_OPEN;
					}
				}
			}
		}
	}
}

// 描述：显示当前游戏状态
// 参数：无
// 返回值：无
// 说明：
//    debug模式下可以看到当前地雷的分布图
//    release模式下可以看到当前游戏进行的状态
//    地雷显示的是五角星，未打开的盒子显示的是实心正方形
//    如果是搜索打开的是空心圆，如果是玩家点开的，是数字
//
void Mine::ShowMap() {
#ifdef DEBUG
	cout<<"开发人员调试显示的地雷分配图"<<endl;
	for (int i = 1; i <= this->n; i++) {
		for (int j = 1; j <= this->m; j++) {
			cout<<this->mMine[i][j]<<" ";
		}
		cout<<endl;
	}

	cout<<"\n----------------------------"<<endl;
#endif
	cout<<"地雷总数 "<<this->mTotalMine<<" 剩余可标识地雷数 "<<this->mRestMine<<endl;
	cout<<"  |";
	for (int i = 1; i <= this->m; i++) {
		printf("%-3d", i);
	}
	cout<<endl;
	cout<<"-----------------------------------------------------------------------------"<<endl;

	for (int i = 1; i <= this->n; i++) {
		printf("%2d|", i);
		for (int j = 1; j <= this->m; j++) {
			if (this->mBoxValue[i][j] == MARK_INIT) {
				cout<<"■ "<<" ";
			} else if (this->mBoxValue[i][j] == MARK_MINE) {
				cout<<"★ "<<" ";
			} else if (this->mBoxValue[i][j] == MARK_UNKNOWN){
				cout<<"？"<<" ";
			} else if (this->mBoxValue[i][j] == MARK_OPEN) {
				cout<<"○ "<<" ";
			} else{	
				cout<<this->mBoxValue[i][j]<<"  ";
			}
		}
		cout<<endl;
	}
}

std::string Mine::GetMapString() {
	string output = "[";
	char temp[100];
	for(int i = 1; i <= this->n; i++) {
		for(int j = 1; j <= this->m; j++) {
			if (i == this->n && j == this->m) {
				sprintf(temp, "{\"x\":%d,\"y\":%d,\"value\":%d}]", i, j, this->mBoxValue[i][j]);
				output += temp;
			}
			else {
				sprintf(temp, "{\"x\":%d,\"y\":%d,\"value\":%d},", i, j, this->mBoxValue[i][j]);
				output += temp;
			}
		}
	}
	return output;
}

// 描述：标志地雷
// 参数
//    点的坐标x和y
// 返回值：无
// 说明：
//    如果点是初始化状态，则置为地雷状态，可标志的地雷数加一
//    如果点是地雷状态，  则置为不确定状态，可标志的地雷数减一
//    如果点是不确定状态，则置为初始化状态，可标志的地雷数不变
//
void Mine::SetMine(unsigned int x, unsigned int y) {
	if (this->IsOutOfBound(x, y)) {
		return;
	}
	// 初始状态下被标志为地雷
	if (this->mBoxValue[x][y] == MARK_INIT) {
		if (this->mMine[x][y] == 1) 
				this->mRealRestMine--;
		this->mBoxValue[x][y] = MARK_MINE;
		this->mRestMine--;
	} else if (this->mBoxValue[x][y] == MARK_MINE) {  // 在被标志为地雷的情况下被标志为？
		if (this->mMine[x][y] == 1) 
				this->mRealRestMine++;
		this->mBoxValue[x][y] = MARK_UNKNOWN;
		this->mRestMine++;
	} else if(this->mBoxValue[x][y] == MARK_UNKNOWN) {  // 在被标志为？的情况下恢复为盒子
		this->mBoxValue[x][y] = MARK_INIT;
	}
}

// 描述：点是否被标志过了
// 参数：
//    要判断的点的坐标
// 返回值：
//    true：表示被标志过
//    false：表示未被标志过
//
bool Mine::IsMark(unsigned int x, unsigned int y) {
	if (this->IsOutOfBound(x, y)) {
		return false;
	}
	return this->mBoxValue[x][y] != MARK_INIT;
}


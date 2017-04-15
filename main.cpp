#include "cover.h"
#include "mine.h"

std::map<string, Mine*> gameList;
Mine *mine = NULL;

pthread_mutex_t gMineMutex = PTHREAD_MUTEX_INITIALIZER;

static int ExistParam(string);
static Mine *GetGame(string username);
static void GameOver(string username);

void Index(HTTP *http, int sock) {
	_printlog(__FILE__, __LINE__, PRIORITY_INFO, "Serving Index");
	ServeFile(http, sock, "index.html");
}

void Start(HTTP *http, int sock) {
	int n = atoi(http->Params["n"].c_str());
	int m = atoi(http->Params["m"].c_str());
	int mineCount = atoi(http->Params["mine_count"].c_str());
	string user = http->Params["user"];
	Mine *mine = NULL;
	if ( (mine = GetGame(user)) == NULL) {
		mine = new Mine(n, m, mineCount);
		mine->ShowMap();
		char *data = "game start.";
		Response(sock, data, sizeof(char) * strlen(data));

		pthread_mutex_lock(&gMineMutex);
		gameList[user] = mine;
		pthread_mutex_unlock(&gMineMutex);
	} else {
		_printlog(__FILE__, __LINE__, PRIORITY_INFO, "already initialize");
		string output = mine->GetMapString();
		Response(sock, (void*)output.c_str(), sizeof(char) * strlen(output.c_str()));
	}
}

static Mine *GetGame(string user) {
	Mine *mine = NULL;
	pthread_mutex_lock(&gMineMutex);
	mine = gameList[user];
	pthread_mutex_unlock(&gMineMutex);
	return mine;
}

static void GameOver(string user) {
	pthread_mutex_lock(&gMineMutex);
	std::map<string, Mine*>::iterator it = gameList.find(user);
	gameList.erase(it);
	pthread_mutex_unlock(&gMineMutex);
}

void Click(HTTP *http, int sock) {
	string user = http->Params["user"];
	string stCmd = http->Params["cmd"];
	string stX = http->Params["x"];
	string stY = http->Params["y"];
	if (user.empty() || stCmd.empty() || stX.empty() || stY.empty()) {
		char *data = "invalid parameter";
		Response(sock, data, sizeof(char) * strlen(data));
		return;
	}
	Mine *mine = GetGame(user);
	if (mine == NULL) {
		char *data = "Please initialize game";
		Response(sock, data, sizeof(char) * strlen(data));
		return;		
	}
	
	int cmd = atoi(stCmd.c_str());
	int x = atoi(stX.c_str());
	int y = atoi(stY.c_str());

	_printlog(__FILE__, __LINE__, PRIORITY_INFO, "%d %d %d", cmd, x, y);
	mine->ShowMap();

	if (mine->IsOutOfBound(x, y)) {
		return;
	} else if (cmd == CMD_MARK_MINE){
		mine->SetMine(x, y);
		if(mine->IsWin()) {
			_printlog(__FILE__, __LINE__, PRIORITY_INFO, "Win!!! Game Over");
			char *data = "game over, you win!";
			Response(sock, data, sizeof(char) * strlen(data));
			GameOver(user);
			return;
		}
		char *data = "mark success";
		string output = mine->GetMapString();
		Response(sock, (void*)output.c_str(), sizeof(char) * strlen(output.c_str()));
		return;

	} else if(cmd == CMD_OPEN) {
		if (mine->IsMark(x, y)) {
			_printlog(__FILE__, __LINE__, PRIORITY_INFO, "(%d, %d) alread marked", x, y);
			return;
		} else if(mine->IsMine(x, y)) {
			_printlog(__FILE__, __LINE__, PRIORITY_INFO, "Lose!!! Game Over");
			char *data = "game over, you loss!";
			Response(sock, data, sizeof(char) * strlen(data));
			GameOver(user);
			return;
		}
		mine->CalculateValues(x, y);
	} else {
		char *data = "invalid command";
		Response(sock, data, sizeof(char) * strlen(data));
		return;
	}
	mine->ShowMap();
	string output = mine->GetMapString();
	Response(sock, (void*)output.c_str(), sizeof(char) * strlen(output.c_str()));
}

int main(int argc, char **argv) {
	initlogs("Cover", PRIORITY_INFO, "./", 1, 1024);
	if (ReadConfig("config.xml") < 0) {
		_printlog(__FILE__, __LINE__, PRIORITY_ERROR, "ReadConfig Error");
		return -1;
	}
	AddModule("/", Index);
	AddModule("/index.html", Index);
	AddModule("/start", Start);
	AddModule("/click", Click);

	ListenAndServe();
	_printlog(__FILE__, __LINE__, PRIORITY_ERROR, "Run Error");
	freelog();
}

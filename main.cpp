#include<graphics.h>
#include <conio.h>
#include <stdlib.h>

#define WIDTH 640
#define HEIGHT 480

#define BY_POINTS 0
#define BY_FUNCTION 1

typedef int ballColor;

typedef struct _Point{
	double x;
	double y;
}Point;

typedef struct _BallOnList {
	ballColor color;
	struct _BallOnList* prev;
	struct _BallOnList* next;
}BallOnList;


typedef struct _BallList {
	int firstBallPosition;
	BallOnList* firstBall;
}BallList;

typedef struct _RouteFunctionArgs {
	//write route function args here
}RouteFunctionArgs;

typedef struct _MapInfo {
	int routeStoringMethod;
	Point* pointsArray;
	RouteFunctionArgs rfa;
	int ballCount;
}MapInfo;

typedef struct _MajorData {
	char* mapDir;
	BallList ballList;

}MajorData;

void coreGaming(MajorData md);
MapInfo loadingMapInfo(int method, char* dir);
void operatingInput();
void computingFlyingBalls();
void computingBallList();
void renderingImage(MapInfo mi);
void generatingBallList(BallList * ballList,int cnt);

int main(){
	MajorData md;
	int zoomingMultiple = 1;//图像缩放比例，默认为1，可在设置中调节



	IMAGE back;
	IMAGE p1;
	IMAGE p2;
	int i;

	initgraph(WIDTH, HEIGHT);

	loadimage(&back, "image\\background.jpg");
	loadimage(&p1, "image\\fish02.jpg");
	loadimage(&p2, "image\\fish01.jpg");
	i = 10;
	while (1) {
		BeginBatchDraw();//开始批量绘图
		putimage(0, 0, &back);
		//	putimage(i, 100, &p2);
		putimage(i, 100, &p1, SRCAND);
		putimage(i, 100, &p2, SRCINVERT);
		EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	
		Sleep(12);	//暂停
		i++;
		if (i > WIDTH - 10) i = 10;
	}

	return 0;
}


void coreGaming(MajorData md){
	bool gameEnded = false;
	MapInfo mi = loadingMapInfo(0, md.mapDir);//TODO:增加地图读取控制系统
	generatingBallList(&(md.ballList), mi.ballCount);
	while (!gameEnded) {
		operatingInput();//处理玩家操作
		computingFlyingBalls();//计算飞出球
		computingBallList();//计算列上球
		renderingImage(mi);//绘制图像
	}
}

MapInfo loadingMapInfo(int method,char* dir){
	MapInfo mi;
	
	//TODO:load file
	//use malloc here
	mi.routeStoringMethod = BY_POINTS;
	

	return mi;
}

void generatingBallList(BallList * pbl,int cnt) {
	pbl->firstBallPosition = 0;
	for (int i = 0; i < cnt; i++) {
		//generate one ball..
	}
	return;
}

void operatingInput(){

}

void computingFlyingBalls(){

}

void computingBallList(){

}

void renderingImage(MapInfo mi){

}
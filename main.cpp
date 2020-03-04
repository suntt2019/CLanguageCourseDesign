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
	int zoomingMultiple = 1;//ͼ�����ű�����Ĭ��Ϊ1�����������е���



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
		BeginBatchDraw();//��ʼ������ͼ
		putimage(0, 0, &back);
		//	putimage(i, 100, &p2);
		putimage(i, 100, &p1, SRCAND);
		putimage(i, 100, &p2, SRCINVERT);
		EndBatchDraw();//����������ͼ�������ƺõ�ͼƬͳһ������Ļ�ϡ�	
		Sleep(12);	//��ͣ
		i++;
		if (i > WIDTH - 10) i = 10;
	}

	return 0;
}


void coreGaming(MajorData md){
	bool gameEnded = false;
	MapInfo mi = loadingMapInfo(0, md.mapDir);//TODO:���ӵ�ͼ��ȡ����ϵͳ
	generatingBallList(&(md.ballList), mi.ballCount);
	while (!gameEnded) {
		operatingInput();//������Ҳ���
		computingFlyingBalls();//����ɳ���
		computingBallList();//����������
		renderingImage(mi);//����ͼ��
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
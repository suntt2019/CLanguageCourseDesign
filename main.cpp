#include<graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>

#define WIDTH 800
#define HEIGHT 600

#define STORE_BY_POINTS 0
#define STORE_BY_FUNCTION 1
#define FILE_NOT_FOUND -1
#define IO_ERROR -2
#define UNKNOWN_STORING_METHOD -3


//TODO:ɾ����ʱ�����õ�ȫ�ֱ���
IMAGE back;
IMAGE p1;
IMAGE p2;
int i;


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
	double minP, maxP;
	double Rx, Nx, Ox, Px, Kx, Bx;
	double Ry, Ny, Oy, Py, Ky, By;
}RouteFunctionArgs;

typedef struct _MapInfo {
	int routeStoringMethod;
	int ballCount;
	int maxLimit;//���ڴ����ԣ��ǵ����Ŀ�����ڴ淽�̶��ԣ��Ƿ��̿�ȡ���ֵ
	Point* pointsArray;
	RouteFunctionArgs* rfag;//route function args group
}MapInfo;

typedef struct _MajorData {
	bool gameEnd;
	char* mapDir;
	MapInfo mi;
	BallList ballList;

}MajorData;

void coreGaming(MajorData md);
MapInfo loadingMapInfo(int method, char* dir);
void operatingInput();
void computingFlyingBalls();
void computingBallList();
void renderingImage(MajorData& md);
void generatingBallList(BallList * ballList,int cnt);

int main(){
	MajorData md;
	int zoomingMultiple = 1;//ͼ�����ű�����Ĭ��Ϊ1�����������е���


	/*
	version 0.1:û��������/��ͣ�˵�/ѡ��˵���ֻ����Ϸ����
	ֻ���ۿ������е����յ�
	*/
	//TODO:���ӵ�ͼ��ȡ����ϵͳ
	md.mapDir = "test_function.zmap";
	

	//TODO: free mapInfo.pointsArray

	initgraph(WIDTH, HEIGHT,SHOWCONSOLE);

	loadimage(&back, "image\\background.jpg");
	loadimage(&p1, "image\\fish02.jpg");
	loadimage(&p2, "image\\fish01.jpg");
	i = 10;

	coreGaming(md);

	return 0;
}


Point route(MapInfo mi,double position){

	if (mi.routeStoringMethod == STORE_BY_POINTS && position <= mi.maxLimit) {//�˴�Ϊ��ģʽ
		printf("route:position=%.2lf,x=%.2lf,y=%.2lf\n", position, mi.pointsArray[(int)position].x, mi.pointsArray[(int)position].y);
		return mi.pointsArray[(int)position];
	}

	Point ret;
	ret.x = -1;//-1��ʾ��ȡʧ��
	ret.y = -1;//������longjump�����ǲ�Ȼ��ȫ�ֱ���....
	///TODO:�����Ƿ�ĳ���longjump
	//IMPORTANT����ǰ�ķ�ʽ�����⣬-1�ǿ��ܴ��ڵ�����������������ֵ���Ǳ��ٴ���������...


	if (mi.routeStoringMethod < 0)//�˴�Ϊ�Ƿ�ģʽ
		return ret;//Exception!!!

	//��ʱΪ·������ģʽ
	int periodId = 0;
	
	while (!(mi.rfag[periodId].minP <= position && position < mi.rfag[periodId].maxP) && periodId < mi.maxLimit)
		periodId++;

	RouteFunctionArgs arg = mi.rfag[periodId];
	if(periodId >= mi.maxLimit)
		return ret;//Exception!!!
	//printf("route-functionx:%.2lf*pow(%.2lf,%.2lf)*sin(%.2lf*%.2lf+%.2lf)+%.2lf*%.2lf+%.2lf\n", arg.Rx, position, arg.Nx, arg.Ox, position, arg.Px, arg.Kx, position, arg.Bx);
	//printf("route-functiony:%.2lf*pow(%.2lf,%.2lf)*sin(%.2lf*%.2lf+%.2lf)+%.2lf*%.2lf+%.2lf\n", arg.Ry, position, arg.Ny, arg.Oy, position, arg.Py, arg.Ky, position, arg.By);

	ret.x = arg.Rx * pow(position, arg.Nx) * sin(arg.Ox * position + arg.Px) + arg.Kx * position + arg.Bx;
	ret.y = arg.Ry * pow(position, arg.Ny) * sin(arg.Oy * position + arg.Py) + arg.Ky * position + arg.By;


	//printf("route:position=%.2lf,x=%.2lf,y=%.2lf\n", position, ret.x, ret.y);
	return ret;
}

void coreGaming(MajorData md){
	md.gameEnd = false;
	md.mi = loadingMapInfo(0, md.mapDir);

	//generatingBallList(&(md.ballList), mi.ballCount);
	while (!md.gameEnd) {
		/*
		operatingInput();//������Ҳ���
		computingFlyingBalls();//����ɳ���
		computingBallList();//����������
		*/
		renderingImage(md);//����ͼ��
		//Sleep(500);	//��ͣ
		//TODO:����һ��Ҫ��Ҫ��ʱ��Ȼ��sleep(1000/fps-��ʱ)
	}
	return;
}

MapInfo loadingMapInfo(int method,char* dir){
	char routeStoringMethodString[100] = { '\0' };
	MapInfo mi;
	int errorCode=0;
	jmp_buf env;
	const char exceptionHints[][30] = {"","map file not found","io exception","unknow storing method"};
	FILE* pf = fopen(dir,"r");
	if ((errorCode=setjmp(env)) != 0) {//�쳣����
		printf("%d\n", errorCode);
		mi.routeStoringMethod = -errorCode;
		printf("[Excption] loadingMapInfo: %s.\n", exceptionHints[errorCode]);
		return mi;
	}

	if (!pf)
		longjmp(env, 1);

	/*��ͼ�ļ���
		��һ�У�·���洢��ʽ(routeStoringMethod)������(ballCount)
		����ģ�������/·����������
	*/

	if (fscanf(pf, "%s %d", routeStoringMethodString, &mi.ballCount) !=2) 
		longjmp(env, 2);

	if (strcmp(routeStoringMethodString,"STORE_BY_POINTS")==0) {
		mi.routeStoringMethod = STORE_BY_POINTS;
		//�ڶ��м�¼������
		int pointCount,readPointCount=0;
		if (fscanf(pf, "%d", &pointCount) !=1)longjmp(env, 2);
		mi.pointsArray = (Point*)malloc(pointCount * sizeof(Point));
		mi.maxLimit = pointCount;

		printf("[DEBUG]map(points)\n");
		for (int i = 0; i < pointCount; i++) {
			if (mi.pointsArray && fscanf(pf, "%lf %lf", &mi.pointsArray[i].x, &mi.pointsArray[i].y) !=2)
				longjmp(env, 2);
				printf("[DEBUG]%.2lf %.2lf\n", mi.pointsArray[i].x, mi.pointsArray[i].y);
		}//��Ȼ������try-catch����һ����ifд�ĺ�����
		
	}else if (strcmp(routeStoringMethodString,"STORE_BY_FUNCTION")==0) {
		mi.routeStoringMethod = STORE_BY_FUNCTION;
		int periodCount;
		RouteFunctionArgs arg;
		if (fscanf(pf, "%d", &periodCount) !=1)longjmp(env, 2);
		mi.rfag = (RouteFunctionArgs*)malloc(periodCount * sizeof(RouteFunctionArgs));
		mi.maxLimit = periodCount;
		
		printf("[DEBUG]map(function)\n");
		for (int i = 0; i < periodCount; i++) {
			if (mi.pointsArray && fscanf(pf, "%lf %lf", &arg.minP,&arg.maxP) != 2)
				longjmp(env, 2);
			if (mi.pointsArray && fscanf(pf, "%lf %lf %lf %lf %lf %lf", &arg.Rx, &arg.Nx, &arg.Ox, &arg.Px, &arg.Kx, &arg.Bx) != 6)
				longjmp(env, 2);
			if (mi.pointsArray && fscanf(pf, "%lf %lf %lf %lf %lf %lf", &arg.Ry, &arg.Ny, &arg.Oy, &arg.Py, &arg.Ky, &arg.By) != 6)
				longjmp(env, 2);
			mi.rfag[i] = arg;
		}




	}else {
		longjmp(env, 3);
	}

	fclose(pf);
	
	

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

void renderingImage(MajorData& md){
	BeginBatchDraw();//��ʼ������ͼ
	putimage(0, 0, &back);
	//	putimage(i, 100, &p2);
	Point p = route(md.mi, i);
	putimage(p.x, p.y, &p1, SRCAND);
	putimage(p.x, p.y, &p2, SRCINVERT);
	EndBatchDraw();//����������ͼ�������ƺõ�ͼƬͳһ������Ļ�ϡ�	
	i++;
	//if (i >= md.mi.maxLimit)
	if (i >= 400)
		//md.gameEnd = true;
		i = 0;


	return;
}
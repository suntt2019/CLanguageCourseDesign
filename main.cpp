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


//TODO:删除临时测试用的全局变量
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
	int maxLimit;//对于存点而言，是点的数目；对于存方程而言，是方程可取最大值
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
	int zoomingMultiple = 1;//图像缩放比例，默认为1，可在设置中调节


	/*
	version 0.1:没有主界面/暂停菜单/选项菜单，只有游戏核心
	只能眼看着球列到达终点
	*/
	//TODO:增加地图读取控制系统
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

	if (mi.routeStoringMethod == STORE_BY_POINTS && position <= mi.maxLimit) {//此处为点模式
		printf("route:position=%.2lf,x=%.2lf,y=%.2lf\n", position, mi.pointsArray[(int)position].x, mi.pointsArray[(int)position].y);
		return mi.pointsArray[(int)position];
	}

	Point ret;
	ret.x = -1;//-1表示读取失败
	ret.y = -1;//很想用longjump，但是不然用全局变量....
	///TODO:考虑是否改成用longjump
	//IMPORTANT：当前的方式有问题，-1是可能存在的情况，如果返回其他值，那边再处理会很难受...


	if (mi.routeStoringMethod < 0)//此处为非法模式
		return ret;//Exception!!!

	//此时为路径函数模式
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
		operatingInput();//处理玩家操作
		computingFlyingBalls();//计算飞出球
		computingBallList();//计算列上球
		*/
		renderingImage(md);//绘制图像
		//Sleep(500);	//暂停
		//TODO:测试一下要不要计时，然后sleep(1000/fps-计时)
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
	if ((errorCode=setjmp(env)) != 0) {//异常处理
		printf("%d\n", errorCode);
		mi.routeStoringMethod = -errorCode;
		printf("[Excption] loadingMapInfo: %s.\n", exceptionHints[errorCode]);
		return mi;
	}

	if (!pf)
		longjmp(env, 1);

	/*地图文件：
		第一行：路径存储方式(routeStoringMethod)、球数(ballCount)
		后面的：点坐标/路径函数参数
	*/

	if (fscanf(pf, "%s %d", routeStoringMethodString, &mi.ballCount) !=2) 
		longjmp(env, 2);

	if (strcmp(routeStoringMethodString,"STORE_BY_POINTS")==0) {
		mi.routeStoringMethod = STORE_BY_POINTS;
		//第二行记录点总数
		int pointCount,readPointCount=0;
		if (fscanf(pf, "%d", &pointCount) !=1)longjmp(env, 2);
		mi.pointsArray = (Point*)malloc(pointCount * sizeof(Point));
		mi.maxLimit = pointCount;

		printf("[DEBUG]map(points)\n");
		for (int i = 0; i < pointCount; i++) {
			if (mi.pointsArray && fscanf(pf, "%lf %lf", &mi.pointsArray[i].x, &mi.pointsArray[i].y) !=2)
				longjmp(env, 2);
				printf("[DEBUG]%.2lf %.2lf\n", mi.pointsArray[i].x, mi.pointsArray[i].y);
		}//忽然好想念try-catch，这一行行if写的好难受
		
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
	BeginBatchDraw();//开始批量绘图
	putimage(0, 0, &back);
	//	putimage(i, 100, &p2);
	Point p = route(md.mi, i);
	putimage(p.x, p.y, &p1, SRCAND);
	putimage(p.x, p.y, &p2, SRCINVERT);
	EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	
	i++;
	//if (i >= md.mi.maxLimit)
	if (i >= 400)
		//md.gameEnd = true;
		i = 0;


	return;
}
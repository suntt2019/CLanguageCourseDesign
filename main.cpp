#include<graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>

#define PI 3.14159269

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

typedef struct _Point {
	double x;
	double y;
}Point;

typedef struct _BallOnList {
	ballColor color;
	struct _BallOnList* prev;
	struct _BallOnList* next;
}BallOnList;


typedef struct _BallList {
	double firstBallPosition;
	BallOnList* firstBall;
}BallList;

typedef struct _RouteFunctionArgs {
	double minP, maxP;
	double Rx, Nx, Ox, Px, Kx, Bx;
	double Ry, Ny, Oy, Py, Ky, By;
}RouteFunctionArgs;

typedef struct _Zuma {
	Point position;
	int thisBallColor,nextBallColor;
	double angle;
}Zuma;

typedef struct _MapInfo {
	int routeStoringMethod, ballCount;
	double ballR, moveSpeed;
	int colorCount,imgCount;
	IMAGE* imgs,*ballImgs,*ballMaskImgs;
	Point zumaPosition;
	int maxLimit;//对于存点而言，是点的数目；对于存方程而言，是方程可取最大值
	Point* pointsArray;
	RouteFunctionArgs* rfag;//route function args group
}MapInfo;

typedef struct _MajorData {
	bool gameEnd;
	char* mapDir;
	MapInfo mi;
	BallList ballList;
	Zuma zuma;
}MajorData;

void coreGaming(MajorData md);
MapInfo loadingMapInfo(int method, char* dir);
void operatingInput(MajorData& md);
void computingFlyingBalls();
void computingBallList(BallList& bl, MapInfo* pmi);
void paintImage(MajorData& md);
void initBallList(BallList* pbl, int cnt, unsigned int seed, int colorCount);
void viewBallList(BallList* pbl);
void initZuma(MajorData& md);
void paintZuma(Zuma zuma, MapInfo* pmi);
void operateMouseEvents(Zuma& zuma);
//TODO:free!!!


int main() {
	printf("%.2lf", atan(0));
	MajorData md;
	printf("[DEBUG]MajorData:%dB,MapInfo:%dB\n", (int)sizeof(MajorData), (int)sizeof(MapInfo));
	int zoomingMultiple = 1;//图像缩放比例，默认为1，可在设置中调节


	/*
	version 0.1:没有主界面/暂停菜单/选项菜单，只有游戏核心
	只能眼看着球列到达终点
	*/
	//TODO:增加地图读取控制系统
	md.mapDir = "maps\\test_function.zmap";


	//TODO: free mapInfo.pointsArray

	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);

	loadimage(&back, "image\\background.jpg");
	loadimage(&p1, "image\\fish02.jpg");
	loadimage(&p2, "image\\fish01.jpg");
	i = 10;

	coreGaming(md);

	return 0;
}


Point route(MapInfo mi, double position) {

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
	if (periodId >= mi.maxLimit)
		return ret;//Exception!!!
	//printf("route-functionx:%.2lf*pow(%.2lf,%.2lf)*sin(%.2lf*%.2lf+%.2lf)+%.2lf*%.2lf+%.2lf\n", arg.Rx, position, arg.Nx, arg.Ox, position, arg.Px, arg.Kx, position, arg.Bx);
	//printf("route-functiony:%.2lf*pow(%.2lf,%.2lf)*sin(%.2lf*%.2lf+%.2lf)+%.2lf*%.2lf+%.2lf\n", arg.Ry, position, arg.Ny, arg.Oy, position, arg.Py, arg.Ky, position, arg.By);

	ret.x = arg.Rx * pow(position, arg.Nx) * sin(arg.Ox * position + arg.Px) + arg.Kx * position + arg.Bx;
	ret.y = arg.Ry * pow(position, arg.Ny) * sin(arg.Oy * position + arg.Py) + arg.Ky * position + arg.By;


	//printf("route:position=%.2lf,x=%.2lf,y=%.2lf\n", position, ret.x, ret.y);
	return ret;
}

void coreGaming(MajorData md) {
	md.gameEnd = false;
	md.mi = loadingMapInfo(0, md.mapDir);
	initBallList(&md.ballList, md.mi.ballCount, (unsigned int)time(0), md.mi.colorCount);
	initZuma(md);
	//viewBallList(&md.ballList);
	while (!md.gameEnd) {
		operatingInput(md);//处理玩家操作
		computingFlyingBalls();//计算飞出球
		computingBallList(md.ballList,&md.mi);//计算列上球
		paintImage(md);//绘制图像
		//Sleep(500);	//暂停
		//TODO:测试一下要不要计时，然后sleep(1000/fps-计时)
	}
	return;
}

MapInfo loadingMapInfo(int method, char* dir) {
	char routeStoringMethodString[100] = { '\0' };
	MapInfo mi;
	int errorCode = 0;
	jmp_buf env;
	const char exceptionHints[][30] = { "","map file not found","io exception","unknow storing method" };
	if ((errorCode = setjmp(env)) != 0) {//异常处理
		printf("%d\n", errorCode);
		mi.routeStoringMethod = -errorCode;
		printf("[Excption] loadingMapInfo: %s.\n", exceptionHints[errorCode]);
		return mi;
	}

	FILE* pf = fopen(dir, "r");
	if (!pf)
		longjmp(env, 1);

	/*地图文件：
		第一行：路径存储方式(routeStoringMethod)、球数(ballCount)、球半径、球列移动速度
		后面的：点坐标/路径函数参数
	*/

	if (fscanf(pf, "%s %d %d %lf %lf", routeStoringMethodString, &mi.colorCount, &mi.ballCount, &mi.ballR, &mi.moveSpeed) != 5)
		longjmp(env, 2);
	
	mi.ballImgs = new IMAGE[mi.colorCount];
	mi.ballMaskImgs = new IMAGE[mi.colorCount];
	if (!mi.ballImgs||!mi.ballMaskImgs)
		longjmp(env, 4);
	char strI[10], imgDir[100], imgMaskDir[100];//C语言处理字符串是真的麻烦Orz
	for (int i = 0; i < mi.colorCount; i++) {
		sprintf(strI, "%d", i);
		strcpy(imgDir, "image\\fish_");
		strcpy(imgMaskDir, "image\\fish_mask_");
		strcat(strcat(imgDir, strI), ".jpg");
		strcat(strcat(imgMaskDir, strI), ".jpg");
		loadimage(&mi.ballImgs[i], imgDir);
		loadimage(mi.ballMaskImgs+i, imgMaskDir);
	}

	mi.imgs = new IMAGE[10];//TODO:增加前景功能（之前测试较大前进会导致明显性能下降）
	loadimage(mi.imgs, "image\\background.jpg");
	
	//if (fscanf(pf, "%d", &mi.imgCount) != 1)
	//	longjmp(env, 2);
	
	if (fscanf(pf, "%lf %lf", &mi.zumaPosition.x, &mi.zumaPosition.y) != 2)
		longjmp(env, 2);
	printf("loaded zuma position:x=%.2lf, y=%.2lf", mi.zumaPosition.x, mi.zumaPosition.y);
	loadimage(mi.imgs+2, "image\\zuma.jpg");
	loadimage(mi.imgs+3, "image\\zuma_mask.jpg");

	if (strcmp(routeStoringMethodString, "STORE_BY_POINTS") == 0) {
		mi.routeStoringMethod = STORE_BY_POINTS;
		//第二行记录点总数
		int pointCount, readPointCount = 0;
		if (fscanf(pf, "%d", &pointCount) != 1)longjmp(env, 2);
		mi.pointsArray = (Point*)malloc(pointCount * sizeof(Point));
		if (!mi.pointsArray)
			longjmp(env, 4);
		mi.maxLimit = pointCount;

		printf("[DEBUG]map(points)\n");
		for (int i = 0; i < pointCount; i++) {
			if (fscanf(pf, "%lf %lf", &mi.pointsArray[i].x, &mi.pointsArray[i].y) != 2)
				longjmp(env, 2);
			printf("[DEBUG]%.2lf %.2lf\n", mi.pointsArray[i].x, mi.pointsArray[i].y);
		}//忽然好想念try-catch，这一行行if写的好难受

	}
	else if (strcmp(routeStoringMethodString, "STORE_BY_FUNCTION") == 0) {
		mi.routeStoringMethod = STORE_BY_FUNCTION;
		int periodCount;
		RouteFunctionArgs arg = { 0 };
		if (fscanf(pf, "%d", &periodCount) != 1)longjmp(env, 2);
		mi.rfag = (RouteFunctionArgs*)malloc(periodCount * sizeof(RouteFunctionArgs));
		if (!mi.pointsArray)
			longjmp(env, 4);
		mi.maxLimit = periodCount;

		printf("[DEBUG]map(function)\n");
		for (int i = 0; i < periodCount; i++) {
			if (fscanf(pf, "%lf %lf", &arg.minP, &arg.maxP) != 2)
				longjmp(env, 2);
			if (fscanf(pf, "%lf %lf %lf %lf %lf %lf", &arg.Rx, &arg.Nx, &arg.Ox, &arg.Px, &arg.Kx, &arg.Bx) != 6)
				longjmp(env, 2);
			if (fscanf(pf, "%lf %lf %lf %lf %lf %lf", &arg.Ry, &arg.Ny, &arg.Oy, &arg.Py, &arg.Ky, &arg.By) != 6)
				longjmp(env, 2);
			mi.rfag[i] = arg;
		}

	}
	else {
		longjmp(env, 3);
	}

	fclose(pf);



	return mi;
}



void initBallList(BallList* pbl, int cnt, unsigned int seed, int colorCount) {
	int errorCode;
	jmp_buf env;
	const char exceptionHints[][40] = { "","excption when creating the first ball","excption when creating ball","unknow storing method" };
	if ((errorCode = setjmp(env)) != 0) {//异常处理
		printf("%d\n", errorCode);
		printf("[Excption] initBallList: %s.\n", exceptionHints[errorCode]);
		return;
	}

	srand(seed);
	pbl->firstBallPosition = 0;
	BallOnList* p = (BallOnList*)malloc(sizeof(BallOnList));
	if (!p)
		longjmp(env, 1);
	pbl->firstBall = p;
	pbl->firstBall->color = rand() % colorCount;
	pbl->firstBall->next = NULL;
	pbl->firstBall->prev = NULL;

	for (int i = 1; i < cnt; i++) {
		p->next = (BallOnList*)malloc(sizeof(BallOnList));
		if (!p->next)
			longjmp(env, 2);
		p->next->prev = p;
		p->next->color = rand() % colorCount;
		p = p->next;
	}
	p->next = NULL;
	return;
}

void viewBallList(BallList* pbl) {
	printf("[viewBallList] firstBallPosition=%.2lf \n", pbl->firstBallPosition);
	BallOnList* p = pbl->firstBall;
	int i = 0;
	while (p) {
		printf("ball#%d: p=%p, p->color=%d, p->prev=%p, p->next=%p\n", i, p, p->color, p->prev, p->next);
		p = p->next;
		i++;
	}
	return;
}

void paintBallList(BallList& bl, MapInfo* pmi) {
	double thisBallPosition = bl.firstBallPosition;
	BallOnList* p = bl.firstBall;
	putimage(0, 0, pmi->imgs);
	while (p) {
		//printf("[DEBUG]thisBallPosition=%lf\n", thisBallPosition);
		Point point = route(*pmi, thisBallPosition);
		//printf("[DEBUG]pointx=%lf, pointy=%lf\n", point.x,point.y);
		putimage(point.x, point.y, pmi->ballMaskImgs + p->color, SRCAND);
		putimage(point.x, point.y, pmi->ballImgs+p->color, SRCINVERT);
		p = p->next;
		thisBallPosition -= pmi->ballR*2;
	} 
	return;
}

void initZuma(MajorData& md) {
	md.zuma.position = md.mi.zumaPosition;
	return;
}

void paintZuma(Zuma zuma, MapInfo* pmi) {
	//printf("paintZuma,x=%.2lf,y=%.2lf\n", zuma.position.x, zuma.position.y);
	IMAGE rotatedZuma, rotatedZumaMask;
	rotateimage(&rotatedZuma, pmi->imgs + 2, zuma.angle + PI / 2, BLACK, true, true);
	rotateimage(&rotatedZumaMask, pmi->imgs + 3, zuma.angle + PI / 2, WHITE, true, true);
	double startingPositionX, startingPositionY;
	startingPositionX = zuma.position.x - rotatedZuma.getwidth()/ 2;
	startingPositionY = zuma.position.y - rotatedZuma.getheight()/2;
	putimage(startingPositionX, startingPositionY, &rotatedZumaMask, SRCAND);
	putimage(startingPositionX, startingPositionY, &rotatedZuma, SRCINVERT);
	return;
}

void operatingInput(MajorData& md) {
	operateMouseEvents(md.zuma);
	return;
}

void computingFlyingBalls() {
	return;
}

void computingBallList(BallList& bl, MapInfo* pmi) {
	bl.firstBallPosition += pmi->moveSpeed;
	return;
}

void paintImage(MajorData& md) {
	BeginBatchDraw();//开始批量绘图
	putimage(0, 0, &back);
	paintBallList(md.ballList, &md.mi);
	paintZuma(md.zuma,&md.mi);
	EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	

	//仅供测试时使用
	if (md.ballList.firstBallPosition > 400)
		md.ballList.firstBallPosition = 0;


	return;
}


void operateMouseEvents(Zuma& zuma) {
	MOUSEMSG mmsg;
	double deltaX, deltaY, tanOfAngle;
	while (MouseHit()) {
		mmsg = GetMouseMsg();
		deltaX = mmsg.x - zuma.position.x;
		deltaY = -(mmsg.y - zuma.position.y);
		if (deltaX == 0)
			deltaX = 0.0001;
		tanOfAngle = deltaY / deltaX;
		zuma.angle = atan(tanOfAngle);
		if (deltaX<0)
			zuma.angle += PI;
		//printf("mouse:x=%d,y=%d,deltaX=%.2lf,deltaY=%.2lf,tanOfAngle=%.4lf,angle=%.4lf\n", mmsg.x, mmsg.y,deltaX,deltaY,tanOfAngle,zuma.angle);
	}
	return;
}
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include <winbase.h>

#define PI 3.14159269

#define WIDTH 800
#define HEIGHT 600

#define STORE_BY_POINTS 0
#define STORE_BY_FUNCTION 1
#define FILE_NOT_FOUND -1
#define IO_ERROR -2
#define UNKNOWN_STORING_METHOD -3

#define FLYING_BALL_ARRAY_SIZE 100

//#define double float


//#define STORED 1


typedef struct _Point {
	double x;
	double y;
}Point;

typedef struct _BallOnList {
	double position;
	int color;
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
	int CDremain;
}Zuma;

typedef struct _FlyingBall {
	Point position;
	//bool launched;//这东西其实没用
	int color;
	double angle;
}FlyingBall;

typedef struct _FlyingBallArray {
	int size;
	FlyingBall* pfb;
}FlyingBallArray;

typedef struct _MapInfo {
	int routeStoringMethod, ballCount;
	double ballR, moveSpeed, flySpeed;
	int colorCount,imgCount;
	IMAGE* imgs,*ballImgs,*ballMaskImgs;
	Point zumaPosition,deltaMouthPosition,deltaHolePosition;
	int shootingCD;
	int maxLimit;//对于存点而言，是点的数目；对于存方程而言，是方程可取最大值
	Point* pointsArray;
	RouteFunctionArgs* rfag;//route function args group
	Point* routePoints,* speedPoints;
	double* speedValues;
	double* speedArgles;
}MapInfo;

typedef struct _MajorData {
	bool gameEnd;
	char* mapDir;
	MapInfo mi;
	BallList ballList;
	Zuma zuma;
	FlyingBallArray flyingBallArray;
}MajorData;

//int STORED;//for debug only


void coreGaming(MajorData md);
MapInfo loadingMapInfo(char* dir);
void operatingInput(MajorData& md);
void computingFlyingBalls(FlyingBallArray& fba, Zuma zuma,BallList& bl, MapInfo* pmi);
bool computingBallList(BallList& bl, MapInfo* pmi);
void paintImage(MajorData& md);
void initBallList(BallList* pbl, MapInfo* pmi, unsigned int seed);
void viewBallList(BallList* pbl);
void initZuma(MajorData& md);
void paintZuma(Zuma zuma, MapInfo* pmi);
void operateMouseEvents(MajorData& md);
void initFlyingBallArray(FlyingBallArray& fba, MapInfo* pmi);
void generateFlyingBall(FlyingBallArray& fba, int colorCount);
void rotateAndPaint(IMAGE* img, IMAGE* imgMask, double angle, Point position,bool highQuality);
inline bool isOutOfScreen(Point p);
void justPaint(IMAGE* img, IMAGE* imgMask, Point position);
inline bool testPointDistance(Point p1, Point p2, double minD);
void insertBallList(BallList& bl,BallOnList* pbol_prev, BallOnList* pbol_next, FlyingBallArray& fba, int index, MapInfo* pmi);
void removeFlyingBall(FlyingBallArray& fba, int index);
inline bool compareDistance(Point p, Point pTrue, Point pFalse);
void initPainting();
Point speed(MapInfo* pmi, double position);
void initStoredRoute(MapInfo* pmi);
double getSpeedValue(MapInfo* pmi, double position);
Point makePoint(double x, double y);
//TODO:free!!!
//TODO:拆分函数！！

int main() {
	//SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
	MajorData md;
	printf("[DEBUG]MajorData:%dB,MapInfo:%dB\n", (int)sizeof(MajorData), (int)sizeof(MapInfo));
	int zoomingMultiple = 1;//图像缩放比例，默认为1，可在设置中调节
	

	/*
	version 0.1:没有主界面/暂停菜单/选项菜单，只有游戏核心
	只能眼看着球列到达终点
	*/
	//TODO:增加地图读取控制系统
	md.mapDir = "maps\\test_function-classic.zmap";


	//TODO: free mapInfo.pointsArray

	coreGaming(md);
	TCHAR s[] = _T("GAME OVER");
	outtextxy(10, 20, s);
	_getch();

	return 0;
}


Point route(MapInfo* pmi, double position) {
	//printf("route(%.2lf)..",position);
	//if (STORED&& pmi->rfag[0].minP < position && position < pmi->rfag[pmi->maxLimit-1].maxP)
		//return pmi->routePoints[(int)position];
	if (pmi->routeStoringMethod == STORE_BY_POINTS && position <= pmi->maxLimit) {//此处为点模式
		printf("route:position=%.2lf,x=%.2lf,y=%.2lf\n", position, pmi->pointsArray[(int)position].x, pmi->pointsArray[(int)position].y);
		return pmi->pointsArray[(int)position];//TODO:做出新的点模式
	}

	Point ret;
	ret.x = -1;//-1表示读取失败
	ret.y = -1;//TODO:全局longjmp 
	///TODO:考虑是否改成用longjump
	//IMPORTANT：当前的方式有问题，-1是可能存在的情况，如果返回其他值，那边再处理会很难受...


	if (pmi->routeStoringMethod < 0)//此处为非法模式
		return ret;//Exception!!!

	//此时为路径函数模式
	int periodId = 0;

	while (!(pmi->rfag[periodId].minP <= position && position < pmi->rfag[periodId].maxP) && periodId < pmi->maxLimit)
		periodId++;

	RouteFunctionArgs arg = pmi->rfag[periodId];
	if (periodId >= pmi->maxLimit)
		return ret;//TODO:Exception!!!
	//printf("route-functionx:%.2lf*pow(%.2lf,%.2lf)*sin(%.2lf*%.2lf+%.2lf)+%.2lf*%.2lf+%.2lf\n", arg.Rx, position, arg.Nx, arg.Ox, position, arg.Px, arg.Kx, position, arg.Bx);
	//printf("route-functiony:%.2lf*pow(%.2lf,%.2lf)*sin(%.2lf*%.2lf+%.2lf)+%.2lf*%.2lf+%.2lf\n", arg.Ry, position, arg.Ny, arg.Oy, position, arg.Py, arg.Ky, position, arg.By);

	ret.x = arg.Rx * pow(position, arg.Nx) * sin(arg.Ox * position + arg.Px) + arg.Kx * position + arg.Bx;
	ret.y = arg.Ry * pow(position, arg.Ny) * sin(arg.Oy * position + arg.Py) + arg.Ky * position + arg.By;


	//printf("route:position=%.2lf,x=%.2lf,y=%.2lf\n", position, ret.x, ret.y);
	return ret;
}


Point speed(MapInfo* pmi, double position) {
	if (STORED&& pmi->rfag[0].minP < position && position < pmi->rfag[pmi->maxLimit-1].maxP)
		return pmi->speedPoints[(int)position];
	Point ret;
	if (pmi->routeStoringMethod == STORE_BY_POINTS && position <= pmi->maxLimit && position >= 1) {
		ret.x = pmi->pointsArray[(int)position].x - pmi->pointsArray[(int)position - 1].x;
		ret.y = pmi->pointsArray[(int)position].y - pmi->pointsArray[(int)position - 1].y;
	}
	else {

		ret.x = -1;
		ret.y = -1;
		int periodId = 0;
		if (position <= pmi->rfag[0].minP){
			ret.x = 1;
			ret.y = 1;
			return ret;
		}

		while (!(pmi->rfag[periodId].minP <= position && position < pmi->rfag[periodId].maxP) && periodId < pmi->maxLimit)
				periodId++;
		RouteFunctionArgs arg = pmi->rfag[periodId];
		if (periodId >= pmi->maxLimit)
			return ret;//TODO:Exception!!!
		ret.x = arg.Rx * pow(position, arg.Nx-1) *( arg.Nx * sin(arg.Ox * position + arg.Px) + arg.Ox * position * cos(arg.Ox * position + arg.Px))+ arg.Kx;
		ret.y = arg.Ry * pow(position, arg.Ny-1) *( arg.Ny * sin(arg.Oy * position + arg.Py) + arg.Oy * position * cos(arg.Oy * position + arg.Py))+ arg.Ky;

	}
	return ret;
}

double getSpeedValue(MapInfo* pmi, double position) {
	//printf("getSpeedValue(pmi,%.2lf)=",position);
	if (pmi->rfag[0].minP >= position) {
		//printf("   1;\n");
		return 1;
	}
	//printf("%.2lf\n", pmi->speedValues[(int)position]);
	return pmi->speedValues[(int)position];
}
/*
	if (STORED&& pmi->rfag[0].minP < position && position < pmi->rfag[pmi->maxLimit-1].maxP)
		return pmi->speedValues[(int)position];
	Point v = speed(pmi, position);
	return sqrt(pow(v.x, 2) + pow(v.y, 2));
	*/
/*
double speedDirection(MapInfo* pmi, double position) {
	if (STORED&& pmi->rfag[0].minP < position && position < pmi->rfag[pmi->maxLimit-1].maxP)
		return pmi->speedArgles[(int)position];
	Point v = speed(pmi, position);
	double tanOfAngle,ret;
	if (v.x == 0)
		v.x = 0.0001;
	tanOfAngle = v.y / v.x;
	ret = - atan(tanOfAngle);
	if (v.x < 0)
		ret += PI;
	return ret;
}
*/


void coreGaming(MajorData md) {
	md.gameEnd = false;
	STORED = 0;
	md.mi = loadingMapInfo(0, md.mapDir);
	STORED = 1;
	initBallList(&md.ballList, &md.mi, (unsigned int)time(0));
	initZuma(md);
	initFlyingBallArray(md.flyingBallArray,&md.mi);
	initPainting();
	//viewBallList(&md.ballList);
	while (!md.gameEnd) {
		operatingInput(md);//处理玩家操作
		computingFlyingBalls(md.flyingBallArray,md.zuma,md.ballList,&md.mi);//计算飞出球
		md.gameEnd = computingBallList(md.ballList, &md.mi);//计算列上球
		paintImage(md);//绘制图像
		if (md.zuma.CDremain > 0)
			md.zuma.CDremain--;
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

	if (fscanf(pf, "%s %d %d %lf %lf %lf", routeStoringMethodString, &mi.colorCount, &mi.ballCount, &mi.ballR, &mi.moveSpeed,&mi.flySpeed) != 6)
		longjmp(env, 2);
	
	mi.ballImgs = new IMAGE[mi.colorCount];//OOP???
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
	
	if (fscanf(pf, "%lf %lf %lf %lf %lf %lf %d", &mi.zumaPosition.x, &mi.zumaPosition.y,
		&mi.deltaMouthPosition.x, &mi.deltaMouthPosition.y, &mi.deltaHolePosition.x, &mi.deltaHolePosition.y,&mi.shootingCD) != 7)
		longjmp(env, 2);
	//printf("loaded zuma position:x=%.2lf, y=%.2lf", mi.zumaPosition.x, mi.zumaPosition.y);
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
		initStoredRoute(&mi);

	}
	else {
		longjmp(env, 3);
	}

	fclose(pf);



	return mi;
}

Point makePoint(double x, double y) {
	Point ret;
	ret.x = x;
	ret.y = y;
	return ret;
}


void initStoredRoute(MapInfo* pmi) {
	printf("Start precomputing...\n");
	int maxPosition = pmi->rfag[pmi->maxLimit - 1].maxP;
	double tanOfAngle;
	Point v;
	pmi->routePoints = (Point*)malloc(sizeof(Point) * (maxPosition + 10 + pmi->ballR * 2));
	pmi->speedPoints = (Point*)malloc(sizeof(Point) * (maxPosition + 10 + pmi->ballR * 2));
	pmi->speedValues = (double*)malloc(sizeof(double) * (maxPosition + 10 + pmi->ballR * 2));
	pmi->speedArgles = (double*)malloc(sizeof(double) * (maxPosition + 10 + pmi->ballR * 2));
	if (!pmi->routePoints || !pmi->speedPoints || !pmi->speedValues || !pmi->speedArgles) {
		printf("Exception when malloc");
		return;
	}
	for (int i = 0; i < maxPosition; i++) {
		pmi->routePoints[i] = route(pmi, i);
		v = speed(pmi, i);
		pmi->speedPoints[i] = v;
		pmi->speedValues[i] = sqrt(pow(v.x, 2) + pow(v.y, 2));
		if (v.x == 0)
			v.x = 0.0001;
		tanOfAngle = v.y / v.x;
		pmi->speedArgles[i] = -atan(tanOfAngle);
		if (v.x < 0)
			pmi->speedArgles[i] += PI;
		printf("[Precomputing]position=%d,route=(%.2lf,%.2lf),speed=(%.2lf,%.2lf),speedValue=%.2lf,speedArgle=%.2lf\n",
			i, pmi->routePoints[i].x, pmi->routePoints[i].y, pmi->speedPoints[i].x, pmi->speedPoints[i].y, pmi->speedValues[i], pmi->speedArgles[i]);
	}
	for (int i = maxPosition;i< maxPosition+10+pmi->ballR*2; i++) {//避免最后结尾的时候蹭出一点超出阈值的，下标越界
		pmi->routePoints[i] = makePoint(-10-pmi->ballR*2, -10 - pmi->ballR * 2);
		pmi->speedPoints[i] = makePoint(0.707,0.707);
		pmi->speedValues[i] = 1;
		pmi->speedArgles[i] = 0;
		printf("[Precomputing]position=%d,route=(%.2lf,%.2lf),speed=(%.2lf,%.2lf),speedValue=%.2lf,speedArgle=%.2lf\n",
			i, pmi->routePoints[i].x, pmi->routePoints[i].y, pmi->speedPoints[i].x, pmi->speedPoints[i].y, pmi->speedValues[i], pmi->speedArgles[i]);

	}
	printf("Precomputing ended.\n");
	return;
}


void initBallList(BallList* pbl, MapInfo* pmi, unsigned int seed) {
	int errorCode;
	double ballPostion = 0;
	jmp_buf env;
	const char exceptionHints[][40] = { "","excption when creating the first ball","excption when creating ball","unknow storing method" };
	if ((errorCode = setjmp(env)) != 0) {//异常处理
		printf("%d\n", errorCode);
		printf("[Excption] initBallList: %s.\n", exceptionHints[errorCode]);
		return;
	}

	srand(seed);
	//pbl->firstBallPosition = 0;
	BallOnList* p = (BallOnList*)malloc(sizeof(BallOnList));
	if (!p)
		longjmp(env, 1);
	pbl->firstBall = p;
	pbl->firstBall->color = rand() % pmi->colorCount;
	pbl->firstBall->next = NULL;
	pbl->firstBall->prev = NULL;

	for (int i = 1; i < pmi->ballCount; i++) {
		p->position = ballPostion;
		p->next = (BallOnList*)malloc(sizeof(BallOnList));
		if (!p->next)
			longjmp(env, 2);
		p->next->prev = p;
		p->next->color = rand() % pmi->colorCount;
		p = p->next;
		ballPostion -= pmi->ballR * 2 / getSpeedValue(pmi, ballPostion);
	}
	p->position = ballPostion;
	p->next = NULL;
	return;
}

void insertBallList(BallList& bl,BallOnList* pbol_prev,BallOnList* pbol_next, FlyingBallArray& fba,int index,MapInfo* pmi) {
	BallOnList* p = (BallOnList*)malloc(sizeof(BallOnList));
	BallOnList* q;
	if (!p){
		printf("[Excption] initBallList: excption when creating ball.\n");
		return;
	}
	p->color = fba.pfb[index].color;
	p->prev = pbol_prev;
	p->next = pbol_next;
	//printf("%.4lf~between %.4lf and %.4lf\n", p->position,pbol_prev->position,pbol_next->position);

	if (!pbol_prev) {
		p->position = pbol_next->position + 2 * pmi->ballR / getSpeedValue(pmi, pbol_next->position);
		bl.firstBall = p;
		pbol_next->prev = p;
	}else if (!pbol_next) {
		p->position = pbol_prev->position - 2 * pmi->ballR / getSpeedValue(pmi, pbol_prev->position);
		pbol_prev->next = p;
	}else {
		p->position = pbol_prev->position;
		pbol_prev->next = p;
		pbol_next->prev = p;
		q = pbol_prev;
		while (q) {
			q->position += 2 * pmi->ballR / getSpeedValue(pmi, q->position);
			q = q->prev;
		}
		/*
		q = pbol_next;
		while (q) {
			q->position -= pmi->ballR / getSpeedValue(pmi, q->position);
			q = q->next;
		}
		*/
	}






	/*
	if(!pbol_next)
		p->position = pbol_prev->position - pmi->ballR / speedValue(*pmi, pbol_prev->position);
	if (pbol_prev) {
		pbol_prev->next = p;
		q = pbol_prev;
		while (q) {
			q->position += pmi->ballR / speedValue(*pmi, q->position);
			q = q->prev;
		}
	}else {
		bl.firstBall = p;
		p->position = pbol_next->position + pmi->ballR / speedValue(*pmi, pbol_next->position);
	}
	if (pbol_next) {
		pbol_next->prev = p;
		q = pbol_next;
		while (q) {
			q->position -= pmi->ballR / speedValue(*pmi, q->position);
			q = q->next;
		}
	}
	if(pbol_prev&& pbol_next)
		p->position = (pbol_prev->position + pbol_next->position) / 2;
	*/
		
	removeFlyingBall(fba, index);
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
	BallOnList* p = bl.firstBall;
	while (p) {
		if (p->position < 0) {
			p = p->next;
			continue;
		}
		//justPaint(pmi->ballImgs + p->color, pmi->ballMaskImgs + p->color, pmi->routePoints[(int)p->position]);
		rotateAndPaint(pmi->ballImgs + p->color, pmi->ballMaskImgs + p->color, pmi->speedArgles[(int)p->position], pmi->routePoints[(int)p->position],false);
		fillcircle(pmi->routePoints[(int)p->position].x, pmi->routePoints[(int)p->position].y, pmi->ballR);
		p = p->next;
	}
	return;
}

void initZuma(MajorData& md) {
	md.zuma.position = md.mi.zumaPosition;
	md.zuma.CDremain = 0;
	return;
}

void paintZuma(Zuma zuma, MapInfo* pmi) {
	//printf("paintZuma,x=%.2lf,y=%.2lf\n", zuma.position.x, zuma.position.y);
	rotateAndPaint(pmi->imgs + 2, pmi->imgs + 3, zuma.angle + PI / 2, zuma.position,true);

	return;
}


void rotateAndPaint(IMAGE* img,IMAGE* imgMask,double angle,Point position,bool highQuality) {
	IMAGE rotatedImg, rotatedImgMask;
	rotateimage(&rotatedImg, img, angle, BLACK, true, highQuality);
	rotateimage(&rotatedImgMask, imgMask, angle, WHITE, true, highQuality);
	double startingPositionX, startingPositionY;
	startingPositionX = position.x - rotatedImg.getwidth() / 2;
	startingPositionY = position.y - rotatedImg.getheight() / 2;
	putimage(startingPositionX, startingPositionY, &rotatedImgMask, SRCAND);
	putimage(startingPositionX, startingPositionY, &rotatedImg, SRCINVERT);
	return;
}

void justPaint(IMAGE* img, IMAGE* imgMask, Point position) {
	putimage(position.x-imgMask->getwidth()/2, position.y-imgMask->getheight()/2, imgMask, SRCAND);
	putimage(position.x-img->getwidth()/2, position.y-img->getheight()/2, img, SRCINVERT);
	return;
}
//统一确定int和double

void initFlyingBallArray(FlyingBallArray& fba, MapInfo* pmi) {
	fba.pfb = (FlyingBall*)malloc(sizeof(FlyingBall) * FLYING_BALL_ARRAY_SIZE);
	fba.size = 0;
	generateFlyingBall(fba, pmi->colorCount);
	generateFlyingBall(fba, pmi->colorCount);
	return;
}

void generateFlyingBall(FlyingBallArray& fba,int colorCount) {
	FlyingBall newBall;
	//有些局部变量只是为了代码看起来美观，不过牺牲了效率，emm，或许其实开优化之后都一样？
	newBall.color = rand() % colorCount;
	//newBall.launched = false;
	fba.pfb[fba.size] = newBall;
	fba.size++;
	return;
}

void removeFlyingBall(FlyingBallArray& fba, int index) {
	for (int i = index+1; i < fba.size; i++) {
		fba.pfb[i - 1] = fba.pfb[i];
	}
	fba.size--;
	return;
}

inline bool isOutOfScreen(Point p) {
	//严格来说应该要计算旋转后有没有出画面，这里就算锚点是否出画面了，算是一种省事而节约资源的方法
	//TODO：写一个重载的isOutOfScreen，计算旋转后的图形是否出画面
	return !(0 <= p.x && p.x <= WIDTH && 0 <= p.y && p.y <= HEIGHT);
}
//TODO：更新WIDTH和HEIGHT为动态窗口大小

void launchFlyingBall(FlyingBallArray& fba, int colorCount) {
	//fba.pfb[fba.size - 3].launched = true;
	generateFlyingBall(fba,colorCount);
	return;
}

void paintFlyingBall(FlyingBallArray& fba,Zuma zuma, MapInfo* pmi) {//TODO:绘制镜像翻转的鱼（flyingball），使之眼睛朝上？
	for (int i = 0; i < fba.size; i++) {
		rotateAndPaint(pmi->ballImgs + fba.pfb[i].color, pmi->ballMaskImgs + fba.pfb[i].color, fba.pfb[i].angle, fba.pfb[i].position,true);
		fillcircle(fba.pfb[i].position.x, fba.pfb[i].position.y, pmi->ballR);
		//floodfill(fba.pfb[i].position.x, fba.pfb[i].position.y, WHITE);
	}
	return;
}

void testCrash(BallList& bl,FlyingBallArray& fba,int index,MapInfo* pmi) {
	BallOnList* p = bl.firstBall;
	//double thisBallPosition = bl.firstBallPosition;
	while (p) {
		if (testPointDistance(pmi->routePoints[(int)p->position], fba.pfb[index].position, pmi->ballR * 2)) {
			if(compareDistance(fba.pfb[index].position,
				(p->position > pmi->ballR * 2 ? pmi->routePoints[(int)(p->position-pmi->ballR*2)] : pmi->routePoints[0]), pmi->routePoints[(int)(p->position+pmi->ballR*2)]))
				insertBallList(bl,p->prev, p, fba, index,pmi);
			else
				insertBallList(bl, p, p->next, fba, index,pmi);
			break;
		}
		p = p->next;
		//thisBallPosition -= pmi->ballR * 2;
	}
}

inline bool testPointDistance(Point p1, Point p2,double minD) {
	return pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2)<pow(minD,2);
}

inline bool compareDistance(Point p, Point pTrue, Point pFalse) {
	return pow((p.x - pTrue.x), 2) + pow((p.y - pTrue.y), 2) > pow((p.x - pFalse.x), 2) + pow((p.y - pFalse.y), 2);
}

void operatingInput(MajorData& md) {
	operateMouseEvents(md);
	return;
}

void computingFlyingBalls(FlyingBallArray& fba,Zuma zuma,BallList& bl,MapInfo* pmi) {
	fba.pfb[fba.size - 2].position.x = zuma.position.x
		+ pmi->deltaMouthPosition.y * cos(zuma.angle)+ pmi->deltaMouthPosition.x * sin(zuma.angle);
	fba.pfb[fba.size - 2].position.y = zuma.position.y
		- pmi->deltaMouthPosition.y * sin(zuma.angle)+ pmi->deltaMouthPosition.x * cos(zuma.angle);
	fba.pfb[fba.size - 1].position.x = zuma.position.x
		+ pmi->deltaHolePosition.y * cos(zuma.angle)+ pmi->deltaHolePosition.x * sin(zuma.angle);
	fba.pfb[fba.size - 1].position.y = zuma.position.y
		- pmi->deltaHolePosition.y * sin(zuma.angle)+ pmi->deltaHolePosition.x * cos(zuma.angle);
	fba.pfb[fba.size - 2].angle = zuma.angle;
	fba.pfb[fba.size - 1].angle = zuma.angle;
	for (int i = 0; i < fba.size - 2; i++) {
		fba.pfb[i].position.x += pmi->flySpeed*cos(fba.pfb[i].angle);
		fba.pfb[i].position.y -= pmi->flySpeed*sin(fba.pfb[i].angle);
		if (isOutOfScreen(fba.pfb[i].position)) {
			removeFlyingBall(fba, i);
		}
		testCrash( bl, fba,i, pmi);
	}

	return;
}

bool computingBallList(BallList& bl, MapInfo* pmi) {
	//bl.firstBallPosition += pmi->moveSpeed;
	BallOnList* p = bl.firstBall;
	if (p->position >= pmi->rfag[pmi->maxLimit - 1].maxP)
		return true;
	while (p) {
		p->position += pmi->moveSpeed / getSpeedValue(pmi, p->position);
		p = p->next;
	}
	return false;
}

void paintImage(MajorData& md) {
	BeginBatchDraw();//开始批量绘图
	putimage(0, 0, md.mi.imgs);
	paintBallList(md.ballList, &md.mi);
	paintFlyingBall(md.flyingBallArray,md.zuma, &md.mi);
	paintZuma(md.zuma,&md.mi);
	FlushBatchDraw();
	EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	

	//仅供测试时使用
	//if (md.ballList.firstBallPosition > 400)
	//	md.ballList.firstBallPosition = 0;


	return;
}

void initPainting() {
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	setfillstyle(BS_NULL);
	return;
}

void operateMouseEvents(MajorData& md) {
	MOUSEMSG mmsg;
	double deltaX, deltaY, tanOfAngle;
	while (MouseHit()) {
		mmsg = GetMouseMsg();
		deltaX = mmsg.x - md.zuma.position.x;
		deltaY = -(mmsg.y - md.zuma.position.y);
		if (deltaX == 0)
			deltaX = 0.0001;
		tanOfAngle = deltaY / deltaX;
		md.zuma.angle = atan(tanOfAngle);
		if (deltaX<0)
			md.zuma.angle += PI;
		if (mmsg.mkLButton && md.zuma.CDremain == 0) {
			launchFlyingBall(md.flyingBallArray, md.mi.colorCount);
			md.zuma.CDremain = md.mi.shootingCD;
		}
		//printf("mouse:x=%d,y=%d,deltaX=%.2lf,deltaY=%.2lf,tanOfAngle=%.4lf,angle=%.4lf\n", mmsg.x, mmsg.y,deltaX,deltaY,tanOfAngle,zuma.angle);
	}
	return;
}
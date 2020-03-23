#pragma once
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include <winbase.h>
#include "cJSON/cJSON.h"

#define PI 3.14159269

#define WIDTH 800
#define HEIGHT 600

#define STORE_BY_POINTS 0
#define STORE_BY_FUNCTION 1
#define FILE_NOT_FOUND -1
#define IO_ERROR -2
#define UNKNOWN_STORING_METHOD -3

#define FLYING_BALL_ARRAY_SIZE 100

#define DEBUG_OUTPUT 2


typedef struct _Point {
	double x;
	double y;
}Point;



typedef struct _RouteFunctionArgs {
	double minP, maxP;
	double Rx, Nx, Ox, Px, Kx, Bx;
	double Ry, Ny, Oy, Py, Ky, By;
}RouteFunctionArgs;

typedef struct _Zuma {
	Point position;
	int thisBallColor, nextBallColor;
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

typedef struct _GameSettings {
	int shootingCD;
	double ballR, moveSpeed, flySpeed, beginningRushSpeed;
}GameSettings;

typedef struct _MapPositionInfo {
	int ballListCount;
	Point zumaPosition, deltaMouthPosition, deltaHolePosition;
}MapPositionInfo;

typedef struct _ResourceInfo {
	int colorCount, imgCount;
	IMAGE* background, *zuma, *zumaMask, * imgs, * ballImgs, * ballMaskImgs;
}ResourceInfo;

typedef struct _Route {
	int ballCount, pointCount, pointStep;
	double beginningRushRound;
	Point* pointArray;
}Route;

typedef struct _BallOnList {
	double position, force;
	int color;
	struct _BallOnList* prev;
	struct _BallOnList* next;
}BallOnList;


typedef struct _BallList {
	BallOnList* tail;
	Route* pr;
	int beginningRushRoundRemain;
}BallList;

typedef struct _MapInfo {
	GameSettings gs;
	MapPositionInfo mpi;
	ResourceInfo ri;
	Route* pr;
}MapInfo;

typedef struct _MajorData {
	bool gameEnd;
	char* mapDir;
	MapInfo mi;
	BallList* pbl;
	Zuma zuma;
	FlyingBallArray flyingBallArray;
}MajorData;



void demo1();

void coreGaming(MajorData md);
void operatingInput(MajorData* pmd);
void computingFlyingBalls(FlyingBallArray& fba, Zuma zuma, BallList& bl, MapInfo* pmi);
bool computingBallList(BallList& bl, MapInfo* pmi);
void paintImage(MajorData* pmd);
void initBallList(BallList* pbl, Route* pr, MapInfo* pmi, unsigned int seed);
void viewBallList(BallList* pbl);
void initZuma(MajorData* pmd);
void paintZuma(Zuma zuma, MapInfo* pmi);
void operateMouseEvents(MajorData* pmd);
void initFlyingBallArray(FlyingBallArray& fba, MapInfo* pmi);
void generateFlyingBall(FlyingBallArray& fba, int colorCount);
void rotateAndPaint(IMAGE* img, IMAGE* imgMask, double angle, Point position, bool highQuality);
bool isOutOfScreen(Point p);
void justPaint(IMAGE* img, IMAGE* imgMask, Point position);
inline bool testPointDistance(Point p1, Point p2, double minD);
void insertBallList(BallList& bl, BallOnList* pbol_prev, BallOnList* pbol_next, FlyingBallArray& fba, int index, MapInfo* pmi);
void removeFlyingBall(FlyingBallArray& fba, int index);
inline bool compareDistance(Point p, Point pTrue, Point pFalse);
void initPainting();
Point speed(MapInfo* pmi, double position);
void initStoredRoute(MapInfo* pmi);
double getSpeedValue(MapInfo* pmi, double position);
Point makePoint(double x, double y);
void paintBallList(BallList& bl, MapInfo* pmi);
void paintFlyingBall(FlyingBallArray& fba, Zuma zuma, MapInfo* pmi);
Point route(MapInfo* pmi, double position);
void loadMap(MapInfo* pmi, char* folder, char* mapName);
void parseJsonInt(const cJSON* pjson, char* name, int* pInt);
void parseJsonDouble(const cJSON* pjson, char* name, double* pDouble);
void parseGameSettingsJson(MapInfo* pmi, const cJSON* json);
void parseMapPositionInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName);
void parseBallListJson(Route* pr, const cJSON* json, char* folder, char* mapName);
void parseResourceInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName);
void parseJsonString(const cJSON* json, char* name, char* str);
void parseJsonPoint(const cJSON* json, char* name, Point* pPoint);
void loadRouteFile(Route* pr, char* dir);
void viewRoute(Route* pr);
void operateException();
void initAllBallList(BallList** ppbl, MapInfo* pmi);
void computeBallList(BallList* pbl, MapInfo* pmi);
void computeBeginningRush(BallList* pbl, MapInfo* pmi);
void computeNormalPush(BallList* pbl, MapInfo* pmi);
void applyForceToPosition(BallList* pbl, MapInfo* pmi);
void computeAllBallList(BallList* pbl, MapInfo* pmi);
Point route(Route* pr, int index);
void paintViewBallList(BallList* pbl, MapInfo* pmi);
void paintViewAllBallList(BallList* pbl, MapInfo* pmi);
void computeZuma(Zuma* pzuma);
void launchFlyingBall(FlyingBallArray& fba, int colorCount);


void test(MapInfo* pmi);




extern jmp_buf env;



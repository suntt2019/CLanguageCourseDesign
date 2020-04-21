#pragma once
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include <winbase.h>
//#include <dirent.h> 
#include "cJSON/cJSON.h"

#define PI 3.14159269
#define SQRT_3 1.7320508

#define WIDTH 1280
#define HEIGHT 720

#define STORE_BY_POINTS 0
#define STORE_BY_FUNCTION 1
#define FILE_NOT_FOUND -1
#define IO_ERROR -2
#define UNKNOWN_STORING_METHOD -3

#define FLYING_BALL_ARRAY_SIZE 100

#define JSON_MAX 10000
#define JSON_LINE_MAX 1000
#define STRING_BUFFER_SIZE 500



#define NULL_COLOR -1

#define DEBUG_OUTPUT 1
#define TORLANCE 0.01

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
	int shootingCD,swappingCD;
}Zuma;

typedef struct _FlyingBall {
	Point position;
	int color;
	double angle;
}FlyingBall;

typedef struct _FlyingBallArray {
	int size;
	FlyingBall* pfb;
}FlyingBallArray;

typedef struct _GameSettings {
	int shootingCD, swappingCD;
	double ballR, moveSpeed, flySpeed, beginningRushSpeed,attractionPullForce,insertingSpeed;
}GameSettings;

typedef struct _MapPositionInfo {
	int ballListCount;
	Point zumaPosition, deltaMouthPosition, deltaHolePosition;
}MapPositionInfo;

typedef struct _ResourceInfo {
	bool* availableColorBook;
	int colorCount, imgCount;
	IMAGE* background, *zuma, *zumaMask, * imgs, * ballImgs, * ballMaskImgs;
}ResourceInfo;

typedef struct _Route {
	int ballCount, pointCount, pointStep;
	double beginningRushRound;
	Point* pointArray;
	char generatingBallMethod[20];
	int probability;
	int* ballListOrderArray;
}Route;

typedef struct _BallOnList {
	double position, force;
	int color, attractLevel;
	bool isInserting;
	double routeBias;
	Point point;
	struct _BallOnList* prev;
	struct _BallOnList* next;
}BallOnList;


typedef struct _BallList {
	BallOnList* tail;
	Route* pr;
	int beginningRushRoundRemain;
	bool isEmpty;
	int score, latestRemovedBallPosition;
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
void paintImage(MajorData* pmd);
void initBallList(BallList* pbl, Route* pr, MapInfo* pmi, unsigned int seed);
void viewBallList(BallList* pbl);
void initZuma(MajorData* pmd);
void paintZuma(Zuma zuma, MapInfo* pmi);
void operateMouseEvents(MajorData* pmd);
void initFlyingBallArray(FlyingBallArray& fba, MapInfo* pmi);
void generateFlyingBall(FlyingBallArray& fba, ResourceInfo* pri);
void rotateAndPaint(IMAGE* img, IMAGE* imgMask, double angle, Point position, bool highQuality);
bool isOutOfScreen(Point p);
void justPaint(IMAGE* img, IMAGE* imgMask, Point position);
bool testPointDistance(Point p1, Point p2, double minD);
void insertBallList(BallList* pbl, BallOnList* pbol_prev, BallOnList* pbol_next, FlyingBallArray& fba, int index, MapInfo* pmi, bool crashPrev);
void removeFlyingBall(FlyingBallArray& fba, int index);
bool compareDistance(Point p, Point pTrue, Point pFalse);
void initPainting();
Point makePoint(double x, double y);
void paintBallList(BallList* pbl, MapInfo* pmi);
void paintFlyingBall(FlyingBallArray& fba, Zuma zuma, MapInfo* pmi);
void loadMap(MapInfo* pmi, char* folder, char* mapName);
void parseJsonInt(const cJSON* pjson, char* name, int* pInt);
void parseJsonDouble(const cJSON* pjson, char* name, double* pDouble);
void parseGameSettingsJson(MapInfo* pmi, const cJSON* json);
void parseBallListJson(Route* pr, const cJSON* json, char* folder, char* mapName);
void parseJsonString(const cJSON* json, char* name, char* str);
void parseJsonPoint(const cJSON* json, char* name, Point* pPoint);
void loadRouteFile(Route* pr, char* dir);
void viewRoute(Route* pr);
int handleException(int errorCode);
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
void launchFlyingBall(FlyingBallArray& fba, ResourceInfo* pri);
double getAngle(Point p);
double getAngle(double x, double y);
double routeArgle(Route* pr, int position);
Point minus(Point a, Point b);
void paintAllBallList(BallList* pbl, MapInfo* pmi);
bool isZero(Point p);
void computeBallListPoint(BallList* pbl, MapInfo* pmi);
void testCrash(BallList* pbl, FlyingBallArray& fba, int index, MapInfo* pmi);
void testCrashAll(BallList* pbl, FlyingBallArray& fba, int index, MapInfo* pmi);
double getGapBetweenBOL(BallList* pbl, GameSettings* pgs, BallOnList* p1, BallOnList* p2);
void parseMapPositionInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName);
void parseResourceInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName);
int gameMain();
bool testAchievingScore(BallList* pbl, MapInfo* pmi, BallOnList* pbol_new, int attractionLevelBase);
void computeAttractionPull(BallList* pbl, MapInfo* pmi);
void viewFlyingBallArray(FlyingBallArray* pfba);
void swapFlyingBall(FlyingBallArray* pfba);
void removeBallOnList(BallList* pbl, BallOnList* p);
int generateRandomColor(ResourceInfo* pri);
bool findColorInGame(MajorData* pmd, int color);
void upadateColorInfo(MajorData* pmd);
void viewColorInfo(ResourceInfo* pri);
void gameover(bool isVectory, BallList* pbl, MapInfo* pmi);
bool checkIfGameover(MapInfo* pmi, BallList* pbl);
bool checkIfBallListEmpty(BallList* pbl);
void computeAllBallListPoint(BallList* pbl, MapInfo* pmi);
bool checkIfBallListOverFlow(BallList* pbl);
int getBallColor(ResourceInfo* pri, Route* pr, int index);
void parseGeneratingBallMethod(Route* pr, const cJSON* json, char* folder, char* mapName);
void paintViewBallList(BallList* pbl, MapInfo* pmi, int index);
void settleScore(bool isVectory, BallList* pbl, MapInfo* pmi);
void outtextxy(Point p, LPCTSTR str);
void demo2();





void test();






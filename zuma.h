#pragma once
#include <graphics.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <math.h>
#include <time.h>
#include <winbase.h>
#include <io.h>
#include <windows.h> 
#include <conio.h>
#include <mmsystem.h>
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


#define BUTTON_DEFAULT_GAP 20
#define BUTTON_TEXT_MAX 100
#define MOUSE_BTN_FOCUS_DEGREE_INCREASE 0.06
#define BUTTON_EDGE_DIFFUSION 64
#define BUTTON_GROWTH 0.1
#define BGFL_LEVEL 48
#define BGFL_STEP 3
#define BGFL_BIAS 2
#define BGFL_THICKNESS 3


#define MAIN_MENU_BTN_WIDTH 300
#define MAIN_MENU_BTN_HEIGHT 64
#define MAIN_MENU_BTN_GAP 20

#define CHOOSING_MAP_PANEL_BTN_WIDTH 200
#define CHOOSING_MAP_PANEL_BTN_HEIGHT 360
#define CHOOSING_MAP_PANEL_BTN_GAP 60
#define CHOOSING_MAP_RETURN_BTN_WIDTH 300
#define CHOOSING_MAP_RETURN_BTN_HEIGHT 64
#define CHOOSING_MAP_RETURN_BTN_GAP 40
#define CHOOSING_MAP_PAGE_BTN_WIDTH 64
#define CHOOSING_MAP_PAGE_BTN_HEIGHT 180
#define CHOOSING_MAP_PAGE_BTN_GAP 40

#define ENDING_PANEL_BTN_WIDTH 300
#define ENDING_PANEL_BTN_HEIGHT 64
#define ENDING_PANEL_BTN_GAP 50

#define NULL_COLOR -1

#define DEBUG_OUTPUT 0
#define TORLANCE 0.01

#pragma comment(lib,"Winmm.lib")

typedef struct _Point {
	double x;
	double y;
}Point;

typedef struct _Score {
	int finalScore;
	int greatestCrash;
	int longestCombo;
}Score;

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
	int scoreInt, latestRemovedBallPosition;
	Score score;
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

typedef struct _Button {
	char str[BUTTON_TEXT_MAX];
	int strLen;
	double strRelativeSize;
	double focusDegree, linear_focusDegree;
	int or_left, or_right, or_top, or_buttom;
	int or_width, or_height, left, right, top, buttom;
	int strBiasX, strBiasY;
	bool strFollowY, strLeftJustify;
	bool isFocusDegreeIncreasing;
	IMAGE* texture;
}Button;

typedef struct _Panel {
	Button* buttonArray;
	int btnCnt;
	IMAGE* flbgArray;
	IMAGE* background, * backgroundDark;
	char* title;
}Panel;

typedef struct _MajorPanels {
	Panel mainMenu, mapChoosingPanel,optionPanel, pausePanel, developerPanel,endingPanel;
	IMAGE* background, * backgroundDark, * backgroundFlbgArray;
	IMAGE* backgroundWithLogo, * backgroundWithLogoDark, * backgroundWithLogoFlbgArray;
	double musicVolume, foleyVolume;
}MajorPanels;

typedef struct _MapPreview {
	int ballCnt;
	char dir[200],name[200];
	IMAGE* zuma, zumaMask, ball, ballMask;
}MapPreview;

typedef struct _MapPreviewArray {
	MapPreview* mpa;
	int cnt;
}MapPreviewArray;




Score coreGaming(MajorData md, MajorPanels* pmp);
void operatingInput(MajorData* pmd);
void computeFlyingBalls(FlyingBallArray& fba, Zuma zuma, BallList& bl, MapInfo* pmi);
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
bool loadMap(MapInfo* pmi, char* folder, char* mapName);
bool parseJsonInt(const cJSON* pjson, char* name, int* pInt);
bool parseJsonDouble(const cJSON* pjson, char* name, double* pDouble);
bool parseGameSettingsJson(MapInfo* pmi, const cJSON* json);
bool parseBallListJson(Route* pr, const cJSON* json, char* folder, char* mapName);
bool parseJsonString(const cJSON* json, char* name, char* str);
bool parseJsonPoint(const cJSON* json, char* name, Point* pPoint);
bool loadRouteFile(Route* pr, char* dir);
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
bool parseMapPositionInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName);
bool parseResourceInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName);
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
bool parseGeneratingBallMethod(Route* pr, const cJSON* json, char* folder, char* mapName);
void paintViewBallList(BallList* pbl, MapInfo* pmi, int index);
void settleScore(bool isVectory, BallList* pbl, MapInfo* pmi);
void outtextxy(Point p, LPCTSTR str);
void initLineMiddleButton(Button* pb, int y, int width, int height, char* str, double strRelativeSize, IMAGE* texture);
void paintPanel(Panel* ppanel);
void paintButton(Button* pb);
bool testBtnFocus(Button* pb, Point p);
void updateBtnFocus(Panel* ppanel, Point mousePoint);
void paintButtonFocusLine(Button* pb, IMAGE* flbgArray);
void initFLBG(IMAGE* pbg, char* name);
void loadFLBG(IMAGE** pflbgArray, char* name);
void updateBtn(Button* pb);
void initPanels(MajorPanels* pmp);
void initMainMenu(Panel* pmm, const MajorPanels* pmp);
void mainMenu(MajorPanels* pmp);
int testBtnClick(Panel* ppanel, Point mouseClickedPoint);
int operatePanelsMouseEvents(Panel* pp);
void initMapChoosingPanel(Panel* pmcp, const MajorPanels* pmp);
void mapChoosingPanel(MajorPanels* pmp);
void initColumnMiddleButton(Button* pb, int x, int btnWidth, int btnHeight, char* str, double strRelativeSize, IMAGE* texture);
void initButton(Button* pb, int left, int right, int top, int buttom, char* str, double strRelativeSize, IMAGE* texture);
void previewMaps(MapPreviewArray** ppmpa);
bool previewSingleMap(char* name, MapPreview* pmp);
Score startCoreGaming(char* dir, MajorPanels* pmp);
void initEndingPanel(Panel* pep, const MajorPanels* pmp);
void endingPanel(MajorPanels* pmp, Score score);
void initPausePanel(Panel* ppp, const MajorPanels* pmp);
bool pausePanel(MajorPanels* pmp);
void clearBtnFocusDegree(Button* pbtn);
int operatePanelsMouseEvents(Panel* pp, Point* pmouseClickedPoint);
void optionPanel(MajorPanels* pmp);
void initOptionPanel(Panel* pop, const MajorPanels* pmp);
void initDeveloperPanel(Panel* pdp, const MajorPanels* pmp);
void developerPanel(MajorPanels* pmp);
void loadAudio();
void playAudio(char* name, int volume);
double musicVolume(double v);
double foleyVolume(double v);
void startBGM();
void paintText(char* str, int index, int startingX, int startingY, int fontSize, int gap);
Score addScore(Score sc1, Score sc2);
Score initScore();







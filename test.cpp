#include "zuma.h"


#define BUTTON_DEFAULT_GAP 20
#define BUTTON_TEXT_MAX 100
#define MOUSE_BTN_FOCUS_DEGREE_INCREASE 0.05
#define BUTTON_EDGE_DIFFUSION 64
#define BUTTON_GROWTH 0.1
#define BGFL_LEVEL 48
#define BGFL_STEP 3
#define BGFL_BIAS 2
#define BGFL_THICKNESS 3


#define MAIN_MENU_BTN_WIDTH 300
#define MAIN_MENU_BTN_WEIGHT 64
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


typedef struct _Button {
	char str[BUTTON_TEXT_MAX];
	int strLen;
	double strRelativeSize;
	double focusDegree, linear_focusDegree;
	int or_left, or_right, or_top, or_buttom;
	int or_width,or_height,left, right, top, buttom;
	int strBiasX,strBiasY;
	bool strFollowY, strLeftJustify;
	IMAGE* texture;
}Button;

typedef struct _Panel {
	Button* buttonArray;
	int btnCnt;
	IMAGE* flbgArray;
	IMAGE* background,*backgroundDark;
	char* title;
}Panel;

typedef struct _MajorPanels {
	Panel mainMenu,mapChoosingPanel,OptionPanel,pausePanel,developerPanel;
	IMAGE *background, *backgroundDark, *backgroundFlbgArray;
	IMAGE *backgroundWithLogo, *backgroundWithLogoDark, *backgroundWithLogoFlbgArray;
}MajorPanels;

void initLineMiddleButton(Button* pb, int y, int width, int height, char* str, double strRelativeSize, IMAGE* texture);
void paintPanel(Panel* ppanel);
void paintButton(Button* pb);
bool testBtnFocus(Button* pb, Point p);
void updateBtnFocus(Panel* ppanel, Point mousePoint);
void paintButtonFocusLine(Button* pb, IMAGE* flbgArray);
void initFLBG(IMAGE* pbg,char* name);
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


void test() {
	MajorPanels mp;
	initPanels(&mp);
	mainMenu(&mp);
	return;
}

void initPanels(MajorPanels* pmp) {
	
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	setbkmode(TRANSPARENT);

	//initFLBG(pmp->background, "mainBackGround");

	pmp->backgroundWithLogo = new IMAGE();
	loadimage(pmp->backgroundWithLogo, "image\\main_background_with_logo.jpg", WIDTH, HEIGHT, true);
	pmp->backgroundWithLogoDark = new IMAGE();
	loadimage(pmp->backgroundWithLogoDark, "image\\main_background_with_logo_dark.jpg", WIDTH, HEIGHT, true);
	loadFLBG(&pmp->backgroundWithLogoFlbgArray, "mainBackGroundWithLogo");
	pmp->background = new IMAGE();
	loadimage(pmp->background, "image\\main_background.jpg", WIDTH, HEIGHT, true);
	pmp->backgroundDark = new IMAGE();
	loadimage(pmp->backgroundDark, "image\\main_background_dark.jpg", WIDTH, HEIGHT, true);
	loadFLBG(&pmp->backgroundFlbgArray, "mainBackGround");

	initMainMenu(&pmp->mainMenu,pmp);
	initMapChoosingPanel(&pmp->mapChoosingPanel,pmp);
	return;
}

void initMainMenu(Panel* pmm,const MajorPanels* pmp) {
	pmm->title = "";
	pmm->background = pmp->backgroundWithLogo;
	pmm->backgroundDark = pmp->backgroundWithLogoDark;
	pmm->flbgArray = pmp->backgroundWithLogoFlbgArray;
	pmm->btnCnt = 3;
	Button* pba = pmm->buttonArray = (Button*)malloc(sizeof(Button) * pmm->btnCnt);
	int startingY = HEIGHT / 2 - (MAIN_MENU_BTN_WEIGHT + MAIN_MENU_BTN_GAP) * pmm->btnCnt / 2 + 120;
	char btnStrs[][20] = { "开","选项","出出出出出" };
	for (int i = 0; i < pmm->btnCnt; i++) {
		initLineMiddleButton(pba + i, startingY + (MAIN_MENU_BTN_WEIGHT + MAIN_MENU_BTN_GAP) * i, MAIN_MENU_BTN_WIDTH, MAIN_MENU_BTN_WEIGHT, btnStrs[i],1, pmm->backgroundDark);
	}
}


void initMapChoosingPanel(Panel* pmcp, const MajorPanels* pmp) {
	pmcp->title = "选择地图";
	pmcp->background = pmp->background;
	pmcp->backgroundDark = pmp->backgroundDark;
	pmcp->flbgArray = pmp->backgroundFlbgArray;
	pmcp->btnCnt = 7;
	Button* pba = pmcp->buttonArray = (Button*)malloc(sizeof(Button) * pmcp->btnCnt);
	int startingX = WIDTH / 2 - (CHOOSING_MAP_PANEL_BTN_WIDTH + CHOOSING_MAP_PANEL_BTN_GAP) * 4 / 2 + CHOOSING_MAP_PANEL_BTN_GAP/2;
	char btnStrs[][20] = { "测","测测测测","maximumm","MAXIMUMM","返回" };
	for (int i = 0; i < 4; i++) {
		initColumnMiddleButton(pba + i, startingX + (CHOOSING_MAP_PANEL_BTN_WIDTH + CHOOSING_MAP_PANEL_BTN_GAP) * i, CHOOSING_MAP_PANEL_BTN_WIDTH, CHOOSING_MAP_PANEL_BTN_HEIGHT, btnStrs[i], 0.16, pmcp->backgroundDark);
		(pba + i)->strLeftJustify = true;
		(pba + i)->strFollowY = true;
	}
	initButton(pba + 4, WIDTH / 2 - CHOOSING_MAP_RETURN_BTN_WIDTH / 2, WIDTH / 2 + CHOOSING_MAP_RETURN_BTN_WIDTH / 2,
		HEIGHT - CHOOSING_MAP_RETURN_BTN_GAP - CHOOSING_MAP_RETURN_BTN_HEIGHT, HEIGHT - CHOOSING_MAP_RETURN_BTN_GAP,
		btnStrs[4],1, pmcp->backgroundDark);
	initColumnMiddleButton(pba + 5, CHOOSING_MAP_PAGE_BTN_GAP, CHOOSING_MAP_PAGE_BTN_WIDTH,
		CHOOSING_MAP_PAGE_BTN_HEIGHT, "<",0.5, pmcp->backgroundDark);
	(pba + 5)->strBiasX = 0;
	(pba + 5)->strBiasY = 40;
	initColumnMiddleButton(pba + 6, WIDTH - CHOOSING_MAP_PAGE_BTN_GAP - CHOOSING_MAP_PAGE_BTN_WIDTH,
		CHOOSING_MAP_PAGE_BTN_WIDTH, CHOOSING_MAP_PAGE_BTN_HEIGHT, ">",0.5, pmcp->backgroundDark);
	(pba + 6)->strBiasX = -10;
	(pba + 6)->strBiasY = 40;

}

void mainMenu(MajorPanels* pmp) {

	while (true) {
		switch (operatePanelsMouseEvents(&pmp->mainMenu)) {
		case 0:
			outtextxy(100, 100, "mapChoosingPanel");
			mapChoosingPanel(pmp);
			pmp->mainMenu.buttonArray[0].focusDegree = 0;
			pmp->mainMenu.buttonArray[0].linear_focusDegree = 0;
			break;
		case 1:
			outtextxy(100, 100, "optionPanel");
			while (1);
			break;
		case 2:
			return;
		default:
			break;
		}
		BeginBatchDraw();
		paintPanel(&pmp->mainMenu);
		FlushBatchDraw();
		EndBatchDraw();
	}
}


void mapChoosingPanel(MajorPanels* pmp) {
	while (true) {
		switch (operatePanelsMouseEvents(&pmp->mapChoosingPanel)) {
		case 4:
			return;
		default:
			break;
		}
		BeginBatchDraw();
		paintPanel(&pmp->mapChoosingPanel);
		FlushBatchDraw();
		EndBatchDraw();
	}
}




















int operatePanelsMouseEvents(Panel* pp) {
	static Point mousePoint = makePoint(0, 0);
	Point mouseClickedPoint = makePoint(0, 0);
	int ret = -1,thisValue;
	MOUSEMSG mmsg;
	while (MouseHit()) {
		mmsg = GetMouseMsg();
		mousePoint = makePoint(mmsg.x, mmsg.y);
		if (mmsg.mkLButton) {
			mouseClickedPoint = mousePoint;
			thisValue = testBtnClick(pp,mouseClickedPoint);
			if (thisValue >= 0)
				ret = thisValue;
		}
	}
	updateBtnFocus(pp, mousePoint);
	return ret;
}



void initFLBG(IMAGE* pbg, char* name) {
	char strBuf[STRING_BUFFER_SIZE];
	DWORD* pMem = GetImageBuffer(pbg);
	for (int i = 0; i <= BGFL_LEVEL; i++) {
		for (int j = 0; j < pbg->getheight() * pbg->getwidth(); j++) {
			pMem[j] = RGB(
				GetRValue(pMem[j]) <= 255 - BGFL_STEP ? GetRValue(pMem[j]) + BGFL_STEP : 255,
				GetGValue(pMem[j]) <= 255 - BGFL_STEP ? GetGValue(pMem[j]) + BGFL_STEP : 255,
				GetBValue(pMem[j]) <= 255 - BGFL_STEP ? GetBValue(pMem[j]) + BGFL_STEP : 255);
		}
		putimage(0,0,pbg);
		sprintf(strBuf, "image\\FLBG_%s_%d.jpg",name,i);
		saveimage(strBuf, pbg);
		//Sleep(50);
	}
}

void loadFLBG(IMAGE** pflbgArray,char* name) {
	char strBuf[STRING_BUFFER_SIZE];
	*pflbgArray = new IMAGE[BGFL_LEVEL+1];
	for (int i = 0; i < BGFL_LEVEL; i++) {
		sprintf(strBuf, "image\\FLBG_%s_%d.jpg",name, i);
		loadimage(*pflbgArray+i, strBuf);
		//putimage(0, 0, *pflbgArray + i);
		//outtextxy(100, 100, strBuf);
		//Sleep(200);
	}
	sprintf(strBuf, "image\\FLBG_%d.jpg", BGFL_LEVEL);
	loadimage(*pflbgArray + BGFL_LEVEL, strBuf);
	return;
}

void initLineMiddleButton(Button* pb,int y,int btnWidth,int btnHeight,char* str,double strRelativeSize,IMAGE* texture) {
	initButton(pb, WIDTH / 2 - btnWidth / 2, WIDTH / 2 + btnWidth / 2, y, y + btnHeight, str, strRelativeSize, texture);
}

void initColumnMiddleButton(Button* pb, int x,int btnWidth,int btnHeight, char* str, double strRelativeSize, IMAGE* texture) {
	initButton(pb, x, x + btnWidth, HEIGHT / 2 - btnHeight / 2, HEIGHT / 2 + btnHeight / 2, str, strRelativeSize, texture);
}

void initButton(Button* pb, int left, int right, int top,int buttom, char* str, double strRelativeSize, IMAGE* texture) {
	pb->or_top = top;
	pb->or_buttom = buttom;
	pb->or_left = left;
	pb->or_right = right;
	pb->or_width = pb->or_right - pb->or_left;
	pb->or_height = pb->or_buttom - pb->or_top;
	pb->linear_focusDegree = 0;
	pb->texture = texture;
	pb->strLen = strlen(str);
	pb->strRelativeSize = strRelativeSize;
	pb->strBiasX = 0;
	pb->strBiasY = 0;
	pb->strFollowY = false;
	pb->strLeftJustify = false;
	strncpy(pb->str, str, BUTTON_TEXT_MAX);
	return;
}

void paintPanel(Panel* ppanel) {
	putimage(0, 0, ppanel->background);
	settextstyle(60,0,_T("微软雅黑 Light"), 0, 0,1000,false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
	outtextxy(WIDTH/2-strlen(ppanel->title)*12, 60, ppanel->title);
	for (int i = 0; i < ppanel->btnCnt; i++) {
		updateBtn(ppanel->buttonArray + i);
		paintButtonFocusLine(ppanel->buttonArray + i,ppanel->flbgArray);
		if(i>0&&ppanel->buttonArray[i].focusDegree< ppanel->buttonArray[i-1].focusDegree)
			paintButtonFocusLine(ppanel->buttonArray + i-1, ppanel->flbgArray);//避免重复渲染出现断线情况
			//这不是很高效/明智的方法，但是很省事，效果也不错，毕竟也性能影响不大
	}
	for (int i = 0; i < ppanel->btnCnt; i++) {
		paintButton(ppanel->buttonArray + i);
	}

}

void paintButtonFocusLine(Button* pb,IMAGE* flbgArray) {
	//rectangle(pb->left - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION, pb->top - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION,
	//	pb->right + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION, pb->buttom + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION);
	if (!pb->focusDegree)
		return;
	int thickness = BGFL_THICKNESS * pb->focusDegree;
	int left = pb->left - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION - BGFL_BIAS - thickness;
	int top = pb->top - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION - BGFL_BIAS - thickness;
	int right = pb->right + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION + BGFL_BIAS;
	int buttom = pb->buttom + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION + BGFL_BIAS;
	int width = pb->right - pb->left + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION * 2  + BGFL_BIAS*2 + thickness *2;
	int height = pb->buttom - pb->top + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION * 2  + BGFL_BIAS * 2 + thickness *2;
	
	IMAGE* flbg = flbgArray + (int)(pb->focusDegree * (BGFL_LEVEL-1));
	putimage(left, top , thickness, height, flbg, left, top );
	putimage(left, top , width, thickness, flbg, left, top);
	putimage(right, top, thickness, height, flbg, right, top);
	putimage(left , buttom, width, thickness, flbg, left , buttom);
}

double nonlinear(double x) {
	return pow(x,0.2);
}

void updateBtn(Button* pb) {
	pb->left = pb->or_left - pb->or_width * pb->focusDegree * BUTTON_GROWTH;
	pb->right = pb->or_right + pb->or_width * pb->focusDegree * BUTTON_GROWTH;
	pb->top = pb->or_top - pb->or_height * pb->focusDegree * BUTTON_GROWTH;
	pb->buttom = pb->or_buttom + pb->or_height * pb->focusDegree * BUTTON_GROWTH;
	pb->focusDegree = nonlinear(pb->linear_focusDegree);
}

void paintButton(Button* pb) {
	rectangle(pb->left, pb->top, pb->right, pb->buttom);
	putimage(pb->left + 1, pb->top + 1, pb->right - pb->left - 1, pb->buttom - pb->top - 1, pb->texture, pb->left + 1, pb->top + 1);
	int strHeight = pb->or_height * pb->strRelativeSize * (1 + pb->focusDegree * BUTTON_GROWTH) / 3 * 2;
	settextstyle(
		strHeight,
		0,
		_T("微软雅黑 Light"), 0, 0,
		pb->focusDegree*500,
		false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);

	/*
	outtextxy(
		pb->or_left + (pb->focusDegree + 1) * (pb->or_width - 24 * pb->strLen) * (1+BUTTON_GROWTH)/2+4,
		pb->strBiasY + pb->or_top + pb->or_height * pb->strRelativeSize /6 - pb->or_height * pb->strRelativeSize * pb->focusDegree * BUTTON_GROWTH / 4,
		_T(pb->str)
	);
	*/
	outtextxy(
		pb->strBiasX + (pb->strLeftJustify ? pb->left+20 : pb->or_left + ((pb->or_width - strHeight*0.38 * pb->strLen)/2)),
		pb->strBiasY + (pb->strFollowY ? pb->top : pb->or_top) + pb->or_height * pb->strRelativeSize / 6 - pb->or_height * pb->strRelativeSize * pb->focusDegree * BUTTON_GROWTH / 4,
		_T(pb->str)
	);
}

bool testBtnFocus(Button* pb,Point p) {
	return pb->or_left <= p.x && p.x <= pb->or_right && pb->or_top <= p.y && p.y <= pb->or_buttom;
}

void updateBtnFocus(Panel* ppanel,Point mousePoint) {
	for (int i = 0; i < ppanel->btnCnt; i++) {
		if (testBtnFocus(ppanel->buttonArray + i, mousePoint)) {
			if(ppanel->buttonArray[i].linear_focusDegree < 1)
				ppanel->buttonArray[i].linear_focusDegree += MOUSE_BTN_FOCUS_DEGREE_INCREASE;
			if (DEBUG_OUTPUT>1)
				printf("button[%d].focusDegree = %.2lf\n", i, ppanel->buttonArray[i].linear_focusDegree);
		}else if(ppanel->buttonArray[i].linear_focusDegree > 0){
			ppanel->buttonArray[i].linear_focusDegree -= MOUSE_BTN_FOCUS_DEGREE_INCREASE;
		}
		if (ppanel->buttonArray[i].linear_focusDegree > 1)
			ppanel->buttonArray[i].linear_focusDegree = 1;
		if (ppanel->buttonArray[i].linear_focusDegree < 0)
			ppanel->buttonArray[i].linear_focusDegree = 0;
	}
	return;
}

int testBtnClick(Panel* ppanel, Point mouseClickedPoint) {
	for (int i = 0; i < ppanel->btnCnt; i++) {
		if (testBtnFocus(ppanel->buttonArray + i, mouseClickedPoint))
			return i;
	}
	return -1;
}
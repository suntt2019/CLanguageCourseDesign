#include "zuma.h"

#define BUTTON_DEFAULT_WIDTH 300
#define BUTTON_DEFAULT_HEIGHT 64
#define BUTTON_DEFAULT_GAP 20
#define BUTTON_TEXT_MAX 100
#define MOUSE_BTN_FOCUS_DEGREE_INCREASE 0.035
#define BUTTON_EDGE_DIFFUSION 64
#define BUTTON_GROWTH 0.1
#define BGFL_LEVEL 48
#define BGFL_STEP 3
#define BGFL_BIAS 2
#define BGFL_THICKNESS 3


typedef struct _Button {
	char str[BUTTON_TEXT_MAX];
	int strLen;
	double focusDegree, linear_focusDegree;
	int or_left, or_right, or_top, or_buttom;
	int or_width,or_height,left, right, top, buttom;
	IMAGE* texture;
}Button;

typedef struct _Panel {
	Button* buttonArray;
	int btnCnt;
	IMAGE* flbgArray;
}Panel;

void initMiddleButton(Button* pb, int y, char* str, IMAGE* texture);
void paintPanel(Panel* ppanel);
void paintButton(Button* pb);
bool testBtnFocus(Button* pb, Point p);
void updateBtnFocus(Panel* ppanel, Point mousePoint);
void paintButtonFocusLine(Button* pb, IMAGE* flbgArray);
void initFLBG(IMAGE* pbg);
void loadFLBG(IMAGE** pflbgArray);
void updateBtn(Button* pb);



void test() {
	//界面测试
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	//setbkmode(TRANSPARENT);
	//setrop2(R2_MERGEPEN);
	setfillcolor(LIGHTGRAY);
	/*
	float sx, sy;
	getaspectratio(&sx,&sy);
	printf("(%.2f,%.2f)\n",sx,sy);
	for (int i = 0; i <= 1000; i += 100) {
		settextstyle(32, 0, _T("微软雅黑"), 0, 0, i, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
		outtextxy(0, i / 3, _T("测试"));
		Sleep(100);
	}
	*/
	
	IMAGE mainBackground, mainBackgroundDark,logo,logoMask;
	loadimage(&mainBackground, "image\\main_background_with_logo.jpg",WIDTH,HEIGHT,true);
	loadimage(&mainBackgroundDark, "image\\main_background3_dark.jpg",WIDTH, HEIGHT, true);
	

	initFLBG(&mainBackground);
	loadimage(&mainBackground, "image\\main_background_with_logo.jpg", WIDTH, HEIGHT, true);
	loadimage(&logo, "image\\logo.jpg", WIDTH/2, HEIGHT/2, true);
	loadimage(&logoMask, "image\\logo_mask.jpg", WIDTH / 2, HEIGHT / 2, true);
	setbkmode(TRANSPARENT);

	
	Panel mainPanel;
	
	loadFLBG(&mainPanel.flbgArray);




	mainPanel.btnCnt = 3;
	Button* pba = mainPanel.buttonArray = (Button*)malloc(sizeof(Button) * mainPanel.btnCnt);
	int startingY = HEIGHT / 2 - (BUTTON_DEFAULT_HEIGHT + BUTTON_DEFAULT_GAP) * mainPanel.btnCnt / 2 +120;
	char btnStrs[][20] = { "开始","选项","退出" };
	for (int i = 0; i < mainPanel.btnCnt; i++) {
		initMiddleButton(pba + i, startingY + (BUTTON_DEFAULT_HEIGHT + BUTTON_DEFAULT_GAP) * i, btnStrs[i], &mainBackgroundDark);
	}


	Point mousePoint = makePoint(0,0);
	MOUSEMSG mmsg;

	while (true) {
		while (MouseHit()) {
			mmsg = GetMouseMsg();
			mousePoint = makePoint(mmsg.x, mmsg.y);
		}
		updateBtnFocus(&mainPanel, mousePoint);
		BeginBatchDraw();//开始批量绘图
		putimage(0, 0, &mainBackground);
		//putimage(WIDTH/4, 0, &logoMask, SRCAND);
		//putimage(WIDTH / 4, 0, &logo, SRCINVERT);
		//putimage(0, 0, &mainBackgroundDark, SRCINVERT);
		//putimage(WIDTH/4, 0, &logo, SRCINVERT);
		paintPanel(&mainPanel);
		FlushBatchDraw();
		EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	
	}
	//这easyx不支持半透明颜色是真的难顶
	return;
}


void initFLBG(IMAGE* pbg) {
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
		sprintf(strBuf, "image\\FLBG_%d.jpg",i);
		saveimage(strBuf, pbg);
		//Sleep(50);
	}
}

void loadFLBG(IMAGE** pflbgArray) {
	char strBuf[STRING_BUFFER_SIZE];
	*pflbgArray = new IMAGE[BGFL_LEVEL+1];
	for (int i = 0; i < BGFL_LEVEL; i++) {
		sprintf(strBuf, "image\\FLBG_%d.jpg", i);
		loadimage(*pflbgArray+i, strBuf);
		putimage(0, 0, *pflbgArray + i);
		outtextxy(100, 100, strBuf);
		//Sleep(200);
	}
	sprintf(strBuf, "image\\FLBG_%d.jpg", BGFL_LEVEL);
	loadimage(*pflbgArray + BGFL_LEVEL, strBuf);
	return;
}

void initMiddleButton(Button* pb,int y,char* str,IMAGE* texture) {
	pb->or_top = y;
	pb->or_buttom = y + BUTTON_DEFAULT_HEIGHT;
	pb->or_left = WIDTH / 2 - BUTTON_DEFAULT_WIDTH / 2;
	pb->or_right = WIDTH / 2 + BUTTON_DEFAULT_WIDTH / 2;
	pb->or_width = pb->or_right - pb->or_left;
	pb->or_height = pb->or_buttom - pb->or_top;
	pb->linear_focusDegree = 0;
	pb->texture = texture;
	pb->strLen = strlen(str);
	strncpy(pb->str, str, BUTTON_TEXT_MAX);
	return;
}

void paintPanel(Panel* ppanel) {
	for (int i = 0; i < ppanel->btnCnt; i++) {
		updateBtn(ppanel->buttonArray + i);
		paintButtonFocusLine(ppanel->buttonArray + i,ppanel->flbgArray);
		if(i>0&&ppanel->buttonArray[i].focusDegree< ppanel->buttonArray[i-1].focusDegree)
			paintButtonFocusLine(ppanel->buttonArray + i-1, ppanel->flbgArray);//避免重复渲染出现断线情况
			//这不是很高效/明智的方法，但是很省事，毕竟也性能影响不大
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
	
	IMAGE* flbg = flbgArray + (int)(pb->focusDegree * BGFL_LEVEL);
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
	settextstyle(
		pb->or_height*(1+pb->focusDegree* BUTTON_GROWTH)/3*2,
		0,
		_T("微软雅黑 Light"), 0, 0,
		pb->focusDegree*500,
		false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);


	outtextxy(
		pb->or_left + pb->focusDegree+1 * (pb->or_width - 24 * pb->strLen) * (1+BUTTON_GROWTH)/2+4,
		pb->or_top + pb->or_height/6 - pb->or_height * pb->focusDegree * BUTTON_GROWTH / 4,
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
}
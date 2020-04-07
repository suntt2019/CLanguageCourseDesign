#include "zuma.h"

#define BUTTON_DEFAULT_WIDTH 300
#define BUTTON_DEFAULT_HEIGHT 64
#define BUTTON_DEFAULT_GAP 16
#define BUTTON_TEXT_MAX 100

typedef struct _Button {
	char str[BUTTON_TEXT_MAX];
	int focusDegree;
	int left, right, top, buttom;
}Button;

typedef struct _Panel {
	Button* buttonArray;
	int btnCnt;
}Panel;

void initMiddleButton(Button* pb, int y, char* str);
void paintPanel(Panel* ppanel);
void paintButton(Button* pb);




void test() {
	//ΩÁ√Ê≤‚ ‘
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	setbkmode(TRANSPARENT);
	setrop2(R2_MERGEPEN);
	setfillcolor(LIGHTGRAY);
	/*
	float sx, sy;
	getaspectratio(&sx,&sy);
	printf("(%.2f,%.2f)\n",sx,sy);
	for (int i = 0; i <= 1000; i += 100) {
		settextstyle(32, 0, _T("Œ¢»Ì—≈∫⁄"), 0, 0, i, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
		outtextxy(0, i / 3, _T("≤‚ ‘"));
		Sleep(100);
	}
	*/
	IMAGE mainBackground, mainBackgroundMask;
	loadimage(&mainBackground, "image\\main_background3.jpg",WIDTH,HEIGHT,true);
	loadimage(&mainBackgroundMask, "image\\level.jpg",WIDTH, HEIGHT, true);
	putimage(0, 0, &mainBackgroundMask, SRCAND);
	putimage(0, 0, &mainBackground, SRCINVERT);

	
	Panel mainPanel;
	mainPanel.btnCnt = 5;
	Button* pba = mainPanel.buttonArray = (Button*)malloc(sizeof(Button) * mainPanel.btnCnt);
	int startingY = HEIGHT / 2 - (BUTTON_DEFAULT_HEIGHT + BUTTON_DEFAULT_GAP) * mainPanel.btnCnt / 2;
	for (int i = 0; i < mainPanel.btnCnt; i++)
		initMiddleButton(pba + i, startingY+ (BUTTON_DEFAULT_HEIGHT + BUTTON_DEFAULT_GAP) * i, "yoo");
	paintPanel(&mainPanel);
	return;
}

void initMiddleButton(Button* pb,int y,char* str) {
	pb->top = y;
	pb->buttom = y + BUTTON_DEFAULT_HEIGHT;
	pb->left = WIDTH / 2 - BUTTON_DEFAULT_WIDTH / 2;
	pb->right = WIDTH / 2 + BUTTON_DEFAULT_WIDTH / 2;
	pb->focusDegree = 0;
	strncpy(pb->str, str, BUTTON_TEXT_MAX);
	return;
}

void paintPanel(Panel* ppanel) {
	for (int i = 0; i < ppanel->btnCnt; i++) {
		paintButton(ppanel->buttonArray + i);
	}

}

void paintButton(Button* pb) {
	fillrectangle(pb->left, pb->top, pb->right, pb->buttom);
}
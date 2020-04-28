#include "zuma.h"

int operatePanelsMouseEvents(Panel* pp, Point* pmouseClickedPoint) {
	static Point mousePoint = makePoint(0, 0);
	Point mouseClickedPoint = makePoint(0, 0);
	bool clicked = false;
	int ret = -1, thisValue;
	MOUSEMSG mmsg;
	while (MouseHit()) {
		mmsg = GetMouseMsg();
		mousePoint = makePoint(mmsg.x, mmsg.y);
		if (mmsg.mkLButton) {
			mouseClickedPoint = mousePoint;
			thisValue = testBtnClick(pp, mouseClickedPoint);
			clicked = true;
			if (thisValue >= 0)
				ret = thisValue;
		}
	}
	if(pmouseClickedPoint)
		*pmouseClickedPoint = mouseClickedPoint;
	if (clicked&& ret!=-1)
		playAudio("clicked", 100);
	updateBtnFocus(pp, mousePoint);
	return ret;
}

int operatePanelsMouseEvents(Panel* pp) {
	return operatePanelsMouseEvents(pp, NULL);
}


/*
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
		putimage(0, 0, pbg);
		sprintf(strBuf, "image\\FLBG_%s_%d.jpg", name, i);
		saveimage(strBuf, pbg);
		//Sleep(50);
	}
}
*/

void loadFLBG(IMAGE** pflbgArray, char* name) {
	char strBuf[STRING_BUFFER_SIZE];
	*pflbgArray = new IMAGE[BGFL_LEVEL + 1];
	for (int i = 0; i < BGFL_LEVEL; i++) {
		sprintf(strBuf, "image\\FLBG_%s_%d.jpg", name, i);
		loadimage(*pflbgArray + i, strBuf);
		//putimage(0, 0, *pflbgArray + i);
		//outtextxy(100, 100, strBuf);
		//Sleep(200);
	}
	sprintf(strBuf, "image\\FLBG_%d.jpg", BGFL_LEVEL);
	loadimage(*pflbgArray + BGFL_LEVEL, strBuf);
	return;
}

void initLineMiddleButton(Button* pb, int y, int btnWidth, int btnHeight, char* str, double strRelativeSize, IMAGE* texture) {
	initButton(pb, WIDTH / 2 - btnWidth / 2, WIDTH / 2 + btnWidth / 2, y, y + btnHeight, str, strRelativeSize, texture);
}

void initColumnMiddleButton(Button* pb, int x, int btnWidth, int btnHeight, char* str, double strRelativeSize, IMAGE* texture) {
	initButton(pb, x, x + btnWidth, HEIGHT / 2 - btnHeight / 2, HEIGHT / 2 + btnHeight / 2, str, strRelativeSize, texture);
}

void initButton(Button* pb, int left, int right, int top, int buttom, char* str, double strRelativeSize, IMAGE* texture) {
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
	pb->isFocusDegreeIncreasing = false;
	strncpy(pb->str, str, BUTTON_TEXT_MAX);
	return;
}

void paintPanel(Panel* ppanel) {
	putimage(0, 0, ppanel->background);
	settextstyle(60, 0, _T("微软雅黑 Light"), 0, 0, 1000, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
	outtextxy(WIDTH / 2 - strlen(ppanel->title) * 12, 60, ppanel->title);
	for (int i = 0; i < ppanel->btnCnt; i++) {
		updateBtn(ppanel->buttonArray + i);
		paintButtonFocusLine(ppanel->buttonArray + i, ppanel->flbgArray);
		if (i > 0 && ppanel->buttonArray[i].focusDegree < ppanel->buttonArray[i - 1].focusDegree)
			paintButtonFocusLine(ppanel->buttonArray + i - 1, ppanel->flbgArray);//避免重复渲染出现断线情况
			//这不是很高效/明智的方法，但是很省事，效果也不错，毕竟也性能影响不大
	}
	for (int i = 0; i < ppanel->btnCnt; i++) {
		paintButton(ppanel->buttonArray + i);
	}

}

void paintButtonFocusLine(Button* pb, IMAGE* flbgArray) {
	//rectangle(pb->left - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION, pb->top - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION,
	//	pb->right + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION, pb->buttom + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION);
	if (!pb->focusDegree)
		return;
	int thickness = BGFL_THICKNESS * pb->focusDegree;
	int left = pb->left - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION - BGFL_BIAS - thickness;
	int top = pb->top - (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION - BGFL_BIAS - thickness;
	int right = pb->right + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION + BGFL_BIAS;
	int buttom = pb->buttom + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION + BGFL_BIAS;
	int width = pb->right - pb->left + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION * 2 + BGFL_BIAS * 2 + thickness * 2;
	int height = pb->buttom - pb->top + (1 - pb->focusDegree) * BUTTON_EDGE_DIFFUSION * 2 + BGFL_BIAS * 2 + thickness * 2;

	IMAGE* flbg = flbgArray + (int)(pb->focusDegree * (BGFL_LEVEL - 1));
	putimage(left, top, thickness, height, flbg, left, top);
	putimage(left, top, width, thickness, flbg, left, top);
	putimage(right, top, thickness, height, flbg, right, top);
	putimage(left, buttom, width, thickness, flbg, left, buttom);
}

double nonlinear(double x) {
	return pow(x, 0.2);
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
		pb->focusDegree * 500,
		false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);

	/*
	outtextxy(
		pb->or_left + (pb->focusDegree + 1) * (pb->or_width - 24 * pb->strLen) * (1+BUTTON_GROWTH)/2+4,
		pb->strBiasY + pb->or_top + pb->or_height * pb->strRelativeSize /6 - pb->or_height * pb->strRelativeSize * pb->focusDegree * BUTTON_GROWTH / 4,
		_T(pb->str)
	);
	*/
	outtextxy(
		pb->strBiasX + (pb->strLeftJustify ? pb->left + 20 : pb->or_left + ((pb->or_width - strHeight * 0.38 * pb->strLen) / 2)),
		pb->strBiasY + (pb->strFollowY ? pb->top : pb->or_top) + pb->or_height * pb->strRelativeSize / 6 - pb->or_height * pb->strRelativeSize * pb->focusDegree * BUTTON_GROWTH / 4,
		_T(pb->str)
	);
}

bool testBtnFocus(Button* pb, Point p) {
	return pb->or_left <= p.x && p.x <= pb->or_right && pb->or_top <= p.y && p.y <= pb->or_buttom;
}

void updateBtnFocus(Panel* ppanel, Point mousePoint) {
	for (int i = 0; i < ppanel->btnCnt; i++) {
		if (testBtnFocus(ppanel->buttonArray + i, mousePoint)) {
			if (ppanel->buttonArray[i].linear_focusDegree < 1)
				ppanel->buttonArray[i].linear_focusDegree += MOUSE_BTN_FOCUS_DEGREE_INCREASE;
			if (!ppanel->buttonArray[i].isFocusDegreeIncreasing) {
				playAudio("focus", 100);
				ppanel->buttonArray[i].isFocusDegreeIncreasing = true;
			}
			if (DEBUG_OUTPUT > 1)
				printf("button[%d].focusDegree = %.2lf\n", i, ppanel->buttonArray[i].linear_focusDegree);
		}
		else if (ppanel->buttonArray[i].linear_focusDegree > 0) {
			ppanel->buttonArray[i].linear_focusDegree -= MOUSE_BTN_FOCUS_DEGREE_INCREASE;
			ppanel->buttonArray[i].isFocusDegreeIncreasing = false;
		}
		if (ppanel->buttonArray[i].linear_focusDegree > 1) {
			ppanel->buttonArray[i].linear_focusDegree = 1;
		}	
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

void clearBtnFocusDegree(Button* pbtn) {
	pbtn->linear_focusDegree = 0;
	pbtn->focusDegree = 0;
	return;
}
#include "zuma.h"










void initPanels(MajorPanels* pmp) {
	
	if (DEBUG_OUTPUT)
		initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	else
		initgraph(WIDTH, HEIGHT);

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
	initEndingPanel(&pmp->endingPanel, pmp);
	return;
}

void initMainMenu(Panel* pmm,const MajorPanels* pmp) {
	pmm->title = "";
	pmm->background = pmp->backgroundWithLogo;
	pmm->backgroundDark = pmp->backgroundWithLogoDark;
	pmm->flbgArray = pmp->backgroundWithLogoFlbgArray;
	pmm->btnCnt = 3;
	Button* pba = pmm->buttonArray = (Button*)malloc(sizeof(Button) * pmm->btnCnt);
	int startingY = HEIGHT / 2 - (MAIN_MENU_BTN_HEIGHT + MAIN_MENU_BTN_GAP) * pmm->btnCnt / 2 + 120;
	char btnStrs[][20] = { "开始","选项","退出" };
	for (int i = 0; i < pmm->btnCnt; i++) {
		initLineMiddleButton(pba + i, startingY + (MAIN_MENU_BTN_HEIGHT + MAIN_MENU_BTN_GAP) * i, MAIN_MENU_BTN_WIDTH, MAIN_MENU_BTN_HEIGHT, btnStrs[i],1, pmm->backgroundDark);
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
	char btnStrs[][20] = { "测","测测测...","maximum","MMMMMM...","返回" };
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

void initEndingPanel(Panel* pep, const MajorPanels* pmp) {
	pep->title = "游戏结束";
	pep->background = pmp->background;
	pep->backgroundDark = pmp->backgroundDark;
	pep->flbgArray = pmp->backgroundFlbgArray;
	pep->btnCnt = 1;
	Button* pba = pep->buttonArray = (Button*)malloc(sizeof(Button) * pep->btnCnt);
	initLineMiddleButton(pba, HEIGHT - ENDING_PANEL_BTN_HEIGHT - ENDING_PANEL_BTN_GAP, ENDING_PANEL_BTN_WIDTH, ENDING_PANEL_BTN_HEIGHT, "返回", 1, pep->backgroundDark);
	return;
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
	MapPreviewArray* pmpa;
	previewMaps(&pmpa);

	int startingIndex=0,btmCommand;
	bool updatedStartingIndex = true;
	
	
	while (true) {
		switch (btmCommand=operatePanelsMouseEvents(&pmp->mapChoosingPanel)) {
		case 0: case 1: case 2: case 3:
			startCoreGaming((pmpa->mpa + startingIndex + btmCommand)->dir);
			endingPanel(pmp);
			return;
		case 4:
			return;
		case 5:
			if (startingIndex - 4 >= 0) {
				startingIndex -= 4;
				updatedStartingIndex = true;
			}
			break;
		case 6:
			if (startingIndex + 4 < pmpa->cnt) {
				startingIndex += 4;
				updatedStartingIndex = true;
			}
			break;
		default:
			break;
		}
		if (updatedStartingIndex) {
			for (int i = 0; i < 4; i++) {
				strcpy(pmp->mapChoosingPanel.buttonArray[i].str, "");
				if (i + startingIndex < pmpa->cnt) {
					strncpy(pmp->mapChoosingPanel.buttonArray[i].str, (pmpa->mpa + i + startingIndex)->dir, 6);
					pmp->mapChoosingPanel.buttonArray[i].str[7] = '\0';
					if (strlen((pmpa->mpa + i + startingIndex)->dir) > 6)
						strcat(pmp->mapChoosingPanel.buttonArray[i].str, "...");
				}
				updatedStartingIndex = false;
			}
		}
		BeginBatchDraw();
		paintPanel(&pmp->mapChoosingPanel);
		FlushBatchDraw();
		EndBatchDraw();
	}
}



void endingPanel(MajorPanels* pmp) {

	while (true) {
		switch (operatePanelsMouseEvents(&pmp->endingPanel)) {
		case 0:
			return;
		default:
			break;
		}
		BeginBatchDraw();
		paintPanel(&pmp->endingPanel);
		FlushBatchDraw();
		EndBatchDraw();
	}
}


















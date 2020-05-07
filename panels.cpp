#include "zuma.h"

void initPanels(MajorPanels* pmp) {
	
	if (DEBUG_OUTPUT)
		initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	else
		initgraph(WIDTH, HEIGHT);

	setbkmode(TRANSPARENT);

	IMAGE loadingBackground;
	loadimage(&loadingBackground, "image\\main_background_loading.jpg", WIDTH, HEIGHT, true);
	putimage(0, 0, &loadingBackground);

	loadAudio();

	startBGM();
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
	initPausePanel(&pmp->pausePanel, pmp);
	initOptionPanel(&pmp->optionPanel, pmp);
	initDeveloperPanel(&pmp->developerPanel, pmp);

	//TODO:���ļ��ж�ȡ
	musicVolume(1);
	foleyVolume(1);
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
	char btnStrs[][20] = { "��ʼ","ѡ��","�˳�" };
	for (int i = 0; i < pmm->btnCnt; i++) {
		initLineMiddleButton(pba + i, startingY + (MAIN_MENU_BTN_HEIGHT + MAIN_MENU_BTN_GAP) * i, MAIN_MENU_BTN_WIDTH, MAIN_MENU_BTN_HEIGHT, btnStrs[i],1, pmm->backgroundDark);
	}
}


void initMapChoosingPanel(Panel* pmcp, const MajorPanels* pmp) {
	pmcp->title = "ѡ���ͼ";
	pmcp->background = pmp->background;
	pmcp->backgroundDark = pmp->backgroundDark;
	pmcp->flbgArray = pmp->backgroundFlbgArray;
	pmcp->btnCnt = 7;
	Button* pba = pmcp->buttonArray = (Button*)malloc(sizeof(Button) * pmcp->btnCnt);
	int startingX = WIDTH / 2 - (CHOOSING_MAP_PANEL_BTN_WIDTH + CHOOSING_MAP_PANEL_BTN_GAP) * 4 / 2 + CHOOSING_MAP_PANEL_BTN_GAP/2;
	char btnStrs[][20] = { "��","����...","maximum","MMMMMM...","����" };
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
	pep->title = "��Ϸ����";
	pep->background = pmp->background;
	pep->backgroundDark = pmp->backgroundDark;
	pep->flbgArray = pmp->backgroundFlbgArray;
	pep->btnCnt = 1;
	Button* pba = pep->buttonArray = (Button*)malloc(sizeof(Button) * pep->btnCnt);
	initLineMiddleButton(pba, HEIGHT - ENDING_PANEL_BTN_HEIGHT - ENDING_PANEL_BTN_GAP, ENDING_PANEL_BTN_WIDTH, ENDING_PANEL_BTN_HEIGHT, "����", 1, pep->backgroundDark);
	return;
}

void initPausePanel(Panel* ppp, const MajorPanels* pmp) {
	ppp->title = "��ͣ";
	ppp->background = pmp->background;
	ppp->backgroundDark = pmp->backgroundDark;
	ppp->flbgArray = pmp->backgroundFlbgArray;
	ppp->btnCnt = 3;
	Button* pba = ppp->buttonArray = (Button*)malloc(sizeof(Button) * ppp->btnCnt);
	int startingY = HEIGHT / 2 - (MAIN_MENU_BTN_HEIGHT + MAIN_MENU_BTN_GAP) * ppp->btnCnt / 2;
	char btnStrs[][20] = { "������Ϸ","ѡ��","������ǰ��Ϸ" };
	for (int i = 0; i < ppp->btnCnt; i++) {
		initLineMiddleButton(pba + i, startingY + (MAIN_MENU_BTN_HEIGHT + MAIN_MENU_BTN_GAP) * i, MAIN_MENU_BTN_WIDTH, MAIN_MENU_BTN_HEIGHT, btnStrs[i], 1, ppp->backgroundDark);
	}
}

void initOptionPanel(Panel* pop, const MajorPanels* pmp) {
	//TODO:���ļ���д������С
	pop->title = "ѡ��";
	pop->background = pmp->background;
	pop->backgroundDark = pmp->backgroundDark;
	pop->flbgArray = pmp->backgroundFlbgArray;
	pop->btnCnt = 6;
	Button* pba = pop->buttonArray = (Button*)malloc(sizeof(Button) * pop->btnCnt);
	initButton(pba + 0, WIDTH / 3 - 60 +50, WIDTH / 3 - 20 +50, HEIGHT / 3 - 60, HEIGHT / 3 - 20, "-", 1, pop->backgroundDark);
	initButton(pba + 1, WIDTH / 3*2 + 20 +70, WIDTH / 3*2 + 60 +70, HEIGHT / 3 - 60, HEIGHT / 3 - 20, "+", 1, pop->backgroundDark);
	initButton(pba + 2, WIDTH / 3 - 60 +50, WIDTH / 3 - 20 +50, HEIGHT / 3 +20, HEIGHT / 3 + 60, "-", 1, pop->backgroundDark);
	initButton(pba + 3, WIDTH / 3*2 + 20 +70, WIDTH / 3*2 + 60 +70, HEIGHT / 3 +20, HEIGHT / 3 +60, "+", 1, pop->backgroundDark);
	int startingY = HEIGHT*2 / 3 -(MAIN_MENU_BTN_HEIGHT + MAIN_MENU_BTN_GAP) *2 / 2;
	char btnStrs[][20] = { "������Ա","����" };
	for (int i = 0; i < 2; i++) {
		initLineMiddleButton(pba+i+4, startingY + (MAIN_MENU_BTN_HEIGHT + MAIN_MENU_BTN_GAP) * i, MAIN_MENU_BTN_WIDTH, MAIN_MENU_BTN_HEIGHT, btnStrs[i], 1, pop->backgroundDark);
	}
}

void initDeveloperPanel(Panel* pdp, const MajorPanels* pmp) {
	pdp->title = "������Ա";
	pdp->background = pmp->background;
	pdp->backgroundDark = pmp->backgroundDark;
	pdp->flbgArray = pmp->backgroundFlbgArray;
	pdp->btnCnt = 1;
	Button* pba = pdp->buttonArray = (Button*)malloc(sizeof(Button) * pdp->btnCnt);
	initLineMiddleButton(pba, HEIGHT - ENDING_PANEL_BTN_HEIGHT - ENDING_PANEL_BTN_GAP, ENDING_PANEL_BTN_WIDTH, ENDING_PANEL_BTN_HEIGHT, "����", 1, pdp->backgroundDark);
}


void mainMenu(MajorPanels* pmp) {
	
	while (true) {
		switch (operatePanelsMouseEvents(&pmp->mainMenu)) {
		case 0:
			mapChoosingPanel(pmp);
			clearBtnFocusDegree(&pmp->mainMenu.buttonArray[0]);
			break;
		case 1:
			optionPanel(pmp);
			clearBtnFocusDegree(&pmp->mainMenu.buttonArray[1]);
			break;
		case 2:
			return;
		default:
			break;
		}
		BeginBatchDraw();
		paintPanel(&pmp->mainMenu);
		settextstyle(30, 0, _T("΢���ź� Light"), 0, 0, 300, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
		outtextxy(30, HEIGHT-60, "version: 0.3.0");
		outtextxy(WIDTH - 260, HEIGHT - 100, "�߼����Գ�����ƿ���");
		outtextxy(WIDTH - 230, HEIGHT - 60, "������  19071110");
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
			clearBtnFocusDegree(&pmp->mapChoosingPanel.buttonArray[btmCommand]);
			if (startingIndex + btmCommand < pmpa->cnt) {
				mciSendString("pause bgm", NULL, 0, NULL);
				if (DEBUG_OUTPUT)
					printf("  MCI: %s\n", "pause bgm");
				startCoreGaming((pmpa->mpa + startingIndex + btmCommand)->dir, pmp);
				endingPanel(pmp);
				mciSendString("resume bgm", NULL, 0, NULL);
				if (DEBUG_OUTPUT)
					printf("  MCI: %s\n", "resume bgm");
				return;
			}else {
				break;
			}
		case 4:
			clearBtnFocusDegree(&pmp->mapChoosingPanel.buttonArray[4]);
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
					strncpy(pmp->mapChoosingPanel.buttonArray[i].str, (pmpa->mpa + i + startingIndex)->name, 6);
					pmp->mapChoosingPanel.buttonArray[i].str[7] = '\0';
					if (strlen((pmpa->mpa + i + startingIndex)->name) > 6)
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
			clearBtnFocusDegree(&pmp->endingPanel.buttonArray[0]);
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

bool pausePanel(MajorPanels* pmp) {
	while (true) {
		if (kbhit() && 27 == getch())
			return false;
		switch (operatePanelsMouseEvents(&pmp->pausePanel)) {
		case 0:
			clearBtnFocusDegree(&pmp->pausePanel.buttonArray[0]);
			return false;
		case 1:
			optionPanel(pmp);
			clearBtnFocusDegree(&pmp->pausePanel.buttonArray[1]);
			break;
		case 2:
			clearBtnFocusDegree(&pmp->pausePanel.buttonArray[2]);
			return true;
		default:
			break;
		}
		BeginBatchDraw();
		paintPanel(&pmp->pausePanel);
		FlushBatchDraw();
		EndBatchDraw();
	}
}

void optionPanel(MajorPanels* pmp) {
	Point mouseClickedPoint;
	double mcX, mcY;
	int musicSliderPosition, foleySliderPosition;
	char musicStr[10], foleyStr[10];
	while (true) {
		switch (operatePanelsMouseEvents(&pmp->optionPanel,&mouseClickedPoint)) {
		case 0:
			musicVolume(musicVolume(-1) - 0.1);
			break;
		case 1:
			musicVolume(musicVolume(-1) + 0.1);
			break;
		case 2:
			foleyVolume(foleyVolume(-1) - 0.1);
			break;
		case 3:
			foleyVolume(foleyVolume(-1) + 0.1);
			break;
		case 4:
			developerPanel(pmp);
			clearBtnFocusDegree(&pmp->optionPanel.buttonArray[4]);
			break;
		case 5:
			clearBtnFocusDegree(&pmp->optionPanel.buttonArray[5]);
			return;
		default:
			break;
		}
		mcX = mouseClickedPoint.x;
		mcY = mouseClickedPoint.y;
		if (WIDTH / 3 + 40 <= mcX && mcX <= WIDTH * 2 / 3 + 80) {
			if (HEIGHT / 3 - 60 <= mcY && mcY <= HEIGHT / 3 + 10) {
				musicVolume((mcX - WIDTH / 3 - 60) / (WIDTH / 3));
			}
			else if(HEIGHT / 3 + 20 <= mcY && mcY <= HEIGHT / 3 + 90)
				foleyVolume((mcX - WIDTH / 3 - 60) / (WIDTH / 3));
		}
		if (musicVolume(-1) < 0)
			musicVolume(0);
		if (musicVolume(-1) > 1)
			musicVolume(1);
		if (foleyVolume(-1) < 0)
			foleyVolume(0);
		if (foleyVolume(-1) > 1)
			foleyVolume(1);
		
		musicSliderPosition = WIDTH / 3 * (1 + musicVolume(-1)) +60;
		foleySliderPosition = WIDTH / 3 * (1 + foleyVolume(-1)) +60;
		BeginBatchDraw();
		paintPanel(&pmp->optionPanel);
		setfillstyle(BS_SOLID);

		settextstyle(36, 0, _T("΢���ź� Light"), 0, 0, 500, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
		outtextxy(WIDTH / 3 - 150, HEIGHT / 3 - 58, "��������");
		outtextxy(WIDTH / 3 - 150, HEIGHT / 3 + 22, "��Ч����");

		sprintf(musicStr, "%d", (int)(musicVolume(-1) * 100));
		sprintf(foleyStr, "%d", (int)(foleyVolume(-1) * 100));

		settextstyle(30, 0, _T("΢���ź� Light"), 0, 0, 600, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
		outtextxy(musicSliderPosition - 6 * strlen(musicStr), HEIGHT / 3 - 30, musicStr);
		outtextxy(foleySliderPosition - 6 * strlen(foleyStr), HEIGHT / 3 + 50, foleyStr);

		fillrectangle(musicSliderPosition-2, HEIGHT / 3 - 50, musicSliderPosition+2 , HEIGHT / 3 - 30);
		fillrectangle(foleySliderPosition-2, HEIGHT / 3 + 50, foleySliderPosition+2 , HEIGHT / 3 + 30);
		
		fillrectangle(WIDTH / 3 + 60, HEIGHT / 3 - 41, WIDTH * 2 / 3 + 60, HEIGHT / 3 - 39);
		fillrectangle(WIDTH / 3 + 60, HEIGHT / 3 + 41, WIDTH * 2 / 3 + 60, HEIGHT / 3 + 39);
		setfillstyle(BS_NULL);
		FlushBatchDraw();
		EndBatchDraw();
	}
}

void developerPanel(MajorPanels* pmp) {
	while (true) {
		switch (operatePanelsMouseEvents(&pmp->developerPanel)) {
		case 0:
			return;
		default:
			break;
		}
		BeginBatchDraw();
		paintPanel(&pmp->developerPanel);
		FlushBatchDraw();
		EndBatchDraw();
	}
}
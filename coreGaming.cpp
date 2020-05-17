#include "zuma.h"


Score startCoreGaming(char* dir, MajorPanels* pmp) {
	MajorData md;
	if (!loadMap(&md.mi, "maps", dir))
		handleException(14);
	//handleException(15);
	mciSendString("pause bgm", NULL, 0, NULL);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n","pause bgm");
	return coreGaming(md, pmp);
}

Score coreGaming(MajorData md, MajorPanels* pmp) {
	md.gameEnd = false;
	initAllBallList(&md.pbl, &md.mi);
	initZuma(&md);
	initFlyingBallArray(md.flyingBallArray, &md.mi);
	initPainting();
	while (!md.gameEnd) {
		upadateColorInfo(&md);
		operateMouseEvents(&md);//������Ҳ���
		computeZuma(&md.zuma);
		computeFlyingBalls(md.flyingBallArray, md.zuma, *md.pbl, &md.mi);//����ɳ���
		computeAllBallList(md.pbl, &md.mi);
		if (kbhit() && 27 == getch() && pausePanel(pmp) || (md.gameEnd = checkIfGameover(&md.mi, md.pbl)) ) {
			Score sc = initScore();
			for (int i = 0; i < md.mi.mpi.ballListCount; i++) {
				sc = addScore(sc, (md.pbl + i)->score);
			}
			if(md.gameEnd)
				Sleep(3000);
			return sc;
		}
		computeAllBallListPoint(md.pbl, &md.mi);
		paintImage(&md);//����ͼ��
		//Sleep(50);	//��ͣ
		//TODO:����һ��Ҫ��Ҫ��ʱ��Ȼ��sleep(1000/fps-��ʱ)
	}
}

bool checkIfGameover(MapInfo* pmi,BallList* pbl) {
	//test if win
	bool isVectory = true;
	for (int i = 0; i < pmi->mpi.ballListCount; i++) {
		if (!(pbl + i)->isEmpty) {
			isVectory = false;
			break;
		}
	}
	if (isVectory) {
		gameover(true,pbl,pmi);
		return true;
	}
	//test if lose
	bool isLose = false;
	for (int i = 0; i < pmi->mpi.ballListCount; i++) {
		if (checkIfBallListOverFlow(pbl + i)) {
			isLose = true;
			break;
		}
	}
	if (isLose) {
		gameover(false,pbl,pmi);
		return true;
	}
	return false;
}

void gameover(bool isVectory,BallList* pbl,MapInfo* pmi) {
	putimage(0, 0, pmi->ri.background);
	if (DEBUG_OUTPUT) {
		for (int i = 0; i < pmi->mpi.ballListCount; i++)
			viewRoute(pmi->pr + i);
		paintViewAllBallList(pbl, pmi);
	}
	settleScore(isVectory, pbl, pmi);
	settextcolor(BLACK);
	settextstyle(100, 0, _T("΢���ź� Light"), 0, 0, 800, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
	if (isVectory) {
		playAudio("victory", foleyVolume(-1)*100);
		outtextxy(WIDTH / 2 - 200, HEIGHT / 2 - 50, "YOU WIN");
	}else {
		playAudio("failure", foleyVolume(-1)*100);
		outtextxy(WIDTH / 2 - 200, HEIGHT / 2 - 50, "YOU LOSE");
	}
	settextcolor(WHITE);
	if (DEBUG_OUTPUT) {
		char stringBuffer[STRING_BUFFER_SIZE];
		for (int i = 0; i < pmi->mpi.ballListCount; i++) {
			settextcolor(BLACK);
			settextstyle(20, 0, _T("΢���ź� Light"), 0, 0, 800, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
			sprintf(stringBuffer, "score[%d]: %d", i, pbl[i].scoreInt);
			//outtextxy(20, 20 + i * 16, stringBuffer);
			settextcolor(WHITE);
		}
	}
	return;
}

void settleScore(bool isVectory, BallList* pbl, MapInfo* pmi) {
	bool finishedRouteRemainScore = false;
	if (isVectory) {
		while (!finishedRouteRemainScore) {
			finishedRouteRemainScore = true;
			for (int i = 0; i < pmi->mpi.ballListCount; i++) {
				if (pbl[i].latestRemovedBallPosition >= pbl[i].pr->pointCount - 100)
					continue;
				finishedRouteRemainScore = false;
				settextstyle(40, 0, _T("΢���ź� Light"), 0, 0, 800, false, false, false, NULL, NULL, NULL, ANTIALIASED_QUALITY, NULL);
				playAudio("score0", foleyVolume(-1)*100);
				outtextxy(minus(route(pbl[i].pr, pbl[i].latestRemovedBallPosition), makePoint(0, 0)), "+100");
				pbl[i].scoreInt += 100;
				pbl[i].score.finalScore += 100;
				pbl[i].latestRemovedBallPosition += 100;
				Sleep(10);
				//TODO:�����Ч�͸��õļӷ�չʾЧ��
			}
			Sleep(200);
		}
	}
	
	
}

Score addScore(Score sc1, Score sc2) {
	Score ret;
	ret.finalScore = sc1.finalScore + sc2.finalScore;
	ret.greatestCrash = max(sc1.greatestCrash, sc2.greatestCrash);
	ret.longestCombo = max(sc1.longestCombo, sc2.longestCombo);
	return ret;
}

Score initScore() {
	Score ret;
	ret.finalScore = 0;
	ret.greatestCrash = 0;
	ret.longestCombo = 0;
	return ret;
}
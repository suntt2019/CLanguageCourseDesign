#include "zuma.h"


void startCoreGaming(char* dir) {
	MajorData md;
	loadMap(&md.mi, "maps",dir);
	coreGaming(md);
	return;
}

void coreGaming(MajorData md) {
	md.gameEnd = false;
	
	initAllBallList(&md.pbl, &md.mi);
	initZuma(&md);
	initFlyingBallArray(md.flyingBallArray, &md.mi);
	initPainting();
	while (!md.gameEnd) {
		upadateColorInfo(&md);
		operatingInput(&md);//处理玩家操作
		computeZuma(&md.zuma);
		computingFlyingBalls(md.flyingBallArray, md.zuma, *md.pbl, &md.mi);//计算飞出球
		computeAllBallList(md.pbl, &md.mi);
		if(md.gameEnd = checkIfGameover(&md.mi, md.pbl))
			break;
		computeAllBallListPoint(md.pbl, &md.mi);
		paintImage(&md);//绘制图像
		//Sleep(50);	//暂停
		//TODO:测试一下要不要计时，然后sleep(1000/fps-计时)
	}
	return;
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
	if (isVectory)
		outtextxy(20, 100, "YOU WIN");
	else
		outtextxy(20, 100, "YOU LOSE");
	if (DEBUG_OUTPUT) {
		char stringBuffer[STRING_BUFFER_SIZE];
		for (int i = 0; i < pmi->mpi.ballListCount; i++) {
			sprintf(stringBuffer, "score-%d: %d", i, pbl[i].score);
			outtextxy(20, 20 + i * 16, stringBuffer);
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
				outtextxy(minus(route(pbl[i].pr, pbl[i].latestRemovedBallPosition), makePoint(0, 0)), "+100");
				pbl[i].score += 100;
				pbl[i].latestRemovedBallPosition += 100;
				Sleep(10);
				//TODO:添加音效和更好的加分展示效果
			}
			Sleep(200);
		}
	}
	
	
}
#include "zuma.h"

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
		gameover(true);
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
		gameover(false);
		return true;
	}
	return false;
}

void gameover(bool isVectory) {
	if (isVectory)
		outtextxy(100, 100, "YOU WIN");
	else
		outtextxy(100, 100, "YOU LOSE");
	return;
}
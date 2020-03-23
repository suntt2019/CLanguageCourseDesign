#include "zuma.h"

void coreGaming(MajorData md) {
	md.gameEnd = false;
	//md.mi = loadingMapInfo(md.mapDir);
	
	initAllBallList(&md.pbl, &md.mi);
	initZuma(&md);
	initFlyingBallArray(md.flyingBallArray, &md.mi);
	initPainting();
	//viewBallList(&md.ballList);
	while (!md.gameEnd) {
		operatingInput(&md);//处理玩家操作
		computeZuma(&md.zuma);
		computingFlyingBalls(md.flyingBallArray, md.zuma, *md.pbl, &md.mi);//计算飞出球
		//md.gameEnd = computingBallList(md.ballList, &md.mi);//计算列上球
		//computeAllBallList(md.pbl, &md.mi);
		paintImage(&md);//绘制图像
		//Sleep(500);	//暂停
		//TODO:测试一下要不要计时，然后sleep(1000/fps-计时)
	}
	return;
}
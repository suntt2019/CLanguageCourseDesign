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
		operatingInput(&md);//������Ҳ���
		computeZuma(&md.zuma);
		computingFlyingBalls(md.flyingBallArray, md.zuma, *md.pbl, &md.mi);//����ɳ���
		//md.gameEnd = computingBallList(md.ballList, &md.mi);//����������
		computeAllBallList(md.pbl, &md.mi);
		paintImage(&md);//����ͼ��
		//Sleep(500);	//��ͣ
		//TODO:����һ��Ҫ��Ҫ��ʱ��Ȼ��sleep(1000/fps-��ʱ)
	}
	return;
}
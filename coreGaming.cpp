#include "zuma.h"

void coreGaming(MajorData md) {
	md.gameEnd = false;
	//md.mi = loadingMapInfo(md.mapDir);
	//initBallList(&md.ballList, &md.mi, (unsigned int)time(0));
	//initZuma(md);
	//initFlyingBallArray(md.flyingBallArray, &md.mi);
	initPainting();
	//viewBallList(&md.ballList);
	while (!md.gameEnd) {
		//operatingInput(md);//������Ҳ���
		//computingFlyingBalls(md.flyingBallArray, md.zuma, md.ballList, &md.mi);//����ɳ���
		//md.gameEnd = computingBallList(md.ballList, &md.mi);//����������
		paintImage(md);//����ͼ��
		/*
		if (md.zuma.CDremain > 0)
			md.zuma.CDremain--;
		*/
		//Sleep(500);	//��ͣ
		//TODO:����һ��Ҫ��Ҫ��ʱ��Ȼ��sleep(1000/fps-��ʱ)
	}
	return;
}
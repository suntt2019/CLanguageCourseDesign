#include "zuma.h"

void initPainting() {
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	setfillstyle(BS_NULL);
	return;
}


void paintImage(MajorData& md) {
	BeginBatchDraw();//��ʼ������ͼ
	putimage(0, 0, md.mi.ri.background);
	//paintBallList(md.ballList, &md.mi);
	//paintFlyingBall(md.flyingBallArray, md.zuma, &md.mi);
	//paintZuma(md.zuma, &md.mi);
	for (int i = 0; i < md.mi.mpi.ballListCount; i++)
		viewRoute(md.mi.pr+i);
	FlushBatchDraw();
	EndBatchDraw();//����������ͼ�������ƺõ�ͼƬͳһ������Ļ�ϡ�	

	//��������ʱʹ��
	//if (md.ballList.firstBallPosition > 400)
	//	md.ballList.firstBallPosition = 0;


	return;
}

void viewRoute(Route* pr) {
	for (int i = 0; i < pr->pointCount; i++) {
		putpixel((pr->pointArray + i)->x, (pr->pointArray + i)->y, WHITE);
	}
	return;
}
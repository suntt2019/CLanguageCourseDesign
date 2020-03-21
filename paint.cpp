#include "zuma.h"

void initPainting() {
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	setfillstyle(BS_NULL);
	return;
}


void paintImage(MajorData& md) {
	BeginBatchDraw();//开始批量绘图
	putimage(0, 0, md.mi.ri.background);
	//paintBallList(md.ballList, &md.mi);
	//paintFlyingBall(md.flyingBallArray, md.zuma, &md.mi);
	//paintZuma(md.zuma, &md.mi);
	for (int i = 0; i < md.mi.mpi.ballListCount; i++)
		viewRoute(md.mi.pr+i);
	FlushBatchDraw();
	EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	

	//仅供测试时使用
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
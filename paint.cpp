#include "zuma.h"

void initPainting() {
	initgraph(WIDTH, HEIGHT, SHOWCONSOLE);
	setfillstyle(BS_NULL);
	return;
}


void paintImage(MajorData* pmd) {
	BeginBatchDraw();//开始批量绘图
	putimage(0, 0, pmd->mi.ri.background);
	paintAllBallList(pmd->pbl, &pmd->mi);
	paintFlyingBall(pmd->flyingBallArray, pmd->zuma, &pmd->mi);
	paintZuma(pmd->zuma, &pmd->mi);
	if (DEBUG_OUTPUT) {
		for (int i = 0; i < pmd->mi.mpi.ballListCount; i++)
			viewRoute(pmd->mi.pr + i);
		paintViewAllBallList(pmd->pbl, &pmd->mi);
	}
	FlushBatchDraw();
	EndBatchDraw();//结束批量绘图，将绘制好的图片统一贴到屏幕上。	

	//仅供测试时使用
	//if (pmd->ballList.firstBallPosition > 400)
	//	pmd->ballList.firstBallPosition = 0;


	return;
}

void viewRoute(Route* pr) {
	for (int i = 0; i < pr->pointCount; i++) {
		putpixel((pr->pointArray + i)->x, (pr->pointArray + i)->y, WHITE);
	}
	return;
}


void paintAllBallList(BallList* pbl, MapInfo* pmi) {
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		paintBallList(pbl + i, pmi);
	return;
}

void paintBallList(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	Point point;
	while (p) {
		rotateAndPaint(pmi->ri.ballImgs + p->color, pmi->ri.ballMaskImgs + p->color,
			routeArgle(pbl->pr,(int)p->position), p->point , false);
		p = p->prev;
	}
	return;
}


void paintViewAllBallList(BallList* pbl, MapInfo* pmi) {
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		paintViewBallList(pbl+i, pmi);
	return;
}

void paintViewBallList(BallList* pbl,MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	Point point;
	while (p) {
		point = route(pbl->pr, (int)p->position);
		fillcircle(p->point.x,p->point.y, pmi->gs.ballR);
		if(p->insertingDegree<1)
			fillcircle(p->point.x, p->point.y, pmi->gs.ballR+2);
		p = p->prev;
	}
	return;
}

void paintZuma(Zuma zuma, MapInfo* pmi) {
	//printf("paintZuma,x=%.2lf,y=%.2lf\n", zuma.position.x, zuma.position.y);
	rotateAndPaint(pmi->ri.zuma, pmi->ri.zumaMask, zuma.angle + PI / 2, zuma.position, true);
	return;
}


void paintFlyingBall(FlyingBallArray& fba, Zuma zuma, MapInfo* pmi) {//TODO:绘制镜像翻转的鱼（flyingball），使之眼睛朝上？
	for (int i = 0; i < fba.size; i++) {
		rotateAndPaint(pmi->ri.ballImgs + fba.pfb[i].color, pmi->ri.ballMaskImgs + fba.pfb[i].color, fba.pfb[i].angle, fba.pfb[i].position, true);
		fillcircle(fba.pfb[i].position.x, fba.pfb[i].position.y, pmi->gs.ballR);
		//floodfill(fba.pfb[i].position.x, fba.pfb[i].position.y, WHITE);
	}
	return;
}



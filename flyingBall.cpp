#include "zuma.h"

void initFlyingBallArray(FlyingBallArray& fba, MapInfo* pmi) {
	fba.pfb = (FlyingBall*)malloc(sizeof(FlyingBall) * FLYING_BALL_ARRAY_SIZE);
	fba.size = 0;
	generateFlyingBall(fba, &pmi->ri);
	generateFlyingBall(fba, &pmi->ri);
	return;
}

void computingFlyingBalls(FlyingBallArray& fba, Zuma zuma, BallList& bl, MapInfo* pmi) {
	fba.pfb[fba.size - 2].position.x = zuma.position.x
		+ pmi->mpi.deltaMouthPosition.y * cos(zuma.angle) + pmi->mpi.deltaMouthPosition.x * sin(zuma.angle);
	fba.pfb[fba.size - 2].position.y = zuma.position.y
		- pmi->mpi.deltaMouthPosition.y * sin(zuma.angle) + pmi->mpi.deltaMouthPosition.x * cos(zuma.angle);
	fba.pfb[fba.size - 1].position.x = zuma.position.x
		+ pmi->mpi.deltaHolePosition.y * cos(zuma.angle) + pmi->mpi.deltaHolePosition.x * sin(zuma.angle);
	fba.pfb[fba.size - 1].position.y = zuma.position.y
		- pmi->mpi.deltaHolePosition.y * sin(zuma.angle) + pmi->mpi.deltaHolePosition.x * cos(zuma.angle);
	fba.pfb[fba.size - 2].angle = zuma.angle;
	fba.pfb[fba.size - 1].angle = zuma.angle;
	if (!pmi->ri.availableColorBook[fba.pfb[fba.size - 2].color])
		fba.pfb[fba.size - 2].color = generateRandomColor(&pmi->ri);
	if (!pmi->ri.availableColorBook[fba.pfb[fba.size - 1].color])
		fba.pfb[fba.size - 1].color = generateRandomColor(&pmi->ri);
	//TODO-optional:zuma口中球变色动画
	for (int i = 0; i < fba.size - 2; i++) {
		fba.pfb[i].position.x += pmi->gs.flySpeed * cos(fba.pfb[i].angle);
		fba.pfb[i].position.y -= pmi->gs.flySpeed * sin(fba.pfb[i].angle);
		if (isOutOfScreen(fba.pfb[i].position)) {
			removeFlyingBall(fba, i);
		}else {
			 testCrashAll(&bl, fba, i, pmi);
		}
	}

	return;
}


void launchFlyingBall(FlyingBallArray& fba, ResourceInfo* pri) {//TODO:这个函数有存在的意义么
	generateFlyingBall(fba, pri);
	return;
}

void generateFlyingBall(FlyingBallArray& fba, ResourceInfo* pri) {
	fba.pfb[fba.size].color = generateRandomColor(pri);
	fba.size++;
	return;
}

void removeFlyingBall(FlyingBallArray& fba, int index) {
	for (int i = index + 1; i < fba.size; i++) {
		fba.pfb[i - 1] = fba.pfb[i];
	}
	fba.size--;
	return;
}


void viewFlyingBallArray(FlyingBallArray* pfba) {
	printf("  viewFlyingBallArray():\n");
	for (int i = 0; i < pfba->size; i++) {
		printf("    flyingBall#%d: position:(%.2lf,%.2lf),color:%d,angle:%.2lf\n",
			i,pfba->pfb[i].position.x, pfba->pfb[i].position.y, pfba->pfb[i].color, pfba->pfb[i].angle);
	}
}

void swapFlyingBall(FlyingBallArray* pfba) {
	FlyingBall temp = pfba->pfb[pfba->size - 1];
	pfba->pfb[pfba->size - 1] = pfba->pfb[pfba->size - 2];
	pfba->pfb[pfba->size - 2] = temp;
	return;
}

bool findColorInGame(MajorData* pmd,int color){
	for (int i = 0; i < pmd->flyingBallArray.size-2; i++) {
		if (pmd->flyingBallArray.pfb[i].color == color) {
			return true;
		}
	}
	for (int i = 0; i < pmd->mi.mpi.ballListCount; i++) {
		BallOnList* p = pmd->pbl[i].tail;
		while (p) {
			if (p->color == color)
				return true;
			p = p->prev;
		}
	}
	return false;
}

void upadateColorInfo(MajorData* pmd) {
	for (int i = 0; i < pmd->mi.ri.colorCount; i++) {
		if (pmd->mi.ri.availableColorBook[i] && !findColorInGame(pmd, i)) {
			pmd->mi.ri.availableColorBook[i] = false;
		}
	}
	if (DEBUG_OUTPUT>1) {
		printf("\n[DEBUG_OUTPUT]upadateColorInfo():\n");
		viewColorInfo(&pmd->mi.ri);
	}
	return;
}

int generateRandomColor(ResourceInfo* pri) {
	int ret;
	//TODO:在coreGaming中添加检测游戏结束的判定，保证不会在所有球都没有时调用此函数
	//或者在这里也加一行判定也行（最好是对函数调用关系分析一下）
	do {
		ret = rand() % pri->colorCount;
	} while (!pri->availableColorBook[ret]);
	return ret;
}

void viewColorInfo(ResourceInfo* pri) {
	printf("  viewColorInfo():\n");
	printf("    availableColor:");
	for (int i = 0; i < pri->colorCount; i++) {
		printf("%d ",i);
	}
	printf("\n    availableColor:");
	for (int i = 0; i < pri->colorCount; i++) {
		printf("%d ", pri->availableColorBook[i] ? 1 : 0);
	}
	printf("\n");
	return;
}
#include "zuma.h"

void initFlyingBallArray(FlyingBallArray& fba, MapInfo* pmi) {
	fba.pfb = (FlyingBall*)malloc(sizeof(FlyingBall) * FLYING_BALL_ARRAY_SIZE);
	fba.size = 0;
	generateFlyingBall(fba, pmi->ri.colorCount);
	generateFlyingBall(fba, pmi->ri.colorCount);
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
	for (int i = 0; i < fba.size - 2; i++) {
		fba.pfb[i].position.x += pmi->gs.flySpeed * cos(fba.pfb[i].angle);
		fba.pfb[i].position.y -= pmi->gs.flySpeed * sin(fba.pfb[i].angle);
		if (isOutOfScreen(fba.pfb[i].position)) {
			removeFlyingBall(fba, i);
		}
		testCrashAll(&bl, fba, i, pmi);
	}

	return;
}


void launchFlyingBall(FlyingBallArray& fba, int colorCount) {
	//fba.pfb[fba.size - 3].launched = true;
	generateFlyingBall(fba, colorCount);
	return;
}

void generateFlyingBall(FlyingBallArray& fba, int colorCount) {
	FlyingBall newBall;
	//有些局部变量只是为了代码看起来美观，不过牺牲了效率，emm，或许其实开优化之后都一样？
	newBall.color = rand() % colorCount;
	//newBall.launched = false;
	fba.pfb[fba.size] = newBall;
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



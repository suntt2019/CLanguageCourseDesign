#include "zuma.h"
//TODO: 这个划分有点尴尬，之后分到其他地方去？
void operatingInput(MajorData* pmd) {
	operateMouseEvents(pmd);
	return;
}

void operateMouseEvents(MajorData* pmd) {
	MOUSEMSG mmsg;
	double deltaX, deltaY, tanOfAngle;
	while (MouseHit()) {
		mmsg = GetMouseMsg();
		deltaX = mmsg.x - pmd->zuma.position.x;
		deltaY = -(mmsg.y - pmd->zuma.position.y);
		pmd->zuma.angle = getAngle(deltaX, deltaY);
		
		if (mmsg.mkLButton && pmd->zuma.CDremain == 0) {
			launchFlyingBall(pmd->flyingBallArray, pmd->mi.ri.colorCount);
			pmd->zuma.CDremain = pmd->mi.gs.shootingCD;
		}
		if(DEBUG_OUTPUT>1)
			printf("mouse:x=%d,y=%d,deltaX=%.2lf,deltaY=%.2lf,tanOfAngle=%.4lf,angle=%.4lf\n", mmsg.x, mmsg.y,deltaX,deltaY,tanOfAngle,pmd->zuma.angle);
	}
	return;
}
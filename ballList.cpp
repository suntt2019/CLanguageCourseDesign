#include "zuma.h"
#define INSERT_PUSH_FORCE 10
#define INSERTING_SPEED 0.05 * SQRT_3
#define TEST_R 10

void viewBallList(BallList* pbl) {
	printf("  [viewBallList] tail=%p, pr=%p\n", pbl->tail, pbl->pr);
	BallOnList* p = pbl->tail;
	int i = 0;
	while (p) {
		printf("    ball#%d: p->color=%d, p->position=%2lf, p->force=%.2lf, p=%p, p->prev=%p, p->next=%p\n",
			i, p->color, p->position, p->force, p, p->prev, p->next);
		p = p->prev;
		i++;
	}
	return;
}


void initAllBallList(BallList** ppbl,MapInfo* pmi) {
	*ppbl = (BallList*)malloc(sizeof(BallList)* pmi->mpi.ballListCount);
	if (!*ppbl)
		longjmp(env, 5);
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		initBallList(*ppbl+i, pmi->pr+i, pmi, (unsigned int)time(0)+i);
	return;
}


void initBallList(BallList* pbl,Route* pr, MapInfo* pmi, unsigned int seed) {

	srand(seed);
	pbl->pr = pr;
	pbl->beginningRushRoundRemain = pr->beginningRushRound;

	pbl->tail = (BallOnList*)malloc(sizeof(BallOnList));
	if (!pbl->tail)
		longjmp(env, 5);
	pbl->tail->color = rand() % pmi->ri.colorCount;
	pbl->tail ->next = NULL;
	pbl->tail->prev = NULL;
	pbl->tail->position = -(pr->ballCount * 2 - 1) * pmi->gs.ballR;
	pbl->tail->force = 0;
	pbl->tail->routeBias = 0;

	BallOnList* p = pbl->tail;
	for (int i = 1; i < pr->ballCount; i++) {
		p->prev = (BallOnList*)malloc(sizeof(BallOnList));
		if (!p->prev)
			longjmp(env, 5);
		p->prev->next = p;
		p->prev->color = rand() % pmi->ri.colorCount;
		p = p->prev;
		p->position = -(pr->ballCount * 2 - 2*i - 1) * pmi->gs.ballR;
		p->force = 0;
		p->routeBias = 0;
	}
	p->prev = NULL;

	if (DEBUG_OUTPUT) {
		printf("\n[DEBUG_OUTPUT]initBallList():\n");
		printf("  Finished initing ballList.\n");
		viewBallList(pbl);
	}

	return;
}

void computeAllBallList(BallList* pbl, MapInfo* pmi) {
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		computeBallList(pbl + i, pmi);
	return;
}

void computeBallList(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	while (p) {
		p->force = 0;
		if (p->routeBias > TORLANCE)
			p->routeBias -= INSERTING_SPEED;
		else if(p->routeBias < -TORLANCE)
			p->routeBias += INSERTING_SPEED;
		p = p->prev;
	}
	//TODO:函数参数轻量化（pmi => pgs 等）

	computeBeginningRush(pbl,pmi);
	computeNormalPush(pbl, pmi);

	applyForceToPosition(pbl,pmi);
	computeBallListPoint(pbl, pmi);

	if (DEBUG_OUTPUT > 1) {
		viewBallList(pbl);
	}

	return;
}

void computeBeginningRush(BallList* pbl, MapInfo* pmi) {
	if (pbl->beginningRushRoundRemain < 0)
		return;
	pbl->tail->force += pmi->gs.beginningRushSpeed;//TODO:改成非线性的Rush
	pbl->beginningRushRoundRemain--;
	return;
}

void computeNormalPush(BallList* pbl, MapInfo* pmi) {
	pbl->tail->force += pmi->gs.moveSpeed;
	return;
}

void applyForceToPosition(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	double overLappingDistance;
	if (!p)
		longjmp(env, 6);
	while (p->prev) {
		if (getGapBetweenBOL(pbl, &pmi->gs,p,p->prev))//如果两球相邻
			p->prev->force += p->force;
		p->position += p->force;
		
		if (p->next) {
			overLappingDistance = -getGapBetweenBOL(pbl, &pmi->gs, p, p->next);
			if (overLappingDistance > TORLANCE)
				p->position += overLappingDistance;
		}
		p = p->prev;
	}
	p->position += p->force;
	if (p->next) {//TODO-ins2：检测若与后球重叠（智能碰撞箱）就多前进一点 OK
		overLappingDistance = -getGapBetweenBOL(pbl, &pmi->gs, p, p->next);
		if (overLappingDistance > TORLANCE)
			p->position += overLappingDistance;
	}
	return;
}

double getGapBetweenBOL(BallList* pbl, GameSettings* pgs, BallOnList* p1, BallOnList* p2) {
	return fabs(p1->position - p2->position) - pgs->ballR * sqrt(4 - pow(p1->routeBias - p2->routeBias, 2));
}

void computeBallListPoint(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	if (DEBUG_OUTPUT > 1)
		printf("[DEBUG_OUTPUT]computeBallListPoint()\n");
	while (p) {
		p->point = route(pbl->pr, p->position);
		if (fabs(p->routeBias) > TORLANCE) {
			p->point.x -= cos(routeArgle(pbl->pr, p->position) + PI / 2)
				* pmi->gs.ballR * p->routeBias;
			p->point.y += sin(routeArgle(pbl->pr, p->position) + PI / 2)
				* pmi->gs.ballR * p->routeBias;
		}
		if (DEBUG_OUTPUT > 1) {
			printf("  p=%p, p->prev=%p, p->next=%p, p->position=%.2lf, p->routeBias=%.2lf, p->point=(%.2lf,%.2lf)\n",
				p,p->prev,p->next,p->position,p->routeBias,p->point.x,p->point.y);
		}
		p = p->prev;
	}
	return;
}

void insertBallList(BallList* pbl, BallOnList* pbol_prev, BallOnList* pbol_next, FlyingBallArray& fba, int index, MapInfo* pmi) {
	BallOnList* p = (BallOnList*)malloc(sizeof(BallOnList));
	BallOnList* q;
	if (!p) { 
		longjmp(env, 5);
		return;
	}
	p->color = fba.pfb[index].color;
	p->prev = pbol_prev;
	p->next = pbol_next;
	p->force = 0;

	if (!pbol_prev) {
		p->position = pbol_next->position + pmi->gs.ballR;
		pbol_next->prev = p;
	}
	else if (!pbol_next) {
		p->position = pbol_prev->position - pmi->gs.ballR;
		pbl->tail = p;
		pbol_prev->next = p;
	}
	else {
		p->position = (pbol_prev->position+pbol_next->position)/2;
		pbol_prev->next = p;
		pbol_next->prev = p;
		q = pbol_prev;
	}
	//judge
	p->point = route(pbl->pr,(int)p->position);
	Point p1 = makePoint(p->point.x - cos(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R,
		p->point.y + sin(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R);
	Point p2 = makePoint(p->point.x + cos(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R,
		p->point.y - sin(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R);
	p->routeBias = compareDistance(fba.pfb[index].position, p1, p2) ? -SQRT_3 : SQRT_3;//TODO:改成根据flyingball消失的位置计算bias
	
	removeFlyingBall(fba, index);
	return;
}

//TODO:用函数指针把这些all整合成一个
void testCrashAll(BallList* pbl, FlyingBallArray& fba, int index, MapInfo* pmi) {
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		testCrash(pbl + i, fba, index, pmi);
	return;
}

void testCrash(BallList* pbl, FlyingBallArray& fba, int index, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	while (p) {
		if (testPointDistance(p->point, fba.pfb[index].position, pmi->gs.ballR * 2)) {
			if (compareDistance(fba.pfb[index].position,
				(p->position > pmi->gs.ballR * 2 ? 
					route(pbl->pr,(int)(p->position - pmi->gs.ballR * 2)) : route(pbl->pr,0)),
				route(pbl->pr,(int)(p->position + pmi->gs.ballR * 2))))
				insertBallList(pbl, p->prev, p, fba, index, pmi);
			else
				insertBallList(pbl, p, p->next, fba, index, pmi);
			break;
		}
		p = p->prev;
	}
}


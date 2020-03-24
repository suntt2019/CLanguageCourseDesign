#include "zuma.h"


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
	pbl->tail->isInserting = false;

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
		p->isInserting = false;
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
		p = p->prev;
	}
	computeBeginningRush(pbl,pmi);
	computeNormalPush(pbl, pmi);

	applyForceToPosition(pbl,pmi);

	correctOverLapping(pbl, pmi);

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
	if (!p)
		longjmp(env, 6);
	while (p->prev) {
		p->position += p->force;
		if (isNextTo(&pmi->gs,p,p->prev))//如果两球相邻
			p->prev->force = p->force;
		p = p->prev;
	}
	p->position += p->force;
	return;
}

bool isNextTo(GameSettings* pgs,BallOnList* p1,BallOnList* p2) {
	if (p1->isInserting || p2->isInserting)
		return true;//TODO:用point测量
	return p2->position - p1->position <= pgs->ballR * 2;
}

bool isOverLapping(GameSettings* pgs, BallOnList* p1, BallOnList* p2) {
	if (p1->isInserting || p2->isInserting)
		return false;
	return p2->position - p1->position < pgs->ballR * 2 - TORLANCE;
}


void correctOverLapping(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	while (p&& p->prev) {
		if (isOverLapping(&pmi->gs, p, p->prev))
			p->prev->position += p->prev->position - p->position - pmi->gs.ballR * 2;
		p = p->prev;
	}
	return;
}


void computeBallListPoint(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	while (p) {
		if (p->isInserting) {

		}else {
			p->point = route(pbl->pr, p->position);
		}
		p = p->prev;
	}
	return;
}

//TODO:add insertBallList(with Animation...)
/*
void insertBallList(BallList& bl, BallOnList* pbol_prev, BallOnList* pbol_next, FlyingBallArray& fba, int index, MapInfo* pmi) {
	BallOnList* p = (BallOnList*)malloc(sizeof(BallOnList));
	BallOnList* q;
	if (!p) { 
		printf("[Excption] initBallList: excption when creating ball.\n");
		return;
	}
	p->color = fba.pfb[index].color;
	p->prev = pbol_prev;
	p->next = pbol_next;
	//printf("%.4lf~between %.4lf and %.4lf\n", p->position,pbol_prev->position,pbol_next->position);

	if (!pbol_prev) {
		p->position = pbol_next->position + 2 * pmi->ballR / getSpeedValue(pmi, pbol_next->position);
		bl.firstBall = p;
		pbol_next->prev = p;
	}
	else if (!pbol_next) {
		p->position = pbol_prev->position - 2 * pmi->ballR / getSpeedValue(pmi, pbol_prev->position);
		pbol_prev->next = p;
	}
	else {
		p->position = pbol_prev->position;
		pbol_prev->next = p;
		pbol_next->prev = p;
		q = pbol_prev;
		while (q) {
			q->position += 2 * pmi->ballR / getSpeedValue(pmi, q->position);
			q = q->prev;
		}
	}
	removeFlyingBall(fba, index);
	return;
}
*/

//TODO：适配testCrash
/*
void testCrash(BallList& bl, FlyingBallArray& fba, int index, MapInfo* pmi) {
	BallOnList* p = bl.firstBall;
	//double thisBallPosition = bl.firstBallPosition;
	while (p) {
		if (testPointDistance(pmi->routePoints[(int)p->position], fba.pfb[index].position, pmi->ballR * 2)) {
			if (compareDistance(fba.pfb[index].position,
				(p->position > pmi->ballR * 2 ? pmi->routePoints[(int)(p->position - pmi->ballR * 2)] : pmi->routePoints[0]), pmi->routePoints[(int)(p->position + pmi->ballR * 2)]))
				insertBallList(bl, p->prev, p, fba, index, pmi);
			else
				insertBallList(bl, p, p->next, fba, index, pmi);
			break;
		}
		p = p->next;
		//thisBallPosition -= pmi->ballR * 2;
	}
}
*/

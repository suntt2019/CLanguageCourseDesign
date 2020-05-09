#include "zuma.h"
#define TEST_R 10
#define SCORE_REMOVE_BALL 10
#define SCORE_ACHIEVE_MORE_THAN_3 20

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
		handleException(5);
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		initBallList(*ppbl+i, pmi->pr+i, pmi, (unsigned int)time(0)+i);
	return;
}


void initBallList(BallList* pbl,Route* pr, MapInfo* pmi, unsigned int seed) {

	srand(seed);
	pbl->pr = pr;
	pbl->beginningRushRoundRemain = pr->beginningRushRound;
	pbl->isEmpty = false;
	pbl->scoreInt = 0;
	pbl->score.finalScore = 0;
	pbl->score.greatestCrash = 0;
	pbl->score.longestCombo = 0;
	pbl->latestRemovedBallPosition = 0;

	pbl->tail = (BallOnList*)malloc(sizeof(BallOnList));
	if (!pbl->tail)
		handleException(5);
	pbl->tail->color = getBallColor(&pmi->ri,pbl->pr,0);
	pbl->tail ->next = NULL;
	pbl->tail->prev = NULL;
	pbl->tail->position = -(pr->ballCount * 2 - 1) * pmi->gs.ballR;
	pbl->tail->force = 0;
	pbl->tail->routeBias = 0;
	pbl->tail->attractLevel = 0;

	BallOnList* p = pbl->tail;
	for (int i = 1; i < pr->ballCount; i++) {
		p->prev = (BallOnList*)malloc(sizeof(BallOnList));
		if (!p->prev)
			handleException(5);
		p->prev->next = p;
		p->prev->color = getBallColor(&pmi->ri, pbl->pr, i);
		p = p->prev;
		p->position = -(pr->ballCount * 2 - 2*i - 1) * pmi->gs.ballR;
		p->force = 0;
		p->attractLevel = 0;
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


int getBallColor(ResourceInfo* pri,Route* pr,int index) {
	int ret = -1;
	static int lastColor;
	//printf("%s\n", pr->generatingBallMethod);
	if (strcmp(pr->generatingBallMethod, "random") == 0) {
		ret = generateRandomColor(pri);
	}else if (strcmp(pr->generatingBallMethod, "probability") == 0) {
		if (index == 0 || (rand() % pr->probability) > 0) {
			ret = generateRandomColor(pri);
			lastColor = ret;
		}else {
			ret = lastColor;
		}
	}else if (strcmp(pr->generatingBallMethod, "fixed") == 0) {
		if (0 <= index && index < pr->ballCount)
			ret = pr->ballListOrderArray[pr->ballCount-1-index];
		else
			handleException(12);
	}else {
		handleException(9);
	}

	if (ret < 0)
		handleException(8);
	return ret;
}


void computeAllBallList(BallList* pbl, MapInfo* pmi) {
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		if(!(pbl+i)->isEmpty)
			computeBallList(pbl + i, pmi);
	return;
}

void computeBallList(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	if (checkIfBallListEmpty(pbl))
		return;

	//TODO:将下面这一块封装成函数
	while (p) {//推动正在插入的球插入球列
		p->force = 0;
		if (p->routeBias > TORLANCE)
			p->routeBias -= pmi->gs.insertingSpeed * SQRT_3;
		else if(p->routeBias < -TORLANCE)
			p->routeBias += pmi->gs.insertingSpeed * SQRT_3;
		p = p->prev;
	}

	//TODO:函数参数轻量化（pmi => pgs 等）

	computeBeginningRush(pbl,pmi);
	computeNormalPush(pbl, pmi);
	computeAttractionPull(pbl, pmi);

	if (checkIfBallListEmpty(pbl))
		return;

	applyForceToPosition(pbl,pmi);
	

	if (DEBUG_OUTPUT > 1) {
		viewBallList(pbl);
	}

	return;
}

bool checkIfBallListEmpty(BallList* pbl) {
	if (!pbl->tail) {
		pbl->isEmpty = true;
		if (DEBUG_OUTPUT) {
			printf("\n[DEBUG_OUTPUT]checkIfBallListEmpty():\n");
			printf("  ballList is empty, pbl=%p\n", pbl);
		}
		return true;
	}else {
		return false;
	}
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

void computeAttractionPull(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	while (p) {
		if (p->next && p->attractLevel) {
			if (getGapBetweenBOL(pbl, &pmi->gs, p, p->next) <= TORLANCE) {
				if (testAchievingScore(pbl,pmi, p, p->attractLevel)) {
					computeAttractionPull(pbl, pmi);
					return;
				}else {
					p->attractLevel = 0;
				}
				//if
				if (DEBUG_OUTPUT) {
					printf("\n[DEBUG_OUTPUT]computeAttractionPull():\n");
					printf("  removed attractLevel,p=%p\n", p);
				}
			}else {
				p->force += pmi->gs.attractionPullForce;//TODO-optional:改为非线性的
			}
		}
		p = p->prev;
	}
	return;
}

void applyForceToPosition(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	double overLappingDistance;
	if (!p)
		handleException(6);//在调用这个函数之前加了判断，理论上不会出现这种情况了
	while (p->prev) {
		if (getGapBetweenBOL(pbl, &pmi->gs, p, p->prev) <= TORLANCE)//如果两球相邻
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


void computeAllBallListPoint(BallList* pbl, MapInfo* pmi) {
	for (int i = 0; i < pmi->mpi.ballListCount; i++)
		computeBallListPoint(pbl+i, pmi);
	return;
}

void computeBallListPoint(BallList* pbl, MapInfo* pmi) {
	BallOnList* p = pbl->tail;
	if (DEBUG_OUTPUT > 1)
		printf("\n[DEBUG_OUTPUT]computeBallListPoint()\n");
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

//TODO:想办法让这个函数的参数数量少一点
void insertBallList(BallList* pbl, BallOnList* pbol_prev, BallOnList* pbol_next, FlyingBallArray& fba, int index, MapInfo* pmi,bool crashPrev) {
	playAudio("crash", foleyVolume(-1)*100);
	BallOnList* p = (BallOnList*)malloc(sizeof(BallOnList));
	if (!p) { 
		handleException(5);
		return;
	}
	p->color = fba.pfb[index].color;
	p->prev = pbol_prev;
	p->next = pbol_next;
	p->force = 0;
	p->attractLevel = 0;

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
		if (getGapBetweenBOL(pbl, &pmi->gs, pbol_prev, pbol_next) > pmi->gs.ballR * 2 - TORLANCE) {
			if (crashPrev)
				p->position = pbol_prev->position - pmi->gs.ballR;
			else
				p->position = pbol_next->position + pmi->gs.ballR;
		}else {
			p->position = (pbol_prev->position + pbol_next->position) / 2;
		}
		
		pbol_prev->next = p;
		pbol_next->prev = p;
	}
	if (checkIfBallListOverFlow(pbl)) {
		if(pbol_next)
			pbol_next->prev = pbol_prev;
		if(pbol_prev)
			pbol_prev->next = pbol_next;
		removeBallOnList(pbl,p);
		return;
	}
	//judge
	p->point = route(pbl->pr,(int)p->position);
	Point p1 = makePoint(p->point.x - cos(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R,
		p->point.y + sin(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R);
	Point p2 = makePoint(p->point.x + cos(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R,
		p->point.y - sin(routeArgle(pbl->pr, p->position) + PI / 2) * TEST_R);
	p->routeBias = compareDistance(fba.pfb[index].position, p1, p2) ? -SQRT_3 : SQRT_3;
	//TODO:改成根据flyingball消失的位置计算bias
	

	testAchievingScore(pbl,pmi,p,0);
	removeFlyingBall(fba, index);
	return;
}


bool checkIfBallListOverFlow(BallList* pbl) {
	bool ret = false;
	BallOnList* p = pbl->tail;
	while (p) {
		if (p->position >= pbl->pr->pointCount) {
			ret = true;
			break;
		}
		p = p->prev;
	}
	return ret;
}

bool testAchievingScore(BallList* pbl, MapInfo* pmi, BallOnList* pbol_new, int attractionLevelBase) {
	bool returnValue = false;
	int cnt = 1,prevColor,nextColor;
	BallOnList* pbol_begin = pbol_new;
	BallOnList* pbol_end = pbol_new;
	BallOnList* p, *pbol_attract=NULL;
	while (pbol_begin->prev && pbol_begin->prev->color == pbol_new->color
		&& getGapBetweenBOL(pbl, &pmi->gs, pbol_begin->prev, pbol_begin) <= TORLANCE){
		pbol_begin = pbol_begin->prev;
		cnt++;
	}
	while (pbol_end->next && pbol_end->next->color == pbol_new->color
		&& getGapBetweenBOL(pbl, &pmi->gs, pbol_end->next, pbol_end) <= TORLANCE) {
		pbol_end = pbol_end->next;
		cnt++;
	}
	if (cnt >= 3) {
		if (pbol_begin->prev) {
			pbol_begin->prev->next = pbol_end->next;
			prevColor = pbol_begin->prev->color;
		}
		else {
			prevColor = NULL_COLOR;
		}
		if (pbol_end->next) {
			pbol_end->next->prev = pbol_begin->prev;
			nextColor = pbol_end->next->color;
		}
		else {
			pbl->tail = pbol_begin->prev;
			nextColor = NULL_COLOR;
		}
		pbol_attract = pbol_begin->prev;
		if (DEBUG_OUTPUT) {
			printf("\n[DEBUG_OUTPUT]testAchievingScore():\n");
			printf("  pbol_new=%p, pbol_begin=%p, pbol_end=%p, cnt=%d\n", pbol_new, pbol_begin, pbol_end, cnt);
		}
		pbl->scoreInt += SCORE_ACHIEVE_MORE_THAN_3 * (cnt - 3);
		pbl->score.finalScore += SCORE_ACHIEVE_MORE_THAN_3 * (cnt - 3);
		if (cnt+1 > pbl->score.greatestCrash)
			pbl->score.greatestCrash = cnt+1;
		//free
		p = pbol_begin;
		pbol_end->next = NULL;
		while (p->next) {
			p = p->next;
			removeBallOnList(pbl,p->prev);
		}
		removeBallOnList(pbl,p);//TODO:检测内存泄露（静态分析工具?）
		if (prevColor == nextColor && pbol_attract) {
			if (DEBUG_OUTPUT) {
				printf("  increased attractLevel,%d->%d,p=%p\n",
					pbol_attract->attractLevel, attractionLevelBase + 1, p);
			}
			pbol_attract->attractLevel = attractionLevelBase + 1;
			if (pbol_attract->attractLevel > pbl->score.longestCombo)
				pbl->score.longestCombo = pbol_attract->attractLevel;
		}

		char audioNameBuf[30] = "";
		int atl = 0;
		static int nextAtl = 0;
		sprintf(audioNameBuf, "score%d", nextAtl);
		playAudio(audioNameBuf, foleyVolume(-1) * 100);
		if (pbol_attract) {
			atl = pbol_attract->attractLevel < 5 ? pbol_attract->attractLevel : 4;
			printf("pbol_attract->attractLevel=%d\n", pbol_attract->attractLevel);
		}
		else {
			printf("!pbol_attract\n");
		}
		nextAtl = atl;

		returnValue = true;
	}
	
	
	
	return returnValue;
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
			if (compareDistance(

				fba.pfb[index].position,

				(p->position > pmi->gs.ballR * 2 ? 
					route(pbl->pr,(int)(p->position - pmi->gs.ballR * 2)) : route(pbl->pr,0)),

				(p->position < pbl->pr->pointCount - pmi->gs.ballR * 2 ?
					route(pbl->pr,(int)(p->position + pmi->gs.ballR * 2)) : route(pbl->pr, 0))

				))
				insertBallList(pbl, p->prev, p, fba, index, pmi,false);
			else
				insertBallList(pbl, p, p->next, fba, index, pmi,true);
			break;
		}
		p = p->prev;
	}
}

void removeBallOnList(BallList* pbl,BallOnList* p) {
	pbl->scoreInt += SCORE_REMOVE_BALL;
	pbl->score.finalScore += SCORE_REMOVE_BALL;
	pbl->latestRemovedBallPosition = p->position;
	free(p);
	return;
}
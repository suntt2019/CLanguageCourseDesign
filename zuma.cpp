#include "zuma.h"

void initZuma(MajorData* pmd) {
	pmd->zuma.position = pmd->mi.mpi.zumaPosition;
	pmd->zuma.shootingCD = 0;
	pmd->zuma.swappingCD = 0;
	return;
}

void computeZuma(Zuma* pzuma) {
	if (pzuma->shootingCD > 0)
		pzuma->shootingCD--;
	if (pzuma->swappingCD > 0)
		pzuma->swappingCD--;
}
#include "zuma.h"

void initZuma(MajorData* pmd) {
	pmd->zuma.position = pmd->mi.mpi.zumaPosition;
	pmd->zuma.CDremain = 0;
	return;
}

void computeZuma(Zuma* pzuma) {
	if (pzuma->CDremain > 0)
		pzuma->CDremain--;
}
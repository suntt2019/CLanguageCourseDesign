#include "zuma.h"
#define NEGTIVE_ROUTE_POINT_X 300
#define NEGTIVE_ROUTE_POINT_Y 300

Point route(Route* pr, int index) {
	if (index < 0)
		return makePoint(NEGTIVE_ROUTE_POINT_X, NEGTIVE_ROUTE_POINT_Y);
	if (index >= pr->pointCount)
		longjmp(env, 7);
	return pr->pointArray[index];
}
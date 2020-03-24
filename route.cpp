#include "zuma.h"
#define NEGTIVE_ROUTE_POINT_X 300
#define NEGTIVE_ROUTE_POINT_Y 300
#define NEGTIVE_ROUTE_ANGLE 0

Point route(Route* pr, int position) {
	if (position < 0)
		return makePoint(NEGTIVE_ROUTE_POINT_X, NEGTIVE_ROUTE_POINT_Y);
	if (position >= pr->pointCount)
		longjmp(env, 7);
	return pr->pointArray[position];
}

double routeArgle(Route* pr, int position) {//TODO:增加对于的正在插入的球的支持
	if (position < 0)
		return NEGTIVE_ROUTE_ANGLE;
	if (position >= pr->pointCount)
		longjmp(env, 7);
	if (position == pr->pointCount - 1)//TODO:这个if会不会有些浪费
		return routeArgle(pr, position - 1);
	Point deltaP = minus(route(pr, position), route(pr, position + 1));
	if(isZero(deltaP))
		return routeArgle(pr, position - 1);
	return -getAngle(deltaP)+PI;
}
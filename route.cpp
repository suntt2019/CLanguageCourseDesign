#include "zuma.h"
#define NEGTIVE_ROUTE_POINT_X -300
#define NEGTIVE_ROUTE_POINT_Y -300
#define NEGTIVE_ROUTE_ANGLE 0

Point route(Route* pr, int position) {
	if (position < 0)
		return makePoint(NEGTIVE_ROUTE_POINT_X, NEGTIVE_ROUTE_POINT_Y);
	if (position >= pr->pointCount)//TODO:pointCount*pointStep?   ������pointStep��
		handleException(7);
	return pr->pointArray[position];//�����±�Խ�磬��û��gameover�ж�����������exception���������
	
}

double routeArgle(Route* pr, int position) {
	if (position < 0)
		return NEGTIVE_ROUTE_ANGLE;
	if (position >= pr->pointCount)
		handleException(7);
	if (position == pr->pointCount - 1)//TODO:���if�᲻����Щ�˷�
		return routeArgle(pr, position - 1);
	Point deltaP = minus(route(pr, position), route(pr, position + 1));
	if(isZero(deltaP))
		return routeArgle(pr, position - 1);
	return -getAngle(deltaP)+PI;
}
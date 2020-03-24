#include "zuma.h"

Point makePoint(double x, double y) {
	Point ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

bool isOutOfScreen(Point p) {
	//严格来说应该要计算旋转后有没有出画面，这里就算锚点是否出画面了，算是一种省事而节约资源的方法
	//TODO：写一个重载的isOutOfScreen，计算旋转后的图形是否出画面
	return !(0 <= p.x && p.x <= WIDTH && 0 <= p.y && p.y <= HEIGHT);
}
//TODO：更新WIDTH和HEIGHT为动态窗口大小


bool testPointDistance(Point p1, Point p2, double minD) {
	return pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2) <= pow(minD, 2);
}

bool compareDistance(Point p, Point pTrue, Point pFalse) {
	return pow((p.x - pTrue.x), 2) + pow((p.y - pTrue.y), 2) > pow((p.x - pFalse.x), 2) + pow((p.y - pFalse.y), 2);
}

double getAngle(Point p) {
	double ret;
	if (p.x == 0)
		p.x = 0.000001;
	ret = atan(p.y/p.x);
	if (p.x < 0)
		ret += PI;
	return ret;
}

double getAngle(double x, double y) {
	double ret;
	if(x==0)
		x = 0.000001;
	ret = atan(y/x);
	if (x < 0)
		ret += PI;
	return ret;
}

Point minus(Point a, Point b) {
	return makePoint(a.x - b.x, a.y - b.y);
}

bool isZero(Point p) {
	if (p.x == 0 && p.y == 0)
		return true;
	return false;
}
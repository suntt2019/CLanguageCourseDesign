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
#include "zuma.h"

Point makePoint(double x, double y) {
	Point ret;
	ret.x = x;
	ret.y = y;
	return ret;
}

bool isOutOfScreen(Point p) {
	//�ϸ���˵Ӧ��Ҫ������ת����û�г����棬�������ê���Ƿ�������ˣ�����һ��ʡ�¶���Լ��Դ�ķ���
	//TODO��дһ�����ص�isOutOfScreen��������ת���ͼ���Ƿ������
	return !(0 <= p.x && p.x <= WIDTH && 0 <= p.y && p.y <= HEIGHT);
}
//TODO������WIDTH��HEIGHTΪ��̬���ڴ�С


bool testPointDistance(Point p1, Point p2, double minD) {
	return pow((p1.x - p2.x), 2) + pow((p1.y - p2.y), 2) < pow(minD, 2);
}

bool compareDistance(Point p, Point pTrue, Point pFalse) {
	return pow((p.x - pTrue.x), 2) + pow((p.y - pTrue.y), 2) > pow((p.x - pFalse.x), 2) + pow((p.y - pFalse.y), 2);
}
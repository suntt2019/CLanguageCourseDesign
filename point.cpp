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
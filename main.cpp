#include "zuma.h"

int main(){
	return handleException(0);	
}

int gameMain() {
	MajorData md;
	loadMap(&md.mi, "maps","test_points_one_color");//TODO����ͼѡ����涯̬����ѡ����ͼ
	coreGaming(md);
	return 0;
}
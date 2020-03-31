#include "zuma.h"

int main(){
	return handleException(0);	
}

int gameMain() {
	MajorData md;
	loadMap(&md.mi, "maps","test_points_one_color");//TODO：地图选择界面动态决定选哪张图
	coreGaming(md);
	return 0;
}
#include "zuma.h"

jmp_buf env;

int main(){
	MajorData md;
	loadMap(&md.mi, "maps","test_points");//TODO：地图选择界面动态决定选哪张图
	coreGaming(md);
	while (1);
	return 0;
}
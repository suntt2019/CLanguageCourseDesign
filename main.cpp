#include "zuma.h"

int main(){
	return handleException(0);	
}

int gameMain() {
	MajorPanels mp;
	initPanels(&mp);
	mainMenu(&mp);
	return 0;
}
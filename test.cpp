#include "zuma.h"
//#pragma comment(lib,"Winmm.lib")

void test() {
	loadAudio();
	//playAudio("focus", foleyVolume(-1)*100);
	//mciSendString("", NULL, 0, NULL);
	
	
	Sleep(30000);
	return;
	mciSendString("open \"Data\\1.mp3\" alias gbtc", NULL, 0, NULL);
	mciSendString("play gbtc", NULL, 0, NULL);
	mciSendString("setaudio gbtc volume to 80", NULL, 0, 0);
	Sleep(500);
	mciSendString("open \"Data\\1.mp3\" alias gbtc1", NULL, 0, NULL);
	mciSendString("play gbtc1", NULL, 0, NULL);
	mciSendString("setaudio gbtc1 volume to 80", NULL, 0, 0);
	Sleep(500);
	mciSendString("play gbtc", NULL, 0, NULL);
	Sleep(50000);
	return;
}
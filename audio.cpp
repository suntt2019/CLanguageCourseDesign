#include "zuma.h"

#define AUDIO_COPY_CNT 10

void loadAudio() {
	char commandBuffer[400];
	char audioName[][50] = { "focus","clicked","1" ,"shooting","crash","standerd","failure","victory","score0" ,"score1" ,"score2" ,"score3" ,"score4" };
	if (DEBUG_OUTPUT)
		printf("[DEBUG_OUTPUT]loadAudio()\n");
	//mciSendString("open \"audio\\1.mp3\" alias gbtc", NULL, 0, NULL);
	for (int i = 0; i < 14; i++) {
		for (int j = 0; j < AUDIO_COPY_CNT; j++) {
			sprintf(commandBuffer, "open \"audio\\%s.mp3\" alias %s_%d", audioName[i], audioName[i], j);
			mciSendString(commandBuffer, NULL, 0, NULL);
			if (DEBUG_OUTPUT)
				printf("  MCI: %s\n", commandBuffer);
		}
		//sprintf(commandBuffer, "play %s", audioName[i]);
		//mciSendString(commandBuffer, NULL, 0, NULL);
		//if (DEBUG_OUTPUT)
		//	printf("  MCI: %s\n", commandBuffer);

		//sprintf(commandBuffer, "pause %s", audioName[i]);
		//mciSendString(commandBuffer, NULL, 0, NULL);
		//if (DEBUG_OUTPUT)
		//	printf("  MCI: %s\n", commandBuffer);
	}
	//mciSendString("open \"audio\\focus.mp3\" alias focus", NULL, 0, NULL);
}

void playAudio(char* name,int volume) {
	static int id;
	char commandBuffer[400];
	if (DEBUG_OUTPUT)
		printf("[DEBUG_OUTPUT]playAudio()\n");

	if (id < AUDIO_COPY_CNT)
		id++;
	else
		id = 0;

	sprintf(commandBuffer, "seek %s_%d to start", name,id);
	mciSendString(commandBuffer, NULL, 0, NULL);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", commandBuffer);

	//sprintf(commandBuffer, "pause %s", name);
	//mciSendString(commandBuffer, NULL, 0, NULL);
	//if (DEBUG_OUTPUT)
	//	printf("  MCI: %s\n", commandBuffer);
	//Sleep(100);
	
	sprintf(commandBuffer, "play %s_%d",name,id);
	mciSendString(commandBuffer, NULL, 0, NULL);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", commandBuffer);
	sprintf(commandBuffer, "setaudio %s_%d volume to %d", name,id,volume);
	mciSendString(commandBuffer, NULL, 0, 0);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", commandBuffer);
	return;
}

void startBGM() {
	mciSendString("open \"audio\\bgm.mp3\" alias bgm", NULL, 0, NULL);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", "open \"audio\\bgm.mp3\" alias bgm");
	mciSendString("play bgm repeat", NULL, 0, NULL);
	if (DEBUG_OUTPUT)
		printf("  MCI: %s\n", "play bgm repeat");

}

double musicVolume(double v) {
	static double volume;
	if (v != -1) {
		char commandBuffer[400];
		volume = v;
		sprintf(commandBuffer, "setaudio bgm volume to %d",(int)(volume*100));
		mciSendString(commandBuffer, NULL, 0, NULL);
		if (DEBUG_OUTPUT)
			printf("  MCI: %s\n", commandBuffer);
	}
	return volume;
}

double foleyVolume(double v) {
	static double volume;
	if (v != -1)
		volume = v;
	return volume;
}
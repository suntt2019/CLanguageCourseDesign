#include "zuma.h"



bool loadMap(MapInfo* pmi,char* folder,char* mapName) {
	if (DEBUG_OUTPUT)
		printf("\nStart loadMap(pmi=%p,folder=%s,mapName=%s)\n", pmi, folder, mapName);
	char jsonString[JSON_MAX] = "";
	char mapJsonLine[JSON_LINE_MAX] = { '\0' };
	char dirBuffer[STRING_BUFFER_SIZE];
	char dirBuffer2[STRING_BUFFER_SIZE];
	char* isNEOF;//未到达文件尾部

	FILE* fp;
	sprintf(dirBuffer, "%s\\%s\\%s.json",folder,mapName,mapName);
	fp = fopen(dirBuffer, "r");
	if (!fp)
		return false;

	do {
		isNEOF=fgets(mapJsonLine, JSON_LINE_MAX, fp);
		if (isNEOF) {
			strcat(jsonString, mapJsonLine);
		}
	} while (isNEOF);

	const cJSON* json = cJSON_Parse(jsonString);
	if(!parseGameSettingsJson(pmi, json))
		return false;
	if (!parseMapPositionInfoJson(pmi, json, folder,mapName))
		return false;
	if (!parseResourceInfoJson(pmi, json, folder, mapName))
		return false;
	//TODO:cJSON_Delete很迷，会在下一次创建cJSON的时候用到之前free的指针，导致多次free的bug
	
	fclose(fp);

	return true;
}

bool parseGameSettingsJson(MapInfo* pmi, const cJSON* json) {
	cJSON* gameSettingsJson = cJSON_GetObjectItemCaseSensitive(json, "gameSettings");
	if (!gameSettingsJson)
		return false;


	int* pInts[] = { &pmi->gs.shootingCD, &pmi->gs.swappingCD };
	char* nameOfInts[] = { "shootingCD","swappingCD" };
	for (int i = 0; i < 2; i++)
		if(!parseJsonInt(gameSettingsJson, nameOfInts[i], pInts[i]))
			return false;


	double* pDoubles[] = { &pmi->gs.ballR,&pmi->gs.moveSpeed,&pmi->gs.flySpeed,&pmi->gs.beginningRushSpeed,&pmi->gs.insertingSpeed,&pmi->gs.attractionPullForce };
	char* nameOfDoubles[] = { "ballR","moveSpeed","flySpeed","beginningRushSpeed","insertingSpeed", "attractionPullForce" };
	for (int i = 0; i < 6; i++)
		if(!parseJsonDouble(gameSettingsJson, nameOfDoubles[i], pDoubles[i]))
			return false;
	
	
	//cJSON_Delete(gameSettingsJson);

	if (DEBUG_OUTPUT) {
		printf("\n[DEBUG_OUTPUT]parseGameSettingsJson():\n");
		for (int i = 0; i < 2; i++)
			printf("  Loaded int variable[%s]=%d  (&=%p)\n", nameOfInts[i], *pInts[i], pInts[i]);
		for (int i = 0; i < 6; i++)
			printf("  Loaded double variable[%s]=%.2lf  (&=%p)\n", nameOfDoubles[i], *pDoubles[i], pDoubles[i]);
	}
	return true;
}


bool parseMapPositionInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName) {
	cJSON* mapPositionInfoJson = cJSON_GetObjectItemCaseSensitive(json, "mapPositionInfo");
	cJSON* ballListsJson = cJSON_GetObjectItemCaseSensitive(mapPositionInfoJson, "ballLists");
	cJSON* zumaJson = cJSON_GetObjectItemCaseSensitive(mapPositionInfoJson, "zuma");
	if (!mapPositionInfoJson || !ballListsJson || !zumaJson)
		return false;

	if(!parseJsonInt(mapPositionInfoJson, "ballListCount", &pmi->mpi.ballListCount))
		return false;
	pmi->pr = (Route*)malloc(sizeof(Route) * pmi->mpi.ballListCount);

	cJSON* ballListJson;
	int i = 0;
	cJSON_ArrayForEach(ballListJson, ballListsJson) {
		if (i > pmi->mpi.ballListCount - 1)
			return false;//TODO:增加新的exception-过多的BallList
		if(!parseBallListJson(pmi->pr + i, ballListJson, folder, mapName))
			return false;
		i++;
	}

	Point* pPoints[] = { &pmi->mpi.zumaPosition,&pmi->mpi.deltaMouthPosition,&pmi->mpi.deltaHolePosition };
	char* nameOfPoints[] = { "zumaPosition","deltaMouthPosition","deltaHolePosition" };
	for (int i = 0; i < 3; i++)
		if(!parseJsonPoint(zumaJson, nameOfPoints[i], pPoints[i]))
			return false;
	
	if (DEBUG_OUTPUT) {
		printf("\n[DEBUG_OUTPUT]parseMapPositionInfoJson():\n");
		for (int i = 0; i < 3; i++)
			printf("  Loaded Point variable[%s]=(%.2lf,%.2lf)  (&=%p)\n",
				nameOfPoints[i], pPoints[i]->x, pPoints[i]->y, pPoints[i]);
	}
	
	//cJSON_Delete(mapPositionInfoJson);
	//cJSON_Delete(routeInfoJson);
	//cJSON_Delete(zumaJson);
	return true;
}



bool parseBallListJson(Route* pr,const cJSON* json,char* folder,char* mapName) {

	if(!parseJsonInt(json, "ballCount", &pr->ballCount))
		return false;
	
	if(!parseGeneratingBallMethod(pr,json,folder,mapName))
		return false;
	

	double* pDoubles[] = { &pr->beginningRushRound };
	char* nameOfDoubles[] = { "beginningRushRound" };
	for (int i = 0; i < 1; i++)
		if(!parseJsonDouble(json, nameOfDoubles[i], pDoubles[i]))
			return false;

	//TODO:适配其他路径储存方式的文件
	//计划是如果函数存储，在读取函数后，即刻将函数变为对应点

	cJSON* routeInfoJson = cJSON_GetObjectItemCaseSensitive(json, "routeInfo");
	int* pInts[] = { &pr->pointCount,&pr->pointStep };
	char* nameOfInts[] = { "pointCount","pointStep" };
	for (int i = 0; i < 2; i++)
		if(!parseJsonInt(routeInfoJson, nameOfInts[i], pInts[i]))
			return false;
	



	if (DEBUG_OUTPUT) {
		printf("  Loaded ballList:\n");
		printf("    Loaded int variable[%s]=%d  (&=%p)\n", "ballCount", pr->ballCount ,&pr->ballCount);
		for (int i = 0; i < 2; i++)
			printf("    Loaded int variable[%s]=%d  (&=%p)\n", nameOfInts[i], *pInts[i], pInts[i]);
		for (int i = 0; i < 1; i++)
			printf("    Loaded double variable[%s]=%.2lf  (&=%p)\n", nameOfDoubles[i], *pDoubles[i], pDoubles[i]);
	}

	char dirBuffer[STRING_BUFFER_SIZE];
	char dirBuffer2[STRING_BUFFER_SIZE];
	if(!parseJsonString(routeInfoJson, "routeFileName", dirBuffer))
		return false;
	sprintf(dirBuffer2, "%s\\%s\\%s", folder, mapName, dirBuffer);
	if (!loadRouteFile(pr, dirBuffer2))
		return false;

	return true;
}


bool parseGeneratingBallMethod(Route* pr, const cJSON* json, char* folder, char* mapName) {
	if(!parseJsonString(json, "generatingBallMethod", pr->generatingBallMethod))
		return false;//TODO:添加DEBUG_OUTPUT
	if (strcmp(pr->generatingBallMethod, "probability") == 0) {
		if(!parseJsonInt(json, "sameBallProbability", &pr->probability))
			return false;
		if (pr->probability == 0)
			return false;
	}else if (strcmp(pr->generatingBallMethod, "fixed") == 0) {
		char orderFileName[STRING_BUFFER_SIZE];
		char orderFileDir[STRING_BUFFER_SIZE];
		if(!parseJsonString(json, "ballListOrderFile", orderFileName))
			return false;
		sprintf(orderFileDir, "%s\\%s\\%s", folder, mapName, orderFileName);
		FILE* fp = fopen(orderFileDir, "r");
		if (!fp)
			return false;
		pr->ballListOrderArray = (int*)malloc(sizeof(int) * pr->ballCount);
		if (!pr->ballListOrderArray)
			return false;
		for (int i = 0; i < pr->ballCount; i++) {
			if (fscanf(fp, "%d",pr->ballListOrderArray+i) != 1)
				return false;
		}
	}else if (strcmp(pr->generatingBallMethod, "random") == 0) {
		;
	}else {
		return false;
	}
	return true;
}


bool parseResourceInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName) {
	cJSON* resourceInfoJson = cJSON_GetObjectItemCaseSensitive(json, "resourceInfo");
	cJSON* ballJson = NULL;
	char strBuffer[STRING_BUFFER_SIZE];
	char dirBuffer[STRING_BUFFER_SIZE];

	IMAGE** pimgs[] = { &pmi->ri.background , &pmi->ri.zuma , &pmi->ri.zumaMask };
	char* nameOfImgs[] = { "backGround" , "zuma" , "zumaMask" };
	for (int i = 0; i < 3; i++) {
		*pimgs[i] = new IMAGE[1];
		if (!*pimgs[i])
			return false;
		if(!parseJsonString(resourceInfoJson, nameOfImgs[i], strBuffer))
			return false;
		sprintf(dirBuffer, "%s\\%s\\%s", folder, mapName, strBuffer);
		if(!i)
			loadimage(*pimgs[i], dirBuffer,WIDTH,HEIGHT,true);
		else
			loadimage(*pimgs[i], dirBuffer);
	}

	

	cJSON* ballsJson = cJSON_GetObjectItemCaseSensitive(resourceInfoJson, "balls");
	if (!ballsJson)
		return false;
	if(!parseJsonInt(ballsJson, "colorCount", &pmi->ri.colorCount))
		return false;
	pmi->ri.ballImgs = new IMAGE[pmi->ri.colorCount];
	pmi->ri.ballMaskImgs = new IMAGE[pmi->ri.colorCount];
	pmi->ri.availableColorBook = (bool*)malloc(sizeof(bool) * pmi->ri.colorCount);
	for (int i = 0; i < pmi->ri.colorCount; i++)
		pmi->ri.availableColorBook[i] = true;

	int i = 0;
	cJSON* ballsResourceJson = cJSON_GetObjectItemCaseSensitive(ballsJson, "resource");
	if (!ballsResourceJson)
		return false;

	if (DEBUG_OUTPUT) {
		printf("\n[DEBUG_OUTPUT]parseResourceInfoJson():\n");
		for(int i=0;i<3;i++)
			printf("  loaded IMAGE(%s)-size:%d*%d  (&=%p)\n",
				nameOfImgs[i],(**pimgs[i]).getwidth(), (**pimgs[i]).getheight(), *pimgs[i]);
		printf("  loaded colorCount:%d  (&=%p)\n", pmi->ri.colorCount,&pmi->ri.colorCount);
	}

	cJSON_ArrayForEach(ballJson, ballsResourceJson) {
		if(!parseJsonString(ballJson, "img", strBuffer))
			return false;
		sprintf(dirBuffer, "%s\\%s\\%s", folder, mapName, strBuffer);
		loadimage(pmi->ri.ballImgs+i, dirBuffer);
		if(!parseJsonString(ballJson, "mask", strBuffer))
			return false;
		sprintf(dirBuffer, "%s\\%s\\%s", folder, mapName, strBuffer);
		loadimage(pmi->ri.ballMaskImgs+i, dirBuffer);
		if (DEBUG_OUTPUT) {
			printf("  loaded IMAGE[%d](img)-size:%d*%d  (&=%p)\n",i,
				(pmi->ri.ballImgs + i)->getwidth(), (pmi->ri.ballImgs + i)->getheight(),pmi->ri.ballImgs + i);
			printf("  loaded IMAGE[%d](mask)-size:%d*%d  (&=%p)\n", i,
				(pmi->ri.ballMaskImgs + i)->getwidth(), (pmi->ri.ballMaskImgs + i)->getheight(), pmi->ri.ballMaskImgs + i);
		}
		i++;
	}
	//cJSON_Delete(resourceInfoJson);
	return true;
}

bool parseJsonString(const cJSON* json, char* name, char* str) {
	cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!stringJson||!cJSON_IsString(stringJson))
		return false;
	strcpy(str, stringJson->valuestring);
	//str = stringJson->valuestring;
	//切记C语言中字符串不能直接=赋值，这个bug导致后面的IMAGE*被sprintf的一堆“烫”覆盖了

	//顺便我还是想吐槽C语言的字符串处理，不能用+将字符串相连，赋值要用单独一行函数
	//像python那样有=有+，或者向java那样有成员函数都可以啊
	//当然我也知道这有时代局限性的原因，C是比较接近底层的，可还是感觉这也太麻烦了
	//简简单单解析一个json就费了一百多行，简直了
	//还有就是异常处理，一处处地写，try-catch简直不要太好用...
	//cJSON_Delete(stringJson);
	return true;
}

bool parseJsonPoint(const cJSON* json, char* name, Point* pPoint) {
	cJSON* pointJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!pointJson)
		return false;
	cJSON* pointJsonX = cJSON_GetObjectItemCaseSensitive(pointJson, "x");
	cJSON* pointJsonY = cJSON_GetObjectItemCaseSensitive(pointJson, "y");
	if (!pointJsonX || !pointJsonY || !cJSON_IsNumber(pointJsonX) || !cJSON_IsNumber(pointJsonY))
		return false;
	*pPoint = makePoint(pointJsonX->valuedouble, pointJsonY->valuedouble);
	//cJSON_Delete(pointJson);
	//cJSON_Delete(pointJsonX);
	//cJSON_Delete(pointJsonY);
	return true;
}

bool parseJsonInt(const cJSON* json,char* name, int* pInt) {
	cJSON* intJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!intJson||!cJSON_IsNumber(intJson))
		return false;
	*pInt = intJson->valueint;
	//cJSON_Delete(intJson);
	return true;
}

bool parseJsonDouble(const cJSON* json, char* name, double* pDouble) {
	cJSON* doubleJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!doubleJson || !cJSON_IsNumber(doubleJson))
		return false;
	*pDouble = doubleJson->valuedouble;
	//cJSON_Delete(doubleJson);
	return true;
}

bool loadRouteFile(Route* pr,char* dir) {
	FILE* fp = fopen(dir, "r");
	if (!fp)
		return false;
	pr->pointArray = (Point*)malloc(sizeof(Point) * pr->pointCount);
	if (!pr->pointArray)
		return false;
	for (int i = 0; i < pr->pointCount; i++)
		if (fscanf(fp, "%lf %lf", &(pr->pointArray + i)->x, &(pr->pointArray + i)->y)!=2)
			return false;
	if (DEBUG_OUTPUT) {
		printf("  loadRouteFile():\n");
		printf("    loaded %d points, first one : (%.2lf,%.2lf)  (&=%p)\n", 
			pr->pointCount, pr->pointArray->x, pr->pointArray->y, pr->pointArray);
		printf("                       last one : (%.2lf,%.2lf)  (&=%p)\n",
			(pr->pointArray + pr->pointCount-1)->x, (pr->pointArray + pr->pointCount-1)->y,
			pr->pointArray + pr->pointCount-1);
	}
	return true;
}
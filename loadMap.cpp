#include "zuma.h"

#define JSON_MAX 10000
#define JSON_LINE_MAX 1000
#define STRING_BUFFER_SIZE 1000

void loadMap(MapInfo* pmi,char* folder,char* mapName) {
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
		longjmp(env, 1);

	do {
		isNEOF=fgets(mapJsonLine, JSON_LINE_MAX, fp);
		if (isNEOF) {
			strcat(jsonString, mapJsonLine);
			//printf("add:%s",mapJsonLine);
		}
	} while (isNEOF);

	//printf("ret:%s", mapJson);

	

	const cJSON* json = cJSON_Parse(jsonString);
	parseGameSettingsJson(pmi, json);
	parseMapPositionInfoJson(pmi, json, folder,mapName);
	parseResourceInfoJson(pmi, json, folder, mapName);
	//TODO:cJSON_Delete很迷，会在下一次创建cJSON的时候用到之前free的指针，导致多次free的bug
	
	

	return;
}

void parseGameSettingsJson(MapInfo* pmi, const cJSON* json) {
	cJSON* gameSettingsJson = cJSON_GetObjectItemCaseSensitive(json, "gameSettings");
	if (!gameSettingsJson)
		longjmp(env, 3);


	int* pInts[] = { &pmi->gs.shootingCD };
	char* nameOfInts[] = { "shootingCD" };
	for (int i = 0; i < 1; i++)
		parseJsonInt(gameSettingsJson, nameOfInts[i], pInts[i]);


	double* pDoubles[] = { &pmi->gs.ballR,&pmi->gs.moveSpeed,&pmi->gs.flySpeed };
	char* nameOfDoubles[] = { "ballR","moveSpeed","flySpeed" };
	for (int i = 0; i < 3; i++)
		parseJsonDouble(gameSettingsJson, nameOfDoubles[i], pDoubles[i]);
	
	
	//cJSON_Delete(gameSettingsJson);

	if (DEBUG_OUTPUT) {
		printf("\n[DEBUG_OUTPUT]parseGameSettingsJson():\n");
		for (int i = 0; i < 1; i++)
			printf("  Loaded int variable[%s]=%d  (&=%p)\n", nameOfInts[i], *pInts[i], pInts[i]);
		for (int i = 0; i < 3; i++)
			printf("  Loaded double variable[%s]=%.2lf  (&=%p)\n", nameOfDoubles[i], *pDoubles[i], pDoubles[i]);
	}
	return;
}


void parseMapPositionInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName) {
	cJSON* mapPositionInfoJson = cJSON_GetObjectItemCaseSensitive(json, "mapPositionInfo");
	cJSON* ballListsJson = cJSON_GetObjectItemCaseSensitive(mapPositionInfoJson, "ballLists");
	cJSON* zumaJson = cJSON_GetObjectItemCaseSensitive(mapPositionInfoJson, "zuma");
	if (!mapPositionInfoJson || !ballListsJson || !zumaJson)
		longjmp(env, 3);

	parseJsonInt(mapPositionInfoJson, "ballListCount", &pmi->mpi.ballListCount);
	pmi->pr = (Route*)malloc(sizeof(Route) * pmi->mpi.ballListCount);

	cJSON* ballListJson;
	int i = 0;
	cJSON_ArrayForEach(ballListJson, ballListsJson) {
		if (i > pmi->mpi.ballListCount - 1)
			longjmp(env, 0);//TODO:增加新的exception-过多的BallList
		parseBallListJson(pmi->pr + i, ballListJson, folder, mapName);
		i++;
	}

	Point* pPoints[] = { &pmi->mpi.zumaPosition,&pmi->mpi.deltaMouthPosition,&pmi->mpi.deltaHolePosition };
	char* nameOfPoints[] = { "zumaPosition","deltaMouthPosition","deltaHolePosition" };
	for (int i = 0; i < 3; i++)
		parseJsonPoint(zumaJson, nameOfPoints[i], pPoints[i]);
	
	if (DEBUG_OUTPUT) {
		printf("\n[DEBUG_OUTPUT]parseMapPositionInfoJson():\n");
		for (int i = 0; i < 3; i++)
			printf("  Loaded double variable[%s]=(%.2lf,%.2lf)  (&=%p)\n",
				nameOfPoints[i], pPoints[i]->x, pPoints[i]->y, pPoints[i]);
	}
	
	//cJSON_Delete(mapPositionInfoJson);
	//cJSON_Delete(routeInfoJson);
	//cJSON_Delete(zumaJson);
	return;
}



void parseBallListJson(Route* pr,const cJSON* json,char* folder,char* mapName) {

	parseJsonInt(json, "ballCount", &pr->ballCount);
	
	//TODO:适配其他路径储存方式的文件
	//计划是如果函数存储，在读取函数后，即刻将函数变为对应点

	cJSON* routeInfoJson = cJSON_GetObjectItemCaseSensitive(json, "routeInfo");
	int* pInts[] = { &pr->pointCount,&pr->pointStep };
	char* nameOfInts[] = { "pointCount","pointStep" };
	for (int i = 0; i < 2; i++)
		parseJsonInt(routeInfoJson, nameOfInts[i], pInts[i]);
	
	if (DEBUG_OUTPUT) {
		printf("  Loaded ballList:\n");
		printf("    Loaded int variable[%s]=%d  (&=%p)\n", "ballCount", pr->ballCount ,&pr->ballCount);
		for (int i = 0; i < 2; i++)
			printf("    Loaded int variable[%s]=%d  (&=%p)\n", nameOfInts[i], *pInts[i], pInts[i]);
	}

	char dirBuffer[STRING_BUFFER_SIZE];
	char dirBuffer2[STRING_BUFFER_SIZE];
	parseJsonString(routeInfoJson, "routeFileName", dirBuffer);
	sprintf(dirBuffer2, "%s\\%s\\%s", folder, mapName, dirBuffer);
	loadRouteFile(pr, dirBuffer2);

	return;
}

void parseResourceInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName) {
	cJSON* resourceInfoJson = cJSON_GetObjectItemCaseSensitive(json, "resourceInfo");
	cJSON* ballJson = NULL;
	char strBuffer[STRING_BUFFER_SIZE];
	char dirBuffer[STRING_BUFFER_SIZE];
	pmi->ri.background = new IMAGE[1];
	if (!pmi->ri.background)
		longjmp(env, 0);

	parseJsonString(resourceInfoJson, "backGround",strBuffer);


	sprintf(dirBuffer,"%s\\%s\\%s", folder, mapName,strBuffer);

	loadimage(pmi->ri.background, dirBuffer);

	cJSON* ballsJson = cJSON_GetObjectItemCaseSensitive(resourceInfoJson, "balls");
	if (!ballsJson)
		longjmp(env, 3);
	parseJsonInt(ballsJson, "colorCount", &pmi->ri.colorCount);
	pmi->ri.ballImgs = new IMAGE[pmi->ri.colorCount];
	pmi->ri.ballMaskImgs = new IMAGE[pmi->ri.colorCount];

	int i = 0;
	cJSON* ballsResourceJson = cJSON_GetObjectItemCaseSensitive(ballsJson, "resource");
	if (!ballsResourceJson)
		longjmp(env, 3);

	if (DEBUG_OUTPUT) {
		printf("\n[DEBUG_OUTPUT]parseResourceInfoJson():\n");
		printf("  loaded IMAGE(background)-size:%d*%d  (&=%p)\n",
			pmi->ri.background->getwidth(), pmi->ri.background->getheight(), pmi->ri.background);
		printf("  loaded colorCount:%d  (&=%p)\n", pmi->ri.colorCount,&pmi->ri.colorCount);
	}

	cJSON_ArrayForEach(ballJson, ballsResourceJson) {
		parseJsonString(ballJson, "img", strBuffer);
		sprintf(dirBuffer, "%s\\%s\\%s", folder, mapName, strBuffer);
		loadimage(pmi->ri.ballImgs+i, dirBuffer);
		parseJsonString(ballJson, "mask", strBuffer);
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
	//TODO:解析img
	//cJSON_Delete(resourceInfoJson);
	return;
}

void parseJsonString(const cJSON* json, char* name, char* str) {
	cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!stringJson||!cJSON_IsString(stringJson))
		longjmp(env, 3);
	strcpy(str, stringJson->valuestring);
	//str = stringJson->valuestring;
	//切记C语言中字符串不能直接=赋值，这个bug导致后面的IMAGE*被sprintf的一堆“烫”覆盖了

	//顺便我还是想吐槽C语言的字符串处理，不能用+将字符串相连，赋值要用单独一行函数
	//像python那样有=有+，或者向java那样有成员函数都可以啊
	//当然我也知道这有时代局限性的原因，C是比较接近底层的，可还是感觉这也太麻烦了
	//简简单单解析一个json就费了一百多行，简直了
	//还有就是异常处理，一处处地写，try-catch简直不要太好用...
	//cJSON_Delete(stringJson);
	return;
}

void parseJsonPoint(const cJSON* json, char* name, Point* pPoint) {
	cJSON* pointJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!pointJson)
		longjmp(env, 3);
	cJSON* pointJsonX = cJSON_GetObjectItemCaseSensitive(pointJson, "x");
	cJSON* pointJsonY = cJSON_GetObjectItemCaseSensitive(pointJson, "y");
	if (!pointJsonX || !pointJsonY || !cJSON_IsNumber(pointJsonX) || !cJSON_IsNumber(pointJsonY))
		longjmp(env, 3);
	*pPoint = makePoint(pointJsonX->valuedouble, pointJsonY->valuedouble);
	//cJSON_Delete(pointJson);
	//cJSON_Delete(pointJsonX);
	//cJSON_Delete(pointJsonY);
	return;
}

void parseJsonInt(const cJSON* json,char* name, int* pInt) {
	cJSON* intJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!intJson||!cJSON_IsNumber(intJson))
		longjmp(env, 3);
	*pInt = intJson->valueint;
	//cJSON_Delete(intJson);
	return;
}

void parseJsonDouble(const cJSON* json, char* name, double* pDouble) {
	cJSON* doubleJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!doubleJson || !cJSON_IsNumber(doubleJson))
		longjmp(env, 3);
	*pDouble = doubleJson->valuedouble;
	//cJSON_Delete(doubleJson);
	return;
}

void loadRouteFile(Route* pr,char* dir) {
	FILE* fp = fopen(dir, "r");
	if (!fp)
		longjmp(env, 1);
	pr->pointArray = (Point*)malloc(sizeof(Point) * pr->pointCount);
	if (!pr->pointArray)
		longjmp(env, 5);
	for (int i = 0; i < pr->pointCount; i++)
		if (fscanf(fp, "%lf %lf", &(pr->pointArray + i)->x, &(pr->pointArray + i)->y)!=2)
			longjmp(env,2);
	if (DEBUG_OUTPUT) {
		printf("  loadRouteFile():\n");
		printf("    loaded %d points, first one : (%.2lf,%.2lf)  (&=%p)\n", 
			pr->pointCount, pr->pointArray->x, pr->pointArray->y, pr->pointArray);
		printf("                       last one : (%.2lf,%.2lf)  (&=%p)\n",
			(pr->pointArray + pr->pointCount-1)->x, (pr->pointArray + pr->pointCount-1)->y,
			pr->pointArray + pr->pointCount-1);
	}
	return;
}
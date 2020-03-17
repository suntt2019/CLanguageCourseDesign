#include "zuma.h"

#define JSON_MAX 10000
#define JSON_LINE_MAX 1000
#define STRING_BUFFER_SIZE 1000

void loadMap(MapInfo* pmi,char* folder,char* mapName) {
	char jsonString[JSON_MAX] = "";
	char mapJsonLine[JSON_LINE_MAX] = { '\0' };
	char dirBuffer[STRING_BUFFER_SIZE];
	char* isNEOF;//δ�����ļ�β��

	pmi = (MapInfo*)malloc(sizeof(MapInfo));
	if (!pmi)
		longjmp(env, 0);//TODO:�ḻ�쳣ϵͳ�Ĵ�����ʾ

	printf("%p,%p\n", pmi, pmi->ri.background);

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
	parseMapPositionInfoJson(pmi, json);
	parseResourceInfoJson(pmi, json, folder, mapName);
	//TODO:cJSON_Delete���ԣ�������һ�δ���cJSON��ʱ���õ�֮ǰfree��ָ�룬���¶��free��bug

	return;
}

void parseGameSettingsJson(MapInfo* pmi, const cJSON* json) {
	cJSON* gameSettingsJson = cJSON_GetObjectItemCaseSensitive(json, "gameSettings");
	if (!gameSettingsJson)
		longjmp(env, 3);


	int* pInts[] = { &pmi->gs.ballCount,&pmi->gs.shootingCD,&pmi->gs.ballListCount };
	char* nameOfInts[] = { "ballCount","shootingCD","ballListCount" };
	for (int i = 0; i < 3; i++)
		parseJsonInt(gameSettingsJson, nameOfInts[i], pInts[i]);


	double* pDoubles[] = { &pmi->gs.ballR,&pmi->gs.moveSpeed,&pmi->gs.flySpeed };
	char* nameOfDoubles[] = { "ballR","moveSpeed","flySpeed" };
	for (int i = 0; i < 3; i++)
		parseJsonDouble(gameSettingsJson, nameOfDoubles[i], pDoubles[i]);
	
	
	//cJSON_Delete(gameSettingsJson);

	if (DEBUG_OUTPUT) {
		printf("parseGameSettingsJson():\n");
		for (int i = 0; i < 3; i++)
			printf("  Loaded int variable[%s]=%d  (&=%p)\n", nameOfInts[i], *pInts[i], pInts[i]);
		for (int i = 0; i < 3; i++)
			printf("  Loaded double variable[%s]=%.2lf  (&=%p)\n", nameOfDoubles[i], *pDoubles[i], pDoubles[i]);
	}
	return;
}


void parseMapPositionInfoJson(MapInfo* pmi, const cJSON* json) {
	cJSON* mapPositionInfoJson = cJSON_GetObjectItemCaseSensitive(json, "mapPositionInfo");
	cJSON* routeInfoJson = cJSON_GetObjectItemCaseSensitive(mapPositionInfoJson, "routeInfo");
	cJSON* zumaJson = cJSON_GetObjectItemCaseSensitive(mapPositionInfoJson, "zuma");
	if (!mapPositionInfoJson || !routeInfoJson || !zumaJson)
		longjmp(env, 3);

	int* pInts[] = { &pmi->r.pointCount,&pmi->r.pointStep };
	char* nameOfInts[] = { "pointCount","pointStep" };
	for (int i = 0; i < 2; i++)
		parseJsonInt(routeInfoJson, nameOfInts[i], pInts[i]);
	
	char routeFileName[STRING_BUFFER_SIZE];
	parseJsonString(routeInfoJson, "routeFileName", routeFileName);
	//TODO:loadRouteFile(routeFileName);

	
	Point* pPoints[] = { &pmi->mpi.zumaPosition,&pmi->mpi.deltaMouthPosition,&pmi->mpi.deltaHolePosition };
	char* nameOfPoints[] = { "zumaPosition","deltaMouthPosition","deltaHolePosition" };
	for (int i = 0; i < 3; i++)
		parseJsonPoint(zumaJson, nameOfPoints[i], pPoints[i]);
	
	if (DEBUG_OUTPUT) {
		printf("parseMapPositionInfoJson():\n");
		for (int i = 0; i < 2; i++)
			printf("  Loaded int variable[%s]=%d  (&=%p)\n", nameOfInts[i], *pInts[i], pInts[i]);
		for (int i = 0; i < 3; i++)
			printf("  Loaded double variable[%s]=(%.2lf,%.2lf)  (&=%p)\n",
				nameOfPoints[i], pPoints[i]->x, pPoints[i]->y, pPoints[i]);
	}
	
	//cJSON_Delete(mapPositionInfoJson);
	//cJSON_Delete(routeInfoJson);
	//cJSON_Delete(zumaJson);
	return;
}

void parseResourceInfoJson(MapInfo* pmi, const cJSON* json, char* folder, char* mapName) {
	cJSON* resourceInfoJson = cJSON_GetObjectItemCaseSensitive(json, "resourceInfo");
	cJSON* ball = NULL;
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

	cJSON_ArrayForEach(ball, ballsResourceJson) {
		parseJsonString(ball, "img", strBuffer);
		sprintf(dirBuffer, "%s\\%s\\%s", folder, mapName, strBuffer);
		loadimage(pmi->ri.ballImgs+i, dirBuffer);
		parseJsonString(ball, "mask", strBuffer);
		sprintf(dirBuffer, "%s\\%s\\%s", folder, mapName, strBuffer);
		loadimage(pmi->ri.ballMaskImgs+i, dirBuffer);
		i++;
	}
	//TODO:DEBUG_OUTPUT
	//TODO:����img
	//cJSON_Delete(resourceInfoJson);
	return;
}

void parseJsonString(const cJSON* json, char* name, char* str) {
	cJSON* stringJson = cJSON_GetObjectItemCaseSensitive(json, name);
	if (!stringJson||!cJSON_IsString(stringJson))
		longjmp(env, 3);
	strcpy(str, stringJson->valuestring);
	//str = stringJson->valuestring;
	//�м�C�������ַ�������ֱ��=��ֵ�����bug���º����IMAGE*��sprintf��һ�ѡ��̡�������

	//˳���һ������²�C���Ե��ַ�������������+���ַ�����������ֵҪ�õ���һ�к���
	//��python������=��+��������java�����г�Ա���������԰�
	//��Ȼ��Ҳ֪������ʱ�������Ե�ԭ��C�ǱȽϽӽ��ײ�ģ��ɻ��Ǹо���Ҳ̫�鷳��
	//��򵥵�����һ��json�ͷ���һ�ٶ��У���ֱ��
	//���о����쳣����һ������д��try-catch��ֱ��Ҫ̫����...
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

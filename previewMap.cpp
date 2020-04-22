#include "zuma.h"

void previewMaps(MapPreviewArray** ppmpa) {
	*ppmpa = (MapPreviewArray*)malloc(sizeof(MapPreviewArray));
    
    (*ppmpa)->cnt = 0;
    
    char dir[MAX_PATH]= "maps//*";
    WIN32_FIND_DATA folderData;
    HANDLE folderHandle = ::FindFirstFile(dir, &folderData);
    if (folderHandle == INVALID_HANDLE_VALUE)
        handleException(13);
    do {
        if (folderData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY
            && strcmp(folderData.cFileName, ".") != 0 && strcmp(folderData.cFileName, "..") != 0) {
            (*ppmpa)->cnt++;
        }
    } while (FindNextFile(folderHandle, &folderData));
    FindClose(folderHandle);

    (*ppmpa)->mpa = (MapPreview*)malloc(sizeof(MapPreview) * (*ppmpa)->cnt);
    int i = 0;
    folderHandle = ::FindFirstFile(dir, &folderData);
    if (folderHandle == INVALID_HANDLE_VALUE)
        handleException(13);
    do{
        if (folderData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY 
            && strcmp(folderData.cFileName,".")!=0 && strcmp(folderData.cFileName, "..")!=0){
            if(!previewSingleMap(folderData.cFileName, (*ppmpa)->mpa+i))
                continue;
            i++;
        }
    }while (FindNextFile(folderHandle, &folderData));
    (*ppmpa)->cnt = i;
    FindClose(folderHandle);
}

bool previewSingleMap(char* name,MapPreview* pmp) {
	char dirBuf[STRING_BUFFER_SIZE];
    char jsonString[JSON_MAX] = "";
    char mapJsonLine[JSON_LINE_MAX] = { '\0' };
    char* isNEOF;


    strcpy(pmp->dir, name);
    FILE* fp;
    sprintf(dirBuf, "%s\\%s\\%s.json", "maps", name, name);
    fp = fopen(dirBuf, "r");
    if (!fp)
        return false;
    do {
        isNEOF = fgets(mapJsonLine, JSON_LINE_MAX, fp);
        if (isNEOF) {
            strcat(jsonString, mapJsonLine);
        }
    } while (isNEOF);
    const cJSON* json = cJSON_Parse(jsonString);
    if(!parseJsonString(json, "name", pmp->name))
        return false;
    MapInfo mi;
    if (!loadMap(&mi, "maps", name))
        return false;
    return true;
}
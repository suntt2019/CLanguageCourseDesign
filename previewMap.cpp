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
            previewSingleMap(folderData.cFileName, (*ppmpa)->mpa+i);
            i++;
        }
    }while (FindNextFile(folderHandle, &folderData));
    FindClose(folderHandle);
}

void previewSingleMap(char* name,MapPreview* pmp) {
	char srtBuf[STRING_BUFFER_SIZE];
    strcpy(pmp->dir, name);
    return;
}
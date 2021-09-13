// MoveExportTable.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <malloc.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "goble.h"




int main(int argc, char* argv[])
{
	
	LPVOID peFileBuffer = NULL;
	LPVOID pNewbuff = NULL;
	LPVOID pImageBuffer =NULL;
	LPVOID pNewImageBuff = NULL;
	LPVOID pNewFileBuffer = NULL;
	char path[] = "c://project/testDll.dll";	
	
	char outpath[] = "c://project/testDllB.DLL";
	LPSTR FILEPATH = path;
	LPSTR FilePath_Out = outpath;

	
	//读取file文件读取
	ReadPeFile(FILEPATH,&peFileBuffer);

	if (!peFileBuffer)
	{
		printf("读取失败");
		return 0;
	}
	

	//第一步：在DLL中新增一个节，并返回新增后的FOA


	MoveExprotTable(peFileBuffer,&pNewbuff);

	 

	getchar();





	return 0;
}


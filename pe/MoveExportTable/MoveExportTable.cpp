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

	
	//��ȡfile�ļ���ȡ
	ReadPeFile(FILEPATH,&peFileBuffer);

	if (!peFileBuffer)
	{
		printf("��ȡʧ��");
		return 0;
	}
	

	//��һ������DLL������һ���ڣ��������������FOA


	MoveExprotTable(peFileBuffer,&pNewbuff);

	 

	getchar();





	return 0;
}


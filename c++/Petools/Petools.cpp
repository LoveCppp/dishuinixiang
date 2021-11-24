// Petools.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "MyPeTools.h"




int main(int argc, char* argv[])
{
	
	LPVOID peFileBuffer = NULL;
	LPVOID pNewbuff = NULL;
	LPVOID pImageBuffer =NULL;
	LPVOID pNewImageBuff = NULL;
	LPVOID pNewFileBuffer = NULL;
	char path[] = "C://project/ipmsg.exe";	


	LPSTR FILEPATH = path;
	//读取file文件读取
	ReadPeFile(FILEPATH,&peFileBuffer);

	readPeResoure(peFileBuffer);
	

	
	getchar();


	return 0;
}


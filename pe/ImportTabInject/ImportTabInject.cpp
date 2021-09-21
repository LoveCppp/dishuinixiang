// ImportTabInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "global.h"

int main(int argc, char* argv[])
{

	LPVOID peFileBuffer = NULL;	
	char path[] = "c://project/ipmsg.exe";	
	LPSTR FILEPATH = path;
	//读取file文件读取
	ReadPeFile(FILEPATH,&peFileBuffer);
	
	if (!peFileBuffer)
	{
		printf("读取失败");
		return 0;
	}

	InjectDll(peFileBuffer);


	getchar();
	return 1;


}



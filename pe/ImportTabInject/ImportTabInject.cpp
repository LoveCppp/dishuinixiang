// ImportTabInject.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "global.h"

int main(int argc, char* argv[])
{

	LPVOID peFileBuffer = NULL;	
	char path[] = "c://project/ipmsg.exe";	
	LPSTR FILEPATH = path;
	//��ȡfile�ļ���ȡ
	ReadPeFile(FILEPATH,&peFileBuffer);
	
	if (!peFileBuffer)
	{
		printf("��ȡʧ��");
		return 0;
	}

	InjectDll(peFileBuffer);


	getchar();
	return 1;


}



// addcode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "globe.h"


void function1(){
	MessageBoxA(NULL,"test","test",0);
}



void main(int argc, char* argv[])
{
	


	LPVOID peFileBuffer = NULL;
	LPVOID pImageBuffer = NULL;
	LPVOID pNewImageBuff = NULL;
	LPVOID pNewFileBuffer = NULL;
	char path[] = "c://project/notepad.exe";	
	
	char outpath[] = "c://project/notepad3.exe";
	LPSTR FILEPATH = path;
	LPSTR FilePath_Out = outpath;
	
	//��ȡfile�ļ�
	ReadPeFile(FILEPATH,&peFileBuffer);
	//���Ƶ��ڴ�
	int ImageSize=	CopyFileBufferToImageBuffer(peFileBuffer,&pImageBuffer);
	//������ ��������CopyFileBufferToImageBuffer ������ֱ��������Ϊ�˺�ʶ�������¼�һ��������ʵ�ָù���
	
	

	//�ϲ���
	//Merge_Sec(pImageBuffer,&pNewImageBuff);
	
	//������
	//Add_SectionInNewSec(pImageBuffer, &pNewImageBuff);
	//����һ����
	//Add_SectionInNewSecExt(pImageBuffer, &pNewImageBuff);
	//����shell code
	//Add_SectionCode(pNewImageBuff);
	//int newFileSize= CopyImageBufferToNewFileBuffer(pNewImageBuff,&pNewFileBuffer);


	//����Ŀ¼
	PrintDriectory(pImageBuffer);

	//�����µ��ļ�
	//NewFileBufferToFile(pNewFileBuffer, newFileSize, FilePath_Out); 

	



	getchar();




}

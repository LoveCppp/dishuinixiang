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
	char path[] = "c://project/testDll.dll";	
	
	char outpath[] = "c://project/testNew.dll";
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
	Add_SectionInNewSec(pImageBuffer, &pNewImageBuff);
	//����һ����

	//LPVOID pNewImageBuff2 = NULL;
	//Add_SectionInNewSecExt(pNewImageBuff, &pNewImageBuff2);
	//����shell code
	//Add_SectionCode(pNewImageBuff);
	int newFileSize= CopyImageBufferToNewFileBuffer(pNewImageBuff,&pNewFileBuffer);

	
	//����Ŀ¼
	//PrintDriectory(pImageBuffer);

	//��ӡ������

	//PrintExport(peFileBuffer);
	//����
	NewFileBufferToFile(pNewFileBuffer, newFileSize, FilePath_Out); 
	//char* FuncName = "Div";

	//GetFunctionAddrByName(peFileBuffer, FuncName);
	
	//DWORD FunctionOrdinals = 0xC;
	//GetFunctionAddrByOrdinals(peFileBuffer,FunctionOrdinals);


	//��ӡ�ض�λ��

	//PrintRelocation(peFileBuffer);

	
	//��һ������DLL������һ���ڣ��������������FOA
	



	getchar();



}

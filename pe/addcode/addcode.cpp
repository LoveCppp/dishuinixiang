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
	
	//读取file文件
	ReadPeFile(FILEPATH,&peFileBuffer);
	//复制到内存
	int ImageSize=	CopyFileBufferToImageBuffer(peFileBuffer,&pImageBuffer);
	//新增节 、可以在CopyFileBufferToImageBuffer 函数中直接新增，为了好识别所以新加一个函数来实现该功能
	
	//合并节
	//Merge_Sec(pImageBuffer,&pNewImageBuff);
	
	//新增节
	Add_SectionInNewSec(pImageBuffer, &pNewImageBuff);
	//扩大一个节

	//LPVOID pNewImageBuff2 = NULL;
	//Add_SectionInNewSecExt(pNewImageBuff, &pNewImageBuff2);
	//新增shell code
	//Add_SectionCode(pNewImageBuff);
	int newFileSize= CopyImageBufferToNewFileBuffer(pNewImageBuff,&pNewFileBuffer);

	
	//数据目录
	//PrintDriectory(pImageBuffer);

	//打印导出表

	//PrintExport(peFileBuffer);
	//存盘
	NewFileBufferToFile(pNewFileBuffer, newFileSize, FilePath_Out); 
	//char* FuncName = "Div";

	//GetFunctionAddrByName(peFileBuffer, FuncName);
	
	//DWORD FunctionOrdinals = 0xC;
	//GetFunctionAddrByOrdinals(peFileBuffer,FunctionOrdinals);


	//打印重定位表

	//PrintRelocation(peFileBuffer);

	
	//第一步：在DLL中新增一个节，并返回新增后的FOA
	



	getchar();



}

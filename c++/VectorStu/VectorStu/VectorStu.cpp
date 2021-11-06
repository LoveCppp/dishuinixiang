// VectorStu.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "myclass.h"



void TestVector() {
    Vector<int>* pvector = new Vector<int>(5);
    pvector->push_back(1);
    pvector->push_back(2);
    pvector->push_back(3);
    pvector->push_back(4);
    pvector->push_back(5);
    
    pvector->insert(3,9);



}




int main()

{
    TestVector();
    std::cout << "Hello World!\n";
}


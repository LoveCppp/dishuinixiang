#pragma once

#include <stdio.h>
#include <windows.h>




#define SUCCESS           			 1 // �ɹ�								
#define ERROR            			 -1 // ʧ��								
#define MALLOC_ERROR			 -2 // �����ڴ�ʧ��								
#define INDEX_ERROR		 	 -3 // �����������								


template <class T_ELE>
class Vector
{
public:
	Vector();
	Vector(DWORD dwSize);
	~Vector();
public:
	DWORD	at(DWORD dwIndex, OUT T_ELE* pEle);					//���ݸ����������õ�Ԫ��				
	DWORD    push_back(T_ELE Element);						//��Ԫ�ش洢���������һ��λ��				
	VOID	pop_back();					//ɾ�����һ��Ԫ��				
	DWORD	insert(DWORD dwIndex, T_ELE Element);					//��ָ��λ������һ��Ԫ��				
	DWORD	capacity();					//�����ڲ����ݵ�����£����ܴ洢����Ԫ��				
	VOID	clear();					//�������Ԫ��				
	BOOL	empty();					//�ж�Vector�Ƿ�Ϊ�� ����trueʱΪ��				
	VOID	erase(DWORD dwIndex);					//ɾ��ָ��Ԫ��				
	DWORD	size();					//����VectorԪ�������Ĵ�С				
private:
	BOOL	expand();
private:
	DWORD  m_dwIndex;						//��һ����������				
	DWORD  m_dwIncrement;						//ÿ�����ݵĴ�С				
	DWORD  m_dwLen;						//��ǰ�����ĳ���				
	DWORD  m_dwInitSize;						//Ĭ�ϳ�ʼ����С				
	T_ELE* m_pVector;						//����ָ��				
};

template <class T_ELE>
Vector<T_ELE>::Vector()
	:m_dwInitSize(100), m_dwIncrement(5)
{
	//1.��������Ϊm_dwInitSize��T_ELE����										
	m_pVector = new T_ELE[m_dwInitSize];
	//2.���´����Ŀռ��ʼ��										
	memset(m_pVector, 0, sizeof(T_ELE)* m_dwInitSize);
	//3.��������ֵ	
	m_dwLen = 100;
	m_dwIndex = 0;
}	
template <class T_ELE>
Vector<T_ELE>::Vector(DWORD dwSize)
	:m_dwIncrement(5)
{
	//1.��������ΪdwSize��T_ELE����
	m_pVector = new T_ELE[dwSize];

	//2.���´����Ŀռ��ʼ��	
	memset(m_pVector, 0, sizeof(T_ELE) * dwSize);

	//3.��������ֵ
	m_dwLen = dwSize;
	m_dwIndex = 0;
}											
template <class T_ELE>
Vector<T_ELE>::~Vector()
{

	//�ͷſռ� delete[]		
	delete[] m_pVector;

}

template <class T_ELE>
BOOL Vector<T_ELE>::expand()
{
	DWORD DwTemp=0;
	T_ELE* Temp_Vector;						//��ʱ����ָ��
	// 1. �������Ӻ�ĳ���										
	DwTemp = m_dwLen + m_dwIncrement;
	// 2. ����ռ�										
	Temp_Vector = new T_ELE[DwTemp];
	// 3. �����ݸ��Ƶ��µĿռ�										
	memcpy(Temp_Vector, m_pVector, sizeof(T_ELE)* DwTemp);
	// 4. �ͷ�ԭ���ռ�										
	delete[] m_pVector;
	m_pVector = Temp_Vector;
	// 5. Ϊ�������Ը�ֵ
	Temp_Vector = NULL;
	m_dwLen = DwTemp;
	return SUCCESS;

}

template <class T_ELE>
DWORD  Vector<T_ELE>::push_back(T_ELE Element)
{
	//1.�ж��Ƿ���Ҫ���ݣ������Ҫ�͵������ݵĺ���										
	if (m_dwIndex >= m_dwLen)
	{
		expand();
	}
	//2.���µ�Ԫ�ظ��Ƶ����������һ��λ��	
	memcpy(&m_pVector[m_dwIndex],&Element,sizeof(Element));
	//3.�޸�����ֵ		
	m_dwIndex++;
	return SUCCESS;
}

template <class T_ELE>
DWORD  Vector<T_ELE>::insert(DWORD dwIndex, T_ELE Element)
{

	//1.�ж������Ƿ��ں�������	

	if (dwIndex<0 || dwIndex>m_dwLen)
	{
		return ERROR;
	}

	//2.�ж��Ƿ���Ҫ���ݣ������Ҫ�͵������ݵĺ���										
	if (dwIndex>=m_dwLen)
	{
		expand();
	}
					
	//3.��dwIndexֻ���Ԫ�غ���	
	for (int i = m_dwIndex; i > dwIndex; i--)
	{
		memcpy(&m_pVector[i], &m_pVector[i-1],sizeof(T_ELE));
	}
	//4.��ElementԪ�ظ��Ƶ�dwIndexλ��										
	memcpy(&m_pVector[dwIndex], &Element, sizeof(T_ELE));
	//5.�޸�����ֵ		
	m_dwIndex++;
}
template <class T_ELE>
DWORD Vector<T_ELE>::at(DWORD dwIndex, T_ELE* pEle)
{
	//�ж������Ƿ��ں�������										
	if (dwIndex<0 || dwIndex>m_dwLen)
	{
		return ERROR;
	}
	//��dwIndex��ֵ���Ƶ�pEleָ�����ڴ�										
	memcpy(pEle, &m_pVector[dwIndex], sizeof(pEle));
}

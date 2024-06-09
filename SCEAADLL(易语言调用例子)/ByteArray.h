#pragma once
#include<windows.h>
#include<vector>
#include<initializer_list>
#include<iostream>
#include<algorithm>
#include<string>

using namespace std;
class ByteArray {
	vector<BYTE> Data;
public:
	ByteArray() :Data() {};
	ByteArray(size_t size) : Data(size) {};//����ֱ����һ��������ʼ����Ȼ�����ɶ�Ӧ��С������
	ByteArray(std::initializer_list<BYTE>list) :Data(list) {};
	friend ostream& operator<<(ostream& os, const ByteArray& obj);
	BYTE& operator[](size_t other);
	ByteArray operator+(const ByteArray& other);
	ByteArray& operator+=(const ByteArray& other);
	//ȡ�ֽڼ�����
	size_t GetLen();
	//ȡ�ֽڼ����
	ByteArray GetLeft(size_t size);
	//ȡ�ֽڼ��ұ�
	ByteArray GetRight(size_t size);
	//ȡ�ֽڼ��м�(��ʼλ��,��С)
	ByteArray GetMid(size_t first, size_t size);
	//Ѱ���ֽڼ� �ɹ�����λ�� ʧ�ܷ���-1 first��ʾ��ʼ����λ�� Ĭ��Ϊ0
	int Find(ByteArray obj,size_t first = 0);
	//�����ֽڼ� �ɹ�����λ�� ʧ�ܷ���-1
	int FindEnd(ByteArray obj);
	//��ָ��λ���滻��ָ�����ֽڼ����滻�ĳ��Ⱦ��Ǵ�����ֽڼ����� 
	void Replace(size_t first, ByteArray obj);
	//�����ֵ�ָ���ֽڼ��滻��ָ���ֽڼ�,���RpObj��findObj������ô���RpObj�����������ȥ���滻����ΪfindObj
	void Replace_Find(ByteArray findObj, ByteArray RpObj);
	//��ָ��λ�ò���ָ�����ȵ��ֽڼ� ����λ�����Ϊ-1����뵽ĩβ ValueΪ����Ĭ��ֵ
	void insert(size_t first, size_t size, BYTE Value);
	//��ָ��λ�ò���ָ�����ֽڼ�����
	void insert(size_t first, ByteArray obj);
	//ȡ��ָ���ֽڼ���ߵ��ֽڼ� ʧ�ܷ��ؿն���
	ByteArray GetLeft_Find(ByteArray obj);
	//ȡ��ָ���ֽڼ��ұߵ��ֽڼ� ʧ�ܷ��ؿն���
	ByteArray GetRight_Find(ByteArray obj);
	//ȡ��ָ�������ֽڼ��м���ֽڼ� ʧ�ܷ��ؿն���
	ByteArray GetMid_Find(ByteArray LObj, ByteArray RObj);
	//�ָ��ֽڼ������������ָ���ֽڼ��������ֽڼ����� 
	vector<ByteArray> splitByteArray(ByteArray obj);
	//����ֽڼ�
	void Clear();
	short ToShort();
	int ToInt();
	long ToLong();
	float ToFloat();
	double ToDouble();
	//�����ֽڼ�����\0�ضϵ�ǰ������ݹ��ɵ��ַ���������ֽڼ���û\0������Զ���ĩβ����
	string ToString();
	//�����ֽ����ݵ�ԭʼ��ַ
	BYTE* GetDataAddress();
};
//��������ת�����ֽ�����
ByteArray ToByteArray(__int16 num);
ByteArray ToByteArray(__int32 num);
ByteArray ToByteArray(__int64 num);
ByteArray ToByteArray(const string &str);
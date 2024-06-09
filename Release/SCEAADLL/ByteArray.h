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
	ByteArray(size_t size) : Data(size) {};//可以直接用一个整数初始化，然后生成对应大小的数组
	ByteArray(std::initializer_list<BYTE>list) :Data(list) {};
	friend ostream& operator<<(ostream& os, const ByteArray& obj);
	BYTE& operator[](size_t other);
	ByteArray operator+(const ByteArray& other);
	ByteArray& operator+=(const ByteArray& other);
	//取字节集长度
	size_t GetLen();
	//取字节集左边
	ByteArray GetLeft(size_t size);
	//取字节集右边
	ByteArray GetRight(size_t size);
	//取字节集中间(起始位置,大小)
	ByteArray GetMid(size_t first, size_t size);
	//寻找字节集 成功返回位置 失败返回-1 first表示起始搜索位置 默认为0
	int Find(ByteArray obj,size_t first = 0);
	//倒找字节集 成功返回位置 失败返回-1
	int FindEnd(ByteArray obj);
	//在指定位置替换成指定的字节集，替换的长度就是传入的字节集长度 
	void Replace(size_t first, ByteArray obj);
	//将出现的指定字节集替换成指定字节集,如果RpObj比findObj长，那么会把RpObj整个都加入进去，替换长度为findObj
	void Replace_Find(ByteArray findObj, ByteArray RpObj);
	//在指定位置插入指定长度的字节集 插入位置如果为-1则插入到末尾 Value为数据默认值
	void insert(size_t first, size_t size, BYTE Value);
	//在指定位置插入指定的字节集对象，
	void insert(size_t first, ByteArray obj);
	//取出指定字节集左边的字节集 失败返回空对象
	ByteArray GetLeft_Find(ByteArray obj);
	//取出指定字节集右边的字节集 失败返回空对象
	ByteArray GetRight_Find(ByteArray obj);
	//取出指定两边字节集中间的字节集 失败返回空对象
	ByteArray GetMid_Find(ByteArray LObj, ByteArray RObj);
	//分割字节集，传入用作分割的字节集，返回字节集数组 
	vector<ByteArray> splitByteArray(ByteArray obj);
	//清空字节集
	void Clear();
	short ToShort();
	int ToInt();
	long ToLong();
	float ToFloat();
	double ToDouble();
	//返回字节集中以\0截断的前面的数据构成的字符串，如果字节集中没\0，则会自动在末尾补充
	string ToString();
	//返回字节数据的原始地址
	BYTE* GetDataAddress();
};
//其他数字转换成字节数组
ByteArray ToByteArray(__int16 num);
ByteArray ToByteArray(__int32 num);
ByteArray ToByteArray(__int64 num);
ByteArray ToByteArray(const string &str);
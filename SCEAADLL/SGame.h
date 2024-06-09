#pragma once
#include"SGame.h"
#include"ByteArray.h"
#include"SCEAA.h"
#include<windows.h>
#include<iostream>
#include"ByteArray.h"
#include<TlHelp32.h>
#include<string>
#include <sstream>
#include<map>
#include<cstdarg>
#include<regex>
#include <stack>

#define BLOCKMAXSIZE 409600//每次读取内存的最大大小

void charTowchar(const char* chr, wchar_t* wchar, int size);
void wcharTochar(const wchar_t* wchar, char* chr, int length);
//读内存  最后一个参数传入什么类型的数据，就返回什么类型
BYTE ReadMemory(DWORD PID, LPCVOID address, BYTE FNumber);
//这个特殊，这个一般用来读地址
int ReadMemory_int(DWORD PID, LPCVOID address, int FNumber = -1);
int ReadMemory(DWORD PID, LPCVOID address, int FNumber);
long ReadMemory(DWORD PID, LPCVOID address, long FNumber);
float ReadMemory(DWORD PID, LPCVOID address, float FNumber);
double ReadMemory(DWORD PID, LPCVOID address, double FNumber);
BOOL ReadMemory(DWORD PID, LPCVOID address, void* Buffer, size_t size);
ByteArray ReadMemory_ByteArray(DWORD PID, LPCVOID address, size_t size);

//写内存
BOOL WriteMemory(DWORD PID, LPVOID address, BYTE data);
BOOL WriteMemory(DWORD PID, LPVOID address, int data);
BOOL WriteMemory(DWORD PID, LPVOID address, long data);
BOOL WriteMemory(DWORD PID, LPVOID address, float data);
BOOL WriteMemory(DWORD PID, LPVOID address, double data);
BOOL WriteMemory(DWORD PID, LPVOID address, void* Buffer,size_t size);
BOOL WriteMemory(DWORD PID, LPVOID address, ByteArray data);

//通过进程名取进程pid 返回-1表示获取失败
DWORD GetProcessID(const string& name);
//提升进程权限到debug -1表示当前进程 默认为-1
BOOL EnableDebugPrivileges(DWORD PID = -1);
//删首空 返回一个删除前面空的字符串
string DelFroNull(string str);
//删首尾空 
string DelFroEndNull(string str);
//使用指定的字符分割字符串
vector<string> splitString(const string& str, char delimiter);
//判断字符串是否为纯数字
bool isNumber(const string &str);
//string转整数型 isHex字符串是否为十六进制，默认为假
int stringToInt(const string& str, bool isHex = false);
//通过表达式读内存地址 [["模块地址"+偏移]+1]
void* ReadMemoryEX(DWORD PID, string str);
//获取指定进程的模块地址
uintptr_t GetProcessModuleBaseAddress(DWORD PID, const string& name);
//指定进程申请内存
LPVOID allocProcess(DWORD PID,size_t asize);
//指定进程释放内存
void freeProcess(DWORD PID, LPVOID address);
//取指定进程函数地址
void* GetProFunAddress(DWORD PID, const string& dllName, const string& funName);
//取指定进程模块大小
size_t GetModuleSize(DWORD PID, const string& dllName);

//特征码搜索
//特征码转字节集
WORD GetTzmArray(const char* Tzm, WORD* TzmArray);
//获取Next数组
void GetNext(short* next, WORD* Tzm, WORD TzmLength);
//搜索一块内存
void SearchMemoryBlock(HANDLE hProcess, WORD* Tzm, WORD TzmLength, unsigned __int64 StartAddress, unsigned long size, vector<unsigned __int64>& ResultArray);
//搜索整个程序
int SearchMemory(DWORD PID, string Tzm, unsigned __int64 StartAddress, unsigned __int64 EndAddress, int InitSize, vector<unsigned __int64>& ResultArray);

//远程创建线程
HANDLE CreateProThread(DWORD PID, void* address);
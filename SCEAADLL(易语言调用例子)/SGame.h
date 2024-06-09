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

#define BLOCKMAXSIZE 409600//ÿ�ζ�ȡ�ڴ������С

void charTowchar(const char* chr, wchar_t* wchar, int size);
void wcharTochar(const wchar_t* wchar, char* chr, int length);
//���ڴ�  ���һ����������ʲô���͵����ݣ��ͷ���ʲô����
BYTE ReadMemory(DWORD PID, LPCVOID address, BYTE FNumber);
//������⣬���һ����������ַ
int ReadMemory_int(DWORD PID, LPCVOID address, int FNumber = -1);
int ReadMemory(DWORD PID, LPCVOID address, int FNumber);
long ReadMemory(DWORD PID, LPCVOID address, long FNumber);
float ReadMemory(DWORD PID, LPCVOID address, float FNumber);
double ReadMemory(DWORD PID, LPCVOID address, double FNumber);
BOOL ReadMemory(DWORD PID, LPCVOID address, void* Buffer, size_t size);
ByteArray ReadMemory_ByteArray(DWORD PID, LPCVOID address, size_t size);

//д�ڴ�
BOOL WriteMemory(DWORD PID, LPVOID address, BYTE data);
BOOL WriteMemory(DWORD PID, LPVOID address, int data);
BOOL WriteMemory(DWORD PID, LPVOID address, long data);
BOOL WriteMemory(DWORD PID, LPVOID address, float data);
BOOL WriteMemory(DWORD PID, LPVOID address, double data);
BOOL WriteMemory(DWORD PID, LPVOID address, void* Buffer,size_t size);
BOOL WriteMemory(DWORD PID, LPVOID address, ByteArray data);

//ͨ��������ȡ����pid ����-1��ʾ��ȡʧ��
DWORD GetProcessID(const string& name);
//��������Ȩ�޵�debug -1��ʾ��ǰ���� Ĭ��Ϊ-1
BOOL EnableDebugPrivileges(DWORD PID = -1);
//ɾ�׿� ����һ��ɾ��ǰ��յ��ַ���
string DelFroNull(string str);
//ɾ��β�� 
string DelFroEndNull(string str);
//ʹ��ָ�����ַ��ָ��ַ���
vector<string> splitString(const string& str, char delimiter);
//�ж��ַ����Ƿ�Ϊ������
bool isNumber(const string &str);
//stringת������ isHex�ַ����Ƿ�Ϊʮ�����ƣ�Ĭ��Ϊ��
int stringToInt(const string& str, bool isHex = false);
//ͨ�����ʽ���ڴ��ַ [["ģ���ַ"+ƫ��]+1]
void* ReadMemoryEX(DWORD PID, string str);
//��ȡָ�����̵�ģ���ַ
uintptr_t GetProcessModuleBaseAddress(DWORD PID, const string& name);
//ָ�����������ڴ�
LPVOID allocProcess(DWORD PID,size_t asize);
//ָ�������ͷ��ڴ�
void freeProcess(DWORD PID, LPVOID address);
//ȡָ�����̺�����ַ
void* GetProFunAddress(DWORD PID, const string& dllName, const string& funName);
//ȡָ������ģ���С
size_t GetModuleSize(DWORD PID, const string& dllName);

//����������
//������ת�ֽڼ�
WORD GetTzmArray(const char* Tzm, WORD* TzmArray);
//��ȡNext����
void GetNext(short* next, WORD* Tzm, WORD TzmLength);
//����һ���ڴ�
void SearchMemoryBlock(HANDLE hProcess, WORD* Tzm, WORD TzmLength, unsigned __int64 StartAddress, unsigned long size, vector<unsigned __int64>& ResultArray);
//������������
int SearchMemory(DWORD PID, string Tzm, unsigned __int64 StartAddress, unsigned __int64 EndAddress, int InitSize, vector<unsigned __int64>& ResultArray);

//Զ�̴����߳�
HANDLE CreateProThread(DWORD PID, void* address);
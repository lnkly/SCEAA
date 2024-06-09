// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include"SGame.h"
using namespace std;
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
#define fun extern "C" __declspec(dllexport)

vector<SCEAA> script;//建造一个脚本类列表

//返回一个id 失败返回-1
fun int initSCEAA(int PID, char* code) {
    string strcode = code;
    //MessageBoxA(0, strcode.data(), nullptr, 0);
    if (strcode == "") {
        return -1;
    }
    script.push_back({});
    size_t index = script.size() - 1;
    script[index].SetPID(PID);
    script[index].loadScript(strcode);
    return index;
}

//执行脚本 返回执行结果
fun void* runScriptCode(int id,int turn) {
    if (id >= script.size() || id < 0) {
        return 0;
    }
    script[id].runScript(turn);
    //返回数据
    if (script[id].GetError().empty())return nullptr;
    size_t errorSize = script[id].GetError().size() + 1;
    void* address = malloc(errorSize);
    if (address == nullptr) {
        return nullptr;
    }
    memcpy(address, script[id].GetError().data(), errorSize);
    return address;
}
//检查错误
fun void* isRightFulCode(int id) {
    script[id].IsRightful();
    //MessageBoxA(0, script[id].GetError().data(), nullptr, 0);
    //返回错误数据
    if (script[id].GetError().empty())return nullptr;
    size_t errorSize = script[id].GetError().size() + 1;
    void* address = malloc(errorSize);
    if (address == nullptr) {
        return nullptr;
    }
    memcpy(address, script[id].GetError().data(), errorSize);
    return address;
}
//释放脚本 成功返回1 失败返回0
fun int deleteScript(int id) {
    if (id >= script.size() || id < 0) { return 0; }
    script.erase(script.begin()+id);
    return 1;
}

fun void* add(char* a) {
    std::string b = a;
    MessageBoxA(0, b.data(), nullptr, 0);
    void* address = malloc(b.size() + 1);
    memcpy(address, b.data(), b.size() + 1);
    return address;
}

BOOL APIENTRY DllMain( HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


#pragma once
#include "SGame.h"
#include "SCEAA.h"
#include <map>

extern "C" {
#include "../XEDParse2/XEDParse.h"
#pragma comment(lib,"../XEDParse2/XEDParse_x86.lib")
}
//所使用的内置函数的地址类型 默认int 4字节 
typedef int sviod;

class SCEAA;

void PrintOpCode(char* pAsm, unsigned char* pOpcode, int nSize);
enum {
	//是[ENABLE]还是[DISABLE]
	e_turn_en,
	e_turn_dis,
	//地址 这个类型表示这里要改写这个地址的数据
	e_address,
	//调用内置的函数 比如alloc等
	e_callLib,
	//命令 if while等
	//e_statement,
	//汇编代码
	e_asm,
	//dd db dw dq
	e_difCode,
	//下面是标签类型
	t_label,
	t_addressLabel,

	r_libcall,//调用内置函数
	r_writeData//写入数据
};
//function address table
struct FAT {
	string funName;//函数名称
	void(*address)(SCEAA& obj, vector<string>& arg);//地址
	bool isDefLabel;//函数是否需要定义地址标签 比如alloc(标签,0) 如果这里填true 第一个参数将被理解为标签然后加入到地址标签列表
	//本来是想着一个函数同时可以传入多个标签，但是扩展增加新功能函数的时候不好办，所以改成了调用一个函数只能传入一个标签
};

//最后执行的数据的结构
struct struct_code {
	int type;//行类型 其实只有两种 1调用内置函数 2写入特定代码
	int inType;//所属e_turn_en还是e_turn_dis
	void* address;//要操作的地址
	vector<string> strcode;//文本代码 strcode[0]保存的也可能是地址的表达式
	ByteArray byArr;//汇编代码转换后的字节数组
};

struct struct_asm {
	string strCode;//文本汇编
	ByteArray byArr;//保存汇编转换后的字节数组
};
//普通标签
struct struct_otherLabel {
	//标签的具体地址就是根据地址标签的数组下标取出地址标签的地址，然后加上偏移获得具体地址
	string name;//标签名称
	size_t address;//标签具体地址 //不是真实的地址，就是为了计算汇编代码的，最后还是得转成字符串
	size_t off;//偏移
	string AddressOfPartName ;//头地址的地址标签名称
	size_t count;//标签被使用的次数
};
//地址标签
struct struct_label {
	string name;//标签名称
	void* address;//标签地址
};

struct lineCode{
	int line;//代码所在行号 一般用来报错用的
	int inType;//所属e_turn_en还是e_turn_dis
	int type;//代码类型
	std::string strCode;//文本形式的代码
	ByteArray byteCode;//字节集形式的代码，一般就只有汇编代码了
};


class SCEAA {
	int _inType;//代码所属
	int _line;//当前行
	DWORD _pid;//进程id
	bool _IsRightful;//代码是否通过 过 一次检测
	vector<lineCode> _codeList;//原始行代码数组
	vector<string>_error;//错误列表 如果执行的时候发生错误，错误信息会报错在这里
	vector<struct_label>_labelAddList;//申请的地址列表
	vector<struct_otherLabel>_labelList;//定义的普通标签，内含普通标签的地址
	vector<FAT> _FunAddList;//自定义库函数列表 
	vector<struct_code> _runCodeList;//最后执行的代码列表
private:
	//判断标签名称是否已被定义 在_labelList和_labelAddList两个数组内检查，因为不允许出现相同的标签名
	bool labelIsDef(const string &name);
	//自定义的函数给地址标签添加数据的时候调用这个就行
	bool add_labelAddList(string labelname,void *a_address);
	//把传入的汇编文本转换成字节集 返回长度
	size_t StrAsmToByArr(const string& str, ByteArray& ByArr, size_t addr);
	//通过名称取地址标签地址
	void* Get_labelAddListData(const string &name);
	//通过名称取普通标签地址
	size_t Get_labelListData(const string& name);
	//将difCode（dd db dw dq）转换成字节集
	size_t difCodeToByArr(const string& str, ByteArray& ByArr);

public:
	SCEAA() :_inType(-1), _line(0), _pid(-1), _IsRightful(false) {
		//先初始化自定义的库函数列表
		initFunAddList();
	};
	//SCEAA(string script) :_inType(-1), _line(0), _pid(-1), _IsRightful(false) { loadScript(script); };
	void loadScript(string script);//加载脚本
	//添加一条脚本代码
	void Addline(string code, bool isAddLineCount = true);
	//输出数据
	void Print_line();
	void Print_runcode();
	//设置要操作的进程的pid
	void SetPID(DWORD PID);
	//返回所有的错误信息
	string GetError();
	//检测脚本是否存在语法错误 //运行前会自动调用
	bool IsRightful();
	//运行脚本，传入真执行[ENABLE] 假执行[DISABLE]
	bool runScript(bool turn);
private:
	//这里可以自定义库函数
	void initFunAddList();
	//下面就全是内置函数了
	//这个函数并不实际调用，就是单纯占位然后通过语法检测的
	friend void lib_label(SCEAA& obj, vector<string>& arg);
	//申请内存
	friend void lib_alloc(SCEAA& obj, vector<string>& arg);
	//释放申请的内存 传入标签名称
	friend void lib_dealloc(SCEAA& obj, vector<string>& arg);
	//特征码搜索  参数表：搜后保存地址的标签，特征码，指定的模块(不写这个参数就搜索全部内存)
	friend void lib_aobscan(SCEAA& obj, vector<string>& arg);
	//创建一条线程  参数表：申请的地址的标签名称
	friend void lib_createthread(SCEAA& obj, vector<string>& arg);
	//获取指定的dll的函数地址，然后保存到标签 参数表:把地址保存到的标签，dll名称，函数名称
	friend void lib_getmodfunaddr(SCEAA& obj, vector<string>& arg);
};
//主要用来替换汇编中的标签 完美匹配 替换的文本 需要替换的文本 替换成什么
void asm_replace(string& asmcode, string& replStr, string str);
//把int转成十六进制文本
string intToHexStr(int num);

//这个函数并不实际调用，就是单纯占位然后通过语法检测的 而且这个函数是唯一一个可以同时定义多个标签的函数
void lib_label(SCEAA& obj, vector<string>& arg);
//这块写内置的函数申明————————————————————
//申请内存空间 最低两个参数 参数1 标签 参数2 字节数
void lib_alloc(SCEAA& bj, vector<string> &arg);
//释放申请内存空间，可以同时释放多个地址
void lib_dealloc(SCEAA& obj, vector<string>& arg);
//特征码搜索  参数表：搜后保存地址的标签，特征码，指定的模块(不写这个参数就搜索全部内存)
void lib_aobscan(SCEAA& obj, vector<string>& arg);
//创建一条线程  参数表：申请的地址的标签名称
void lib_createthread(SCEAA& obj, vector<string>& arg);
//获取指定的dll的函数地址，然后保存到标签 参数表:把地址保存到的标签，dll名称，函数名称
void lib_getmodfunaddr(SCEAA& obj, vector<string>& arg);
//——————————————————————————————

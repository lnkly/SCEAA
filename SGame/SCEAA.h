#pragma once
#include "SGame.h"
#include "SCEAA.h"
#include <map>

extern "C" {
#include "../XEDParse2/XEDParse.h"
#pragma comment(lib,"../XEDParse2/XEDParse_x86.lib")
}
//��ʹ�õ����ú����ĵ�ַ���� Ĭ��int 4�ֽ� 
typedef int sviod;

class SCEAA;

void PrintOpCode(char* pAsm, unsigned char* pOpcode, int nSize);
enum {
	//��[ENABLE]����[DISABLE]
	e_turn_en,
	e_turn_dis,
	//��ַ ������ͱ�ʾ����Ҫ��д�����ַ������
	e_address,
	//�������õĺ��� ����alloc��
	e_callLib,
	//���� if while��
	//e_statement,
	//������
	e_asm,
	//dd db dw dq
	e_difCode,
	//�����Ǳ�ǩ����
	t_label,
	t_addressLabel,

	r_libcall,//�������ú���
	r_writeData//д������
};
//function address table
struct FAT {
	string funName;//��������
	void(*address)(SCEAA& obj, vector<string>& arg);//��ַ
	bool isDefLabel;//�����Ƿ���Ҫ�����ַ��ǩ ����alloc(��ǩ,0) ���������true ��һ�������������Ϊ��ǩȻ����뵽��ַ��ǩ�б�
	//����������һ������ͬʱ���Դ�������ǩ��������չ�����¹��ܺ�����ʱ�򲻺ð죬���Ըĳ��˵���һ������ֻ�ܴ���һ����ǩ
};

//���ִ�е����ݵĽṹ
struct struct_code {
	int type;//������ ��ʵֻ������ 1�������ú��� 2д���ض�����
	int inType;//����e_turn_en����e_turn_dis
	void* address;//Ҫ�����ĵ�ַ
	vector<string> strcode;//�ı����� strcode[0]�����Ҳ�����ǵ�ַ�ı��ʽ
	ByteArray byArr;//������ת������ֽ�����
};

struct struct_asm {
	string strCode;//�ı����
	ByteArray byArr;//������ת������ֽ�����
};
//��ͨ��ǩ
struct struct_otherLabel {
	//��ǩ�ľ����ַ���Ǹ��ݵ�ַ��ǩ�������±�ȡ����ַ��ǩ�ĵ�ַ��Ȼ�����ƫ�ƻ�þ����ַ
	string name;//��ǩ����
	size_t address;//��ǩ�����ַ //������ʵ�ĵ�ַ������Ϊ�˼��������ģ�����ǵ�ת���ַ���
	size_t off;//ƫ��
	string AddressOfPartName ;//ͷ��ַ�ĵ�ַ��ǩ����
	size_t count;//��ǩ��ʹ�õĴ���
};
//��ַ��ǩ
struct struct_label {
	string name;//��ǩ����
	void* address;//��ǩ��ַ
};

struct lineCode{
	int line;//���������к� һ�����������õ�
	int inType;//����e_turn_en����e_turn_dis
	int type;//��������
	std::string strCode;//�ı���ʽ�Ĵ���
	ByteArray byteCode;//�ֽڼ���ʽ�Ĵ��룬һ���ֻ�л�������
};


class SCEAA {
	int _inType;//��������
	int _line;//��ǰ��
	DWORD _pid;//����id
	bool _IsRightful;//�����Ƿ�ͨ�� �� һ�μ��
	vector<lineCode> _codeList;//ԭʼ�д�������
	vector<string>_error;//�����б� ���ִ�е�ʱ�������󣬴�����Ϣ�ᱨ��������
	vector<struct_label>_labelAddList;//����ĵ�ַ�б�
	vector<struct_otherLabel>_labelList;//�������ͨ��ǩ���ں���ͨ��ǩ�ĵ�ַ
	vector<FAT> _FunAddList;//�Զ���⺯���б� 
	vector<struct_code> _runCodeList;//���ִ�еĴ����б�
private:
	//�жϱ�ǩ�����Ƿ��ѱ����� ��_labelList��_labelAddList���������ڼ�飬��Ϊ�����������ͬ�ı�ǩ��
	bool labelIsDef(const string &name);
	//�Զ���ĺ�������ַ��ǩ������ݵ�ʱ������������
	bool add_labelAddList(string labelname,void *a_address);
	//�Ѵ���Ļ���ı�ת�����ֽڼ� ���س���
	size_t StrAsmToByArr(const string& str, ByteArray& ByArr, size_t addr);
	//ͨ������ȡ��ַ��ǩ��ַ
	void* Get_labelAddListData(const string &name);
	//ͨ������ȡ��ͨ��ǩ��ַ
	size_t Get_labelListData(const string& name);
	//��difCode��dd db dw dq��ת�����ֽڼ�
	size_t difCodeToByArr(const string& str, ByteArray& ByArr);

public:
	SCEAA() :_inType(-1), _line(0), _pid(-1), _IsRightful(false) {
		//�ȳ�ʼ���Զ���Ŀ⺯���б�
		initFunAddList();
	};
	//SCEAA(string script) :_inType(-1), _line(0), _pid(-1), _IsRightful(false) { loadScript(script); };
	void loadScript(string script);//���ؽű�
	//���һ���ű�����
	void Addline(string code, bool isAddLineCount = true);
	//�������
	void Print_line();
	void Print_runcode();
	//����Ҫ�����Ľ��̵�pid
	void SetPID(DWORD PID);
	//�������еĴ�����Ϣ
	string GetError();
	//���ű��Ƿ�����﷨���� //����ǰ���Զ�����
	bool IsRightful();
	//���нű���������ִ��[ENABLE] ��ִ��[DISABLE]
	bool runScript(bool turn);
private:
	//��������Զ���⺯��
	void initFunAddList();
	//�����ȫ�����ú�����
	//�����������ʵ�ʵ��ã����ǵ���ռλȻ��ͨ���﷨����
	friend void lib_label(SCEAA& obj, vector<string>& arg);
	//�����ڴ�
	friend void lib_alloc(SCEAA& obj, vector<string>& arg);
	//�ͷ�������ڴ� �����ǩ����
	friend void lib_dealloc(SCEAA& obj, vector<string>& arg);
	//����������  �������Ѻ󱣴��ַ�ı�ǩ�������룬ָ����ģ��(��д�������������ȫ���ڴ�)
	friend void lib_aobscan(SCEAA& obj, vector<string>& arg);
	//����һ���߳�  ����������ĵ�ַ�ı�ǩ����
	friend void lib_createthread(SCEAA& obj, vector<string>& arg);
	//��ȡָ����dll�ĺ�����ַ��Ȼ�󱣴浽��ǩ ������:�ѵ�ַ���浽�ı�ǩ��dll���ƣ���������
	friend void lib_getmodfunaddr(SCEAA& obj, vector<string>& arg);
};
//��Ҫ�����滻����еı�ǩ ����ƥ�� �滻���ı� ��Ҫ�滻���ı� �滻��ʲô
void asm_replace(string& asmcode, string& replStr, string str);
//��intת��ʮ�������ı�
string intToHexStr(int num);

//�����������ʵ�ʵ��ã����ǵ���ռλȻ��ͨ���﷨���� �������������Ψһһ������ͬʱ��������ǩ�ĺ���
void lib_label(SCEAA& obj, vector<string>& arg);
//���д���õĺ�����������������������������������������������
//�����ڴ�ռ� ����������� ����1 ��ǩ ����2 �ֽ���
void lib_alloc(SCEAA& bj, vector<string> &arg);
//�ͷ������ڴ�ռ䣬����ͬʱ�ͷŶ����ַ
void lib_dealloc(SCEAA& obj, vector<string>& arg);
//����������  �������Ѻ󱣴��ַ�ı�ǩ�������룬ָ����ģ��(��д�������������ȫ���ڴ�)
void lib_aobscan(SCEAA& obj, vector<string>& arg);
//����һ���߳�  ����������ĵ�ַ�ı�ǩ����
void lib_createthread(SCEAA& obj, vector<string>& arg);
//��ȡָ����dll�ĺ�����ַ��Ȼ�󱣴浽��ǩ ������:�ѵ�ַ���浽�ı�ǩ��dll���ƣ���������
void lib_getmodfunaddr(SCEAA& obj, vector<string>& arg);
//������������������������������������������������������������

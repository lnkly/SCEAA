#include"SGame.h"
#include <fstream>
using namespace std;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

void test() {
	DWORD PID = GetProcessID("PlantsVsZombies.exe");
	cout << "����PID��" << PID<<endl;
	SCEAA a;
	a.SetPID(PID);
	while (true) {
	cout << "�����������ִ��" << endl;
	system("pause");
	
	string route = "C:\\Users\\86134\\OneDrive\\����\\a.asm";
	ifstream file(route);
	if (!file.is_open()) {
		std::cerr << "�޷����ļ�" << std::endl;
		return;
	}
	std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	a.loadScript(code);
	a.Print_line();

	if (a.runScript(true)) {
		cout << "��ʼִ��" << endl;
		cout << a.GetError() << endl;
	}
	else {
		cout << "����ûͨ����飬�д���" << endl;
		cout << a.GetError() << endl;
	}
	a.Print_runcode();
	cout << "��ʼ׼��ִ�йرմ���" << endl;
	system("pause");
	a.runScript(false);
	cout << "ִ����[disable]" << endl;
	system("pause");
	}
}

void test3(){
	//string route = "C:\\Users\\86134\\OneDrive\\����\\a.asm";
	string route;
	cout << "������ű��ļ�·��:" << endl;
	cin >> route;
	ifstream file(route);
	if (!file.is_open()) {
		std::cerr << "�޷����ļ�" << std::endl;
		return ;
	}
	string proName;
	cout << "�������������:" << endl;
	cin >> proName;
	//wchar_t name[MAX_PATH];
	//charTowchar(proName.data(), name, MAX_PATH);
	DWORD PID = GetProcessID(proName);
	if (PID == -1) {
		cout << "���̻�ȡidʧ��" << endl;
		return;
	}
	std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	SCEAA a;
	a.SetPID(PID);
	while (true) {
		cout << "�������ַ���������" << endl;
		system("pause");

	}
}

int main() {
	test();
	return 0;
}
#include"SGame.h"
#include <fstream>
using namespace std;

#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif

void test() {
	DWORD PID = GetProcessID("PlantsVsZombies.exe");
	cout << "进程PID：" << PID<<endl;
	SCEAA a;
	a.SetPID(PID);
	while (true) {
	cout << "按任意键即可执行" << endl;
	system("pause");
	
	string route = "C:\\Users\\86134\\OneDrive\\桌面\\a.asm";
	ifstream file(route);
	if (!file.is_open()) {
		std::cerr << "无法打开文件" << std::endl;
		return;
	}
	std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	a.loadScript(code);
	a.Print_line();

	if (a.runScript(true)) {
		cout << "开始执行" << endl;
		cout << a.GetError() << endl;
	}
	else {
		cout << "代码没通过检查，有错误哎" << endl;
		cout << a.GetError() << endl;
	}
	a.Print_runcode();
	cout << "开始准备执行关闭代码" << endl;
	system("pause");
	a.runScript(false);
	cout << "执行了[disable]" << endl;
	system("pause");
	}
}

void test3(){
	//string route = "C:\\Users\\86134\\OneDrive\\桌面\\a.asm";
	string route;
	cout << "请输入脚本文件路径:" << endl;
	cin >> route;
	ifstream file(route);
	if (!file.is_open()) {
		std::cerr << "无法打开文件" << std::endl;
		return ;
	}
	string proName;
	cout << "请输入进程名称:" << endl;
	cin >> proName;
	//wchar_t name[MAX_PATH];
	//charTowchar(proName.data(), name, MAX_PATH);
	DWORD PID = GetProcessID(proName);
	if (PID == -1) {
		cout << "进程获取id失败" << endl;
		return;
	}
	std::string code((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	file.close();
	SCEAA a;
	a.SetPID(PID);
	while (true) {
		cout << "按任意字符开启功能" << endl;
		system("pause");

	}
}

int main() {
	test();
	return 0;
}
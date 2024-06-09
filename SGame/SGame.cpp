#include"SGame.h"

BYTE* MemoryData;//ÿ�ν���ȡ���ڴ��������
short Next[260];

//��ͨ�ַ������ַ�
void charTowchar(const char* chr, wchar_t* wchar, int size) {
	MultiByteToWideChar(CP_ACP, 0, chr,
		-1, wchar, size);
}
void wcharTochar(const wchar_t* wchar, char* chr, int length) {
	WideCharToMultiByte(CP_ACP, 0, wchar, -1,
		chr, length, NULL, NULL);
}
//���ڴ�
BYTE ReadMemory(DWORD PID, LPCVOID address, BYTE FNumber) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BYTE buffer = 0;
	if (ReadProcessMemory(handle, address, (LPVOID)&buffer, sizeof(BYTE), 0)) {
		FNumber = buffer;
	}
	CloseHandle(handle);
	return FNumber;
}
int ReadMemory_int(DWORD PID,LPCVOID address,int FNumber) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS,false,PID);
	if (handle == NULL) {
		return 0;
	}
	int buffer = 0;
	if (ReadProcessMemory(handle, address, (LPVOID)&buffer, sizeof(int), 0)) {
		FNumber = buffer;
	}
	CloseHandle(handle);
	return FNumber;
}
int ReadMemory(DWORD PID, LPCVOID address, int FNumber) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	int buffer = 0;
	if (ReadProcessMemory(handle, address, (LPVOID)&buffer, sizeof(int), 0)) {
		FNumber = buffer;
	}
	CloseHandle(handle);
	return FNumber;
}
long ReadMemory(DWORD PID, LPCVOID address, long FNumber) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	long buffer = 0;
	if (ReadProcessMemory(handle, address, (LPVOID)&buffer, sizeof(long), 0)) {
		FNumber = buffer;
	}
	CloseHandle(handle);
	return FNumber;
}
float ReadMemory(DWORD PID, LPCVOID address, float FNumber) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	float buffer = 0;
	if (ReadProcessMemory(handle, address, (LPVOID)&buffer, sizeof(float), 0)) {
		FNumber = buffer;
	}
	CloseHandle(handle);
	return FNumber;
}
double ReadMemory(DWORD PID, LPCVOID address, double FNumber) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	double buffer = 0;
	if (ReadProcessMemory(handle, address, (LPVOID)&buffer, sizeof(double), 0)) {
		FNumber = buffer;
	}
	CloseHandle(handle);
	return FNumber;
}
BOOL ReadMemory(DWORD PID, LPCVOID address, void* Buffer, size_t size) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return false;
	}
	bool ret = ReadProcessMemory(handle, address, (LPVOID)Buffer, size, 0);
	CloseHandle(handle);
	return ret;
}
ByteArray ReadMemory_ByteArray(DWORD PID, LPCVOID address,size_t size) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	ByteArray buffer(size);
	BOOL ret = FALSE;
	ret = ReadProcessMemory(handle, address, (LPVOID)buffer.GetDataAddress(), size, 0);
	CloseHandle(handle);
	if (ret) {
		return buffer;
	}
	return ByteArray();
}
//д�ڴ�
BOOL WriteMemory(DWORD PID, LPVOID address, BYTE data) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BOOL r = WriteProcessMemory(handle, address, &data, sizeof(BYTE), 0);
	CloseHandle(handle);
	return r;
}
BOOL WriteMemory(DWORD PID, LPVOID address, int data) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BOOL r = WriteProcessMemory(handle, address, &data, sizeof(int), 0);
	CloseHandle(handle);
	return r;
}
BOOL WriteMemory(DWORD PID, LPVOID address, long data) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BOOL r = WriteProcessMemory(handle, address, &data, sizeof(long), 0);
	CloseHandle(handle);
	return r;
}
BOOL WriteMemory(DWORD PID, LPVOID address, float data) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BOOL r = WriteProcessMemory(handle, address, &data, sizeof(float), 0);
	CloseHandle(handle);
	return r;
}
BOOL WriteMemory(DWORD PID, LPVOID address, double data) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BOOL r = WriteProcessMemory(handle, address, &data, sizeof(double), 0);
	CloseHandle(handle);
	return r;
}
BOOL WriteMemory(DWORD PID, LPVOID address, void* Buffer, size_t size) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BOOL r = WriteProcessMemory(handle, address, Buffer, size, 0);
	CloseHandle(handle);
	return r;
}
BOOL WriteMemory(DWORD PID, LPVOID address, ByteArray data) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {
		return 0;
	}
	BOOL r = WriteProcessMemory(handle, address, data.GetDataAddress(), data.GetLen(), 0);
	CloseHandle(handle);
	return r;
}

DWORD GetProcessID(const string&name) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnap == INVALID_HANDLE_VALUE) {
		return -1;
	}
	DWORD PID = -1;
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);
	BOOL bRet = Process32First(hSnap,&pe32);
	wchar_t wname[MAX_PATH];
	charTowchar(name.data(), wname, MAX_PATH);
	while (bRet) {
		if (lstrcmpW((LPCWCHAR)wname, pe32.szExeFile) == 0) {
			PID =  pe32.th32ProcessID;
			break;
		}
		bRet = Process32NextW(hSnap, &pe32);
	}
	CloseHandle(hSnap);
	return PID;
}

BOOL EnableDebugPrivileges(DWORD PID) {
	HANDLE proHand;
	if (PID == -1) {
		proHand = GetCurrentProcess();
	}
	else {
		proHand = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	}

	if (proHand == NULL) {
		return FALSE;
	}
	HANDLE hToken;
	LUID sedebugNameValue;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(proHand, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
		return FALSE;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugNameValue)) {
		CloseHandle(hToken);
		return FALSE;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = sedebugNameValue;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL)) {
		CloseHandle(hToken);
		return FALSE;
	}
	if (PID != -1) {
		CloseHandle(proHand);
	}
	return TRUE;
}

string DelFroNull(string str) {
	size_t found = str.find_first_not_of(" \t\r\n");
	//cout << found << endl;
	if (found != string::npos) {
		return str.substr(found);
	}
	return str;
}

string DelFroEndNull(string str) {
	size_t first = str.find_first_not_of(" \t\r\n");
	size_t end = str.find_last_not_of(" \t\r\n");
	if (first != string::npos && end != string::npos) {
		str = str.substr(first, end - first+1);
	}
	return str;
}

vector<string> splitString(const string& str, char delimiter) {
	std::istringstream iss(str);
	vector<string> ret;
	string temp;
	while (std::getline(iss, temp, delimiter)) {
		ret.push_back(temp);
	}
	return ret;
}

bool isNumber(const string& str) {
	const char* p = str.data();
	for (size_t i = 0; i < str.size(); i++) {
		if (*(p + i) == ' ')continue;
		if (*(p + i) < '0' || *(p + i) > '9') {
			return false;
		}
	}
	return true;
}

int stringToInt(const string& str,bool isHex) {
	std::istringstream iss(str);
	int num;
	if (isHex) {
		iss >> std::hex >> num;
	}
	else {
		iss >> num;
	}
	return num;
}
//ȡ����ģ���ַ
uintptr_t GetProcessModuleBaseAddress(DWORD PID, const string& name) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PID);
	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	WCHAR moduleName[MAX_PATH] = { 0 };
	charTowchar(name.data(), moduleName, name.size());
	// ��������ģ�飬����Ŀ��ģ��
	if (Module32First(hSnapshot, &moduleEntry)) {
		do {
			if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
				// �ҵ���Ŀ��ģ�飬���������ַ
				CloseHandle(hSnapshot);
				return (uintptr_t)moduleEntry.modBaseAddr;
			}
		} while (Module32Next(hSnapshot, &moduleEntry));
	}

	// δ�ҵ�Ŀ��ģ�飬���� 0
	CloseHandle(hSnapshot);
	return 0;

}
//��ȡ���ʽָ����ڴ��ַ [["ģ���ַ"+ƫ��]+1]
void* ReadMemoryEX(DWORD PID,string str) {
	//�����һ�������ĵ�ַ ֱ��ת���������ؾ���
	if (str.find_first_of('+') == string::npos && str.find_first_of('.') == string::npos) {
		return (void*)stringToInt(str,true);
	}
	//�Ȱ�ȫ���ո�ɾ�ˣ�̫������
	str.erase(std::remove_if(str.begin(), str.end(), [](char c) { return std::isspace(c); }), str.end());
	vector<string> strcode;
	size_t dllAddress = 0;//
	size_t result = 0;//��󷵻صĽ��
	size_t endoff = 0;
	//����д���ʽ�����Ĺ��� �����ɹ����ص�ַ������ʧ�ܷ��ؿ�
	size_t first = str.rfind('[');
	size_t end = str.find(']');
	int count = 0;//�����ģ���ֹ����д��������ѭ�� �ӵ�ƫ����಻�ܳ���15��
	while (first != std::string::npos) {
		string a = str.substr(first + 1, end - first-1);

		if (a[0] == '+') {
			strcode.push_back(a.substr(1,a.size()-1));
		}
		else {
			strcode.push_back(a);
		}
		str.erase(first, end - first+1);
		first = str.rfind('[');
		end = str.find(']');
		if (count == 15)break;
		count++;
	}
	if (count == 0) {
		strcode.push_back(str);
	}
	if (str[0] == '+') {
		endoff = stringToInt(str.substr(1, str.size() - 1), true);//���һ��ƫ��
	}
	
	end = strcode[0].find('+');//�ظ������±�����������������
	string dllname;
	size_t off = 0;
	dllname = strcode[0];
	if (end != std::string::npos) {//��������ľ��� dllname+off
		if (strcode[0].find('\"') == std::string::npos) {
			dllname = strcode[0].substr(0, end);
		}
		else {
			dllname = strcode[0].substr(1, end - 2);
		}
		off = stringToInt(strcode[0].substr(end + 1, strcode[0].size() - end), true);
		//����ģ���ַ���ټ�ƫ��
	}
	if (dllname.find('.') == std::string::npos) {//�����dll��ֱ�ӵĵ�ַ���е��˾�ӲҪ��ôд
		dllAddress = stringToInt(dllname, true) + off;
	}
	else {
		dllAddress = GetProcessModuleBaseAddress(PID, dllname)+off;
	}
	//���ʱ��dllAddressģ��+ƫ�Ƶĵ�ַ��������� ������һ��һ�������
	if (strcode.size() == 1 && endoff == 0) {
		return (void*)dllAddress;	
	}
	result = ReadMemory(PID, (LPVOID)dllAddress,(int) - 1);
		//���������������������
	for (size_t i = 1; i < strcode.size(); i++) {//ʡ�Ե�һ��
		result = ReadMemory(PID, (LPVOID)(result+stringToInt(strcode[i],true)), (int)-1);
	}
	result = result + endoff;
	return (void*)result;
}

//ָ�����������ڴ�
LPVOID allocProcess(DWORD PID, size_t asize) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) {return nullptr;}
	LPVOID lpBaseAddress = VirtualAllocEx(handle, NULL, asize, MEM_COMMIT, PAGE_READWRITE);
	CloseHandle(handle);
	return lpBaseAddress;
}
//ָ�������ͷ��ڴ�
void freeProcess(DWORD PID, LPVOID address) {
	HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, PID);
	if (handle == NULL) { return ; }
	VirtualFreeEx(handle, address, 0, MEM_RELEASE);
	CloseHandle(handle);
	return ;
}
//ȡģ�麯����ַ
void* GetProFunAddress(DWORD PID, const string& dllName, const string& funName) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		return nullptr;
	}
	wchar_t wdllName[MAX_PATH];
	charTowchar(dllName.data(), wdllName, MAX_PATH);
	wchar_t wfunName[MAX_PATH];
	charTowchar(funName.data(), wfunName, MAX_PATH);


	// ��ȡĿ��ģ��Ļ�ַ
	HMODULE hModule;
	if (!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, wdllName, &hModule)) {
		CloseHandle(hProcess);
		return nullptr;
	}

	// ��ȡ������ַ
	FARPROC functionAddress = GetProcAddress(hModule, funName.data());
	if (functionAddress == NULL) {
		CloseHandle(hProcess);
		return nullptr;
	}

	// �رվ��
	CloseHandle(hProcess);
	return functionAddress;
}
//ȡģ���С
size_t GetModuleSize(DWORD PID, const string& dllName) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, PID);
	MODULEENTRY32 moduleEntry;
	moduleEntry.dwSize = sizeof(MODULEENTRY32);
	WCHAR moduleName[MAX_PATH] = { 0 };
	charTowchar(dllName.data(), moduleName, dllName.size());
	// ��������ģ�飬����Ŀ��ģ��
	if (Module32First(hSnapshot, &moduleEntry)) {
		do {
			if (_wcsicmp(moduleEntry.szModule, moduleName) == 0) {
				// �ҵ���Ŀ��ģ�飬���������ַ
				CloseHandle(hSnapshot);
				return moduleEntry.modBaseSize;
			}
		} while (Module32Next(hSnapshot, &moduleEntry));
	}

	// δ�ҵ�Ŀ��ģ�飬���� 0
	CloseHandle(hSnapshot);
	return 0;
}

//����������
//������ת�ֽڼ�
WORD GetTzmArray(const char* Tzm, WORD* TzmArray) {
	int len = 0;
	WORD TzmLength = strlen(Tzm) / 3 + 1;

	for (int i = 0; i < strlen(Tzm); )//��ʮ������������תΪʮ����
	{
		char num[2];
		num[0] = Tzm[i++];
		num[1] = Tzm[i++];
		i++;
		if (num[0] != '?' && num[1] != '?') {
			int sum = 0;
			WORD a[2];
			for (int i = 0; i < 2; i++) {
				if (num[i] >= '0' && num[i] <= '9') {
					a[i] = num[i] - '0';
				}
				else if (num[i] >= 'a' && num[i] <= 'z') {
					a[i] = num[i] - 87;
				}
				else if (num[i] >= 'A' && num[i] <= 'Z') {
					a[i] = num[i] - 55;
				}

			}
			sum = a[0] * 16 + a[1];
			TzmArray[len++] = sum;
		}
		else {
			TzmArray[len++] = 256;
		}
	}
	return TzmLength;
}
//��ȡNext����
void GetNext(short* next, WORD* Tzm, WORD TzmLength) {
	//������(�ֽڼ�)��ÿ���ֽڵķ�Χ��0-255(0-FF)֮�䣬256������ʾ�ʺţ���260��Ϊ�˷�ֹԽ��
	for (int i = 0; i < 260; i++)
		next[i] = -1;
	for (int i = 0; i < TzmLength; i++)
		next[Tzm[i]] = i;
}
//����һ���ڴ�
void SearchMemoryBlock(HANDLE hProcess, WORD* Tzm, WORD TzmLength, unsigned __int64 StartAddress, unsigned long size, vector<unsigned __int64>& ResultArray) {
	if (!ReadProcessMemory(hProcess, (LPCVOID)StartAddress, MemoryData, size, NULL)) {
		return;
	}

	for (int i = 0, j, k; i < size;) {
		j = i; k = 0;

		for (; k < TzmLength && j < size && (Tzm[k] == MemoryData[j] || Tzm[k] == 256); k++, j++);

		if (k == TzmLength) {
			ResultArray.push_back(StartAddress + i);
		}

		if ((i + TzmLength) >= size) {
			return;
		}

		int num = Next[MemoryData[i + TzmLength]];
		if (num == -1)
			i += (TzmLength - Next[256]);//������������ʺţ��ʹ��ʺŴ���ʼƥ�䣬���û�о�i+=-1
		else
			i += (TzmLength - num);
	}
}
//������������
int SearchMemory(DWORD PID, string Tzm, unsigned __int64 StartAddress, unsigned __int64 EndAddress, int InitSize, vector<unsigned __int64>& ResultArray) {
	
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		return 0;
	}
	MemoryData = new BYTE[BLOCKMAXSIZE];
	int i = 0;
	unsigned long BlockSize;
	MEMORY_BASIC_INFORMATION mbi;

	WORD TzmLength = Tzm.length() / 3 + 1;
	WORD* TzmArray = new WORD[TzmLength];

	GetTzmArray(Tzm.data(), TzmArray);
	GetNext(Next, TzmArray, TzmLength);

	//��ʼ���������
	ResultArray.clear();
	ResultArray.reserve(InitSize);

	while (VirtualQueryEx(hProcess, (LPCVOID)StartAddress, &mbi, sizeof(mbi)) != 0) {
		//��ȡ�ɶ���д�Ϳɶ���д��ִ�е��ڴ��
		if (mbi.Protect == PAGE_READWRITE || mbi.Protect == PAGE_EXECUTE_READWRITE || mbi.Protect == PAGE_EXECUTE_READ) {
			i = 0;
			BlockSize = mbi.RegionSize;
			//��������ڴ�
			while (BlockSize >= BLOCKMAXSIZE) {
				SearchMemoryBlock(hProcess, TzmArray, TzmLength, StartAddress + (BLOCKMAXSIZE * i), BLOCKMAXSIZE, ResultArray);
				BlockSize -= BLOCKMAXSIZE; i++;
			}
			SearchMemoryBlock(hProcess, TzmArray, TzmLength, StartAddress + (BLOCKMAXSIZE * i), BlockSize, ResultArray);

		}
		StartAddress += mbi.RegionSize;

		if (EndAddress != 0 && StartAddress > EndAddress) {
			CloseHandle(hProcess);
			return ResultArray.size();
		}
	}
	delete[] TzmArray;
	CloseHandle(hProcess);
	return ResultArray.size();
}
//Զ�̴����߳�
HANDLE CreateProThread(DWORD PID,void* address) {
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		return 0;
	}
	HANDLE hRemoteThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)address, NULL, 0, NULL);
	if (hRemoteThread == NULL) {
		CloseHandle(hProcess);
		return 0;
	}
	return hRemoteThread;
	CloseHandle(hProcess);
	
}
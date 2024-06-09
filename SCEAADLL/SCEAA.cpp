#include"SCEAA.h"
#include "pch.h"
void PrintOpCode(char* pAsm, unsigned char* pOpcode, int nSize) {
	for (int i = 0; i < nSize; ++i) {
		printf("%02X ", pOpcode[i]);
	}
	printf("%30s \n", pAsm);
}

//�Զ���ĺ�������ַ��ǩ������ݵ�ʱ������������ ��ӳɹ�������
bool SCEAA::add_labelAddList(string labelname, void* a_address) {
	for (size_t i = 0; i < _labelAddList.size(); i++) {
		if (_labelAddList[i].name == labelname) {
			_labelAddList[i].address = a_address;
			return true;
		}
	}
	return false;
}
//ͨ������ȡ��ַ��ǩ��ַ
void* SCEAA::Get_labelAddListData(const string& name) {
	for (size_t i = 0; i < _labelAddList.size(); i++) {
		if (_labelAddList[i].name == name) {
			return _labelAddList[i].address;
		}
	}
	return nullptr;
}
//ͨ������ȡ��ͨ��ǩ��ַ
size_t SCEAA::Get_labelListData(const string& name) {
	for (size_t i = 0; i < _labelList.size(); i++) {
		if (_labelList[i].name == name) {
			return _labelList[i].address;
		}
	}
	return 0;
}
//��difCode��dd db dw dq��ת�����ֽڼ�
size_t SCEAA::difCodeToByArr(const string& str, ByteArray& ByArr) {
	vector<string> strArr = splitString(str, ' ');
	if (strArr.empty()) {
		_error.push_back("����ʱ����:��Ч����->" + str);
		return 0;
	}
	if (strArr[0] == "db") {
		for (size_t i = 1; i < strArr.size(); i++) {
			ByArr += {(unsigned char)stringToInt(strArr[i],true)};
		}
		return (strArr.size() - 1) * 1;
	}
	if (strArr[0] == "dw") {
		for (size_t i = 1; i < strArr.size(); i++) {
			std::istringstream iss(strArr[i]);
			__int16 num = 0;
			iss >> hex>>num;
			ByArr += ToByteArray(num);
		}
		return (strArr.size() - 1) * 2;
	}
	//strcmp(strArr[0].data(), "dd")
	if (strArr[0] == "dd") {
		for (size_t i = 1; i < strArr.size(); i++) {
			std::istringstream iss(strArr[i]);
			__int32 num = 0;
			iss >> hex >> num;
			ByArr += ToByteArray(num);
		}
		return (strArr.size() - 1) * 4;
	}
	if (strArr[0] == "dq") {
		for (size_t i = 1; i < strArr.size(); i++) {
			std::istringstream iss(strArr[i]);
			__int64 num = 0;
			iss >> hex >> num;
			ByArr += ToByteArray(num);
		}
		return (strArr.size() - 1) * 8;
	}
	return 0;
}

string SCEAA::GetError() {
	string a;
	for (size_t i = 0; i < _error.size(); i++) {
		a += _error[i];
		a += ";\n";
	}
	return a;
}
void SCEAA::Addline(string code,bool isAddLineCount) {
	if(isAddLineCount)_line++;//�к�+1
	
	code = DelFroEndNull(code);
	if (code.find_first_of(" \t\r\n") == 0 || code=="")return;//ɾ������

	//�������ر�ʶ�Ͳ��������һ�д�����
	if (code == "[ENABLE]") {
		_inType = e_turn_en;
		//_codeList.push_back({ line,e_turn_en,e_turn_en ,code , {}});
		return;
	}

	if (code == "[DISABLE]") {
		_inType = e_turn_dis;
		//_codeList.push_back({ line,e_turn_dis,e_turn_dis ,code , {} });
		return;
	}

	//������һ���ַ�Ϊ:˵�������address 
	if (code[code.size() - 1] == ':') {
		code.erase(code.size() - 1);
		_codeList.push_back({ _line,_inType, e_address ,code ,{} });
		return;
	}

	//�����һ���ַ�����.�����һ���ַ���)˵�������ú�������
	if (code[0] != '.' && code[code.size() - 1] == ')') {
		//����ֱ�ӽ����˵������ú����ĺ������������� name = callLibName vector = ����
		_codeList.push_back({ _line, _inType,e_callLib,code,{} });
		return;
	}
	//˵����db ���ָ�ʽ��
	if (code[0] == 'd') {
		_codeList.push_back({ _line, _inType,e_difCode,code,{} });
		return;
	}

	//ʣ�µĶ��������봦��
	_codeList.push_back({ _line, _inType,e_asm,code,{} });
	return;
}
void SCEAA::Print_line() {
	for (const auto& a : _codeList) {
		cout << "line��" << a.line <<
			"   in:" << a.inType <<
			"   type:" << a.type <<
			"   code:" << a.strCode << endl;
		//"   ByteArray:" << a.byteCode << endl;
	}
}
void SCEAA::SetPID(DWORD PID) {
	_pid = PID;
	return;
}
void SCEAA::Print_runcode() {
	for (size_t i = 0; i < _runCodeList.size(); i++) {
		cout << "type:" << _runCodeList[i].type
			<< "  inType:" << _runCodeList[i].inType
			<< "  address:" << hex << _runCodeList[i].address << "  strcode:";
			for (size_t j = 0; j < _runCodeList[i].strcode.size(); j++) {
				cout << _runCodeList[i].strcode[j]<<"--";
			}
			cout << "  ByteArray" << hex << _runCodeList[i].byArr << endl;
	}
}
bool SCEAA::IsRightful() {
	if (_IsRightful)return true;
	_error.clear();
	_runCodeList.clear();
	_labelList.clear();
	_labelAddList.clear();

	bool _isAddasm = false;//�Ƿ���ӻ����� ������Ǹ����_runCodeList��ӵ�
	//Ҫ����ǰΪ��ǩ��Ҫ��Ϊ��ַ��ֻ��ͬʱ����һ��
	string AddressOfPartName;//����������ͨ��ǩ��ƫ�Ƶ�
	size_t AddressOfPartAddr = 0;//����������ͨ��ǩ��ƫ�Ƶ�(�����ֱ���Ե�ַ������Ҫ��ŵ�ַ��ǩ��ֱ�Ӽ����ַ����)
	size_t labelOff = 0;//��ͨ��ǩ��ַ��ƫ����


	if (_codeList.empty()) {
		_error.push_back("����:����Ϊ��");
		return false;
	}

	if (_pid == -1) {
		_error.push_back("����:��Ч�Ľ���id");
	}

	for (size_t i = 0; i < _codeList.size(); i++) {
		//cout << "���룺" << _codeList[i].strCode << endl;
		//����ǵ��õ�����call����ô����
		if (_codeList[i].type == e_callLib) {
			_isAddasm = false;
			size_t found = _codeList[i].strCode.find('(');
			string callName = _codeList[i].strCode.substr(0, found);
			vector<string> arg = splitString(_codeList[i].strCode.substr(found + 1, _codeList[i].strCode.size() - found - 2), ',');;
			void* libcall = nullptr;//call�ĵ�ַ
			for (size_t j = 0; j < _FunAddList.size(); j++) {
				if (callName == _FunAddList[j].funName) {
					libcall = _FunAddList[j].address;
					break;
				}
			}
			if (libcall != nullptr) {
				//����Ϳ���ִ�������ú����Ĺ����ˣ�������Щ���� ��������ִ�к�������ĵ�ʱ���ִ�У������ǩ��صĺ���
				//����ĺ��� label
				if (callName == "label") {
					for (size_t j = 0; j < arg.size(); j++) {
						//����ǩ�����Ƿ��ѱ�ռ��
						if (labelIsDef(arg[j])) {
							_error.push_back("����" + to_string(_codeList[i].line) + "��:��ǩ" + arg[j] + "�Ѿ���ʹ��");
							//continue;//���ﻹ����ִ�аѣ����������еĴ����ҳ���
						}
						else {
							_labelList.push_back({ arg[j] ,0,0,"",0 });
						}
					}
					continue;//�������������󶼲���Ҫִ��ר�ŵĺ���
				}
				//��ôд�ĺô����Ǻ���չ��Ҫ���´�д�������ǩ�ĺ����Ű����жϰ������鷳 ����������ܾͲ���ʶ��
				//���ĸ�������Ҫ�����ǩ ����alloc �����ڴ� ���������� �ܵ���˵��������Ѷ���ı�ǩ����ַʹ��
				
				for (size_t j = 0; j < _FunAddList.size(); j++) {
					if (callName == _FunAddList[j].funName && _FunAddList[j].isDefLabel) {
						//������Ҫ�����ǩ�ˣ��ǲ���������Ͳ���С������ ����1����ǩ������2������
						if (arg.size() < 2) {
							_error.push_back("����" + to_string(_codeList[i].line) + "��:��������̫��");
							continue;
						}
						//����ǩ�����Ƿ��ѱ�ռ��
						if (labelIsDef(arg[0])) {
							_error.push_back("����" + to_string(_codeList[i].line) + "��:��ǩ" + arg[0] + "�Ѿ���ʹ��");
							continue;
						}

						//������ı�ǩ���뵽��ַ��ǩ�б�_labelAddList
						_labelAddList.push_back({ arg[0], 0 });
					}
				}
				
				_runCodeList.push_back({ r_libcall ,_codeList[i].inType,libcall ,arg,{} });
				continue;
			}
			else {
				//û�ҵ�����
				_error.push_back("����" + to_string(_codeList[i].line) + "��:δ����ĺ���" + callName);
				continue;//���ﻹ����ִ�У���ΪҪ�������еĴ���
			}
			
		}
		//����Ǳ�ǩ����д���ַ���ݾ���ô����
		if (_codeList[i].type == e_address) {
			//�������Ǳ�ǩ���ǵ�ַ�����������ͨ��ǩ�б����ҵ�������ͨ��ǩ����Ȼ�Ͷ����ַ����
			bool isLabel = false;
			size_t t_off = 0;
			for (size_t j = 0; j < _labelList.size(); j++) {
				if (_codeList[i].strCode == _labelList[j].name) {
					isLabel = true;
					t_off = j;
					_labelList[j].count++;//��ǩʹ�ô���+1
					break;
				}
			}
			if (isLabel) {
				//��ͨ��ǩִ������
				if (_isAddasm) {
					//���д�����ʵ�Ѿ�����Ҫ����ˣ���Ϊ��ͨ��ǩ��ַһ���������������û��Ҫ����ӵ�����������
					//_runCodeList[_runCodeList.size() - 1].strcode.push_back(_codeList[i].strCode);
					//����ͨ��ǩ��ӵ�ַ��ƫ��
					if (AddressOfPartAddr == 0) {
						//��ǰΪ��ַ��ǩ��
						_labelList[t_off].off = labelOff;
						_labelList[t_off].AddressOfPartName = AddressOfPartName;
					}
					else {
						//��ǰΪֱ�ӵ�ַ��

						//_labelList[t_off].off = labelOff;//��Ȼûɶ�ã����Է���һ
						_labelList[t_off].address = AddressOfPartAddr + labelOff;//����Ϳ���ֱ�Ӽ������ͨ��ǩ�ĵ�ַ
					}
				}
				else {
					_error.push_back("����" + to_string(_codeList[i].line) + "��:�����﷨�����㲻��������д������");
				}
	
			}
			else {
				//��ַ��ǩִ������
				//������Ϊ��û�����ڴ��ַ��������ʱ�Ѵ����ַ�ı�ǩ��ŵ�strcode[0]��
				
				if (labelIsDef(_codeList[i].strCode)) {
					_runCodeList.push_back({ r_writeData ,_codeList[i].inType,nullptr ,{_codeList[i].strCode},{} });
					_isAddasm = true;
					AddressOfPartName = _codeList[i].strCode;//���ε�ַ��ǩ�����ƴ��������
					AddressOfPartAddr = 0;
					labelOff = 0;//�����µ�ַ �Ͱ�ƫ�Ƹ�Ϊ0
					continue;
				}
				//��֤������ַ�Ƿ�����
				void* address = ReadMemoryEX(_pid, _codeList[i].strCode);
				if (address == nullptr) {
					_error.push_back("����" + to_string(_codeList[i].line) + "��:�Ƿ���ַ"+ _codeList[i].strCode);
				}
				else {
					_runCodeList.push_back({ r_writeData ,_codeList[i].inType,address ,{},{} });
					_isAddasm = true;
					AddressOfPartAddr = (int)address;//���ε�ַ������������ֱ�Ӽ�����ͨ��ǩ�ĵ�ַ��
					AddressOfPartName = "";
					labelOff = 0;//�����µ�ַ �Ͱ�ƫ�Ƹ�Ϊ0
				}
			}
		}
		if (_codeList[i].type == e_asm || _codeList[i].type == e_difCode) {
			if (_isAddasm) {
				_runCodeList[_runCodeList.size() - 1].strcode.push_back(_codeList[i].strCode);
			}
			else {
				_error.push_back("����" + to_string(_codeList[i].line) + "��:�����﷨�����㲻��������д������");
			}
		}
		//���������Ƿ�����
		if (_codeList[i].type == e_asm) {
			//size_t pos;
			string temp = _codeList[i].strCode;;
			//����ַ��ǩ
			//�滻���еĵ�ַ��ǩ
			for (size_t j = 0; j < _labelAddList.size(); j++) {
				asm_replace(temp, _labelAddList[j].name, "1000000");//ǧ���ܸ����������Ϊ��ռλ�õģ����˼�����ĵ�ַ�Ͳ�׼ȷ��
			}
			//�����ͨ��ǩ
			//�滻���е���ͨ��ǩ
			for (size_t j = 0; j < _labelList.size(); j++) {
				asm_replace(temp, _labelList[j].name, "00");//ǧ���ܸ����������Ϊ��ռλ�õģ����˼�����ĵ�ַ�Ͳ�׼ȷ��
			}
			ByteArray t_ByArr;
			size_t aaa = 0;
			aaa = StrAsmToByArr(temp, t_ByArr, 0);//���ֻ�����⣬����ʵ��ת��
			//cout << temp << "   ����:" << aaa << " code:" << t_ByArr << endl;
			labelOff += aaa;
			if (aaa == 0) {//ת��ʧ��
				_error.push_back("����" + to_string(_codeList[i].line) + "��:���������");
			}
		}
		//����û����ϸ������Щ����������Ϊ�˼���ƫ����
		if (_codeList[i].type == e_difCode) {
			//Ŀ�ľ��ǰ�ƫ��������ȥ
			vector<string> strArr = splitString(_codeList[i].strCode, ' ');
			if (strArr.empty()) {
				_error.push_back("����" + to_string(_codeList[i].line) + "��:��Ч����->" + _codeList[i].strCode);
				continue;
			}
			if (strArr[0] == "db") {
				labelOff += (strArr.size() - 1) * 1;
				continue;
			}
			if (strArr[0] == "dw") {
				labelOff += (strArr.size() - 1) * 2;
				continue;
			}
			//strcmp(strArr[0].data(), "dd")
			if (strArr[0] == "dd") {
				labelOff += (strArr.size() - 1) * 4;
				continue;
			}
			if (strArr[0] == "dq") {
				labelOff += (strArr.size() - 1) * 8;
				continue;
			}

			//labelOff = 
		}
	}

	//ִ�е�����������﷨����Ѿ�ͨ���ˣ��������͵��ټ������������
	//����ͳ������ͨ��ǩ��ʹ�ô������������� ����(���û��ʹ�õı�ǩ���ᱻɾ��)
	for (size_t j = 0; j < _labelList.size(); j++) {
		if (_labelList[j].count == 0) {
			_labelList.erase(_labelList.begin() + j);
			j--;
		}
	}
	_IsRightful = _error.empty();
	return _error.empty();
}
bool SCEAA::labelIsDef(const string& name) {
	for (size_t i = 0; i < _labelList.size(); i++) {
		if (name == _labelList[i].name) {
			return true;
		}
	}
	for (size_t i = 0; i < _labelAddList.size(); i++) {
		if (name == _labelAddList[i].name) {
			return true;
		}
	}
	return false;
}
bool SCEAA::runScript(bool turn) {
	if (!_IsRightful) {
		if (!IsRightful()) {
			return false;
		}
	}
	for (size_t i = 0; i < _runCodeList.size(); i++) {
		//����ѵ�ǰִ�еĶ����ݿ���һ�ݣ���Ϊ֮���޸�������У��ڶ��������е�ʱ����Ҫ�����﷨�����˾�
		//�������е�ʱ�������Ǳ䶯�ģ����ܰѽ����õ��������޸���
		struct_code runcodedata = _runCodeList[i];
		if (runcodedata.inType == e_turn_en + !turn) {
			if (runcodedata.type == r_libcall) {
				//cout << "���õĺ���:" << _runCodeList[i].address <<endl;
				((void (*)(SCEAA&, vector<string>&))runcodedata.address)(*this, runcodedata.strcode);
				continue;
			}


			if (runcodedata.type == r_writeData) {
				//����д��ĵ�ַ(��Ե�ַ��ǩ) ��Ϊһ��ֻ��һ��д���ַ������ÿ�ζ�Ҫִ��һ�Σ�����һ��ȫ���붼������
				if (runcodedata.address == nullptr) {
					void* proAddress = nullptr;
					//˵���Ǳ�ǩ��ַ���ڵ�ַ��ǩ���Ҿ���
					proAddress = (void*)Get_labelAddListData(runcodedata.strcode[0]);
					//ɾ����һ��Ԫ�� ���Ǳ�ǩ����
					runcodedata.strcode.erase(runcodedata.strcode.begin());
					runcodedata.address = proAddress;
					/*
					for (size_t j = 0; j < _labelAddList.size(); j++) {
						//�����ַΪ�� Ĭ�ϵ�һ���ַ���������Ǳ�ǩ����
						if (_labelAddList[j].name == runcodedata.strcode[0]) {
							proAddress = (void*)_labelAddList[j].address;
							//ɾ����һ��Ԫ�� ���Ǳ�ǩ����
							runcodedata.strcode.erase(runcodedata.strcode.begin());
							runcodedata.address = proAddress;
							break;
						}
					}*/
					if (proAddress == nullptr) {
						_error.push_back("����ʱ����," + _runCodeList[i].strcode[0] + "��ַΪ�գ���ǿ����ִֹ��");
						return false;
					}
				}
				//ִ�е������ʱ��ǰ���д��ĵ�ַ�Ѿ���ȷ����
				// ��Ȼǰ��ĵ�ַȷ���ˣ��Ϳ��Կ�ʼ������ͨ��ǩ�ĵ�ַ��
				for (size_t z = 0; z < _labelList.size(); z++) {
					if(_labelList[z].address == 0){
						_labelList[z].address = (size_t)Get_labelAddListData(_labelList[z].AddressOfPartName)+ _labelList[z].off;
						
					}
				}
				//�����������ݿ�ʼ����������������������������������������������������
				//ֱ���Ȱ��ı���������еĻ�����ĵ�ַ��ǩ�ı�ǩ�������ɾ����ַ
				size_t a_off = 0;//���㵱ǰ��ַ��ƫ����  �ε�ַ+ƫ�� = ��ǰ��ַ
				for (size_t j = 0; j < runcodedata.strcode.size(); j++) {
					ByteArray a_temp;
					//�Ƿ�Ϊdefcode
					if (runcodedata.strcode[j][0] == 'd') {
						a_off += difCodeToByArr(runcodedata.strcode[j],a_temp);
						runcodedata.byArr += a_temp;
						continue;
					}

					//�滻��ַ��ǩ
					for (size_t z = 0; z < _labelAddList.size(); z++) {
						asm_replace(runcodedata.strcode[j], _labelAddList[z].name, intToHexStr((int)_labelAddList[z].address));
					}
					//�滻��ͨ��ǩ
					for (size_t z = 0; z < _labelList.size(); z++) {
						asm_replace(runcodedata.strcode[j], _labelList[z].name, intToHexStr((int)_labelList[z].address));
					}
					//�滻��ֱ��ת��
					
					a_off+=StrAsmToByArr(runcodedata.strcode[j], a_temp,(int)runcodedata.address + a_off);
					runcodedata.byArr += a_temp;
					//cout << runcodedata.strcode[j] << "->" << a_temp << endl;
					
				}
				//һ�о�����ֱ��д��
				WriteMemory(_pid, runcodedata.address, runcodedata.byArr);
				//cout << runcodedata.address << "-----" << runcodedata.byArr << endl;
			}
			//cout << "ִ�е�����:" << _runCodeList[i].inType << endl;
		}
	}
	return true;
}
void SCEAA::loadScript(string script) {
	_codeList.clear();
	_IsRightful = false;
	//ɾ������ע��
	std::regex blockCommentRegex("/\\*.*?\\*/");
	script = std::regex_replace(script, blockCommentRegex, "");

	vector<string> code = splitString(script,'\n');
	int Labelindex = 0;
	string labelset;//����ӵı�ǩ�ļ��� ��ǩ������ڿ�ʼ��ʱ��������
	auto addLable = [&Labelindex]() {
		Labelindex++;
		return "InComman" + to_string(Labelindex);
	};//����һ����ǩ

	auto GetAsmAndOpe = [](const string&str,string&col,string& op,string&cor) {
		std::regex pattern("([a-zA-Z0-9_]+)\\s*([<=>!]+)\\s*([a-zA-Z0-9_]+)");
		std::smatch matches;
		if (std::regex_search(str, matches, pattern)) {
			if (matches.size() == 4) {
				col = matches[1].str();
				op = matches[2].str();
				cor = matches[3].str();
			}
		}
		return;
	};//����һ����ǩ

	auto OperatorToCode = [](const string& op)->string {
		if (op == "<=") {
			return "jg";
		}
		if (op == ">=") {
			return "jl";
		}
		if (op == "==") {
			return "jne";
		}
		if (op == "!=") {
			return "je";
		}
		if (op == "<") {
			return "jge";
		}
		if (op == ">") {
			return "jle";
		}
		return "";
	};

	stack<string> opstack;

	for (size_t i = 0; i < code.size(); i++) {
		//�ж���û��ע��
		size_t index = code[i].find("//");
		if (index != std::string::npos) {code[i] = code[i].substr(0, index);}//���˵�ע��
		code[i] = DelFroNull(code[i]);//ɾ��ǰ��Ŀ�
		//_labelList.push_back();
		if (code[i] != "" && code[i][0] == '.') {//�Զ������չ����
			if (code[i].substr(1, 2) == "if") {
				string col;
				string op;
				string cor;
				GetAsmAndOpe(code[i].substr(3, code[i].size()-1), col, op, cor);
				Addline("cmp "+ col+","+ cor);
				string aaalabel = addLable();
				Addline(OperatorToCode(op) + " "+aaalabel, false);
				opstack.push(aaalabel);//����ǩ���뵽ջ
				labelset+=","+aaalabel;//���뵽��ǩ�б�
				continue;
			}
			if (code[i].substr(1, 5) == "endif") {
				Addline(opstack.top()+":");
				opstack.pop();
			}
			if (code[i].substr(1, 4) == "else") {
				string aaalabel = addLable();
				labelset += "," + aaalabel;//���뵽��ǩ�б�
				Addline("jmp " + aaalabel);
				Addline(opstack.top()+":", false);
				opstack.pop();
				opstack.push(aaalabel);//���±�ǩ���뵽ջ
			}
		}else {
			Addline(code[i]);
		}
	}
	if(labelset!="")
	_codeList.insert(_codeList.begin(), { 0, e_turn_en, e_callLib,"label("+ labelset.substr(1, labelset.size() - 1) + ")" , {}});
}
void asm_replace(string& asmcode, string& replStr, string str) {
	std::regex pattern("[^A-Za-z0-9\u4E00-\u9FA5]" + replStr + "([^A-Za-z0-9\u4E00-\u9FA5]|$)");
	std::smatch match;
	if (std::regex_search(asmcode, match, pattern)) {
			asmcode.replace(match.position() + 1, replStr.length(), str);
	}
}
string intToHexStr(int num) {
	std::stringstream ss;
	ss << std::hex << num; // ��������ʮ�����Ƹ�ʽд�� stringstream
	std::string hex_string = ss.str(); // �� stringstream �л�ȡ�ַ���
	return hex_string;
}
size_t SCEAA::StrAsmToByArr(const string& str, ByteArray& ByArr, size_t addr) {
	XEDPARSE xed = { 0 };
	xed.x64 = FALSE; 
	xed.cip = addr;
	ByArr = {};
	memcpy(xed.instr, str.data(), str.size() + 1);//+��ĩβ��0˳��Ҳ��������
	if (XEDPARSE_OK != XEDParseAssemble(&xed)) {
		_error.push_back(xed.error);
		return 0;
	}
	for (size_t i = 0; i < xed.dest_size; i++) {
		ByArr += {xed.dest[i]};
	}
	return xed.dest_size;
}


//��������Զ���⺯��������������������������������������������������������������
void lib_label(SCEAA& obj, vector<string>& arg) {}//���������ֹ�޸�
void SCEAA::initFunAddList() {
	//���뺯�����ƣ�������ַ������������������
	_FunAddList.push_back({ "label", &lib_label,true});//�����������ʵ�ʵ��õ����⺯������Ϊ��ֻ����ǩ���������
	//�������������ֹ�޸�
	//���һ��boolָ������������Ƿ���Ҫ�����µ�ַ��ǩ ���Ϊtrue ��ô��������Ĳ����Ͳ���С������
	_FunAddList.push_back({ "alloc", &lib_alloc,true});
	_FunAddList.push_back({ "�����ڴ�", &lib_alloc,true });
	_FunAddList.push_back({ "dealloc", &lib_dealloc,false });
	_FunAddList.push_back({ "�ͷ��ڴ�", &lib_dealloc,false });
	_FunAddList.push_back({ "aobscan", &lib_aobscan,true });
	_FunAddList.push_back({ "����������", &lib_aobscan,true });
	_FunAddList.push_back({ "createthread", &lib_createthread,false });
	_FunAddList.push_back({ "�����߳�", &lib_createthread,false });
	_FunAddList.push_back({ "getmodfunaddr", &lib_getmodfunaddr,true });
	_FunAddList.push_back({ "ȡdll������ַ", &lib_getmodfunaddr,true });
}

void lib_alloc(SCEAA &obj, vector<string>& arg){
	
	//obj.add_labelAddList(arg[0],(void*)0x00A30000);
	if (arg.size() == 2 && isNumber(arg[1])) {
		//���жϲ������Ƿ�Ϊ������
		LPVOID a = allocProcess(obj._pid, stringToInt(arg[1]));
		obj.add_labelAddList(arg[0], (void*)a);
		//cout << "�������lib_alloc:" << arg[0] <<"  ��ַ��" << a << endl;
	}
}

void lib_dealloc(SCEAA &obj, vector<string>& arg) {
	if (arg.empty())return;
	LPVOID add = obj.Get_labelAddListData(arg[0]);
	freeProcess(obj._pid, add);
	//cout << "�������lib_dealloc:" << arg[0] << "  ��ַ��" << add << endl;
}

//����������  �������Ѻ󱣴��ַ�ı�ǩ�������룬ָ����ģ��(��д�������������ȫ���ڴ�)
void lib_aobscan(SCEAA& obj, vector<string>& arg) {
	if (arg.size() >= 2) {
		//���жϲ������Ƿ�Ϊ������
		unsigned long firstaddr = 0x00401000;
		unsigned long endaddr = 0xFFFFFFFF;
		if (arg.size() >= 3) {
			firstaddr = GetProcessModuleBaseAddress(obj._pid, arg[2]);
			endaddr = firstaddr + GetModuleSize(obj._pid, arg[2]);
		}
		vector<unsigned __int64> ResultArray;
		SearchMemory(obj._pid, arg[1], firstaddr, endaddr, 30, ResultArray);
		//��ӵ�ַ��ǩ
		if(ResultArray.size()>0){
			obj.add_labelAddList(arg[0], (void*)ResultArray[0]);//���ѵ��ĵ�һ����ַ��ֵ
			//cout << "�������lib_aobscan:" << arg[0] << "  ��ַ��" << ResultArray[0] << endl;
		}
	}
	//cout << "�������lib_aobscan:��"<< endl;
}
//����һ���߳�  ����������ĵ�ַ�ı�ǩ����
void lib_createthread(SCEAA& obj, vector<string>& arg) {
	if (arg.empty())return;
	LPVOID add = obj.Get_labelAddListData(arg[0]);
	HANDLE A = CreateProThread(obj._pid, add);
	//cout << "�������lib_createthread:" << arg[0] << "  ��ַ��"<< A<< endl;
}
//��ȡָ����dll�ĺ�����ַ��Ȼ�󱣴浽��ǩ ������:�ѵ�ַ���浽�ı�ǩ��dll���ƣ���������
void lib_getmodfunaddr(SCEAA& obj, vector<string>& arg) {
	if (arg.size()<3)return;
	void* funaddress = GetProFunAddress(obj._pid, arg[1], arg[2]);
	obj.add_labelAddList(arg[0], funaddress);
	//cout << "�������lib_getmodfunaddr:" << arg[0] << "  ��ַ��" << funaddress << endl;
}
#include"SCEAA.h"
#include "pch.h"
void PrintOpCode(char* pAsm, unsigned char* pOpcode, int nSize) {
	for (int i = 0; i < nSize; ++i) {
		printf("%02X ", pOpcode[i]);
	}
	printf("%30s \n", pAsm);
}

//自定义的函数给地址标签添加数据的时候调用这个就行 添加成功返回真
bool SCEAA::add_labelAddList(string labelname, void* a_address) {
	for (size_t i = 0; i < _labelAddList.size(); i++) {
		if (_labelAddList[i].name == labelname) {
			_labelAddList[i].address = a_address;
			return true;
		}
	}
	return false;
}
//通过名称取地址标签地址
void* SCEAA::Get_labelAddListData(const string& name) {
	for (size_t i = 0; i < _labelAddList.size(); i++) {
		if (_labelAddList[i].name == name) {
			return _labelAddList[i].address;
		}
	}
	return nullptr;
}
//通过名称取普通标签地址
size_t SCEAA::Get_labelListData(const string& name) {
	for (size_t i = 0; i < _labelList.size(); i++) {
		if (_labelList[i].name == name) {
			return _labelList[i].address;
		}
	}
	return 0;
}
//将difCode（dd db dw dq）转换成字节集
size_t SCEAA::difCodeToByArr(const string& str, ByteArray& ByArr) {
	vector<string> strArr = splitString(str, ' ');
	if (strArr.empty()) {
		_error.push_back("运行时错误:无效代码->" + str);
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
	if(isAddLineCount)_line++;//行号+1
	
	code = DelFroEndNull(code);
	if (code.find_first_of(" \t\r\n") == 0 || code=="")return;//删除空行

	//两个开关标识就不用再添加一行代码了
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

	//如果最后一个字符为:说明这个是address 
	if (code[code.size() - 1] == ':') {
		code.erase(code.size() - 1);
		_codeList.push_back({ _line,_inType, e_address ,code ,{} });
		return;
	}

	//如果第一个字符不是.且最后一个字符是)说明是内置函数调用
	if (code[0] != '.' && code[code.size() - 1] == ')') {
		//这里直接解析了调用内置函数的函数名，参数等 name = callLibName vector = 参数
		_codeList.push_back({ _line, _inType,e_callLib,code,{} });
		return;
	}
	//说明是db 这种格式的
	if (code[0] == 'd') {
		_codeList.push_back({ _line, _inType,e_difCode,code,{} });
		return;
	}

	//剩下的都按汇编代码处理
	_codeList.push_back({ _line, _inType,e_asm,code,{} });
	return;
}
void SCEAA::Print_line() {
	for (const auto& a : _codeList) {
		cout << "line：" << a.line <<
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

	bool _isAddasm = false;//是否添加汇编代码 这里就是给最后_runCodeList添加的
	//要不当前为标签，要不为地址，只能同时存在一个
	string AddressOfPartName;//用来计算普通标签的偏移的
	size_t AddressOfPartAddr = 0;//用来计算普通标签的偏移的(如果是直接性地址都不需要存放地址标签，直接计算地址即可)
	size_t labelOff = 0;//普通标签地址的偏移量


	if (_codeList.empty()) {
		_error.push_back("错误:代码为空");
		return false;
	}

	if (_pid == -1) {
		_error.push_back("错误:无效的进程id");
	}

	for (size_t i = 0; i < _codeList.size(); i++) {
		//cout << "代码：" << _codeList[i].strCode << endl;
		//如果是调用的内置call就这么处理
		if (_codeList[i].type == e_callLib) {
			_isAddasm = false;
			size_t found = _codeList[i].strCode.find('(');
			string callName = _codeList[i].strCode.substr(0, found);
			vector<string> arg = splitString(_codeList[i].strCode.substr(found + 1, _codeList[i].strCode.size() - found - 2), ',');;
			void* libcall = nullptr;//call的地址
			for (size_t j = 0; j < _FunAddList.size(); j++) {
				if (callName == _FunAddList[j].funName) {
					libcall = _FunAddList[j].address;
					break;
				}
			}
			if (libcall != nullptr) {
				//这里就可以执行下内置函数的功能了，对于有些功能 不能真正执行函数代码的的时候才执行，比如标签相关的函数
				//特殊的函数 label
				if (callName == "label") {
					for (size_t j = 0; j < arg.size(); j++) {
						//检测标签名称是否已被占用
						if (labelIsDef(arg[j])) {
							_error.push_back("错误" + to_string(_codeList[i].line) + "行:标签" + arg[j] + "已经被使用");
							//continue;//这里还继续执行把，让他把所有的错误都找出来
						}
						else {
							_labelList.push_back({ arg[j] ,0,0,"",0 });
						}
					}
					continue;//这个命令甚至最后都不需要执行专门的函数
				}
				//这么写的好处就是好扩展，要不下次写个定义标签的函数才挨个判断啊，真麻烦 几天后代码可能就不认识了
				//看哪个函数需要定义标签 例如alloc 申请内存 特征搜索等 总的来说就是最后会把定义的标签当地址使用
				
				for (size_t j = 0; j < _FunAddList.size(); j++) {
					if (callName == _FunAddList[j].funName && _FunAddList[j].isDefLabel) {
						//都申明要定义标签了，那参数个数最低不能小于两个 参数1：标签，参数2：操作
						if (arg.size() < 2) {
							_error.push_back("错误" + to_string(_codeList[i].line) + "行:函数参数太少");
							continue;
						}
						//检测标签名称是否已被占用
						if (labelIsDef(arg[0])) {
							_error.push_back("错误" + to_string(_codeList[i].line) + "行:标签" + arg[0] + "已经被使用");
							continue;
						}

						//将定义的标签加入到地址标签列表_labelAddList
						_labelAddList.push_back({ arg[0], 0 });
					}
				}
				
				_runCodeList.push_back({ r_libcall ,_codeList[i].inType,libcall ,arg,{} });
				continue;
			}
			else {
				//没找到函数
				_error.push_back("错误" + to_string(_codeList[i].line) + "行:未定义的函数" + callName);
				continue;//这里还接着执行，因为要检测出所有的错误
			}
			
		}
		//如果是标签或者写入地址数据就这么处理
		if (_codeList[i].type == e_address) {
			//先区分是标签还是地址（如果能在普通标签列表里找到就是普通标签，不然就都算地址处理）
			bool isLabel = false;
			size_t t_off = 0;
			for (size_t j = 0; j < _labelList.size(); j++) {
				if (_codeList[i].strCode == _labelList[j].name) {
					isLabel = true;
					t_off = j;
					_labelList[j].count++;//标签使用次数+1
					break;
				}
			}
			if (isLabel) {
				//普通标签执行这里
				if (_isAddasm) {
					//这行代码其实已经不需要添加了，因为普通标签地址一旦被计算出来，就没必要再添加到汇编代码中了
					//_runCodeList[_runCodeList.size() - 1].strcode.push_back(_codeList[i].strCode);
					//给普通标签添加地址和偏移
					if (AddressOfPartAddr == 0) {
						//当前为地址标签段
						_labelList[t_off].off = labelOff;
						_labelList[t_off].AddressOfPartName = AddressOfPartName;
					}
					else {
						//当前为直接地址段

						//_labelList[t_off].off = labelOff;//虽然没啥用，但以防万一
						_labelList[t_off].address = AddressOfPartAddr + labelOff;//这里就可以直接计算出普通标签的地址
					}
				}
				else {
					_error.push_back("错误" + to_string(_codeList[i].line) + "行:严重语法错误，你不该在这里写这句代码");
				}
	
			}
			else {
				//地址标签执行这里
				//这里因为还没申请内存地址。所以暂时把代表地址的标签存放到strcode[0]里
				
				if (labelIsDef(_codeList[i].strCode)) {
					_runCodeList.push_back({ r_writeData ,_codeList[i].inType,nullptr ,{_codeList[i].strCode},{} });
					_isAddasm = true;
					AddressOfPartName = _codeList[i].strCode;//将段地址标签的名称存放在这里
					AddressOfPartAddr = 0;
					labelOff = 0;//遇到新地址 就把偏移改为0
					continue;
				}
				//验证正常地址是否正常
				void* address = ReadMemoryEX(_pid, _codeList[i].strCode);
				if (address == nullptr) {
					_error.push_back("错误" + to_string(_codeList[i].line) + "行:非法地址"+ _codeList[i].strCode);
				}
				else {
					_runCodeList.push_back({ r_writeData ,_codeList[i].inType,address ,{},{} });
					_isAddasm = true;
					AddressOfPartAddr = (int)address;//将段地址存放在这里（可以直接计算普通标签的地址）
					AddressOfPartName = "";
					labelOff = 0;//遇到新地址 就把偏移改为0
				}
			}
		}
		if (_codeList[i].type == e_asm || _codeList[i].type == e_difCode) {
			if (_isAddasm) {
				_runCodeList[_runCodeList.size() - 1].strcode.push_back(_codeList[i].strCode);
			}
			else {
				_error.push_back("错误" + to_string(_codeList[i].line) + "行:严重语法错误，你不该在这里写这句代码");
			}
		}
		//检测汇编代码是否正常
		if (_codeList[i].type == e_asm) {
			//size_t pos;
			string temp = _codeList[i].strCode;;
			//检测地址标签
			//替换所有的地址标签
			for (size_t j = 0; j < _labelAddList.size(); j++) {
				asm_replace(temp, _labelAddList[j].name, "1000000");//千万不能改这个，这是为了占位用的，改了计算出的地址就不准确了
			}
			//检测普通标签
			//替换所有的普通标签
			for (size_t j = 0; j < _labelList.size(); j++) {
				asm_replace(temp, _labelList[j].name, "00");//千万不能改这个，这是为了占位用的，改了计算出的地址就不准确了
			}
			ByteArray t_ByArr;
			size_t aaa = 0;
			aaa = StrAsmToByArr(temp, t_ByArr, 0);//这边只负责检测，并不实际转换
			//cout << temp << "   长度:" << aaa << " code:" << t_ByArr << endl;
			labelOff += aaa;
			if (aaa == 0) {//转换失败
				_error.push_back("错误" + to_string(_codeList[i].line) + "行:汇编语句错误");
			}
		}
		//这里没有详细解析这些东西，就是为了计算偏移量
		if (_codeList[i].type == e_difCode) {
			//目的就是把偏移量加上去
			vector<string> strArr = splitString(_codeList[i].strCode, ' ');
			if (strArr.empty()) {
				_error.push_back("错误" + to_string(_codeList[i].line) + "行:无效代码->" + _codeList[i].strCode);
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

	//执行到这里基础的语法检查已经通过了，接下来就得再检查汇编代码的问题
	//这里统计下普通标签的使用次数，操作随便吧 比如(如果没被使用的标签将会被删除)
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
		//这里把当前执行的段数据拷贝一份，因为之后修改这个就行，第二次再运行的时候不需要进行语法解析了就
		//但是运行的时候数据是变动的，不能把解析好的数据再修改了
		struct_code runcodedata = _runCodeList[i];
		if (runcodedata.inType == e_turn_en + !turn) {
			if (runcodedata.type == r_libcall) {
				//cout << "调用的函数:" << _runCodeList[i].address <<endl;
				((void (*)(SCEAA&, vector<string>&))runcodedata.address)(*this, runcodedata.strcode);
				continue;
			}


			if (runcodedata.type == r_writeData) {
				//计算写入的地址(针对地址标签) 因为一段只有一个写入地址，所以每段都要执行一次，不是一下全代码都缓过来
				if (runcodedata.address == nullptr) {
					void* proAddress = nullptr;
					//说明是标签地址，在地址标签表找就行
					proAddress = (void*)Get_labelAddListData(runcodedata.strcode[0]);
					//删除第一个元素 就是标签名称
					runcodedata.strcode.erase(runcodedata.strcode.begin());
					runcodedata.address = proAddress;
					/*
					for (size_t j = 0; j < _labelAddList.size(); j++) {
						//如果地址为空 默认第一个字符串代码就是标签名称
						if (_labelAddList[j].name == runcodedata.strcode[0]) {
							proAddress = (void*)_labelAddList[j].address;
							//删除第一个元素 就是标签名称
							runcodedata.strcode.erase(runcodedata.strcode.begin());
							runcodedata.address = proAddress;
							break;
						}
					}*/
					if (proAddress == nullptr) {
						_error.push_back("运行时错误," + _runCodeList[i].strcode[0] + "地址为空，已强制终止执行");
						return false;
					}
				}
				//执行到这里的时候，前面的写入的地址已经都确定了
				// 既然前面的地址确定了，就可以开始计算普通标签的地址了
				for (size_t z = 0; z < _labelList.size(); z++) {
					if(_labelList[z].address == 0){
						_labelList[z].address = (size_t)Get_labelAddListData(_labelList[z].AddressOfPartName)+ _labelList[z].off;
						
					}
				}
				//正常处理数据开始——————————————————————————
				//直接先把文本汇编中所有的汇编代码的地址标签的标签名都换成具体地址
				size_t a_off = 0;//计算当前地址的偏移量  段地址+偏移 = 当前地址
				for (size_t j = 0; j < runcodedata.strcode.size(); j++) {
					ByteArray a_temp;
					//是否为defcode
					if (runcodedata.strcode[j][0] == 'd') {
						a_off += difCodeToByArr(runcodedata.strcode[j],a_temp);
						runcodedata.byArr += a_temp;
						continue;
					}

					//替换地址标签
					for (size_t z = 0; z < _labelAddList.size(); z++) {
						asm_replace(runcodedata.strcode[j], _labelAddList[z].name, intToHexStr((int)_labelAddList[z].address));
					}
					//替换普通标签
					for (size_t z = 0; z < _labelList.size(); z++) {
						asm_replace(runcodedata.strcode[j], _labelList[z].name, intToHexStr((int)_labelList[z].address));
					}
					//替换后直接转换
					
					a_off+=StrAsmToByArr(runcodedata.strcode[j], a_temp,(int)runcodedata.address + a_off);
					runcodedata.byArr += a_temp;
					//cout << runcodedata.strcode[j] << "->" << a_temp << endl;
					
				}
				//一切就绪，直接写入
				WriteMemory(_pid, runcodedata.address, runcodedata.byArr);
				//cout << runcodedata.address << "-----" << runcodedata.byArr << endl;
			}
			//cout << "执行的类型:" << _runCodeList[i].inType << endl;
		}
	}
	return true;
}
void SCEAA::loadScript(string script) {
	_codeList.clear();
	_IsRightful = false;
	//删除多行注释
	std::regex blockCommentRegex("/\\*.*?\\*/");
	script = std::regex_replace(script, blockCommentRegex, "");

	vector<string> code = splitString(script,'\n');
	int Labelindex = 0;
	string labelset;//待添加的标签的集合 标签必须得在开始的时候加入才行
	auto addLable = [&Labelindex]() {
		Labelindex++;
		return "InComman" + to_string(Labelindex);
	};//生成一个标签

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
	};//生成一个标签

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
		//判断有没有注释
		size_t index = code[i].find("//");
		if (index != std::string::npos) {code[i] = code[i].substr(0, index);}//过滤掉注释
		code[i] = DelFroNull(code[i]);//删除前面的空
		//_labelList.push_back();
		if (code[i] != "" && code[i][0] == '.') {//自定义的扩展命令
			if (code[i].substr(1, 2) == "if") {
				string col;
				string op;
				string cor;
				GetAsmAndOpe(code[i].substr(3, code[i].size()-1), col, op, cor);
				Addline("cmp "+ col+","+ cor);
				string aaalabel = addLable();
				Addline(OperatorToCode(op) + " "+aaalabel, false);
				opstack.push(aaalabel);//将标签加入到栈
				labelset+=","+aaalabel;//加入到标签列表
				continue;
			}
			if (code[i].substr(1, 5) == "endif") {
				Addline(opstack.top()+":");
				opstack.pop();
			}
			if (code[i].substr(1, 4) == "else") {
				string aaalabel = addLable();
				labelset += "," + aaalabel;//加入到标签列表
				Addline("jmp " + aaalabel);
				Addline(opstack.top()+":", false);
				opstack.pop();
				opstack.push(aaalabel);//将新标签加入到栈
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
	ss << std::hex << num; // 将整数按十六进制格式写入 stringstream
	std::string hex_string = ss.str(); // 从 stringstream 中获取字符串
	return hex_string;
}
size_t SCEAA::StrAsmToByArr(const string& str, ByteArray& ByArr, size_t addr) {
	XEDPARSE xed = { 0 };
	xed.x64 = FALSE; 
	xed.cip = addr;
	ByArr = {};
	memcpy(xed.instr, str.data(), str.size() + 1);//+把末尾的0顺便也拷贝过来
	if (XEDPARSE_OK != XEDParseAssemble(&xed)) {
		_error.push_back(xed.error);
		return 0;
	}
	for (size_t i = 0; i < xed.dest_size; i++) {
		ByArr += {xed.dest[i]};
	}
	return xed.dest_size;
}


//这里可以自定义库函数——————————————————————————————
void lib_label(SCEAA& obj, vector<string>& arg) {}//这个函数禁止修改
void SCEAA::initFunAddList() {
	//传入函数名称，函数地址，函数参数个数即可
	_FunAddList.push_back({ "label", &lib_label,true});//这个函数不是实际调用的特殊函数，因为他只给标签表添加数据
	//上面这个函数禁止修改
	//最后一个bool指的是这个函数是否需要定义新地址标签 如果为true 那么这个函数的参数就不能小于两个
	_FunAddList.push_back({ "alloc", &lib_alloc,true});
	_FunAddList.push_back({ "申请内存", &lib_alloc,true });
	_FunAddList.push_back({ "dealloc", &lib_dealloc,false });
	_FunAddList.push_back({ "释放内存", &lib_dealloc,false });
	_FunAddList.push_back({ "aobscan", &lib_aobscan,true });
	_FunAddList.push_back({ "搜索特征码", &lib_aobscan,true });
	_FunAddList.push_back({ "createthread", &lib_createthread,false });
	_FunAddList.push_back({ "创建线程", &lib_createthread,false });
	_FunAddList.push_back({ "getmodfunaddr", &lib_getmodfunaddr,true });
	_FunAddList.push_back({ "取dll函数地址", &lib_getmodfunaddr,true });
}

void lib_alloc(SCEAA &obj, vector<string>& arg){
	
	//obj.add_labelAddList(arg[0],(void*)0x00A30000);
	if (arg.size() == 2 && isNumber(arg[1])) {
		//先判断参数二是否为纯数字
		LPVOID a = allocProcess(obj._pid, stringToInt(arg[1]));
		obj.add_labelAddList(arg[0], (void*)a);
		//cout << "你调用了lib_alloc:" << arg[0] <<"  地址：" << a << endl;
	}
}

void lib_dealloc(SCEAA &obj, vector<string>& arg) {
	if (arg.empty())return;
	LPVOID add = obj.Get_labelAddListData(arg[0]);
	freeProcess(obj._pid, add);
	//cout << "你调用了lib_dealloc:" << arg[0] << "  地址：" << add << endl;
}

//特征码搜索  参数表：搜后保存地址的标签，特征码，指定的模块(不写这个参数就搜索全部内存)
void lib_aobscan(SCEAA& obj, vector<string>& arg) {
	if (arg.size() >= 2) {
		//先判断参数二是否为纯数字
		unsigned long firstaddr = 0x00401000;
		unsigned long endaddr = 0xFFFFFFFF;
		if (arg.size() >= 3) {
			firstaddr = GetProcessModuleBaseAddress(obj._pid, arg[2]);
			endaddr = firstaddr + GetModuleSize(obj._pid, arg[2]);
		}
		vector<unsigned __int64> ResultArray;
		SearchMemory(obj._pid, arg[1], firstaddr, endaddr, 30, ResultArray);
		//添加地址标签
		if(ResultArray.size()>0){
			obj.add_labelAddList(arg[0], (void*)ResultArray[0]);//将搜到的第一个地址赋值
			//cout << "你调用了lib_aobscan:" << arg[0] << "  地址：" << ResultArray[0] << endl;
		}
	}
	//cout << "你调用了lib_aobscan:空"<< endl;
}
//创建一条线程  参数表：申请的地址的标签名称
void lib_createthread(SCEAA& obj, vector<string>& arg) {
	if (arg.empty())return;
	LPVOID add = obj.Get_labelAddListData(arg[0]);
	HANDLE A = CreateProThread(obj._pid, add);
	//cout << "你调用了lib_createthread:" << arg[0] << "  地址："<< A<< endl;
}
//获取指定的dll的函数地址，然后保存到标签 参数表:把地址保存到的标签，dll名称，函数名称
void lib_getmodfunaddr(SCEAA& obj, vector<string>& arg) {
	if (arg.size()<3)return;
	void* funaddress = GetProFunAddress(obj._pid, arg[1], arg[2]);
	obj.add_labelAddList(arg[0], funaddress);
	//cout << "你调用了lib_getmodfunaddr:" << arg[0] << "  地址：" << funaddress << endl;
}
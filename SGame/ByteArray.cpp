#include"ByteArray.h"
//ByteArray 字节集(字节数组)class方法实现
ostream& operator<<(ostream& os, const ByteArray& obj) {
	os << obj.Data.size();
	os << "{";
	size_t t_length = obj.Data.size();
	for (int i = 0; i < t_length; i++) {
		os << "0x" << hex << int(obj.Data[i]);
		if (i != t_length - 1) {
			os << ",";
		}
	}

	os << "}";
	return os;
}

BYTE& ByteArray::operator[](size_t index) {
	return Data[index];
}

ByteArray ByteArray::operator+(const ByteArray& other) {
	ByteArray temp(*this);
	temp.Data.insert(temp.Data.end(), other.Data.begin(), other.Data.end());
	return temp;
}

ByteArray& ByteArray::operator+=(const ByteArray& other) {
	Data.insert(Data.end(), other.Data.begin(), other.Data.end());
	return *this;
}

size_t ByteArray::GetLen() {
	return Data.size();
}

ByteArray ByteArray::GetLeft(size_t size) {
	if (size > Data.size())size = Data.size();
	ByteArray temp(size);
	memcpy(temp.Data.data(), Data.data(), size);
	return temp;
}

ByteArray ByteArray::GetRight(size_t size) {
	if (size > Data.size())size = Data.size();
	size_t t_begin = Data.size() - size;

	/*这个方法是用原生方法挨个添加，如果字节集太长时间可能会延长
	ByteArray temp;
	temp.Data.reserve(size);
	for (size_t i = 0; i < size; i++) {
		temp.Data.push_back(Data[t_begin + i]);
	}*/
	//下面这种方法是直接拷贝内存 效率肯定高
	ByteArray temp(size);
	memcpy(temp.Data.data(), Data.data() + t_begin, size);
	return temp;
}

ByteArray ByteArray::GetMid(size_t first, size_t size) {
	size_t s = Data.size();
	if (size > s || first + 1 > s)return *this;
	if (first + size > s)size = s - first;
	/*这个方法是用原生方法挨个添加，如果字节集太长时间可能会延长
	ByteArray temp;
	temp.Data.reserve(size);
	for (size_t i = 0; i < size; i++) {
		temp.Data.push_back(Data[first + i]);
	}*/
	//下面这种方法是直接拷贝内存 效率肯定高
	ByteArray temp(size);
	memcpy(temp.Data.data(), Data.data() + first, size);
	return temp;
}

int ByteArray::Find(ByteArray obj, size_t first) {
	auto it = search(Data.begin()+ first, Data.end(), obj.Data.begin(), obj.Data.end());
	if (it != Data.end()) {
		return distance(Data.begin(), it);
	}
	return -1;
}

int ByteArray::FindEnd(ByteArray obj) {
	auto it = find_end(Data.begin(), Data.end(), obj.Data.begin(), obj.Data.end());
	if (it != Data.end()) {
		return distance(Data.begin(), it);
	}
	return -1;
}

void ByteArray::Replace(size_t first, ByteArray obj) {
	if (first + 1 > Data.size() || obj.Data.empty())return;

	/*for (size_t i = 0; i < obj.Data.size(); i++) {
		Data[first + i] = obj.Data[i];
	}*/
	BYTE* p = Data.data();
	memcpy(Data.data() + first, obj.Data.data(), obj.Data.size());
	return;
}

void ByteArray::Replace_Find(ByteArray findObj, ByteArray RpObj) {
	auto it = search(Data.begin(), Data.end(), findObj.Data.begin(), findObj.Data.end());
	size_t first = 0;
	if (it == Data.end()) {
		return;//没找到直接返回
	}
	first = distance(Data.begin(), it);
	size_t f = findObj.Data.size();
	size_t r = RpObj.Data.size();
	if (r > f) {
		Data.insert(Data.begin() + first, r - f, 0);//先腾出空间
	}if (r < f) {
		Data.erase(Data.begin() + first, Data.begin() + first + (f - r));
	}
	memcpy(Data.data() + first, RpObj.Data.data(), RpObj.Data.size());

}

void ByteArray::insert(size_t first, size_t size, BYTE Value) {
	if (first < Data.size()) {
		Data.insert(Data.begin() + first, size, Value);
	}
	else {
		Data.insert(Data.end(), size, Value);
	}
}

void ByteArray::insert(size_t first, ByteArray obj) {
	if (first < Data.size()) {
		Data.insert(Data.begin() + first, obj.Data.begin(), obj.Data.end());
	}
	else {
		Data.insert(Data.end(), obj.Data.begin(), obj.Data.end());
	}
}

ByteArray ByteArray::GetLeft_Find(ByteArray obj) {
	int index = Find(obj);
	if (index == -1)return ByteArray();
	ByteArray temp(index);
	memcpy(temp.Data.data(), Data.data(), index);
	return temp;
}

ByteArray ByteArray::GetRight_Find(ByteArray obj) {
	int index = Find(obj);
	size_t a = Data.size() - obj.Data.size();
	if (index == -1 || index == a)return ByteArray();
	ByteArray temp(a - index);
	memcpy(temp.Data.data(), Data.data() + index + obj.Data.size(), a - index);
	return temp;
}

ByteArray ByteArray::GetMid_Find(ByteArray LObj, ByteArray RObj) {
	size_t lindex = Find(LObj);
	if (lindex == -1)return ByteArray();
	size_t rindex = Find(RObj);
	size_t  lsize = LObj.Data.size();
	if (rindex == -1 || rindex < lindex || lindex + lsize == rindex)return ByteArray();
	size_t arySize = rindex - lindex - lsize;
	ByteArray temp(arySize);
	memcpy(temp.Data.data(), Data.data() + lindex + lsize, arySize);
	return temp;
}

vector<ByteArray> ByteArray::splitByteArray(ByteArray obj){
	vector<ByteArray> BA;
	size_t osize = obj.Data.size();
	auto it = search(Data.begin(), Data.end(), obj.Data.begin(), obj.Data.end());
	int index = 0;
	int oneIndex = 0;
	while (it != Data.end()) {
		index = distance(Data.begin(), it);
		BA.push_back(GetMid(oneIndex, index - oneIndex));
		oneIndex = index + osize;
		it = search(Data.begin() + oneIndex, Data.end(), obj.Data.begin(), obj.Data.end());
	}
	if (index != 0 && oneIndex != 0) {
		BA.push_back(GetMid(oneIndex, Data.size() - index - osize));
	}
	return BA;
}

void ByteArray::Clear() {
	Data.clear();
}

short ByteArray::ToShort() {
	short temp = 0;
	if (sizeof(short) <= Data.size()) {
		memcpy(&temp, Data.data(), sizeof(short));
	}
	return temp;
}
int ByteArray::ToInt() {
	int temp = 0;
	if (sizeof(int) <= Data.size()) {
		memcpy(&temp, Data.data(), sizeof(int));
	}
	return temp;
}
long ByteArray::ToLong() {
	long temp = 0;
	if (sizeof(long) <= Data.size()) {
		memcpy(&temp, Data.data(), sizeof(long));
	}
	return temp;
}
float ByteArray::ToFloat() {
	float temp = 0;
	if (sizeof(float) <= Data.size()) {
		memcpy(&temp, Data.data(), sizeof(float));
	}
	return temp;
}
double ByteArray::ToDouble() {
	double temp = 0;
	if (sizeof(double) <= Data.size()) {
		memcpy(&temp, Data.data(), sizeof(double));
	}
	return temp;
}
string ByteArray::ToString() {
	if (Data.size() == 0)return string();
	size_t len = 0;
	BYTE* p = Data.data();
	for (size_t i = 0; i < Data.size(); i++) {
		if (*(p + i) == '\0') {
			len = i + 1;
			break;
		}
	}
	if (len == 0) {
		len = Data.size() + 1;//前面没找到0，所以这里加上0的空间，然后初始化的时候让他自动填充
	}
	string temp(len, '\0');
	memcpy(&temp[0], (char*)Data.data(), len - 1);
	return temp;
}

BYTE* ByteArray::GetDataAddress() {
	return Data.data();
}

ByteArray ToByteArray(__int16 num) {
	ByteArray temp(sizeof(num));
	memcpy(temp.GetDataAddress(),&num, sizeof(num));
	return temp;
}
ByteArray ToByteArray(__int32 num) {
	ByteArray temp(sizeof(num));
	memcpy(temp.GetDataAddress(), &num, sizeof(num));
	return temp;
}
ByteArray ToByteArray(__int64 num) {
	ByteArray temp(sizeof(num));
	memcpy(temp.GetDataAddress(), &num, sizeof(num));
	return temp;
}
ByteArray ToByteArray(const string &str) {
	//连\0一起拷贝
	ByteArray temp(str.size()+1);
	memcpy(temp.GetDataAddress(), str.data(), str.size()+1);
	return temp;
}
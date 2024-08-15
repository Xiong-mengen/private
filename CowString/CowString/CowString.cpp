#include <iostream>
using std::endl;
using std::cout;
using std::ostream;

class CowString {

	class CharProxy {

	public:
		//这里我写错了 self要用引用保证同一个对象
		CharProxy(int idx,CowString &self):_idx(idx),_self(self)
		{
			cout << "CharProxy()" << endl;
		}
		char& operator=(char ch);

		//类型转换函数 将类对象转换成char
		//operator char() { return _self._pstr[_idx]; }
		friend ostream& operator<<(ostream& os, const CowString::CharProxy& rhs);
		
	private:
		CowString& _self;
		int _idx;
	};
	friend ostream& operator<<(ostream& os, const CowString::CharProxy& rhs);
public:
	//重载
	
	CharProxy operator[](size_t _idx);

	int size() const { return strlen(_pstr); }
	//无参数的构造函数
	CowString() :_pstr(new char[5]() + 4)
	{
		InitRefCount();
	}
	//有参数的构造函数
	CowString(const char* pstr) :_pstr(new char[strlen(pstr) + 5]() + 4)
	{
		cout << "CowString(const char *pstr) " << endl;
		strcpy_s(_pstr, sizeof(_pstr), pstr);
		InitRefCount();
	}
	~CowString()
	{
		release();
	}
	//拷贝构造函数 这里使用浅拷贝
	CowString(const CowString& rsh):_pstr(rsh._pstr) //这个是浅拷贝        //_pstr(new char[strlen(rsh._pstr) +5]() + 4) 这个是深拷贝
	{
		//strcpy_s(_pstr, sizeof(_pstr), rsh._pstr);
		cout << "CowString(const CowString& rsh) " << endl;
		IncreaseRefCount();//引用计数加1
	}
	//重载赋值运算符
	CowString& operator=(const CowString&rhs) 
	{
		// 赋值构造函数
		cout << "  CowString &operator=(const CowString &rsh) " << endl;
		if (this!= &rhs)
		{
			
			//应该删除就是左边的值
			release();
			this->_pstr = rhs._pstr;
			IncreaseRefCount();
			
		}
		return *this;
	}
	//重载输出流运算符
	friend ostream& operator<< (ostream& os, const CowString& rhs);
	
	const char* c_str() { return _pstr; }
	//获得当前的引用计数
	int getRefCount()const
	{
		return *(int*)(_pstr - 4);
	}

	
private:
	//初始化引用计数
	void InitRefCount()
	{
		*(int*)(_pstr - 4) = 1;
	}
	//增加引用计数
	void IncreaseRefCount()
	{
		++*(int*)(_pstr - 4);
	}
	//减少引用计数
	void DecreaseRefCount()
	{
		--*(int*)(_pstr - 4);
	}
	

	void release()
	{
			DecreaseRefCount();
			if (getRefCount() == 0)
			{
				//delete[]_pstr;// 回收的时候也需要向前偏移
				delete[] (_pstr - 4);
				_pstr = nullptr;
				cout << " >> free heap space() " << endl;
			}
	}
	
private:
	char* _pstr;
};
ostream&  operator<< (ostream & os, const CowString & rhs)
{
	os << rhs._pstr;
	return os;
}
ostream& operator<<(ostream& os, const CowString::CharProxy& rhs)
{
	os << rhs._self._pstr[rhs._idx];
	

	return os;

}
// 下标访问运算符  必须返回一个自定义的类型 CharProxy
CowString::CharProxy CowString::operator[](size_t _idx)
{
	return CharProxy(_idx, *this);
}

////实现写操作
char& CowString::CharProxy::operator=(char ch)
{
	if (_idx < _self.size())
	{
		if (_self.getRefCount() > 1)
		{
			//进行深拷贝
			_self.DecreaseRefCount();
			char* ptmp = new char[_self.size() + 5]() + 4;
			strcpy_s(ptmp,sizeof(ptmp),_self._pstr);
			_self._pstr = ptmp;
			_self.InitRefCount();
		}
		_self._pstr[_idx] = ch;
		return _self._pstr[_idx];
	}
	else
	{
		static char nullchar = '\0';
		return nullchar;
	}

}
void test()
{
	CowString s0;
	cout<< "s0 = "<< s0 << endl;
	CowString s1("hello");
	cout << "s1 = " << s1 << endl;

	CowString s2 = s1;
	cout << "s2: " << s2 << endl;
	printf("s1's content address : %p , %s \n", s1.c_str(), s1.c_str());
	printf("s2's content address : %p , %s \n", s2.c_str(), s2.c_str());
	cout << "s1.getRefcount() : " << s1.getRefCount() << endl;
	cout << "s2.getRefcount() " << s2.getRefCount() << endl;
	cout << endl;
	CowString s3("world");
	
	printf("s3's content address : %p , %s \n", s3.c_str(), s3.c_str());
	cout << "s3.getRefcount() " << s3.getRefCount() << endl;
	s3 = s1;
	cout << "\n赋值运算符 s3 = s1: " << endl;
	cout << "s3 :" << s3 << endl;
	printf("s1's content address : %p , %s \n", s1.c_str(), s1.c_str());
	printf("s2's content address : %p , %s \n", s2.c_str(), s2.c_str());
	printf("s3's content address : %p , %s \n", s3.c_str(), s3.c_str());
	cout << "s1.getRefcount() : " << s1.getRefCount() << endl;
	cout << "s2.getRefcount() " << s2.getRefCount() << endl;
	cout << "s3.getRefcount() " << s3.getRefCount() << endl;

	cout << endl;
	cout << "\n执行写操作s3[0] = 'H' " << endl; // 这里会进行写时复制
	s3[0] = 'H';//
	printf("&s1 = %p\n", &s1);
	printf("&s2 = %p\n", &s2);
	printf("&s3 = %p\n", &s3);
	printf("s1's content address : %p , %s \n", s1.c_str(), s1.c_str());
	printf("s2's content address : %p , %s \n", s2.c_str(), s2.c_str());
	printf("s3's content address : %p , %s \n", s3.c_str(), s3.c_str());
	cout << "s1.getRefcount() : " << s1.getRefCount() << endl;
	cout << "s2.getRefcount() " << s2.getRefCount() << endl;
	cout << "s3.getRefcount() " << s3.getRefCount() << endl;

	cout << "\n 执行读操作 s1[0]" << endl;
	cout << "s1[0] = " << s1[0] << endl; // 解决问题执行读时不应该进行深拷贝，下标访问运算符无法区分接下来是读操作还是写操作，
	printf("s1's content address : %p , %s \n", s1.c_str(), s1.c_str());
	printf("s2's content address : %p , %s \n", s2.c_str(), s2.c_str());
	printf("s3's content address : %p , %s \n", s3.c_str(), s3.c_str());
	cout << "s1.getRefcount() : " << s1.getRefCount() << endl;
	cout << "s2.getRefcount() " << s2.getRefCount() << endl;
	cout << "s3.getRefcount() " << s3.getRefCount() << endl;
	


	//char对象变成char类型
	/*希望将charproxy 对象转化为char类型的对象 借助于类型转换函数完成*/

}

int main()
{
	test();
}
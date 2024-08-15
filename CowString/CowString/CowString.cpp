#include <iostream>
using std::endl;
using std::cout;
using std::ostream;

class CowString {

	class CharProxy {

	public:
		//������д���� selfҪ�����ñ�֤ͬһ������
		CharProxy(int idx,CowString &self):_idx(idx),_self(self)
		{
			cout << "CharProxy()" << endl;
		}
		char& operator=(char ch);

		//����ת������ �������ת����char
		//operator char() { return _self._pstr[_idx]; }
		friend ostream& operator<<(ostream& os, const CowString::CharProxy& rhs);
		
	private:
		CowString& _self;
		int _idx;
	};
	friend ostream& operator<<(ostream& os, const CowString::CharProxy& rhs);
public:
	//����
	
	CharProxy operator[](size_t _idx);

	int size() const { return strlen(_pstr); }
	//�޲����Ĺ��캯��
	CowString() :_pstr(new char[5]() + 4)
	{
		InitRefCount();
	}
	//�в����Ĺ��캯��
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
	//�������캯�� ����ʹ��ǳ����
	CowString(const CowString& rsh):_pstr(rsh._pstr) //�����ǳ����        //_pstr(new char[strlen(rsh._pstr) +5]() + 4) ��������
	{
		//strcpy_s(_pstr, sizeof(_pstr), rsh._pstr);
		cout << "CowString(const CowString& rsh) " << endl;
		IncreaseRefCount();//���ü�����1
	}
	//���ظ�ֵ�����
	CowString& operator=(const CowString&rhs) 
	{
		// ��ֵ���캯��
		cout << "  CowString &operator=(const CowString &rsh) " << endl;
		if (this!= &rhs)
		{
			
			//Ӧ��ɾ��������ߵ�ֵ
			release();
			this->_pstr = rhs._pstr;
			IncreaseRefCount();
			
		}
		return *this;
	}
	//��������������
	friend ostream& operator<< (ostream& os, const CowString& rhs);
	
	const char* c_str() { return _pstr; }
	//��õ�ǰ�����ü���
	int getRefCount()const
	{
		return *(int*)(_pstr - 4);
	}

	
private:
	//��ʼ�����ü���
	void InitRefCount()
	{
		*(int*)(_pstr - 4) = 1;
	}
	//�������ü���
	void IncreaseRefCount()
	{
		++*(int*)(_pstr - 4);
	}
	//�������ü���
	void DecreaseRefCount()
	{
		--*(int*)(_pstr - 4);
	}
	

	void release()
	{
			DecreaseRefCount();
			if (getRefCount() == 0)
			{
				//delete[]_pstr;// ���յ�ʱ��Ҳ��Ҫ��ǰƫ��
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
// �±���������  ���뷵��һ���Զ�������� CharProxy
CowString::CharProxy CowString::operator[](size_t _idx)
{
	return CharProxy(_idx, *this);
}

////ʵ��д����
char& CowString::CharProxy::operator=(char ch)
{
	if (_idx < _self.size())
	{
		if (_self.getRefCount() > 1)
		{
			//�������
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
	cout << "\n��ֵ����� s3 = s1: " << endl;
	cout << "s3 :" << s3 << endl;
	printf("s1's content address : %p , %s \n", s1.c_str(), s1.c_str());
	printf("s2's content address : %p , %s \n", s2.c_str(), s2.c_str());
	printf("s3's content address : %p , %s \n", s3.c_str(), s3.c_str());
	cout << "s1.getRefcount() : " << s1.getRefCount() << endl;
	cout << "s2.getRefcount() " << s2.getRefCount() << endl;
	cout << "s3.getRefcount() " << s3.getRefCount() << endl;

	cout << endl;
	cout << "\nִ��д����s3[0] = 'H' " << endl; // ��������дʱ����
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

	cout << "\n ִ�ж����� s1[0]" << endl;
	cout << "s1[0] = " << s1[0] << endl; // �������ִ�ж�ʱ��Ӧ�ý���������±����������޷����ֽ������Ƕ���������д������
	printf("s1's content address : %p , %s \n", s1.c_str(), s1.c_str());
	printf("s2's content address : %p , %s \n", s2.c_str(), s2.c_str());
	printf("s3's content address : %p , %s \n", s3.c_str(), s3.c_str());
	cout << "s1.getRefcount() : " << s1.getRefCount() << endl;
	cout << "s2.getRefcount() " << s2.getRefCount() << endl;
	cout << "s3.getRefcount() " << s3.getRefCount() << endl;
	


	//char������char����
	/*ϣ����charproxy ����ת��Ϊchar���͵Ķ��� ����������ת���������*/

}

int main()
{
	test();
}
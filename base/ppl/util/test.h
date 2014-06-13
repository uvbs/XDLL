
#pragma once

#pragma warning(disable:4127)

/**
 * @file
 * @brief һ���򵥵Ĳ��Կ��
 */



#define PPL_LOG_TYPE_TESTCASE 110

#define TESTCASE_EVENT(message) PPL_LOG_IMPL(PPL_LOG_TYPE_TESTCASE, _PPL_EVENT, message)




#if defined(_DEBUG) && !defined(_WIN32_WCE)

/// ��ʾ���ò��Կ��(Ŀǰdebug���Զ�����)
#define _PPL_RUN_TEST

/// �����ϵĺ�
#define _RUN_TEST

#pragma message("------enable automatical unit tests")

#endif



#ifdef _PPL_RUN_TEST

#include <boost/noncopyable.hpp>

/// ����������
class TestCase : private boost::noncopyable
{
public:
	TestCase() { }
	virtual ~TestCase() { }

	/// ��ʼ��
	bool Init()
	{
		return DoInit();
	}

	/// ����
	void Run()
	{
		DoRun();
	}

	void SetName(const string& name) { m_name = name; }
	const string& GetName() const { return m_name; }

protected:
	/// ��ʼ����ʵ��
	virtual bool DoInit() { return true; }

	/// ���е�ʵ��
	virtual void DoRun() { }

private:
	string m_name;
};





/// ��ʾһ���������
class TestSuite : public TestCase
{
public:
	TestSuite() { }
	~TestSuite() { Clear(); }

	/// ������еĲ�������
	void Clear()
	{
		STL_FOR_EACH(TestCaseCollection, m_tests, iter)
		{
			TestCase* tc = *iter;
			delete tc;
		}
		m_tests.clear();
	}

	/// ���Ӳ�������
	void AddTest(TestCase* tc)
	{
		m_tests.insert(tc);
	}

	/// skeleton��ʵ��
	static TestSuite& Instance()
	{
		static TestSuite suite;
		return suite;
	}

protected:
	virtual void DoRun()
	{
		TESTCASE_EVENT("Start running " << m_tests.size() << " test cases:");
		int i = 0;
		STL_FOR_EACH_CONST(TestCaseCollection, m_tests, iter)
		{
			TestCase* tc = *iter;
			TESTCASE_EVENT("Start running test case " << i << ": " << tc->GetName());
			tc->Run();
			i++;
		}
		TESTCASE_EVENT("All of " << m_tests.size() << " test cases passed.");
	}

private:
	/// ������������
	typedef std::set<TestCase*> TestCaseCollection;
	/// �����������в�������
	TestCaseCollection	m_tests;
};



/// �����������Զ�ע����
class AutoRegisterSuite
{
public:
	explicit AutoRegisterSuite(TestCase* tc, const char* name)
	{
		tc->SetName(name);
		TestSuite::Instance().AddTest(tc);
	}
};


/// �Զ�ע��ָ���Ĳ�������
#define CPPUNIT_TEST_SUITE_REGISTRATION(testCase)	static AutoRegisterSuite CPPUNIT_MAKE_UNIQUE_NAME(testCase)(new testCase, #testCase)


/// ͨ�õ�friend class�����ܹ�����ָ�����protected��Ա
#define PPL_DEFINE_TEST_FRIEND_CLASS(thisClass, baseClass, firendClass)		\
	class thisClass : public baseClass										\
	{																		\
		friend class firendClass;											\
	}


/// ���в�������
inline void RunTests()
{
	TestSuite::Instance().Run();
	TestSuite::Instance().Clear();
}

#else

/// ���в�������(��ʵ��)
inline void RunTests()
{
}

#endif


#pragma warning(default:4127)
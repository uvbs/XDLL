
#pragma once

#pragma warning(disable:4127)

/**
 * @file
 * @brief 一个简单的测试框架
 */



#define PPL_LOG_TYPE_TESTCASE 110

#define TESTCASE_EVENT(message) PPL_LOG_IMPL(PPL_LOG_TYPE_TESTCASE, _PPL_EVENT, message)




#if defined(_DEBUG) && !defined(_WIN32_WCE)

/// 表示启用测试框架(目前debug版自动运行)
#define _PPL_RUN_TEST

/// 兼容老的宏
#define _RUN_TEST

#pragma message("------enable automatical unit tests")

#endif



#ifdef _PPL_RUN_TEST

#include <boost/noncopyable.hpp>

/// 测试用例类
class TestCase : private boost::noncopyable
{
public:
	TestCase() { }
	virtual ~TestCase() { }

	/// 初始化
	bool Init()
	{
		return DoInit();
	}

	/// 运行
	void Run()
	{
		DoRun();
	}

	void SetName(const string& name) { m_name = name; }
	const string& GetName() const { return m_name; }

protected:
	/// 初始化的实现
	virtual bool DoInit() { return true; }

	/// 运行的实现
	virtual void DoRun() { }

private:
	string m_name;
};





/// 表示一组测试用例
class TestSuite : public TestCase
{
public:
	TestSuite() { }
	~TestSuite() { Clear(); }

	/// 清除所有的测试用例
	void Clear()
	{
		STL_FOR_EACH(TestCaseCollection, m_tests, iter)
		{
			TestCase* tc = *iter;
			delete tc;
		}
		m_tests.clear();
	}

	/// 增加测试用例
	void AddTest(TestCase* tc)
	{
		m_tests.insert(tc);
	}

	/// skeleton的实例
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
	/// 测试用例集合
	typedef std::set<TestCase*> TestCaseCollection;
	/// 所包含的所有测试用例
	TestCaseCollection	m_tests;
};



/// 测试用例的自动注册器
class AutoRegisterSuite
{
public:
	explicit AutoRegisterSuite(TestCase* tc, const char* name)
	{
		tc->SetName(name);
		TestSuite::Instance().AddTest(tc);
	}
};


/// 自动注册指定的测试用例
#define CPPUNIT_TEST_SUITE_REGISTRATION(testCase)	static AutoRegisterSuite CPPUNIT_MAKE_UNIQUE_NAME(testCase)(new testCase, #testCase)


/// 通用的friend class，以能够访问指定类的protected成员
#define PPL_DEFINE_TEST_FRIEND_CLASS(thisClass, baseClass, firendClass)		\
	class thisClass : public baseClass										\
	{																		\
		friend class firendClass;											\
	}


/// 运行测试用例
inline void RunTests()
{
	TestSuite::Instance().Run();
	TestSuite::Instance().Clear();
}

#else

/// 运行测试用例(空实现)
inline void RunTests()
{
}

#endif


#pragma warning(default:4127)
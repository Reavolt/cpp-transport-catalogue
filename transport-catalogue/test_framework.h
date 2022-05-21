#pragma once

#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace TestRunnerPrivate
{
    template<class Map>
    std::ostream& PrintMap(std::ostream& os, const Map& m)
    {
        os << "{";
        bool first = true;
        for(const auto& kv : m)
        {
            if(!first)
            {
                os << ", ";
            }
            first = false;
            os << kv.first << ": " << kv.second;
        }
        return os << "}";
    }

    template<class Set>
    std::ostream& PrintSet(std::ostream& os, const Set& s)
    {
        os << "{";
        bool first = true;
        for(const auto& x : s)
        {
            if(!first)
            {
                os << ", ";
            }
            first = false;
            os << x;
        }
        return os << "}";
    }

}    // namespace TestRunnerPrivate

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& s)
{
    os << "{";
    bool first = true;
    for(const auto& x : s)
    {
        if(!first)
        {
            os << ", ";
        }
        first = false;
        os << x;
    }
    return os << "}";
}

template<class T, class C>
std::ostream& operator<<(std::ostream& os, const std::set<T, C>& s)
{
    return TestRunnerPrivate::PrintSet(os, s);
}

template<class T, class H, class Eq>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T, H, Eq>& s)
{
    return TestRunnerPrivate::PrintSet(os, s);
}

template<class K, class V, class C>
std::ostream& operator<<(std::ostream& os, const std::map<K, V, C>& m)
{
    return TestRunnerPrivate::PrintMap(os, m);
}

template<class K, class V, class H, class Eq>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V, H, Eq>& m)
{
    return TestRunnerPrivate::PrintMap(os, m);
}

/**
 * РЎСЂР°РІРЅРёРІР°РµС‚ Р·РЅР°С‡РµРЅРёСЏ t Рё u. Р•СЃР»Рё РѕРЅРё РЅРµ СЂР°РІРЅС‹, С‚РµСЃС‚ РїСЂРѕРІР°Р»РёРІР°РµС‚СЃСЏ.
 * РЎС‚СЂРѕРєР° hint СЃРѕРґРµСЂР¶РёС‚ РїРѕРґСЃРєР°Р·РєСѓ, РєРѕС‚РѕСЂР°СЏ РІС‹РІРѕРґРёС‚СЃСЏ, РµСЃР»Рё С‚РµСЃС‚ РїСЂРѕРІР°Р»РµРЅ.
 *
 * РџСЂРёРјРµСЂ:
 *  void Test() {
 *      Assert("Hello "s + "world"s, "Hello world"s, "String concatenation error"s);
 *  }
 */
template<class T, class U>
void AssertEqual(const T& t, const U& u, const std::string& hint = {})
{
    if(!(t == u))
    {
        std::ostringstream os;
        os << "Assertion failed: " << t << " != " << u;
        if(!hint.empty())
        {
            os << " hint: " << hint;
        }
        throw std::runtime_error(os.str());
    }
}

/**
 * РџСЂРѕРІРµСЂСЏРµС‚ РёСЃС‚РёРЅРЅРѕСЃС‚СЊ Р·РЅР°С‡РµРЅРёСЏ b, РµСЃР»Рё РЅРµС‚, С‚РµСЃС‚ РїСЂРѕРІР°Р»РёРІР°РµС‚СЃСЏ.
 * РЎС‚СЂРѕРєР° hint СЃРѕРґРµСЂР¶РёС‚ РїРѕРґСЃРєР°Р·РєСѓ, РєРѕС‚РѕСЂР°СЏ РІС‹РІРѕРґРёС‚СЃСЏ, РµСЃР»Рё С‚РµСЃС‚ РїСЂРѕРІР°Р»РµРЅ.
 */
inline void Assert(bool b, const std::string& hint)
{
    AssertEqual(b, true, hint);
}

/**
 * РљР»Р°СЃСЃ TestRunner Р·Р°РїСѓСЃРєР°РµС‚ С‚РµСЃС‚-С„СѓРЅРєС†РёРё.
 * РџСЂРёРјРµСЂ:
 *  void Test1() {
 *      // ...
 *  }
 *
 *  void Test2() {
 *      // ...
 *  }
 *
 *  int main() {
 *      TestRunner tr;
 *      // Р—Р°РїСѓСЃРєР°РµС‚ С„СѓРЅРєС†РёСЋ Test1. Р•СЃР»Рё С‚РµСЃС‚ Р±СѓРґРµС‚ РїСЂРѕРІР°Р»РµРЅ, РµРіРѕ РёРјСЏ Р±СѓРґРµС‚ РІС‹РІРµРґРµРЅРѕ РєР°Рє
 *      // First test
 *      tr.RunTest(Test1, "First test"s);
 *      // Р•СЃР»Рё РёРјСЏ С‚РµСЃС‚Р°, СЃРѕРІРїР°РґР°РµС‚ СЃ РёРјРµРЅРµРј С‚РµСЃС‚-С„СѓРЅРєС†РёРё, РјРѕР¶РЅРѕ РёСЃРїРѕР»СЊР·РѕРІР°С‚СЊ РјР°РєСЃСЂРѕ RUN_TEST:
 *      RUN_TEST(tr, Test2); // РђРЅР°Р»РѕРіРёС‡РЅРѕ tr.RunTest(Test2, "Test2");
 *  }
 */
class TestRunner
{
public:
    template<class TestFunc>
    void RunTest(TestFunc func, const std::string& test_name)
    {
        try
        {
            func();
            std::cerr << test_name << " OK" << std::endl;
        }
        catch(std::exception& e)
        {
            ++fail_count;
            std::cerr << test_name << " fail: " << e.what() << std::endl;
        }
        catch(...)
        {
            ++fail_count;
            std::cerr << "Unknown exception caught" << std::endl;
        }
    }

    ~TestRunner()
    {
        std::cerr.flush();
        if(fail_count > 0)
        {
            std::cerr << fail_count << " unit tests failed. Terminate" << std::endl;
            exit(1);
        }
    }

private:
    int fail_count = 0;
};

#ifndef FILE_NAME
    #define FILE_NAME __FILE__
#endif

/**
 * РњР°РєСЂРѕСЃ ASSERT_EQUAL РїСЂРѕРІРµСЂСЏРµС‚ Р·РЅР°С‡РµРЅРёСЏ РІС‹СЂР°Р¶РµРЅРёР№ x Рё y РЅР° СЂР°РІРµРЅСЃС‚РІРѕ.
 * Р•СЃР»Рё Р·РЅР°С‡РµРЅРёСЏ РЅРµ СЂР°РІРЅС‹, С‚РµСЃС‚ СЃС‡РёС‚Р°РµС‚СЃСЏ РїСЂРѕРІР°Р»РµРЅРЅС‹Рј.
 *
 * РџСЂРёРјРµСЂ:
 *  void Test() {
 *      ASSERT_EQUAL(2 + 2, 4);
 *      ASSERT_EQUAL(2 + 2, 5); // Р­С‚Р° РїСЂРѕРІРµСЂРєР° РЅРµ СЃСЂР°Р±РѕС‚Р°РµС‚, Рё С‚РµСЃС‚ Р±СѓРґРµС‚ РїСЂРѕРІР°Р»РµРЅ
 *  }
 */
#define ASSERT_EQUAL(x, y)                                                                       \
    {                                                                                            \
        std::ostringstream __assert_equal_private_os;                                            \
        __assert_equal_private_os << #x << " != " << #y << ", " << FILE_NAME << ":" << __LINE__; \
        AssertEqual(x, y, __assert_equal_private_os.str());                                      \
    }

/**
 * РњР°РєСЂРѕСЃ ASSERT РїСЂРѕРІРµСЂСЏРµС‚ РёСЃС‚РёРЅРЅРѕСЃС‚СЊ РІС‹СЂР°Р¶РµРЅРёСЏ x. Р’С‹СЂР°Р¶РµРЅРёРµ x РґРѕР»Р¶РЅРѕ
 * РєРѕРЅРІРµСЂС‚РёСЂРѕРІР°С‚СЊСЃСЏ Рє С‚РёРїСѓ bool.
 * Р•СЃР»Рё РІС‹СЂР°Р¶РµРЅРёРµ x Р»РѕР¶РЅРѕ, С‚РµСЃС‚ СЃС‡РёС‚Р°РµС‚СЃСЏ РїСЂРѕРІР°Р»РµРЅРЅС‹Рј. Р•СЃР»Рё РІС‹СЂР°Р¶РµРЅРёРµ x РёСЃС‚РёРЅРЅРѕ,
 * РІС‹РїРѕР»РЅРµРЅРёРµ С‚РµСЃС‚Р° РїСЂРѕРґРѕР»Р¶Р°РµС‚СЃСЏ.
 *
 * РџСЂРёРјРµСЂ:
 *  void Test() {
 *      ASSERT(2 + 2 == 4);
 *      ASSERT(2); // С‡РёСЃР»Рѕ 2 РїСЂРё РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅРёРё Рє bool СЃС‚Р°РЅРµС‚ Р·РЅР°С‡РµРЅРёРµРј true
 *      ASSERT(false); // Р·РґРµСЃСЊ С‚РµСЃС‚ РїСЂРѕРІР°Р»РёС‚СЃСЏ
 *      string user_name = "Harry Potter"s;
 *      // Р•СЃР»Рё СЂР°СЃРєРѕРјРјРµРЅС‚РёСЂРѕРІР°С‚СЊ СЃР»РµРґСѓСЋС‰СѓСЋ СЃС‚СЂРѕРєСѓ, РїСЂРѕРіСЂР°РјРјР° РЅРµ СЃРєРѕРјРїРёР»РёСЂСѓРµС‚СЃСЏ,
 *      // С‚Р°Рє РєР°Рє string РЅРµ РјРѕР¶РµС‚ Р±С‹С‚СЊ РїСЂРµРѕР±СЂР°Р·РѕРІР°РЅ Рє С‚РёРїСѓ bool.
 *      // ASSERT(user_name);
 *  }
 */
#define ASSERT(x)                                                                   \
    {                                                                               \
        std::ostringstream __assert_private_os;                                     \
        __assert_private_os << #x << " is false, " << FILE_NAME << ":" << __LINE__; \
        Assert(static_cast<bool>(x), __assert_private_os.str());                    \
    }

/**
 * РњР°РєСЂРѕСЃ RUN_TEST СЃР»СѓР¶РёС‚ РґР»СЏ СѓРґРѕР±РЅРѕРіРѕ Р·Р°РїСѓСЃРєР° С‚РµСЃС‚-С„СѓРЅРєС†РёРё func.
 * РџР°СЂР°РјРµС‚СЂ tr Р·Р°РґР°С‘С‚ РёРјСЏ РїРµСЂРµРјРµРЅРЅРѕР№ С‚РёРїР° TestRunner.
 *
 * РџСЂРёРјРµСЂ:
 *  void Test1() {
 *      // РЎРѕРґРµСЂР¶РёРјРѕРµ С‚РµСЃС‚-С„СѓРЅРєС†РёРё ...
 *  }
 *
 *  void Test2() {
 *      // РЎРѕРґРµСЂР¶РёРјРѕРµ С‚РµСЃС‚-С„СѓРЅРєС†РёРё ...
 *  }
 *
 *  int main() {
 *      TestRunner tr;
 *      RUN_TEST(tr, Test1);
 *      RUN_TEST(tr, Test2);
 *  }
 */
#define RUN_TEST(tr, func) tr.RunTest(func, #func)

/**
 * РњР°РєСЂРѕСЃ ASSERT_THROWS РїСЂРѕРІРµСЂСЏРµС‚, С‡С‚Рѕ РїСЂРё РІС‹С‡РёСЃР»РµРЅРёРё РІС‹СЂР°Р¶РµРЅРёСЏ expr Р±СѓРґРµС‚
 * РІС‹Р±СЂРѕС€РµРЅРѕ РёСЃРєР»СЋС‡РµРЅРёРµ С‚РёРїР° expected_exception.
 * Р•СЃР»Рё РёСЃРєР»СЋС‡РµРЅРёРµ РІС‹Р±СЂРѕС€РµРЅРѕ РЅРµ Р±СѓРґРµС‚, Р»РёР±Рѕ РІС‹Р±СЂРѕСЃРёС‚СЃСЏ РёСЃРєР»СЋС‡РµРЅРёРµ РґСЂСѓРіРѕРіРѕ С‚РёРїР°,
 * С‚РµСЃС‚ СЃС‡РёС‚Р°РµС‚СЃСЏ РїСЂРѕРІР°Р»РµРЅРЅС‹Рј.
 *
 * РџСЂРёРјРµСЂ:
 *  void Test() {
 *      using namespace std;
 *      ASSERT_THROWS(stoi("not-a-number"s), invalid_argument);
 *  }
 */
#define ASSERT_THROWS(expr, expected_exception)                                         \
    {                                                                                   \
        bool __assert_private_flag = true;                                              \
        try                                                                             \
        {                                                                               \
            expr;                                                                       \
            __assert_private_flag = false;                                              \
        }                                                                               \
        catch(expected_exception&)                                                      \
        {                                                                               \
        }                                                                               \
        catch(...)                                                                      \
        {                                                                               \
            std::ostringstream __assert_private_os;                                     \
            __assert_private_os << "Expression " #expr " threw an unexpected exception" \
                                   " " FILE_NAME ":"                                    \
                                << __LINE__;                                            \
            Assert(false, __assert_private_os.str());                                   \
        }                                                                               \
        if(!__assert_private_flag)                                                      \
        {                                                                               \
            std::ostringstream __assert_private_os;                                     \
            __assert_private_os << "Expression " #expr " is expected to "               \
                                   "throw " #expected_exception " " FILE_NAME ":"       \
                                << __LINE__;                                            \
            Assert(false, __assert_private_os.str());                                   \
        }                                                                               \
    }

/**
 * РњР°РєСЂРѕСЃ ASSERT_DOESNT_THROW РїСЂРѕРІРµСЂСЏРµС‚, С‡С‚Рѕ РїСЂРё РІС‹С‡РёСЃР»РµРЅРёРё РІС‹СЂР°Р¶РµРЅРёСЏ expr
 * РЅРµ Р±СѓРґРµС‚ РІС‹Р±СЂРѕС€РµРЅРѕ РЅРёРєР°РєРёС… РёСЃРєР»СЋС‡РµРЅРёР№.
 * Р•СЃР»Рё РїСЂРё РІС‹С‡РёСЃР»РµРЅРёРё РІС‹СЂР°Р¶РµРЅРёСЏ expr РІС‹Р±СЂРѕСЃРёС‚СЃСЏ РёСЃРєР»СЋС‡РµРЅРёРµ, С‚РµСЃС‚ Р±СѓРґРµС‚ РїСЂРѕРІР°Р»РµРЅ.
 *
 * РџСЂРёРјРµСЂ:
 *  void Test() {
 *      vector<int> v;
 *      v.push_back(1);
 *      ASSERT_DOESNT_THROW(v.at(0)));
 *  }
 */
#define ASSERT_DOESNT_THROW(expr)                                                   \
    try                                                                             \
    {                                                                               \
        expr;                                                                       \
    }                                                                               \
    catch(...)                                                                      \
    {                                                                               \
        std::ostringstream __assert_private_os;                                     \
        __assert_private_os << "Expression " #expr " threw an unexpected exception" \
                               " " FILE_NAME ":"                                    \
                            << __LINE__;                                            \
        Assert(false, __assert_private_os.str());                                   \
    }

#ifndef UNITTEST_H
#define UNITTEST_H

#include <string>
#include <functional>
#include <list>
#include <utility>
#include <sstream>
#include <vector>
#include <iostream>

class Assert;

class TestRunner {
    int passed;
public:
    void setGroupName(const std::string& groupName);
    void addTest(const std::string& testName, std::function<void()> test);
    int run();

    friend class Assert;
};

extern TestRunner unit_test_runner;

struct __TestWrapper {
    __TestWrapper(const std::string& testName, std::function<void()> test) {
        unit_test_runner.addTest(testName, test);
    }
};

struct __TestGroupWrapper {
    __TestGroupWrapper(const std::string& groupName) {
        unit_test_runner.setGroupName(groupName);
    }
};

#define START_TEST_GROUP(X) __TestGroupWrapper __group##X(#X); namespace __unit_test ## X {
#define END_TEST_GROUP }
#define TEST(X,Y) __TestWrapper X ## __COUNTER__ (#X,Y);

class Assert {
    static void throwif(bool b, const std::string& message);
    template<typename A> static std::string str(A a) {
        std::ostringstream buf;
        buf << typeid(A).name() << ":" <<  a;
        return buf.str();
    }

public:
    template<typename A>
    static void debug(A a, const std::string& message = std::string()) {
        std::cout << "[DEBUG] " << message << ": " << str(a) << std::endl;
    }

    template<typename A, typename B> static void eq(A a, B b, const std::string& message = std::string()) {
        std::string msg = std::string("Values should be equal [") +str(a) + std::string(" and ") + str(b) + std::string("]");
        throwif(!(a == b), message.size()>0?message+":"+msg:msg);
    }
    template<typename A, typename B> static void ne(A a, B b, const std::string& message = std::string()) {
        std::string msg = std::string("Values should not be equal [") +str(a) + std::string(" and ") + str(b) + std::string("]");
        throwif(a == b, message.size()>0?message+":"+msg:msg);
    }
    static void istrue(bool b, const std::string& message = std::string("Value should be true"));
    static void isfalse(bool b, const std::string& message = std::string("Value should be false"));
};

#endif // UNITTEST_H

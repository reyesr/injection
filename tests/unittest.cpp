#include "unittest.h"
#include <vector>
#include <iostream>

using namespace std;

TestRunner unit_test_runner;

struct TestUnit {
    string name;
    function<void()> test;
};

struct TestSet
{
    string label;
    list<TestUnit> tests;
};

vector<TestSet>& GET_TESTS_SINGLETON() {
    static vector<TestSet> all_tests; // having it here ensures it's always initialized
    return all_tests;
}

void Assert::throwif(bool b, const string &message)
{
    if (b) {
        std::ostringstream buf(std::ostringstream::out);
        buf << "Assert error: " << message;
        throw buf.str();
    }
    ++unit_test_runner.passed;
}

void Assert::istrue(bool b, const string &message)
{
    throwif(!b,message);
}

void Assert::isfalse(bool b, const string &message)
{
    throwif(b,message);
}


void TestRunner::setGroupName(const string &groupName)
{
    TestSet ts;
    ts.label = groupName;
    GET_TESTS_SINGLETON().push_back(ts);
}

void TestRunner::addTest(const string &testName, std::function<void ()> test)
{
    if (GET_TESTS_SINGLETON().size()==0) {
        setGroupName("Default");
    }

    TestSet& set = GET_TESTS_SINGLETON()[GET_TESTS_SINGLETON().size()-1];
    TestUnit tu;
    tu.name = testName;
    tu.test = test;
    set.tests.push_back(tu);
}

int TestRunner::run()
{
    for (auto set = GET_TESTS_SINGLETON().begin(); set != GET_TESTS_SINGLETON().end(); ++set) {
        cout << "Running test set [" << set->label << "]" << endl;
        for (auto unittest = set->tests.begin(); unittest != set->tests.end(); ++unittest) {
            cout << "   * test [" << unittest->name << "]";
            passed = 0;
            try {
                unittest->test();
                cout << " OK (" << passed << " tests)" << endl;
            } catch (std::string e) {
                cout << endl << "     ERROR: " << e << endl;
                return 99;
            }
        }
    }
    return 0;
}

int main(int, char**) {
    return unit_test_runner.run();
}



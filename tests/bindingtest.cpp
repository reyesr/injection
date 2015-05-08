#include <functional>
#include <string>
#include <iostream>
#include "unittest.h"
#include "injectionmanager.h"
#include "testsfixtures.h"

using namespace injection;

START_TEST_GROUP(bindings)

TEST(Redefining_the_binding, ([](){

    InjectionManager::bind_singleton<ITest, Test1> ();
    checkITest(Test1::VALUE, Test1::VALUE2);

    InjectionManager::bind_singleton<ITest>([](){
        return new ITest(3, 0.33);
    });
    checkITest(3, 0.33);

    InjectionManager::bind_singleton<ITest, Test2>();
    checkITest(Test2::VALUE, Test2::VALUE2);

}));

TEST(Redefining_the_binding_different_scope, ([](){

    InjectionManager::bind_singleton<ITest>([](){
        return new ITest(3, 0.33);
    });
    checkITest(3, 0.33);
    Injected<ITest> test0;
    Assert::eq(test0->value, 3);
    Assert::eq(test0->value2, 0.33);

    IncrementingTest::init(0,0.0);

    InjectionManager::bind_transient<ITest, IncrementingTest>();
    checkITest(1, 2);
    checkITest(2, 4);
    checkITest(3, 6);
    Injected<ITest> test;
    Assert::eq(test->value, 4);
    Assert::eq(test->value2, 8.0);

    // No change on test0
    Assert::eq(test0->value, 3);
    Assert::eq(test0->value2, 0.33);
}));


TEST(Checking_binding_injected, ([](){
    InjectionManager::bind_singleton<ITest, Test1>();
    Injected<ITest> test;
    Assert::eq(Test1::VALUE, test->value);
    Assert::istrue(test.operator *() != 0);
    Assert::istrue(test);
}));

TEST(Checking_binding_not_injected, ([](){
    InjectionManager::bind_singleton<ITest, Test1>();
    Injected<Test1> test;
    Assert::isfalse(test.operator *() != 0);
    Assert::isfalse(test);

    Injected<Test2> test2;
    Assert::isfalse(test2.operator *() != 0);
    Assert::isfalse(test2);
}));


struct SelfReferenced {
    int value;
    SelfReferenced() : value(-1) {}
    SelfReferenced(int i) : value(i) {}
};

TEST(injecting_without_interfaces, ([](){
    InjectionManager::bind_singleton<SelfReferenced, SelfReferenced>();
    Injected<SelfReferenced> test;
    Assert::eq(test->value, -1);

    InjectionManager::bind_singleton<SelfReferenced>(new SelfReferenced(-2));
    Injected<SelfReferenced> test2;
    Assert::eq(test2->value, -2);
}));

class SelfCountingInstance {
public:
    static int counter;
    int value;
    SelfCountingInstance() {
        value = counter++;
    }
};
int SelfCountingInstance::counter = 0;

TEST(injecting_then_copy_constructor, ([](){
    SelfCountingInstance::counter = 0;
    InjectionManager::bind_transient<SelfCountingInstance, SelfCountingInstance> ();
    Injected<SelfCountingInstance> test;
    Assert::eq(test->value, 0);
    Assert::eq(SelfCountingInstance::counter, 1);

    Injected<SelfCountingInstance>  testcopy = test;
    Assert::eq(testcopy->value, 0);
    Assert::eq(SelfCountingInstance::counter, 1);

    Injected<SelfCountingInstance>  testcopy2(test);
    Assert::eq(testcopy2->value, 0);
    Assert::eq(SelfCountingInstance::counter, 1);

    Injected<SelfCountingInstance> test2;
    Assert::eq(test2->value, 1);
    Assert::eq(SelfCountingInstance::counter, 2);

    Injected<SelfCountingInstance>  test2copy = test2;
    Assert::eq(test2copy->value, 1);
    Assert::eq(SelfCountingInstance::counter, 2);

    Injected<SelfCountingInstance>  test2copy2(test2);
    Assert::eq(test2copy2->value, 1);
    Assert::eq(SelfCountingInstance::counter, 2);

    Assert::eq(test->value, 0); // no side effect from test2
    Assert::eq(testcopy2->value, 0); // no side effect from test2

    testcopy = test;
    Assert::eq(testcopy->value, 0);
    Assert::eq(SelfCountingInstance::counter, 2);

}));


END_TEST_GROUP

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

    IncrementingTest::init(0,0.0);

    InjectionManager::bind_transient<ITest, IncrementingTest>();
    checkITest(1, 2);
    checkITest(2, 4);
    checkITest(3, 6);
    Injected<ITest> test;
    Assert::eq(test->value, 4);
    Assert::eq(test->value2, 8.0);
}));

END_TEST_GROUP



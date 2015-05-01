#include <functional>
#include <string>
#include <iostream>
#include "unittest.h"
#include "injectionmanager.h"
#include "testsfixtures.h"

using namespace injection;

START_TEST_GROUP(singleton)

TEST(Singletons_really_are_singletons, ([](){

    InjectionManager::bind_singleton<ITest>([](){
        static int used = 0;
        used += 1;
        Assert::eq(1, used);
        return new ITest(11,12.34);
    });

    checkITest(11,12.34);
    checkITest(11,12.34);
    checkITest(11,12.34);
}));

TEST(Singletons_really_are_singletons_auto_inst, ([](){

    InjectionManager::bind_singleton<ITest, IncrementingTest>();
    IncrementingTest::init(10,20);
    checkITest(11,22);
    checkITest(11,22);
    checkITest(11,22);
}));

TEST(Singletons_really_are_singletons_instance_passed, ([](){

    InjectionManager::bind_singleton<ITest>(new Test1);
    checkITest(1, 3.14);
}));


END_TEST_GROUP

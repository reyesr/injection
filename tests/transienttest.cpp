#include <functional>
#include <string>
#include <iostream>
#include "unittest.h"
#include "injectionmanager.h"
#include "testsfixtures.h"

using namespace injection;

START_TEST_GROUP(transient)

TEST(Transients_are_transients, ([](){
    InjectionManager::bind_transient<ITest>([](){
        static int used = 0;
        used += 1;
        return new ITest(used,used);
    });

    checkITest(1,1.0);
    checkITest(2, 2.0);
    checkITest(3, 3.0);
}));

TEST(Transients_are_transients_auto_inst, ([](){
    InjectionManager::bind_transient<ITest, IncrementingTest>();
    IncrementingTest::init(0,0);
    checkITest(1, 2.0);
    checkITest(2, 4.0);
    checkITest(3, 6.0);
}));


END_TEST_GROUP



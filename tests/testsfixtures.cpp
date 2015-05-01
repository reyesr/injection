#include "testsfixtures.h"

using namespace injection;

const int Test1::VALUE = 1;
const double Test1::VALUE2 = 3.14;

const int Test2::VALUE = 2;
const double Test2::VALUE2 = 6.66;


int IncrementingTest::incvalue1;
double IncrementingTest::incvalue2;

void IncrementingTest::init(int v1, double v2)
{
    incvalue1 = v1;
    incvalue2 = v2;
}

void checkITest(int v1, double v2) {
    Injected<ITest> test;
    Assert::eq(v1, test->value);
    Assert::eq(v2, test->value2);
}

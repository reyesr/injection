#ifndef TESTSFIXTURES_H
#define TESTSFIXTURES_H

#include <string>
#include "injectionmanager.h"
#include "unittest.h"

struct ITest {
    int value;
    double value2;
    ITest(int v, double v2): value(v), value2(v2) {}
};

struct Test1 : public ITest
{
    static const int VALUE;
    static const double VALUE2;
    Test1(): ITest(VALUE,VALUE2) {}
};

struct Test2 : public ITest
{
    static const int VALUE;
    static const double VALUE2;
    Test2(): ITest(VALUE,VALUE2) {}
};

struct IncrementingTest : public ITest
{
    static int incvalue1;
    static double incvalue2;
    static void init(int v1, double v2);
    IncrementingTest() : ITest(incvalue1+=1, incvalue2+=2.0) {}
private:
};

struct IRecTest {
    injection::Injected<std::string> label;
    injection::Injected<ITest> test;
};

void checkITest(int v1, double v2);

#endif // TESTSFIXTURES_H

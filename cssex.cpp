#include <vector>
#include <stdexcept>
#include "cRunWatch.h"

#include "cssex.h"

void cSSex::SolutionSpace(int count, int max)
{
    myVarCount = count;
    myVarMax = max;
}

bool cSSex::nextTestValues(
    std::vector<int> &test,
    int max)
{
    int k = 0;
    while (true)
    {
        int *p = &test[k];
        *p += myRez;
        if (*p <= max)
            break;
        *p = 0;
        k++;
        if (k == myVarTestVals.size())
        {
            // search is complete
            return false;
        }
    }

    return true;
}

void cSSex::checkFunctionValue()
{
     copy(&myVarTestVals[0]);

    // check that all variable constraints are true
    if (!isFeasible())
        return;

    // check for improved function value
    int o = optFunVal();
    if (o > myOptValue)
    {
        // save the improved value
        // and the variable values that gave it
        myOptValue = o;
        myVarBestVals = myVarTestVals;
    }
}

void cSSex::search()
{
    raven::set::cRunWatch("cSSex::search");
    
    // search space at low rez
    myOptValue = 0;
    myVarTestVals.clear();
    myVarTestVals.resize(myVarCount, 0);
    myRez = myVarMax / 5;

    while (true)
    {
        // next test value set
        if (!nextTestValues(myVarTestVals, myVarMax))
        {
            // search is complete
            copyOptVals();
            break;
        }
        checkFunctionValue();
    }
    if (!myVarBestVals.size())
        throw std::runtime_error(
            "cSex::search low rez failed to find feasible solution");

    // search local space around low rez opt at high rez
    int tvMax = 2 * (myRez - 1);
    int tvd = tvMax / 2;
    myRez = 1;
    auto start = myVarBestVals;
    std::vector<int> test(myVarCount, 0);
    while (true)
    {
        if (!nextTestValues(test, tvMax))
            break;
        for (int i = 0; i < myVarCount; i++)
            myVarTestVals[i] = start[i] + test[i] - tvd;

        checkFunctionValue();
    }

    copyOptVals();
}

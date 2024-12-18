#include <vector>
#include <iostream>
#include <stdexcept>
#include "cRunWatch.h"

#include "cssex.h"

cSSex::cSSex()
    : myVarCount(-1)
{
    regObjectiveFunction(
        [this]() -> int
        {
            return noRegisteredFunction("Objective");
        });
    regFeasibleFunction(
        [this]() -> bool
        {
            noRegisteredFunction("Feasible");
            return false;
        });
    regCopyTestFunction(
        [this](int *p)
        {
            noRegisteredFunction("CopyTest");
        });
}

int cSSex::noRegisteredFunction(
    const std::string &missing)
{
    throw std::runtime_error(
        " cSSex No registered function " + missing);
}

void cSSex::SolutionSpace(int count, int max)
{
    myVarCount = count;
    myVarMax = max;
}

bool cSSex::nextTestValues(
    std::vector<int> &test,
    int max,
    int rez)
{
    int k = 0;
    while (true)
    {
        int *p = &test[k];
        *p += rez;
        if (*p <= max)
            break;
        // cary over
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

bool cSSex::checkFunctionValue()
{
    copyTest(&myVarTestVals[0]);

    // check that all variable constraints are true
    if (!feasibleFunction())
        return false;

    // check for improved function value
    int o = objectiveFunction();
    if (o > myOptObjFunValue)
    {
        // save the improved value
        // and the variable values that gave it
        myOptObjFunValue = o;
        myVarBestVals = myVarTestVals;
        return true;
    }
    return false;
}

void cSSex::search()
{
    raven::set::cRunWatch("cSSex::search");

    // search space at low rez
    myOptObjFunValue = 0;
    myVarTestVals.clear();
    myVarTestVals.resize(myVarCount, 0);
    int rez = myVarMax / 5;

    while (true)
    {
        // next test value set
        if (!nextTestValues(
                myVarTestVals,
                myVarMax,
                rez))
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
    int tvMax = 2 * (rez - 1);
    int tvd = tvMax / 2;
    auto start = myVarBestVals;
    std::vector<int> test(myVarCount, 0);
    while (true)
    {
        if (!nextTestValues(test, tvMax, 1))
            break;
        for (int i = 0; i < myVarCount; i++)
            myVarTestVals[i] = start[i] + test[i] - tvd;

        checkFunctionValue();
    }

    copyOptVals();
}

void cSSex::anneal(int tryBudget)
{
    srand(time(NULL));
    myOptObjFunValue = 0;
    myVarBestVals.clear();
    myVarBestVals.resize(myVarCount, 0);

    int acceptWorseProb = 50; // the annealing temperature
    while (true)
    {
        // reset to "best" so far
        myVarTestVals = myVarBestVals;

        // small random change
        int var = rand() % myVarCount;
        int up = rand() % 2;
        if (up == 0)
            myVarTestVals[var] += 1;
        else
            myVarTestVals[var] -= 1;

        // set to user's solution space
        copyTest(&myVarTestVals[0]);

        // check that all variable constraints are true

        if (!feasibleFunction())
            continue;

        // check for new optimum
        int o = objectiveFunction();
        if (o >= myOptObjFunValue)
        {
            // always accept improvement
            myOptObjFunValue = o;
            myVarBestVals = myVarTestVals;
            continue;
        }
        if (o < myOptObjFunValue)
        {
            // accept worse result if temperature still high
            if (rand() % 100 < acceptWorseProb)
            {
                myOptObjFunValue = o;
                myVarBestVals = myVarTestVals;
                acceptWorseProb--;
                if (acceptWorseProb == 0)
                    break;
            }
        }

        tryBudget--;
        if (!tryBudget)
            break;
    }
    copyOptVals();
}

void cSSex::greedy()
{
    raven::set::cRunWatch("cSSex::greedy");

    myOptObjFunValue = 0;
    myVarBestVals.clear();
    myVarBestVals.resize(myVarCount, 0);
    std::vector<int> vupdown = {-1, 1};

    while (true)
    {
        bool improved = false;
        for (int k = 0; k < myVarCount; k++)
        {
            for (int ud : vupdown)
            {
                // reset to "best" so far
                myVarTestVals = myVarBestVals;

                // make small change
                myVarTestVals[k] += ud;

                if (checkFunctionValue())
                    improved = true;
            }
        }
        if (!improved)
            break;
    }
}

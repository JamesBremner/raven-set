/// @brief Solution space explorer

class cSSex
{
public:
    /// @brief set solution space size
    /// @param count number of variables
    /// @param max maximum of variable range

    void SolutionSpace(int count, int max);

    /** @brief exhaustive search through variable test ranges

        The solution space is searched in two passes.
        First checking every 1 in 5 solution points
        Second checking every point around optimum found in first pass
    */

    void search();

    /// @brief copy 1D test values to user's multidimensional solution space
    /// @param p pointer to start of 1D values

    virtual void copy(int *p) = 0;

    /// @brief check feasability of test values
    /// @return true if feasible
    ///
    /// over-ride to implement constraints on variable values

    virtual bool isFeasible() = 0;

    /// calculate value of funtion to be optimized with these variable test values

    virtual int optFunVal() = 0;

protected:
    std::vector<int> myVarTestVals;
    std::vector<int> myVarBestVals;
    int myRez;
    int myOptValue;

private:
    int myVarCount; // variable count
    int myVarMax;   // max variable value

    bool nextTestValues(std::vector<int> &test, int max);
    void checkFunctionValue();

    /// @brief copy 1D optimum values to user's multidimensional solution space
    void copyOptVals()
    {
        if (myVarBestVals.size())
            copy(&myVarBestVals[0]);
    }
};

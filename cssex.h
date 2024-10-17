/// @brief Solution space explorer

class cSSex
{
public:
    cSSex();

    /// @brief set solution space size
    /// @param count number of variables
    /// @param max maximum of variable range

    void SolutionSpace(int count, int max);

    /** @brief exhaustive search through solution space

        The solution space is searched in two passes.
        First checking every 1 in 5 solution points ( lo rez )
        Second checking every point around optimum found in first pass ( hi rez )
    */

    void search();

    /** @brief simulated annealing
     *
     * At each step, the simulated annealing heuristic
     * considers some neighboring state s* of the current state s,
     * and probabilistically decides between moving the system to state s*
     * or staying in state s.
     * The probability of making the transition from the current state
     * s to a candidate new state is specified by an acceptance probability function
     * that depends on the energies
     * of the two states, and on a global time-varying parameter T called the temperature.
     * These probabilities ultimately lead the system to move to states of lower energy.
     * Typically this step is repeated until a given computation budget has been exhausted.
     *
     * https://en.wikipedia.org/wiki/Simulated_annealing
     *
     */

    void anneal(int tryBudget);

    void greedy();


    /// @brief register objetive function to be optimized
    /// @param f

    void regObjectiveFunction(
        std::function<int()> f)
    {
        objectiveFunction = f;
    }

    /// @brief register function to implement constraints on variable values
    /// @param f

    void regFeasibleFunction(
        std::function<bool()> f)
    {
        feasibleFunction = f;
    }

    /// @brief register function to copy 1D test values to user's multidimensional solution space
    /// @param p pointer to start of 1D values
    
    void regCopyTestFunction(
        std::function<void(int *)> f)
    {
        copyTest = f;
    }

private:
    int myVarCount; // variable count
    int myVarMax;   // max variable value

    std::function<int()> objectiveFunction;
    std::function<bool()> feasibleFunction;
    std::function<void(int *)> copyTest;

    std::vector<int> myVarTestVals;
    std::vector<int> myVarBestVals;

    int myOptObjFunValue; // objective function value, best so far

    /// @brief odometer style iteration through test values
    /// @param[in,out] test vector of test values
    /// @param[in] max maximum test value
    /// @return true if more to come, false if iteration complete

    bool nextTestValues(
        std::vector<int> &test,
        int max,
        int rez);

    /// @brief check for improved function value
    /// @return true if improved value saved

    bool checkFunctionValue();

    /// @brief copy 1D optimum values to user's multidimensional solution space
    void copyOptVals()
    {
        if (myVarBestVals.size())
            copyTest(&myVarBestVals[0]);
    }

    int noRegisteredFunction(const std::string &missing);
};

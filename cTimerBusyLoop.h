#pragma once
#include <thread>
#include <mutex>
namespace raven
{
namespace set
{
/** Timer for sub-millisecond times, using busy loop running in own thread */
class cTimerBusyLoopContinuous
{
public:
    cTimerBusyLoopContinuous();

    /** disable copy constructor */
    cTimerBusyLoopContinuous( const cTimerBusyLoopContinuous& bl ) = delete;

    ~cTimerBusyLoopContinuous();

    /** Start Busy Loop running */
    void Run();

    /** Stop busy loop */
    void Stop();

    /** Set wait time in microseconds */
    void WaitMicroSecs( int wait )
    {
        myWaitMicroSecs = wait;
    }

    /** Start a wait

    This returns immediatly.
    Calling code must check for completion
     */
    void Wait();

    /** True if timer done */
    bool IsDone();

private:
    std::thread * myThread;       ///< thread where the busy loop is running
    bool myfStop;                ///< true if busy loop stop requested
    bool myfDone;                ///< true if times has completed
    std::mutex myMutex;          ///< protection against data races
    int myWaitMicroSecs;            ///< requested wait time

    void BusyLoop();

    /** Thread safe stop request flag read */
    bool IsStopRequest();
};
}
}

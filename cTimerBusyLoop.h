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

    /** Start Busy Loop running */
    void Run();

    /** Stop busy loop */
    void Stop();

    /** Set wait time in microseconds */
    void WaitMicroSecs( int wait )
    {
        myWaitMicroSecs = wait;
    }

    /** Start a wait */
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

cTimerBusyLoopContinuous::cTimerBusyLoopContinuous()
: myfStop( false )
{

}

void cTimerBusyLoopContinuous::Run()
{
    myThread = new std::thread( std::bind(
                                    &cTimerBusyLoopContinuous::BusyLoop,
                                    this ) );
}

void cTimerBusyLoopContinuous::Wait()
{
    std::lock_guard<std::mutex> lock( myMutex );
    myfDone = false;
}

/** Thread safe read of busy loop timer done flag */
bool cTimerBusyLoopContinuous::IsDone()
{
    bool f;
    std::lock_guard<std::mutex> lock( myMutex );
    f = myfDone;
    return f;
}
bool cTimerBusyLoopContinuous::IsStopRequest()
{
    bool f;
    std::lock_guard<std::mutex> lock( myMutex );
    f = myfStop;
    return f;
}

void cTimerBusyLoopContinuous::BusyLoop()
{
    bool WaitingForNewWaitRequest = true;
    __int64 myTimeStart, t, myFreq;
    float MicroSecs;
    QueryPerformanceCounter( (LARGE_INTEGER *)&myTimeStart );
    QueryPerformanceFrequency( (LARGE_INTEGER *)&myFreq );
    while( true )
    {

        // the busy loop, consumes some time
        int loopCount = 2000;
        while( loopCount > 0 )
            loopCount--;

        // check for stop request
        if( IsStopRequest() )
            break;

        // check if waiting for new wait request
        if( IsDone() )
            continue;

        // check if new wait request
        if( WaitingForNewWaitRequest )
        {
            //cout << "New wait " << WaitMicroSecs << " ";
            QueryPerformanceCounter( (LARGE_INTEGER *)&myTimeStart );
            WaitingForNewWaitRequest = false;
            continue;
        }

        // currently doing a wait request
        // find out how long we have been waiting
        QueryPerformanceCounter( (LARGE_INTEGER *)&t );
        MicroSecs = 1000000.0 * (float)(t-myTimeStart) / (float)myFreq;
        //cout << MicroSecs << " ";


        // check if wait is over
        if( MicroSecs > myWaitMicroSecs )
        {
            // flag completion
            std::lock_guard<std::mutex> lock( myMutex );
            myfDone = true;

            WaitingForNewWaitRequest = true;

            //cout << "Wait Done ";
        }

        // loop around to consume some more time in the busy loop
    }


}
void cTimerBusyLoopContinuous::Stop()
{
    {
    std::lock_guard<std::mutex> lock( myMutex );
    myfStop = true;
    }

    // wait until it does stop
    myThread->join();
}
}
}

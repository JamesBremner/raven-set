#include <windows.h>

#include "cTimerBusyLoop.h"

namespace raven
{
namespace set
{

cTimerBusyLoopContinuous::cTimerBusyLoopContinuous()
    : myThread( NULL )
    , myfStop( false )
{

}

cTimerBusyLoopContinuous::~cTimerBusyLoopContinuous()
{
    Stop();
}

void cTimerBusyLoopContinuous::Run()
{
    // check if already running
    if( myThread )
        return;

    myThread = new std::thread( std::bind(
                                    &cTimerBusyLoopContinuous::BusyLoop,
                                    this ) );
}
void cTimerBusyLoopContinuous::Stop()
{
    if( ! myThread )
        return;

    {
        std::lock_guard<std::mutex> lock( myMutex );
        myfStop = true;
    }

    // wait until it does stop
    myThread->join();

    delete myThread;
    myThread = 0;
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

}
}

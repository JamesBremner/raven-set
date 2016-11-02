/** A test program for a continuous busy loop timer

For discussion see http://stackoverflow.com/q/40320926/16582

*/

#include <windows.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>

#include <boost/asio.hpp>
#include <boost/asio/high_resolution_timer.hpp>

#include "cRunCount.h"
#include "cTimerBusyLoop.h"

using namespace std;

/** Timer for sub-millisecond times, using busy loop running in own thread */
raven::set::cTimerBusyLoopContinuous Timer;

/** Frequency counter from https://github.com/JamesBremner/raven-set
 Construct to report after 2000 calls
*/
raven::set::cRunCount        myRunCounter( 2000 );

/** Length of timed wait */
int WaitMicroSecs;

/** Target frequency */
int TargetHz;

/** The event loop */
boost::asio::io_service io_service;

/** Timer used when wait is more than 1 millisecond */
boost::asio::high_resolution_timer  * myPlaybackTimer;

/** Function forward declerations */
void StartWait();
void handle_expire(const boost::system::error_code& );


void CheckBusyLoopDone()
{
    // read completion flag
    if( ! Timer.IsDone() )
    {
        // still running
        // post the next check
        // in a real application, this would allow other processing
        // to execute in this thread
        io_service.post( CheckBusyLoopDone );
    }
    else
    {
        // busy loop timer completed
        // execute completion handler
        handle_expire( boost::system::error_code() );
    }
}

/** Set up for next tests */
void NextTestDelay()
{
    // check if test is over
    if( TargetHz >= 5000 )
    {
        cout << "Test completed\n";

        // request the busy loop to stop
        Timer.Stop();

        // stop the event loop
        // Do not do this in real application!
        // but this causes the test program to shut down cleanly
        io_service.stop();
    }

    // target frequency for next tests
    TargetHz += 1000;

    // fudge factor
    // the idea here is that we reduce the actual wait time
    // to allow for the time taken setting up and tearing down
    // However, I would think that this would be very short
    // for the continuous busy loop counter
    // so I do not understand why this is so large
    // and so dependant on the TargetHz
    // the actual value has been set by experimentation on my machine
    const int overheadmsecs = 10 + TargetHz / 300;

    // Calculate microseconds wait required
    WaitMicroSecs = 1000000 / TargetHz - overheadmsecs;

    Timer.WaitMicroSecs( WaitMicroSecs );

}

/** Called when timer expires

In production code this would start the next read
but for this test it simply restarts the timer

*/
void handle_expire(const boost::system::error_code& /*e*/)
{
    if( myRunCounter.Count() )
    {
        // The run counter has been called reuired times, output report
        cout << "Target " << TargetHz << " Hz Actual: "
             //            << WaitMicroSecs << " microsecs ( nominal "
             //         << 1000000 / WaitMicroSecs << " Hz ) -> "
             << (int)myRunCounter.Hz() << " Hz, delta "
             << (int)(myRunCounter.Hz() - TargetHz) << "\n";

        // check if test complete
        NextTestDelay();
    }

    // start next timed wait
    StartWait();
}

/** Called to start a new timed wait */
void StartWait()
{
    if( WaitMicroSecs > 1000 )
    {
        // use high performance timer
        // which should be reliable for waits longer than a millisecond

        myPlaybackTimer->expires_from_now(
            std::chrono::microseconds( WaitMicroSecs ) );
        myPlaybackTimer->async_wait( handle_expire );
    }
    else
    {
        //cout << "StartWait ";
        // too fast for clock interupt timer
        // so start busy loop timer in its own thread
        Timer.Wait();

        // post the first check for timer completion
        io_service.post( CheckBusyLoopDone );
    }

}

int main( int argc, char * argv[])
{
    cout << "Busy Loop Continuous timer\n";

    // ensure that the interrupt clock rate is at its maximum
    // 1 millisecond
    MMRESULT  res = timeBeginPeriod( 1 );
    if( res != TIMERR_NOERROR )
    {
        cout << "timeBeginPeriod no can do\n";
        exit(1);
    }

    // start the busy loop timer
    // for now this will just spin in its own thread
    // "waiting" for a request to start timing
    Timer.Run();

    TargetHz = 0;
    NextTestDelay();

    myPlaybackTimer = new boost::asio::high_resolution_timer( io_service );

    // Start first wait
    StartWait();

    // start event loop
    io_service.run();

    return 0;
}

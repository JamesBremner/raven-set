namespace raven
{
namespace set
{

/** @brief Report frequency at which a call is made

Usage:

<pre>
    const int macCount = ???;
    raven::set::cRunCount runCounter( maxCount );

    for( int k = 0; k < ???; k++ )
    {
        // Some processing  to be measured
        ...

        if( runCounter.Count() )
        {
            cout << maxCount << " calls at "
                << runCounter.Hz() << " Hz\n";
        }
    }
</pre>

 */

class cRunCount
{
public:
    /** Constructor
        @param[in] maxCount  Number of counts over which the time is measured
    */
    cRunCount( int maxCount )
    : myMaxCount( maxCount )
    , myCount( -1 )
    {
        QueryPerformanceFrequency( (LARGE_INTEGER *)&myFreq );
    }
    /** Count
        @return true if the maxCount calls have been made
    */
    bool Count()
    {

        if( myCount == -1 )
        {
            QueryPerformanceCounter( (LARGE_INTEGER *)&myTimeStart );
        }

        myCount++;

        if( myCount >= myMaxCount )
        {
            myCount = 0;
            __int64 t;
            QueryPerformanceCounter( (LARGE_INTEGER *)&t );
            mySecs = (float)(t-myTimeStart) / (float)myFreq;
            myTimeStart = t;
            return true;
        }

        return false;
    }
    float Secs()
    {
        return mySecs;
    }

    /** Frequency at which maxCount calles were made */
    float Hz()
    {
        return myMaxCount / mySecs;
    }
    int MaxCount()
    {
        return myMaxCount;
    }

private:
    int myMaxCount;
    int myCount;
    __int64 myTimeStart;
    __int64 myFreq;             //Performance counter frequency
    float mySecs;
} ;
}
}



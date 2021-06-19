/*

Copyright (c) 2010 James Bremner, Raven's Point Consulting, ravenspoint@yahoo.com

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#include <iomanip>
#include <iostream>
#include <fstream>
#ifndef raven_set_crunwatch
#include "cRunWatch.h"
#endif
using namespace std;
using namespace boost::accumulators;

namespace raven
{
namespace set
{

int cRunWatch::myFlagProfiling = 0;
map < string, accumulator_set<double, stats<tag::variance(lazy)> > > cRunWatch::myMap;
int cRunWatch::myFlagDaisyChain;
thread_local std::vector< std::string * > * cRunWatch::myDaisyChain;

boost::mutex cRavenProfileMutex;

cRunWatch::cRunWatch( const char* name )
{
    if( myFlagProfiling )
    {
        myFlagTimer = 1;
        myName = std::string( name );
        QueryPerformanceCounter( (LARGE_INTEGER *)&myTimeStart );
    }
    else
    {
        myFlagTimer = 0;
    }

    if( myFlagDaisyChain )
    {
        // using daisy chain feature
        if( ! myDaisyChain )
        {
            // first time called by this thread
            // construct daisy chain to be used in all subsequent calls from this thread
            myDaisyChain = new std::vector< std::string * >;
        }
        // add name of this watcher to the end of the daisy chain
        myDaisyChain->push_back( &myName );

    }
}

/**

  Destructor

  If the timer is running, stop timer and store in accumulator

*/
cRunWatch::~cRunWatch()
{
    // check timer is running
    if( ! myFlagTimer )
        return;

    // get duration of this scope
    __int64 t;
    QueryPerformanceCounter( (LARGE_INTEGER *)&t );
    t -= myTimeStart;

    // name of accumulator where this runtime will be added
    // unless daisy chain is being used, simply the name of the watcher
    std::string acc_name( myName );

    if( myFlagDaisyChain )
    {
        // using daisy chain feature

        // loop through the daisy chain
        // ( list of cuurrently running watcher names in this thread )
        // to build the name of the accumulator
        acc_name = "";
        for( std::vector< std::string * >::iterator it = myDaisyChain->begin();
                it != myDaisyChain->end(); it++ )
        {
            acc_name += **it + '/';
        }

        // remove name of this watcher from the daisy chain
        myDaisyChain->pop_back();
    }

    // ensure that no other thread is updating accumulors
    boost::mutex::scoped_lock
    lock(cRavenProfileMutex);

    // find accumulator for this scope
    map_iter_t p = myMap.find( acc_name );
    if( p == myMap.end() )
    {
        // this is the first time this scope has run
        p = myMap.insert( pair<string,acc_t > (acc_name, acc_t()) ).first;
    }
    // add the time of running to the accumulator for this scope
    (p->second)( (double) t );

}
/**

  Extract statistics from accumulator

  @param[in] f clock frequency
  @param[in] p iterator into map of accumumators

*/
void cRunWatch::Tally(__int64& f, map_iter_t& p)
{
    aver	= ( (float) mean(p->second) / f );
    //stdev	= ( (float) sqrt( ((double) variance(p->second))  ) / f );
    count	= ( boost::accumulators::count(p->second) );
    myTotalTime = count * aver;

}

FILE * cRunWatch::fp;

string cRunWatch::Print() const
{
    stringstream s;
    s << setw(8) << count
        << "\t" << aver
        << "\t" << count*aver
        << "\t" <<  myName << "\n";
    return s.str();
}

/**

  Generate profile report

  @param[in] filename Name of result log file to create, default standard out
  @return the report in a string

  This is a static function, so it can be invoked without an instance of the profiler.


*/
string cRunWatch::Report( const char * filename )
{
    if( ! myFlagProfiling )
    {
        return "";
    }

    // number of ticks per second
    __int64 f;
    QueryPerformanceFrequency( (LARGE_INTEGER *)&f );


    // extract final statistics for all scopes
    multiset < cRunWatch,less<cRunWatch> > sdata;
    for( map_iter_t p = myMap.begin();
            p != myMap.end(); p++ )
    {
        sdata.insert( cRunWatch( p, f ) );
    }

    // format report
    stringstream s;
    s << "raven::set::cRunWatch code timing profile\n";
    s << "Calls\t\tMean (secs)\tTotal\t\tScope\n";
    for( auto& w : sdata )
    {
        s << w.Print();
    }

    cout << s.str();

    if( filename )
    {
        ofstream ofs( filename );
        if( ofs ) {
            ofs << s.str();
        }
    }

    return s.str();
}

}
}

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

#pragma once

#include "windows.h"

#include <boost/mem_fn.hpp>
#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/thread/mutex.hpp>

#include <cstdio>
#include <map>
#include <string>
#include <set>
#include <vector>

#define raven_set_crunwatch

namespace raven {
	namespace set {

class cRunWatch
{
public:

	// construct profiler for a particular scope
	// call at begining of scope to be timed
	// pass unique name of scope
	cRunWatch( const char* name );

	// destructor - automatically called when scope ends to stop timer
	~cRunWatch();

	// switch on, or off, basic profiling
	static void Start( int start=1 ) { if ( start ) myFlagProfiling = 1; else myFlagProfiling = 0; }

	/**  Switch on, or off, profiling with daisy chaining

	This will output the "call stack" if watched routines are called from watched routines
	So if a routine is called from more than one higher level routines
	the runtime will be stored separately for each call path.

	The idea for this feature and the implementation method
	is due to Moshe Rubin moshe@hyperlync.com

	*/
	static void StartDaisyChain( int start=1 ) { myFlagDaisyChain = 1; Start( start ); }

	// output report;
	static std::string Report(const char * filename = 0 );

	// overload < operator so that the scope with the longest total time is sorted first
	bool operator<(const cRunWatch other ) const
	{ return myTotalTime > other.myTotalTime; }

	/** Clear previous results */
	static void Clear()
	{
	    myMap.clear();
	}

private:

	typedef boost::accumulators::accumulator_set<double, boost::accumulators::stats<boost::accumulators::tag::variance(boost::accumulators::lazy)> > acc_t;
	typedef std::map < std::string, acc_t > map_t;
	typedef map_t::iterator map_iter_t;

	// Data store for run times
	// A map of statistical accumulators keyed by scope name
	// One map is shared by all instances of cRavenProfile
	static map_t myMap;

	// The name of the scope
	std::string myName;

	// The time when the scope was last entered
	__int64 myTimeStart;

	// True if timer is running
	int myFlagTimer;

	// True if profiling required
	static int myFlagProfiling;

	// True if daisychaining required
	static int myFlagDaisyChain;

	// thread specific store of watchers currently running
	static  thread_local std::vector< std::string * > * myDaisyChain;

	// Statistics
	float aver;
	float stdev;
	int count;
	float myTotalTime;

	static FILE * fp;

	// private constructor - used to tally results and print report
	cRunWatch( map_iter_t& p, __int64& f ) :
	myName( p->first ),
	myFlagTimer( 0 )
	{ Tally( f, p ); }
	void Tally(__int64& f, map_iter_t& p);
	std::string Print() const;

};
	}
}

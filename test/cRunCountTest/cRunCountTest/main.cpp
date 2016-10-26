#include <iostream>
#include <windows.h>

#include "cRunCount.h"

using namespace std;

int main()
{
    const int maxCount = 1000;
    raven::set::cRunCount runCount( maxCount );

    for( int k = 0; k < 10000; k++ )
    {
        Sleep( 2 );
        if( runCount.Count() )
        {
            cout << maxCount << " calls at "
                << runCount.Hz() << " Hz\n";
        }
    }
    return 0;
}

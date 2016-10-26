#pragma once
#include <vector>
#include <string>



namespace raven
{
namespace sqlite
{

#include "sqlite3.h"

/**

SQLITE Wrapper

Stores everything as a UTF16 string

*/
class cDB
{
public:
    /// Query results stored in vector of wstrings
    std::vector < std::wstring > myResult;
    std::vector < std::string > myResultA;

    /// Number of columns returned by last query
    int myColCount;

    /// if not null, points to last error message
    char * myError;

    /// the last formatted query submitted to the db engine
    wchar_t queryformatted[1000];
    char queryformattedA[1000];

    /// Construct class, but do not open database
    cDB() : db( 0 ) {}

    /// Construct and open database
    cDB( const wchar_t * fname ) : db( 0 )
    {
        Open( fname );
    }
    cDB( const char * fname ) : db( 0 )
    {
        Open( fname );
    }

    /// Open database
    void Open( const wchar_t * fname );
    void Open( const char * fname );

    /// Open database, wstring
    void Open( const std::wstring fname )
    {
        Open( fname.c_str() );
    }
    void Open( const std::string fname )
    {
        Open( fname.c_str() );
    }

    /// Close database
    void Close();

    /// close database, destroy class
    ~cDB();

    /**
    Submit query

    @param[in] f query format string, like printf

    @return  number of rows in result, -1 if error

    Any results stored as UTF16 strings in cQuery::myResult
    */
    int Query( const wchar_t * f, ... );
    int Query( const char * f, ... );

    /**

    Get handle to database, for C API calls

    @return  handle to database, 0 if database is closed

    */
    sqlite3 * getHandle()
    {
        return db;
    }

private:
    sqlite3 * db;
};


}
}

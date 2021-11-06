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
    wchar_t queryformatted[2000];
    char queryformattedA[3000];

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
    void Open( const std::wstring& fname )
    {
        Open( fname.c_str() );
    }
    void Open( const std::string& fname )
    {
        Open( fname.c_str() );
    }

    /// Close database
    void Close();

    /// close database, destroy class
    ~cDB();

    /// true if db is open
    bool isOpen()
    { return (bool) db;
    }

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

    /** compile SQL statement
     * @param[in] f the SQL statement
     * @return pointer to compiled stement, 0 on error
     */
    sqlite3_stmt * Prepare(
        const std::string & f );

    /** bind value to SQL parameter
     * @param[in] index 1-based index of SQL parameter
     * @param[in] value
     */
    int Bind( int index, int value );
    int Bind( int index, float value )
    {
        return Bind( index, (double) value);
    }
    int Bind( int index, double value );

    /** bind blob to SQL parameter
     * @param[in] index 1-based index of SQL parameter
     * @param[in] pointer to start of blob data
     * @param[in] size of blob in bytes
     */
    int Bind( int index, void* p, int size );

    /** bind string value to SQL parameter
     * @param[in] index 1-based index of SQL parameter
     * @param[in] value
     */
    int Bind( int index, const std::string& str );

    int step()
    {
        return sqlite3_step(stmt);
    }
    int reset()
    {
        return sqlite3_reset(stmt);
    }
    int finalize()
    {
        return sqlite3_finalize(stmt);
    }
    /** get value read from colum
     * @param[in] index zero-based index of column
     * @return value as an integer
     */
    int ColumnInt( int index ) {
        return sqlite3_column_int(stmt, index);
    }
    /** get value read from colum
     * @param[in] index zero-based index of column
     * @return value as an integer
     */
    double ColumnDouble( int index ) {
        return sqlite3_column_double( stmt, index );
    }

private:
    sqlite3 * db;
    sqlite3_stmt * stmt;

    /// error in query - provide human readable msg
    void decodeError();
};


}
}

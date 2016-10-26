#include "raven_sqlite.h"

#include <cstdio>
#include <stdarg.h>

using std::wstring;
using std::string;
using std::vector;



#ifdef _mingw_
#include <cstring>
#define vswprintf_s vsnwprintf
#define vsprintf_s vsnprintf
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

namespace raven {
	namespace sqlite {

		/**

		Open database file UTF16

		@param[in] fname

		*/
		void cDB::Open( const wchar_t * fname )
		{
			if( db ) {
				myError = "ERROR: db already open";
				return;
			}
			int ret = sqlite3_open16(fname,&db);
			if( ret ) {
				myError = "ERROR: failed to open db";
				return;
			}
			if( ! db ) {
				myError = "ERROR: failed to open db";
				return;
			}
			myError = 0;
		}
		/**

		Open database file UTF8

		@param[in] fname

		*/
		void cDB::Open( const char * fname )
		{
			if( db ) {
				myError = "ERROR: db already open";
				return;
			}
			int ret = sqlite3_open(fname,&db);
			if( ret ) {
				myError = "ERROR: failed to open db";
				return;
			}
			if( ! db ) {
				myError = "ERROR: failed to open db";
				return;
			}
			myError = 0;
		}

		void cDB::Close()
		{
			if( db )
				sqlite3_close(db);
			db = 0;

		}

		cDB::~cDB()
		{
			Close();
		}

		int cDB::Query( const wchar_t * f, ... )
		{
			if( ! db ) {
				myError = "ERROR: db not open";
				return -1;
			}

			va_list _Arglist;
			va_start(_Arglist, f);
			int r;

			// clear results
			myResult.clear();
			myError = 0;

			// format query
			r = vswprintf_s(queryformatted, 999, f, _Arglist);
			va_end(_Arglist);
			if( r < 1 ) {
				myError = "Query string too long for buffer";
				return -1;
			}

			// submit query - just one
			sqlite3_stmt *st = 0;
			int ret;
			void const * tail = 0;
			ret = sqlite3_prepare16_v2( db, queryformatted, -1, &st, &tail );
			if( ret != SQLITE_OK ) {
				// error in query - provide human readable msg
				int nErrMsg = 1 + (int)strlen(sqlite3_errmsg(db));
				myError = (char*)sqlite3_malloc(nErrMsg);
				if( myError ){
					memcpy((void*)myError, sqlite3_errmsg(db), nErrMsg);
				} else {
					myError = "out of memory";
				}
				return -1;
			}

			if( sqlite3_step( st ) != SQLITE_ROW ) {
				// no data returned, so finalize
				sqlite3_finalize( st );
				return 0;
			}

			// check for some results
			if( ! st )
				return 0;
			myColCount = sqlite3_column_count( st );
			if( ! myColCount ) {
				return 0;
			}

			int row_count = 0;
			// loop over rows returned
			do {
				// loop over columns
				for( int kcol = 0; kcol < myColCount; kcol++ ) {
					// check for null value
					if( sqlite3_column_type( st, kcol ) == SQLITE_NULL )
						myResult.push_back( wstring(L""));
					else
						myResult.push_back( wstring( ( wchar_t * )sqlite3_column_text16(st, kcol)) );
				}
				row_count++;
			} while( sqlite3_step( st ) == SQLITE_ROW );

			// clean up
			sqlite3_finalize( st );

			return row_count;

		}
        int cDB::Query( const char * f, ... )
		{
			if( ! db ) {
				myError = "ERROR: db not open";
				return -1;
			}

			va_list _Arglist;
			va_start(_Arglist, f);
			int r;

			// clear results
			myResultA.clear();
			myError = 0;

			// format query
			r = vsprintf_s(queryformattedA, 999, f, _Arglist);
			va_end(_Arglist);
			if( r < 1 ) {
				myError = "Query string too long for buffer";
				return -1;
			}

			// submit query - just one
			sqlite3_stmt *st = 0;
			int ret;
			char const * tail = 0;
			ret = sqlite3_prepare_v2( db, queryformattedA, -1, &st, &tail );
			if( ret != SQLITE_OK ) {
				// error in query - provide human readable msg
				int nErrMsg = 1 + (int)strlen(sqlite3_errmsg(db));
				myError = (char*)sqlite3_malloc(nErrMsg);
				if( myError ){
					memcpy((void*)myError, sqlite3_errmsg(db), nErrMsg);
				} else {
					myError = "out of memory";
				}
				return -1;
			}

			if( sqlite3_step( st ) != SQLITE_ROW ) {
				// no data returned, so finalize
				sqlite3_finalize( st );
				return 0;
			}

			// check for some results
			if( ! st )
				return 0;
			myColCount = sqlite3_column_count( st );
			if( ! myColCount ) {
				return 0;
			}

			int row_count = 0;
			// loop over rows returned
			do {
				// loop over columns
				for( int kcol = 0; kcol < myColCount; kcol++ ) {
					// check for null value
					if( sqlite3_column_type( st, kcol ) == SQLITE_NULL )
						myResultA.push_back( string(""));
					else
						myResultA.push_back( string( ( char * )sqlite3_column_text(st, kcol)) );
				}
				row_count++;
			} while( sqlite3_step( st ) == SQLITE_ROW );

			// clean up
			sqlite3_finalize( st );

			return row_count;

		}
	}
}

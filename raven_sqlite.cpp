#include "raven_sqlite.h"

#include <cstdio>
#include <stdarg.h>
#include <cstring>

using std::string;
using std::vector;
using std::wstring;

#ifdef _mingw_
#include <cstring>
#define vswprintf_s vsnwprintf
#define vsprintf_s vsnprintf
#pragma GCC diagnostic ignored "-Wwrite-strings"
#endif

namespace raven
{
	namespace sqlite
	{

		/**

		Open database file UTF16

		@param[in] fname

		*/
		void cDB::Open(const wchar_t *fname)
		{
			if (db)
			{
				myError = (char*)"ERROR: db already open";
				return;
			}
			int ret = sqlite3_open16(fname, &db);
			if (ret)
			{
				myError = (char*)"ERROR: failed to open db";
				return;
			}
			if (!db)
			{
				myError = (char*)"ERROR: failed to open db";
				return;
			}
			myError = 0;
		}
		/**

		Open database file UTF8

		@param[in] fname

		*/
		void cDB::Open(const char *fname)
		{
			if (db)
			{
				myError = (char*)"ERROR: db already open";
				return;
			}
			int ret = sqlite3_open(fname, &db);
			if (ret)
			{
				myError = (char*)"ERROR: failed to open db";
				return;
			}
			if (!db)
			{
				myError = (char*)"ERROR: failed to open db";
				return;
			}
			myError = 0;
		}

		void cDB::Close()
		{
			if (db)
				sqlite3_close(db);
			db = 0;
		}

		cDB::~cDB()
		{
			Close();
		}

		int cDB::Query(const wchar_t *f, ...)
		{
			if (!db)
			{
				myError = (char*)"ERROR: db not open";
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
			if (r < 1)
			{
				myError = (char*)"Query string too long for buffer";
				return -1;
			}

			// submit query - just one
			sqlite3_stmt *st = 0;
			int ret;
			void const *tail = 0;
			ret = sqlite3_prepare16_v2(db, queryformatted, -1, &st, &tail);
			if (ret != SQLITE_OK)
			{
				// error in query - provide human readable msg
				int nErrMsg = 1 + (int)strlen(sqlite3_errmsg(db));
				myError = (char *)sqlite3_malloc(nErrMsg);
				if (myError)
				{
					memcpy((void *)myError, sqlite3_errmsg(db), nErrMsg);
				}
				else
				{
					myError = (char*)"out of memory";
				}
				return -1;
			}

			if (sqlite3_step(st) != SQLITE_ROW)
			{
				// no data returned, so finalize
				sqlite3_finalize(st);
				return 0;
			}

			// check for some results
			if (!st)
				return 0;
			myColCount = sqlite3_column_count(st);
			if (!myColCount)
			{
				return 0;
			}

			int row_count = 0;
			// loop over rows returned
			do
			{
				// loop over columns
				for (int kcol = 0; kcol < myColCount; kcol++)
				{
					// check for null value
					if (sqlite3_column_type(st, kcol) == SQLITE_NULL)
						myResult.push_back(wstring(L""));
					else
						myResult.push_back(wstring((wchar_t *)sqlite3_column_text16(st, kcol)));
				}
				row_count++;
			} while (sqlite3_step(st) == SQLITE_ROW);

			// clean up
			sqlite3_finalize(st);

			return row_count;
		}
		int cDB::Query(const char *f, ...)
		{
			if (!db)
			{
				myError = (char*)"ERROR: db not open";
				return -1;
			}

			va_list _Arglist;
			va_start(_Arglist, f);
			int r;

			// clear results
			myResultA.clear();
			myError = 0;

			// format query
			r = vsprintf_s(queryformattedA, 1999, f, _Arglist);
			va_end(_Arglist);
			if (r < 1)
			{
				myError = (char*)"Query string too long for buffer";
				return -1;
			}

			// submit query - just one
			sqlite3_stmt *st = 0;
			int ret;
			char const *tail = 0;
			ret = sqlite3_prepare_v2(db, queryformattedA, -1, &st, &tail);
			if (ret != SQLITE_OK)
			{
				// error in query - provide human readable msg
				int nErrMsg = 1 + (int)strlen(sqlite3_errmsg(db));
				myError = (char *)sqlite3_malloc(nErrMsg);
				if (myError)
				{
					memcpy((void *)myError, sqlite3_errmsg(db), nErrMsg);
				}
				else
				{
					myError = (char*)"out of memory";
				}
				return -1;
			}

			if (sqlite3_step(st) != SQLITE_ROW)
			{
				// no data returned, so finalize
				sqlite3_finalize(st);
				return 0;
			}

			// check for some results
			if (!st)
				return 0;
			myColCount = sqlite3_column_count(st);
			if (!myColCount)
			{
				return 0;
			}

			int row_count = 0;
			// loop over rows returned
			do
			{
				// loop over columns
				for (int kcol = 0; kcol < myColCount; kcol++)
				{
					// check for null value
					if (sqlite3_column_type(st, kcol) == SQLITE_NULL)
						myResultA.push_back(string(""));
					else
						myResultA.push_back(string((char *)sqlite3_column_text(st, kcol)));
				}
				row_count++;
			} while (sqlite3_step(st) == SQLITE_ROW);

			// clean up
			sqlite3_finalize(st);

			return row_count;
		}
		sqlite3_stmt *cDB::Prepare(
			const std::string &f)
		{
			stmt = 0;
			const char *tail = 0;
			int ret = sqlite3_prepare_v2(
				db,
				f.c_str(),
				-1,
				&stmt,
				&tail);
			if (ret != SQLITE_OK)
			{
				stmt = 0;
				decodeError();
				return stmt;
			}
			return stmt;
		}
		int cDB::Bind(int index, int value)
		{
			if (!stmt)
			{
				myError = (char*)"no compiled statement";
				return -1;
			}
			if (sqlite3_bind_int(stmt, index, value))
			{
				decodeError();
				return -1;
			}
			return 0;
		}
		int cDB::Bind(int index, double value)
		{
			if (!stmt)
			{
				myError = (char*)"no compiled statement";
				return -1;
			}
			if (sqlite3_bind_double(stmt, index, value))
			{
				decodeError();
				return -1;
			}
			return 0;
		}
		int cDB::Bind(int index, void *p, int size)
		{
			if (!stmt)
			{
				myError = (char*)"no compiled statement";
				return -1;
			}
			if (sqlite3_bind_blob(stmt, index, p, size, SQLITE_STATIC))
			{
				decodeError();
				return -1;
			}
			return 0;
		}
		int cDB::Bind(int index, const std::string &str)
		{
			if (!stmt)
			{
				myError = (char*)"no compiled statement";
				return -1;
			}
			if (sqlite3_bind_text(stmt, index, str.c_str(), str.length(), SQLITE_STATIC))
			{
				decodeError();
				return -1;
			}
			return 0;
		}
		void cDB::decodeError()
		{
			int nErrMsg = 1 + (int)strlen(sqlite3_errmsg(db));
			myError = (char *)sqlite3_malloc(nErrMsg);
			if (myError)
			{
				memcpy((void *)myError, sqlite3_errmsg(db), nErrMsg);
			}
			else
			{
				myError = (char*)"out of memory";
			}
		}
	}
}

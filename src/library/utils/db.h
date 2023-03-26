#include "iostream"
#include <chrono>
#include <sqlite3.h>

static int callback( void *data, int argc, char **argv, char **azColName ) { return 0; }

class DB
{
  public:
    ~DB() { sqlite3_close( mDb ); }

    void add_los_timing_data_to( std::string table, std::chrono::nanoseconds timeToCopy,
                                 std::chrono::nanoseconds timeToEval )
    {
        std::string sql = "INSERT INTO " + table +
                          "( time_to_copy, time_to_eval ) "
                          " VALUES( " +
                          std::to_string( timeToCopy.count() ) + "," + std::to_string( timeToEval.count() ) + " )";

        rc = sqlite3_exec( mDb, sql.c_str(), callback, (void *)data, &zErrMsg );

        if ( rc != SQLITE_OK )
        {
            fprintf( stderr, "SQL error: %s\n", zErrMsg );
            sqlite3_free( zErrMsg );
        }
    }

  private:
    sqlite3 *mDb;
    int rc = sqlite3_open( "/home/cahik/Codes/C++/cpp-viewshed-library/tests.db", &mDb );
    char *zErrMsg = 0;
    const char *data = "Callback function called";
};

DB db = DB();
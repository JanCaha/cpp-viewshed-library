#include "iostream"
#include <chrono>
#include <pqxx/pqxx>

class PG
{
  public:
    void add_los_timing_data_to( std::string table, std::chrono::nanoseconds timeToCopy,
                                 std::chrono::nanoseconds timeToEval, long long losSize )
    {
        pqxx::connection *mConn =
            new pqxx::connection( "host=localhost dbname=habilitace user=admin password=root port=5430" );

        pqxx::work W( *mConn );

        std::string sql = "INSERT INTO " + table +
                          "( time_to_copy, time_to_eval, los_size, inserted ) "
                          " VALUES( " +
                          std::to_string( timeToCopy.count() ) + "," + std::to_string( timeToEval.count() ) + "," +
                          std::to_string( losSize ) + ", now() );";
        try
        {
            W.exec( sql );
            W.commit();
        }
        catch ( const std::exception &e )
        {
            std::cout << e.what() << std::endl;
        }
        delete mConn;
    }
};

PG pg = PG();
#include "iostream"
#include <chrono>
#include <pqxx/pqxx>

#include "abstractlos.h"

using viewshed::AbstractLoS;

class PG
{
  public:
    void add_los_timing_data_to( std::string table, std::shared_ptr<AbstractLoS> los )
    {
        const char *p_raster_size = getenv( "RASTER_SIZE" );
        std::string raster_size;

        if ( p_raster_size == nullptr )
        {
            raster_size = "-1";
        }
        else
        {
            raster_size = p_raster_size;
        }

        std::string sql =
            "INSERT INTO " + table +
            "( time_to_copy, time_to_eval, los_size, inserted, raster_size, time_to_prepare, los_size_original ) "
            " VALUES( " +
            std::to_string( los->timeToCopy.count() ) + "," + std::to_string( los->timeToEval.count() ) + "," +
            std::to_string( los->numberOfNodes() ) + ", now(), " + raster_size + ", " +
            std::to_string( los->timeToPrepare.count() ) + ", " + std::to_string( los->originalNodesCount ) + " );";

        executeSql( sql );
    }

    void executeSql( std::string &sql )
    {
        pqxx::connection mConn( mConnString );

        pqxx::work W( mConn );

        try
        {
            W.exec( sql );
            W.commit();
        }
        catch ( const std::exception &e )
        {
            std::cout << e.what() << std::endl;
        }
    }

    std::string mConnString = "host=localhost dbname=habilitace user=admin password=root port=5430";
};

PG pg = PG();

ulong CALLS = 0;
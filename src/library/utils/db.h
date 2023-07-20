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
        pqxx::connection *mConn =
            new pqxx::connection( "host=localhost dbname=habilitace user=admin password=root port=5430" );

        pqxx::work W( *mConn );

        std::string raster_size = getenv( "RASTER_SIZE" );

        std::string sql = "INSERT INTO " + table +
                          "( time_to_copy, time_to_eval, los_size, inserted, raster_size, time_to_prepare ) "
                          " VALUES( " +
                          std::to_string( los->timeToCopy.count() ) + "," + std::to_string( los->timeToEval.count() ) +
                          "," + std::to_string( los->numberOfNodes() ) + ", now(), " + raster_size + ", " +
                          std::to_string( los->timeToPrepare.count() ) + " );";
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
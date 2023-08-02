#include "iostream"
#include <chrono>
#include <pqxx/pqxx>
#include <random>

#include "abstractlos.h"

using viewshed::AbstractLoS;

std::uniform_real_distribution<double> Distribution( 0.0, 1.0 );
std::default_random_engine Generator;

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

        mNumberCalls++;
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

    void resetCounter() { mNumberCalls = 0; }

    void setSamplingValues( double toSavePercent ) { mPercentToSave = toSavePercent; }

    bool shouldSample()
    {
        if ( mNumberCalls > mMaxValuesToSave )
        {
            return false;
        }

        if ( Distribution( Generator ) < mPercentToSave )
        {
            return true;
        }

        return false;
    }

    std::string mConnString = "host=localhost dbname=habilitace user=admin password=root port=5430";

    size_t mNumberCalls = 0;

    size_t mMaxValuesToSave = 100000;

    double mPercentToSave = 0.01;
};

PG pg = PG();

void handle_inverse_viewshed_los_timing( std::shared_ptr<AbstractLoS> los )
{
    pg.add_los_timing_data_to( "inverseviewshed_los_timing", los );
}

void handle_viewshed_los_timing( std::shared_ptr<AbstractLoS> los )
{
    pg.add_los_timing_data_to( "viewshed_los_timing", los );
}
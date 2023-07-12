#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <cstdlib>
#include <filesystem>

#include "simplerasters.h"

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "point.h"
#include "viewshed.h"
#include "viewshedutils.h"
#include "visibility.h"
#include "visibilityangledifferencetolocalhorizon.h"
#include "visibilityboolean.h"
#include "visibilityhorizons.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;
using namespace std::chrono::_V2;

using MiliSeconds = std::chrono::milliseconds;

class ViewshedTimingTest : public ::testing::Test
{
  protected:
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    std::shared_ptr<Point> vp = std::make_shared<Point>( OGRPoint( -336364.021, -1189108.615 ), dem );
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs = ViewshedUtils::allAlgorithms();
    std::shared_ptr<Viewshed> viewshed = std::make_shared<Viewshed>( vp, dem, algs );
};

std::string TRUE_VALUE = "true";

u_long estimateTime( u_long measuredTime, double percent = 0.1 )
{
    if ( TRUE_VALUE.compare( std::getenv( "GITHUB_ACTIONS" ) ) )
    {
        percent = 0.75;
    }

    return measuredTime + static_cast<u_long>( measuredTime * percent );
}

TEST_F( ViewshedTimingTest, initEventList )
{
    system_clock::time_point startTime = high_resolution_clock::now();
    viewshed->initEventList();
    system_clock::time_point endTime = high_resolution_clock::now();

    MiliSeconds diff = std::chrono::duration_cast<MiliSeconds>( endTime - startTime );

    ASSERT_GE( estimateTime( 60 ), diff.count() );
}

TEST_F( ViewshedTimingTest, sortEventList )
{
    viewshed->initEventList();

    system_clock::time_point startTime = high_resolution_clock::now();
    viewshed->sortEventList();
    system_clock::time_point endTime = high_resolution_clock::now();

    MiliSeconds diff = std::chrono::duration_cast<MiliSeconds>( endTime - startTime );

    ASSERT_GE( estimateTime( 30 ), diff.count() );
}

TEST_F( ViewshedTimingTest, parseEventList )
{
    viewshed->initEventList();
    viewshed->sortEventList();
    viewshed->setMaxThreads( 1 );

    system_clock::time_point startTime = high_resolution_clock::now();
    viewshed->parseEventList();
    system_clock::time_point endTime = high_resolution_clock::now();

    MiliSeconds diff = std::chrono::duration_cast<MiliSeconds>( endTime - startTime );

    ASSERT_GE( estimateTime( 1300 ), diff.count() );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
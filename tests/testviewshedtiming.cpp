#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
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

class ViewshedTimingTest : public ::testing::Test
{
  protected:
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    std::shared_ptr<Point> vp = std::make_shared<Point>( OGRPoint( -336364.021, -1189108.615 ), dem );
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs = ViewshedUtils::allAlgorithms();
    std::shared_ptr<Viewshed> viewshed = std::make_shared<Viewshed>( vp, dem, algs );
};

u_long estimateTime( u_long measuredTime, double percent = 0.05 )
{
    return measuredTime + static_cast<u_long>( measuredTime * percent );
}

TEST_F( ViewshedTimingTest, initEventList )
{
    system_clock::time_point startTime = high_resolution_clock::now();
    viewshed->initEventList();
    system_clock::time_point endTime = high_resolution_clock::now();

    std::chrono::nanoseconds diff = std::chrono::duration_cast<std::chrono::nanoseconds>( endTime - startTime );

    ASSERT_GE( estimateTime( 99805811 ), diff.count() );
}

TEST_F( ViewshedTimingTest, sortEventList )
{
    viewshed->initEventList();

    system_clock::time_point startTime = high_resolution_clock::now();
    viewshed->sortEventList();
    system_clock::time_point endTime = high_resolution_clock::now();

    std::chrono::nanoseconds diff = std::chrono::duration_cast<std::chrono::nanoseconds>( endTime - startTime );

    ASSERT_GE( estimateTime( 81400891 ), diff.count() );
}

TEST_F( ViewshedTimingTest, parseEventList )
{
    viewshed->initEventList();
    viewshed->sortEventList();

    system_clock::time_point startTime = high_resolution_clock::now();
    viewshed->parseEventList();
    system_clock::time_point endTime = high_resolution_clock::now();

    std::chrono::nanoseconds diff = std::chrono::duration_cast<std::chrono::nanoseconds>( endTime - startTime );

    ASSERT_GE( estimateTime( 2355788714 ), diff.count() );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
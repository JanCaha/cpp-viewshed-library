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

bool isRunningOnGithubActions()
{
    const char *env_github_action = std::getenv( "GITHUB_ACTIONS" );

    if ( env_github_action != nullptr )
    {
        if ( TRUE_VALUE.compare( std::string( env_github_action ) ) == 0 )
        {
            return true;
        }
    }

    return false;
}

u_long estimateTime( u_long measuredTime, double percent = 0.1 )
{
    const char *env_github_action = std::getenv( "GITHUB_ACTIONS" );

    if ( isRunningOnGithubActions() )
    {
        percent = 0.75;
    }

    return measuredTime + static_cast<u_long>( measuredTime * percent );
}

void printTimes( size_t estimatedTime, MiliSeconds realTime )
{
    if ( !isRunningOnGithubActions() )
    {
        std::cout << "Recorded time: " << std::to_string( realTime.count() )
                  << " estimated time: " << std::to_string( estimateTime( estimatedTime ) ) << std::endl;
    }
}

TEST_F( ViewshedTimingTest, initEventList )
{
    u_long time = 30;

    steady_clock::time_point startTime = steady_clock::now();
    viewshed->initEventList();
    steady_clock::time_point endTime = steady_clock::now();

    MiliSeconds diff = std::chrono::duration_cast<MiliSeconds>( endTime - startTime );

    printTimes( time, diff );
    ASSERT_GE( estimateTime( time ), diff.count() );
}

TEST_F( ViewshedTimingTest, sortEventList )
{
    u_long time = 20;
    viewshed->initEventList();

    steady_clock::time_point startTime = steady_clock::now();
    viewshed->sortEventList();
    steady_clock::time_point endTime = steady_clock::now();

    MiliSeconds diff = std::chrono::duration_cast<MiliSeconds>( endTime - startTime );

    printTimes( time, diff );
    ASSERT_GE( estimateTime( time ), diff.count() );
}

TEST_F( ViewshedTimingTest, parseEventList )
{
    u_long time = 900;

    viewshed->initEventList();
    viewshed->sortEventList();
    viewshed->setMaxThreads( 1 );

    steady_clock::time_point startTime = steady_clock::now();
    viewshed->parseEventList();
    steady_clock::time_point endTime = steady_clock::now();

    MiliSeconds diff = std::chrono::duration_cast<MiliSeconds>( endTime - startTime );

    printTimes( time, diff );
    ASSERT_GE( estimateTime( time ), diff.count() );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
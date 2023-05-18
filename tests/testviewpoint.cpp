#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "simplerasters.h"

#include "testsettings.h"

#include "point.h"
#include "viewshedutils.h"

using viewshed::Point;
using namespace viewshed;

TEST( ViewPoint, constructOk )
{
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    Point vp( OGRPoint( -336404.98, -1189162.66 ), dem, 1.6 );
    ASSERT_TRUE( vp.isValid() );
    ASSERT_NEAR( vp.x, -336404.98, 0.01 );
    ASSERT_NEAR( vp.y, -1189162.66, 0.01 );
    ASSERT_EQ( vp.row, 130 );
    ASSERT_EQ( vp.col, 126 );
    ASSERT_NEAR( vp.elevation, 1017.5416, 0.0001 );
    ASSERT_NEAR( vp.offset, 1.6, 0.001 );
    ASSERT_NEAR( vp.totalElevation(), 1017.5416 + 1.6, 0.0001 );
}

TEST( ViewPoint, constructOutside )
{
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    Point vp( OGRPoint( -336699.62, -1189319.20 ), dem, 1.6 );

    ASSERT_FALSE( vp.isValid() );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
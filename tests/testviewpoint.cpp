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
    ASSERT_NEAR( vp.mX, -336404.98, 0.01 );
    ASSERT_NEAR( vp.mY, -1189162.66, 0.01 );
    ASSERT_EQ( vp.mRow, 130 );
    ASSERT_EQ( vp.mCol, 126 );
    ASSERT_NEAR( vp.mElevation, 1017.5416, 0.0001 );
    ASSERT_NEAR( vp.mOffset, 1.6, 0.001 );
    ASSERT_NEAR( vp.totalElevation(), 1017.5416 + 1.6, 0.0001 );
}

TEST( ViewPoint, constructOutside )
{
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    Point vp( OGRPoint( -336699.62, -1189319.20 ), dem, 1.6 );

    ASSERT_FALSE( vp.isValid() );
}

TEST( ViewPoint, constructOutsideFarAway )
{
    // World origin (0,0) is in a different part of the world from the test DEM
    // which is in a projected coordinate system around (-336000, -1189000)
    auto dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    Point vp( OGRPoint( 0.0, 0.0 ), dem, 1.6 );
    ASSERT_FALSE( vp.isValid() );
}

TEST( ViewPoint, manualConstructSkipsBoundsCheck )
{
    // Point(row, col, elevation, offset, cellSize) sets mValid = true unconditionally.
    // A caller using negative row/col (clearly outside any raster) still gets isValid() == true.
    auto dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    Point vp( -1, -1, 1000.0, 1.6, dem->xCellSize() );
    ASSERT_TRUE( vp.isValid() );
}

TEST( ValidateRaster, nullPointer )
{
    std::shared_ptr<SingleBandRaster> nullRaster = nullptr;
    std::string error;
    ASSERT_FALSE( ViewshedUtils::validateRaster( nullRaster, error ) );
    ASSERT_FALSE( error.empty() );
}

TEST( ValidateRaster, nonExistentFile )
{
    auto raster = std::make_shared<ProjectedSquareCellRaster>( "/nonexistent/path/to/file.tif" );
    std::string error;
    ASSERT_FALSE( ViewshedUtils::validateRaster( raster, error ) );
    ASSERT_FALSE( error.empty() );
}

TEST( ValidateRaster, validFile )
{
    auto raster = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    std::string error;
    ASSERT_TRUE( ViewshedUtils::validateRaster( raster, error ) );
    ASSERT_TRUE( error.empty() );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
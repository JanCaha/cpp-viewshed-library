#include <gmock/gmock.h>
#include <gtest/gtest.h>

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

class ViewshedTest : public ::testing::Test
{
  protected:
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    std::shared_ptr<Point> vp = std::make_shared<Point>( OGRPoint( -336364.021, -1189108.615 ), dem );
    std::shared_ptr<ViewshedAlgorithms> algs = ViewshedUtils::allAlgorithms();
};

TEST_F( ViewshedTest, testLoS )
{
    OGRPoint poiPoint = OGRPoint( -336428.767, -1189102.785 );

    Viewshed v( vp, dem, algs );
    v.initEventList();
    v.sortEventList();

    LoSNode poi = v.statusNodeFromPoint( poiPoint );

    std::shared_ptr<LoS> los = v.getLoS( poiPoint );
    ASSERT_EQ( los->size(), 182 );

    los = v.getLoS( poiPoint, true );
    ASSERT_EQ( los->size(), 70 );

    std::vector<DataTriplet> data = ViewshedUtils::distanceElevation( los );
    ASSERT_EQ( data.size(), 72 );

    ViewshedUtils::saveToCsv( data, "distance,elevation\n", TEST_DATA_LOS );

    ASSERT_TRUE( std::filesystem::exists( TEST_DATA_LOS ) );
}

TEST_F( ViewshedTest, testInitCells )
{
    Viewshed v( vp, dem, algs );

    ASSERT_EQ( v.numberOfValidCells(), 0 );
    ASSERT_EQ( v.numberOfCellEvents(), 0 );

    v.initEventList();

    ASSERT_EQ( v.numberOfValidCells(), 37990 );
    ASSERT_EQ( v.numberOfCellEvents(), ( v.numberOfValidCells() - 1 ) * 3 );

#if ( CELL_EVENT_USE_FLOAT )
    ASSERT_EQ( v.sizeOfEvents(), 4102812 );
#else
    ASSERT_EQ( v.sizeOfEvents(), 7293888 );
#endif
}

TEST_F( ViewshedTest, testOutputRasterSize )
{
    Viewshed v( vp, dem, algs );
    v.prepareMemoryRasters();
#if ( CELL_EVENT_USE_FLOAT )
    ASSERT_EQ( v.sizeOfOutputRaster(), 156100 );
#else
    ASSERT_EQ( v.sizeOfOutputRaster(), 312200 );
#endif
}

TEST_F( ViewshedTest, testViewshedCalculation )
{
    Viewshed v( vp, dem, algs );
    v.calculate();

    ASSERT_EQ( v.numberOfResultRasters(), algs->size() );

    ASSERT_FALSE( v.hasError() );
    ASSERT_TRUE( v.saveResults( TEST_DATA_RESULTS_DIR ) );
}

TEST_F( ViewshedTest, saveResultsToInvalidLocationFails )
{
    Viewshed v( vp, dem, algs );
    v.calculate();

    ASSERT_FALSE( v.saveResults( "/nonexistent/folder/for/results" ) );
    ASSERT_TRUE( v.hasError() );
    ASSERT_FALSE( v.errorMessage().empty() );
}

TEST_F( ViewshedTest, resultRasterHonoursDataType )
{
    Viewshed v( vp, dem, algs );
    v.setDefaultResultDataType( GDALDataType::GDT_Float64 );
    v.calculate();

    ASSERT_EQ( v.resultRaster( 0 )->gdalDataType(), GDALDataType::GDT_Float64 );
}

TEST_F( ViewshedTest, testVisibilityRaster )
{
    OGRPoint poiPoint = OGRPoint( -336428.767, -1189102.785 );

    Viewshed v( vp, dem, algs );
    v.initEventList();
    v.sortEventList();

    v.calculateVisibilityRaster();
    ASSERT_TRUE( v.saveVisibilityRaster( TEST_DATA_RESULTS_VISIBILITY_RASTER ) );
}

TEST_F( ViewshedTest, testCalculateVisibilityMask )
{
    OGRPoint poiPoint = OGRPoint( -336428.767, -1189102.785 );

    Viewshed v( vp, dem, algs );
    v.initEventList();
    v.sortEventList();

    v.calculateVisibilityMask();
    ASSERT_TRUE( v.saveVisibilityRaster( TEST_DATA_RESULTS_VISIBILITY_RASTER ) );
}

TEST_F( ViewshedTest, saveVisibilityRasterWithoutCalculationFails )
{
    Viewshed v( vp, dem, algs );

    ASSERT_FALSE( v.saveVisibilityRaster( TEST_DATA_RESULTS_VISIBILITY_RASTER ) );
    ASSERT_TRUE( v.hasError() );
}

TEST_F( ViewshedTest, invalidViewpointProducesNoEvents )
{
    // A viewpoint outside the DEM extent is invalid, which makes the whole
    // viewshed invalid and initEventList produces zero events.
    auto invalidVp = std::make_shared<Point>( OGRPoint( 0.0, 0.0 ), dem );
    ASSERT_FALSE( invalidVp->isValid() );

    Viewshed v( invalidVp, dem, algs );
    ASSERT_FALSE( v.isValid() );

    v.initEventList();

    ASSERT_EQ( v.numberOfCellEvents(), 0 );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
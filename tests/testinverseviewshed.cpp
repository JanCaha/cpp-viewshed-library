#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>

#include "simplerasters.h"

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "inverseviewshed.h"
#include "point.h"
#include "viewshedtypes.h"
#include "viewshedutils.h"
#include "visibility.h"
#include "visibilityangledifferencetolocalhorizon.h"
#include "visibilityboolean.h"
#include "visibilityhorizons.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

class InverseViewshedTest : public ::testing::Test
{
  protected:
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM );
    std::shared_ptr<Point> tp = std::make_shared<Point>( OGRPoint( -336364.021, -1189108.615 ), dem, 0 );
    std::shared_ptr<ViewshedAlgorithms> algs = ViewshedUtils::allAlgorithms();
};

TEST_F( InverseViewshedTest, testLoS )
{
    OGRPoint poiPoint = OGRPoint( -336409.028, -1189172.056 );

    InverseViewshed v( tp, 3, dem, algs );
    v.initEventList();
    v.sortEventList();

    LoSNode poi = v.statusNodeFromPoint( poiPoint );

    std::shared_ptr<InverseLoS> los = v.getLoS( poiPoint );
    ASSERT_EQ( los->size(), 219 );

    los = v.getLoS( poiPoint, true );
    ASSERT_EQ( los->size(), 98 );

    std::vector<DataTriplet> data = ViewshedUtils::distanceElevation( los );
    ASSERT_EQ( data.size(), 100 );

    ViewshedUtils::saveToCsv( data, "distance,elevation,target_point\n", TEST_DATA_LOS );

    ASSERT_TRUE( std::filesystem::exists( TEST_DATA_LOS ) );
}

TEST_F( InverseViewshedTest, testInitCells )
{
    InverseViewshed v( tp, 2, dem, algs );
    ASSERT_EQ( v.numberOfValidCells(), 0 );
    ASSERT_EQ( v.numberOfCellEvents(), 0 );

    v.initEventList();

    ASSERT_EQ( v.numberOfValidCells(), 37990 );
    ASSERT_EQ( v.numberOfCellEvents(), ( v.numberOfValidCells() - 1 ) * 5 );
}

TEST_F( InverseViewshedTest, testInverseViewshedCalculation )

{
    InverseViewshed v( tp, 2, dem, algs );
    v.calculate();

    ASSERT_EQ( v.numberOfResultRasters(), algs->size() );

    v.saveResults( TEST_DATA_RESULTS_DIR, "Inverse" );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
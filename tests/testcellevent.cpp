#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "simplerasters.h"

#include "cellevent.h"
#include "point.h"
#include "viewshed.h"
#include "viewshedutils.h"
#include "visibilityalgorithms.h"

#include "testsettings.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::CellEvent;
using viewshed::Point;
using viewshed::Viewshed;
using viewshed::ViewshedUtils;

class CellEventTest : public ::testing::Test
{
  protected:
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM_SMALL );
    std::shared_ptr<Point> vp = std::make_shared<Point>( OGRPoint( -336311.5, -1189034.5 ), dem );
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs = ViewshedUtils::allAlgorithms();
};

TEST( CellEventTestStatic, size )
{
    int size;
#if ( CELL_EVENT_USE_FLOAT )
    size = 36;
#else
    size = 64;
#endif
    ASSERT_EQ( sizeof( CellEvent ), size );
}

TEST_F( CellEventTest, testCellEventCreation1 )
{
    Viewshed v( vp, dem, algs );

    int row = 2;
    int col = 1;
    bool solveCell = true;

    v.addEventsFromCell( row, col, dem->value( row, col ), solveCell );
    ASSERT_EQ( v.numberOfCellEvents(), 3 );

    CellEvent c;

    c = v.cellEvent( 0 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::ENTER );
    ASSERT_EQ( c.mRow, 2 );
    ASSERT_EQ( c.mCol, 1 );
    ASSERT_NEAR( c.mDist2point, 2.121320, 0.000001 );
    ASSERT_NEAR( c.mAngle, 2.356194, 0.000001 );
    ASSERT_NEAR( c.mElevation[0], 1008.2345, 0.0001 );
    ASSERT_NEAR( c.mElevation[1], 1006.3314, 0.0001 );
    ASSERT_NEAR( c.mElevation[2], 1004.1940, 0.0001 );

    c = v.cellEvent( 1 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::EXIT );
    ASSERT_NEAR( c.mDist2point, 2.549509, 0.000001 );
    ASSERT_NEAR( c.mAngle, 2.944197, 0.000001 );

    c = v.cellEvent( 2 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::CENTER );
    ASSERT_NEAR( c.mDist2point, 2.236068, 0.000001 );
    ASSERT_NEAR( c.mAngle, 2.677945, 0.000001 );
}

TEST_F( CellEventTest, testCellEventCreation2 )
{
    Viewshed v( vp, dem, algs );

    int row = 4;
    int col = 2;
    bool solveCell = true;

    v.addEventsFromCell( row, col, dem->value( row, col ), solveCell );
    ASSERT_EQ( v.numberOfCellEvents(), 3 );

    CellEvent c;

    c = v.cellEvent( 0 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::ENTER );
    ASSERT_EQ( c.mRow, 4 );
    ASSERT_EQ( c.mCol, 2 );
    ASSERT_NEAR( c.mDist2point, 1.581138, 0.000001 );
    ASSERT_NEAR( c.mAngle, 3.463343, 0.000001 );
    ASSERT_NEAR( c.mElevation[0], 1002.4038, 0.0001 );
    ASSERT_NEAR( c.mElevation[1], 1000.3604, 0.0001 );
    ASSERT_NEAR( c.mElevation[2], 1000.3604, 0.0001 );

    c = v.cellEvent( 1 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::EXIT );
    ASSERT_NEAR( c.mDist2point, 1.581138, 0.000001 );
    ASSERT_NEAR( c.mAngle, 4.390638, 0.000001 );

    c = v.cellEvent( 2 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::CENTER );
    ASSERT_NEAR( c.mDist2point, 1.414213, 0.000001 );
    ASSERT_NEAR( c.mAngle, 3.926990, 0.000001 );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
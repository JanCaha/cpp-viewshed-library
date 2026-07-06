#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "simplerasters.h"

#include "cellevent.h"
#include "point.h"
#include "viewshed.h"
#include "viewshedtypes.h"
#include "viewshedutils.h"
#include "visibilityalgorithms.h"

#include "testsettings.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::CellEvent;
using viewshed::Point;
using viewshed::Viewshed;
using viewshed::ViewshedAlgorithms;
using viewshed::ViewshedUtils;

class CellEventTest : public ::testing::Test
{
  protected:
    std::shared_ptr<ProjectedSquareCellRaster> dem = std::make_shared<ProjectedSquareCellRaster>( TEST_DATA_DSM_SMALL );
    std::shared_ptr<Point> vp = std::make_shared<Point>( OGRPoint( -336311.5, -1189034.5 ), dem );
    std::shared_ptr<ViewshedAlgorithms> algs = ViewshedUtils::allAlgorithms();
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
    ASSERT_NEAR( c.mElevation[0], 1008.2345, 0.001 );
    ASSERT_NEAR( c.mElevation[1], 1006.3314, 0.001 );
    ASSERT_NEAR( c.mElevation[2], 1004.1940, 0.001 );

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
    ASSERT_NEAR( c.mElevation[0], 1002.4038, 0.001 );
    ASSERT_NEAR( c.mElevation[1], 1000.3604, 0.001 );
    ASSERT_NEAR( c.mElevation[2], 1000.3604, 0.001 );

    c = v.cellEvent( 1 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::EXIT );
    ASSERT_NEAR( c.mDist2point, 1.581138, 0.000001 );
    ASSERT_NEAR( c.mAngle, 4.390638, 0.000001 );

    c = v.cellEvent( 2 );

    ASSERT_TRUE( c.mEventType == CellEventPositionType::CENTER );
    ASSERT_NEAR( c.mDist2point, 1.414213, 0.000001 );
    ASSERT_NEAR( c.mAngle, 3.926990, 0.000001 );
}

CellEvent makeEvent( CellEventPositionType type, int row, int col, double angle )
{
    double elevs[3] = { 1000, 1000, 1000 };
    return CellEvent( type, row, col, 1.0, angle, elevs );
}

// operator< is used by std::sort in AbstractViewshed::sortEventList and must therefore
// be a strict weak ordering, otherwise the sort is undefined behaviour
// (it segfaults with libc++ on macOS)

TEST( CellEventComparison, irreflexivity )
{
    // a < a must always be false
    CellEvent enter = makeEvent( CellEventPositionType::ENTER, 5, 5, 0.785398 );
    CellEvent center = makeEvent( CellEventPositionType::CENTER, 5, 5, 0.785398 );
    CellEvent exit = makeEvent( CellEventPositionType::EXIT, 5, 5, 0.785398 );

    ASSERT_FALSE( enter < enter );
    ASSERT_FALSE( center < center );
    ASSERT_FALSE( exit < exit );
}

TEST( CellEventComparison, asymmetry )
{
    // a < b and b < a must never both be true
    // events of the same type with equal angles exist for cells on the same ray from the viewpoint
    CellEvent exitA = makeEvent( CellEventPositionType::EXIT, 5, 5, 0.785398 );
    CellEvent exitB = makeEvent( CellEventPositionType::EXIT, 10, 10, 0.785398 );

    ASSERT_FALSE( ( exitA < exitB ) && ( exitB < exitA ) );

    CellEvent enterA = makeEvent( CellEventPositionType::ENTER, 5, 5, 0.785398 );
    CellEvent enterB = makeEvent( CellEventPositionType::ENTER, 10, 10, 0.785398 );

    ASSERT_FALSE( ( enterA < enterB ) && ( enterB < enterA ) );

    CellEvent centerA = makeEvent( CellEventPositionType::CENTER, 5, 5, 0.785398 );
    CellEvent centerB = makeEvent( CellEventPositionType::CENTER, 10, 10, 0.785398 );

    ASSERT_FALSE( ( centerA < centerB ) && ( centerB < centerA ) );
}

TEST( CellEventComparison, behindTargetDuplicatesAreEquivalent )
{
    // InverseViewshed creates at most one behind-target event per cell and type,
    // so two events matching on row, col and type always share the same angle
    // and neither may compare less than the other
    CellEvent a = makeEvent( CellEventPositionType::EXIT, 5, 5, 0.785398 );
    a.mBehindTargetForInverseLoS = true;

    CellEvent b = makeEvent( CellEventPositionType::EXIT, 5, 5, 0.785398 );
    b.mBehindTargetForInverseLoS = true;

    ASSERT_FALSE( a < b );
    ASSERT_FALSE( b < a );
}

TEST( CellEventComparison, orderedByAngle )
{
    CellEvent smallerAngle = makeEvent( CellEventPositionType::ENTER, 5, 5, 0.5 );
    CellEvent largerAngle = makeEvent( CellEventPositionType::EXIT, 10, 10, 1.5 );

    ASSERT_TRUE( smallerAngle < largerAngle );
    ASSERT_FALSE( largerAngle < smallerAngle );
}

TEST( CellEventComparison, equalAngleOrderedByEventType )
{
    // at equal angle the order is EXIT, CENTER, ENTER
    CellEvent enter = makeEvent( CellEventPositionType::ENTER, 5, 5, 0.785398 );
    CellEvent center = makeEvent( CellEventPositionType::CENTER, 10, 10, 0.785398 );
    CellEvent exit = makeEvent( CellEventPositionType::EXIT, 15, 15, 0.785398 );

    ASSERT_TRUE( exit < center );
    ASSERT_FALSE( center < exit );

    ASSERT_TRUE( center < enter );
    ASSERT_FALSE( enter < center );

    ASSERT_TRUE( exit < enter );
    ASSERT_FALSE( enter < exit );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
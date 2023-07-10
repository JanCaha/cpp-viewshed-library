#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "cellevent.h"
#include "enums.h"
#include "inverselos.h"
#include "losevaluator.h"
#include "losnode.h"
#include "point.h"
#include "viewshedutils.h"
#include "visibilityalgorithms.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

class InverseLoSAlgorithmTest : public ::testing::Test
{
  protected:
    std::vector<CellEvent> eventList;
    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>();
    std::shared_ptr<Point> tp = std::make_shared<Point>( 0, 0, 0, 0.001, 1 );
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();
    LoSEvaluator losEval = LoSEvaluator( los, algs );

    void resetArray( double *arr, double value )
    {
        arr[0] = value;
        arr[1] = value;
        arr[2] = value;
    }

    void SetUp()
    {
        double elevs[3] = { 1.0, 1.0, 1.0 };

        resetArray( elevs, 1.0 - 0.2 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 1, 1.0, 0.0, elevs ) );
        resetArray( elevs, 2.0 - 0.1 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 2, 2.0, 0.0, elevs ) );
        resetArray( elevs, 3.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 3, 3.0, 0.0, elevs ) );
        resetArray( elevs, 1.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 4, 4.0, 0.0, elevs ) );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 5, 5.0, 0.0, elevs ) );
        resetArray( elevs, 8.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 6, 6.0, 0.0, elevs ) );
        resetArray( elevs, 2.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 7, 7.0, 0.0, elevs ) );
        resetArray( elevs, 4.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 8, 8.0, 0.0, elevs ) );
        resetArray( elevs, 15.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 9, 9.0, 0.0, elevs ) );
        resetArray( elevs, 10.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, 10, 10.0, 0.0, elevs ) );
        resetArray( elevs, 2.0 - 0.2 );
        CellEvent e = CellEvent( CellEventPositionType::CENTER, 0, -1, 1.0, 0.0, elevs );
        e.mBehindTargetForInverseLoS = true;
        eventList.push_back( e );
        resetArray( elevs, 3.0 - 0.1 );
        e = CellEvent( CellEventPositionType::CENTER, 0, -2, 2.0, 0.0, elevs );
        e.mBehindTargetForInverseLoS = true;
        eventList.push_back( e );
        resetArray( elevs, 1.0 );
        e = CellEvent( CellEventPositionType::CENTER, 0, -3, 3.0, 0.0, elevs );
        e.mBehindTargetForInverseLoS = true;
        eventList.push_back( e );

        double cellSize = 1.0;
        for ( int i = 0; i < eventList.size(); i++ )
        {
            CellEvent e = eventList.at( i );
            los->push_back( LoSNode( tp->mRow, tp->mCol, &e, cellSize ) );
        }
    }

    std::shared_ptr<LoSNode> getPointLoS( int i ) { return std::make_shared<LoSNode>( los->at( i ) ); }

    double losEvalForPoint( int position, int result = 0 )
    {
        std::shared_ptr<InverseLoS> tmpLos = std::make_shared<InverseLoS>( *los );
        losEval = LoSEvaluator( tmpLos, algs );
        tmpLos->setTargetPoint( tp, tp->mOffset );
        tmpLos->setViewPoint( getPointLoS( position ), 0.001 );

        losEval.calculate();
        return losEval.resultAt( result );
    }
};

TEST_F( InverseLoSAlgorithmTest, testIsVisibile )
{
    algs->clear();

    algs->push_back( std::make_shared<Boolean>() );

    ASSERT_EQ( losEvalForPoint( 0 ), 1.0 );
    ASSERT_EQ( losEvalForPoint( 1 ), 1.0 );
    ASSERT_EQ( losEvalForPoint( 2 ), 1.0 );
    ASSERT_EQ( losEvalForPoint( 3 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 4 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 5 ), 1.0 );
    ASSERT_EQ( losEvalForPoint( 6 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 8 ), 1.0 );
    ASSERT_EQ( losEvalForPoint( 9 ), 0.0 );
}

TEST_F( InverseLoSAlgorithmTest, testViewAngle )
{
    algs->clear();

    algs->push_back( std::make_shared<ViewAngle>() );

    ASSERT_NEAR( losEvalForPoint( 0 ), -38.6, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1 ), -43.5, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2 ), -45.0, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3 ), -14.0, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4 ), -11.3, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5 ), -53.1, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6 ), -15.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8 ), -59.0, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9 ), -45, 0.1 );
}

TEST_F( InverseLoSAlgorithmTest, differenceLocalHorizon )
{
    algs->clear();

    double invisible = -181;
    double noHorizon = -180;

    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false, invisible, noHorizon ) );
    algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true, invisible, noHorizon ) );

    ASSERT_NEAR( losEvalForPoint( 0, 0 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 0, 1 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 0 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 1 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 0 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 1 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 1 ), -77.4, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 1 ), -56.2, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 0 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 1 ), noHorizon, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 1 ), -96.4, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 1 ), -89.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 0 ), 7.7, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 1 ), 7.7, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 1 ), -123.6, 0.1 );
}

TEST_F( InverseLoSAlgorithmTest, isHorizon )
{
    algs->clear();

    algs->push_back( std::make_shared<Horizons>() );

    losEval = LoSEvaluator( los, algs );

    ASSERT_EQ( losEvalForPoint( 0 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 1 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 2 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 3 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 4 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 5 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 6 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 7 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 8 ), 0.0 );
    ASSERT_EQ( losEvalForPoint( 9 ), 0.0 );
}

TEST_F( InverseLoSAlgorithmTest, differenceGlobalHorizon )
{
    algs->clear();

    double invisible = -91;

    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false, invisible ) );
    algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true, invisible ) );

    ASSERT_NEAR( losEvalForPoint( 0, 0 ), -73.6, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 0 ), -57.5, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 0 ), -43.8, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 0 ), -20.6, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 0 ), -11.3, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 0 ), invisible, 0.1 );

    ASSERT_NEAR( losEvalForPoint( 0, 1 ), -73.6, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 1 ), -57.5, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 1 ), -43.8, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 1 ), -77.4, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 1 ), -56.2, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 1 ), -20.6, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 1 ), -96.4, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 1 ), -89.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 1 ), -11.3, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 1 ), -123.6, 0.1 );
}

TEST_F( InverseLoSAlgorithmTest, differenceElevationLocalHorizon )
{
    algs->clear();

    double noHorizonBefore = -99;
    double invisible = -9999;

    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false, invisible, noHorizonBefore ) );
    algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true, invisible, noHorizonBefore ) );

    LoSEvaluator losEval = LoSEvaluator( los, algs );

    ASSERT_NEAR( losEvalForPoint( 0, 0 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 0 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 0 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 0 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 0 ), 6.0, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 0 ), invisible, 0.1 );

    ASSERT_NEAR( losEvalForPoint( 0, 1 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 1 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 1 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 1 ), -8.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 1 ), -5.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 1 ), noHorizonBefore, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 1 ), -43.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 1 ), -19.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 1 ), 6.0, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 1 ), -59.9, 0.1 );
}

TEST_F( InverseLoSAlgorithmTest, differenceElevationGlobalHorizon )
{
    algs->clear();

    double noHorizonBefore = -99;
    double invisible = -9999;

    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false, invisible, noHorizonBefore ) );
    algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true, invisible, noHorizonBefore ) );

    ASSERT_NEAR( losEvalForPoint( 0, 0 ), -1.5, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 0 ), -2.4, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 0 ), -2.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 0 ), -4.1, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 0 ), invisible, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 0 ), -5.0, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 0 ), invisible, 0.1 );

    ASSERT_NEAR( losEvalForPoint( 0, 1 ), -1.5, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 1, 1 ), -2.3, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 2, 1 ), -2.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 3, 1 ), -8.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 4, 1 ), -5.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 5, 1 ), -4.1, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 6, 1 ), -43.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 7, 1 ), -19.9, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 8, 1 ), -5.0, 0.1 );
    ASSERT_NEAR( losEvalForPoint( 9, 1 ), -59.9, 0.1 );
}

int main( int argc, char **argv )
{
    testing::InitGoogleTest( &argc, argv );
    return RUN_ALL_TESTS();
}
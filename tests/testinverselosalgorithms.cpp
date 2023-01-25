#include "QObject"
#include "QTest"

#include "qgis.h"

#include "testsettings.h"

#include "cellevent.h"
#include "enums.h"
#include "iviewshedalgorithm.h"
#include "losevaluator.h"
#include "losnode.h"
#include "memoryraster.h"
#include "points.h"
#include "viewshedangledifferencetoglobalhorizon.h"
#include "viewshedangledifferencetolocalhorizon.h"
#include "viewshedelevationdifferencetoglobalhorizon.h"
#include "viewshedelevationdifferencetolocalhorizon.h"

#include "viewshedhorizons.h"
#include "viewshedvisibility.h"

using namespace viewshed;

class TestInverseLosAlgorithms : public QObject
{
    Q_OBJECT

  private:
    std::vector<CellEvent> eventList;
    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>();
    std::shared_ptr<Point> tp = std::make_shared<Point>( 0, 0, 0, 0.001, 1 );
    std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<IViewshedAlgorithm>>>();
    LoSEvaluator losEval = LoSEvaluator( los, algs );

  private slots:

    void resetArray( double *arr, double value )
    {
        arr[0] = value;
        arr[1] = value;
        arr[2] = value;
    }

    void initTestCase()
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
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, -1, -1.0, 0.0, elevs ) );
        resetArray( elevs, 3.0 - 0.1 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, -2, -2.0, 0.0, elevs ) );
        resetArray( elevs, 1.0 );
        eventList.push_back( CellEvent( CellEventPositionType::CENTER, 0, -3, -3.0, 0.0, elevs ) );

        double cellSize = 1.0;
        for ( int i = 0; i < eventList.size(); i++ )
        {
            CellEvent e = eventList.at( i );
            los->push_back( LoSNode( tp, &e, cellSize ) );
        }
    }

    std::shared_ptr<LoSNode> getPointLoS( int i ) { return std::make_shared<LoSNode>( los->at( i ) ); }

    double losEvalForPoint( int position, int result = 0 )
    {
        std::shared_ptr<InverseLoS> tmpLos = std::make_shared<InverseLoS>( *los );
        losEval = LoSEvaluator( tmpLos, algs );
        tmpLos->setTargetPoint( tp, tp->offset );
        tmpLos->setViewPoint( getPointLoS( position ), 0.001 );
        losEval.calculate();
        return losEval.resultAt( result );
    }

    void isVisibile()
    {
        algs->clear();

        algs->push_back( std::make_shared<ViewshedVisibility>() );

        QVERIFY( losEvalForPoint( 0 ) == 1.0 );

        QVERIFY( losEvalForPoint( 1 ) == 1.0 );

        QVERIFY( losEvalForPoint( 2 ) == 1.0 );

        QVERIFY( losEvalForPoint( 3 ) == 0.0 );

        QVERIFY( losEvalForPoint( 4 ) == 0.0 );

        QVERIFY( losEvalForPoint( 5 ) == 1.0 );

        QVERIFY( losEvalForPoint( 6 ) == 0.0 );

        QVERIFY( losEvalForPoint( 8 ) == 1.0 );

        QVERIFY( losEvalForPoint( 9 ) == 0.0 );
    }

    // void isHorizon()
    // {
    //     algs->clear();

    //     algs->push_back( std::make_shared<ViewshedHorizons>() );

    //     losEval = LoSEvaluator( los, algs );

    //     QVERIFY( losEvalForPoint( 0 ) == 0.0 );

    //     QVERIFY( losEvalForPoint( 1 ) == 0.0 );

    //     QVERIFY( losEvalForPoint( 2 ) == 1.0 );

    //     QVERIFY( losEvalForPoint( 3 ) == 0.0 );

    //     QVERIFY( losEvalForPoint( 4 ) == 0.0 );

    //     QVERIFY( losEvalForPoint( 5 ) == 1.0 );

    //     QVERIFY( losEvalForPoint( 6 ) == 0.0 );

    //     QVERIFY( losEvalForPoint( 7 ) == 0.0 );

    //     QVERIFY( losEvalForPoint( 8 ) == 1.0 );

    //     QVERIFY( losEvalForPoint( 9 ) == 0.0 );
    // }

    // void differenceLocalHorizon()
    // {
    //     algs->clear();

    //     double invisible = -99;

    //     algs->push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( false, invisible ) );
    //     algs->push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( true, invisible ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), 128.6, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), 128.6, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), 133.5, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 1 ), 133.5, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), 135, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 1 ), 135, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -30.9, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 1 ), -33.6, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), 8.1, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 1 ), 8.1, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 1 ), -37.1, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -26.56, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 5.9, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 5.9, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -14.0, 0.1 ) );
    // }

    // void differenceGlobalHorizon()
    // {
    //     algs->clear();

    //     double invisible = -91;

    //     algs->push_back( std::make_shared<ViewshedAngleDifferenceToGlobalHorizon>( false, invisible ) );
    //     algs->push_back( std::make_shared<ViewshedAngleDifferenceToGlobalHorizon>( true, invisible ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), -20.4, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), -20.4, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -45.0, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -32.4, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 0, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 0.0, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -14.0, 0.1 ) );
    // }

    // void differenceElevationLocalHorizon()
    // {
    //     algs->clear();

    //     double noHorizonBefore = -99;
    //     double invisible = -9999;

    //     algs->push_back(
    //         std::make_shared<ViewshedElevationDifferenceToLocalHorizon>( false, invisible, noHorizonBefore ) );
    //     algs->push_back(
    //         std::make_shared<ViewshedElevationDifferenceToLocalHorizon>( true, invisible, noHorizonBefore ) );

    //     LoSEvaluator losEval = LoSEvaluator( los, algs );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), noHorizonBefore, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), noHorizonBefore, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), noHorizonBefore, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 1 ), noHorizonBefore, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), noHorizonBefore, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 1 ), noHorizonBefore, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -2.9, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 1 ), -3.9, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), 2, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 1 ), 2, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 1 ), -7.3, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -6.6, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 3, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 3, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -6.6, 0.1 ) );
    // }

    // void differenceElevationGlobalHorizon()
    // {
    //     algs->clear();

    //     double noHorizonBefore = -99;
    //     double invisible = -9999;

    //     algs->push_back(
    //         std::make_shared<ViewshedElevationDifferenceToGlobalHorizon>( false, invisible, noHorizonBefore ) );
    //     algs->push_back(
    //         std::make_shared<ViewshedElevationDifferenceToGlobalHorizon>( true, invisible, noHorizonBefore ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), noHorizonBefore, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), noHorizonBefore, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), noHorizonBefore, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 1 ), noHorizonBefore, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), noHorizonBefore, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 1 ), noHorizonBefore, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -5.6, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 1 ), -7.3, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), -2.0, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 1 ), -2.0, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 1 ), -9.6, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -9.3, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 0, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 0, 0.1 ) );

    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
    //     QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -6.6, 0.1 ) );
    // }
};

QTEST_MAIN( TestInverseLosAlgorithms )
#include "testinverselosalgorithms.moc"

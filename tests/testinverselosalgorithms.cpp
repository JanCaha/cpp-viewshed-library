#include "QObject"
#include "QTest"

#include "qgis.h"

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "cellevent.h"
#include "enums.h"
#include "inverselos.h"
#include "losevaluator.h"
#include "losnode.h"
#include "memoryraster.h"
#include "point.h"
#include "utils.h"
#include "visibilityangledifferencetoglobalhorizon.h"
#include "visibilityangledifferencetolocalhorizon.h"
#include "visibilityboolean.h"
#include "visibilityelevationdifferencetoglobalhorizon.h"
#include "visibilityelevationdifferencetolocalhorizon.h"
#include "visibilityhorizons.h"
#include "visibilityviewangle.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

class TestInverseLosAlgorithms : public QObject
{
    Q_OBJECT

  private:
    std::vector<CellEvent> eventList;
    std::shared_ptr<InverseLoS> los = std::make_shared<InverseLoS>();
    std::shared_ptr<Point> tp = std::make_shared<Point>( 0, 0, 0, 0.001, 1 );
    std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs =
        std::make_shared<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>>();
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
        CellEvent e = CellEvent( CellEventPositionType::CENTER, 0, -1, 1.0, 0.0, elevs );
        e.behindTargetForInverseLoS = true;
        eventList.push_back( e );
        resetArray( elevs, 3.0 - 0.1 );
        e = CellEvent( CellEventPositionType::CENTER, 0, -2, 2.0, 0.0, elevs );
        e.behindTargetForInverseLoS = true;
        eventList.push_back( e );
        resetArray( elevs, 1.0 );
        e = CellEvent( CellEventPositionType::CENTER, 0, -3, 3.0, 0.0, elevs );
        e.behindTargetForInverseLoS = true;
        eventList.push_back( e );

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

        // tmpLos->prepareForCalculation();
        // std::vector<std::pair<double, double>> data = Utils::distanceElevation( tmpLos );
        // Utils::saveToCsv( data, "distance,elevation\n", TEST_DATA_LOS );

        losEval.calculate();
        return losEval.resultAt( result );
    }

    void testIsVisibile()
    {
        algs->clear();

        algs->push_back( std::make_shared<Boolean>() );

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

    void testViewAngle()
    {
        algs->clear();

        algs->push_back( std::make_shared<ViewAngle>() );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 0 ), -38.6, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 1 ), -43.5, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 2 ), -45.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 3 ), -14.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 4 ), -11.3, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 5 ), -53.1, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 6 ), -15.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8 ), -59.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9 ), -45, 0.1 ) );
    }

    void differenceLocalHorizon()
    {
        algs->clear();

        double invisible = -181;
        double noHorizon = -180;

        algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( false, invisible, noHorizon ) );
        algs->push_back( std::make_shared<AngleDifferenceToLocalHorizon>( true, invisible, noHorizon ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), noHorizon, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), noHorizon, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), noHorizon, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 1 ), noHorizon, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), noHorizon, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 1 ), noHorizon, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -77.4, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 1 ), -56.2, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), noHorizon, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 1 ), noHorizon, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 1 ), -96.4, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -89.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 7.7, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 7.7, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -123.6, 0.1 ) );
    }

    // void isHorizon()
    // {
    //     algs->clear();

    //     algs->push_back( std::make_shared<Horizons>() );

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

    // void differenceGlobalHorizon()
    // {
    //     algs->clear();

    //     double invisible = -91;

    //     algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false, invisible ) );
    //     algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true, invisible ) );

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
    //         std::make_shared<ElevationDifferenceToLocalHorizon>( false, invisible, noHorizonBefore ) );
    //     algs->push_back(
    //         std::make_shared<ElevationDifferenceToLocalHorizon>( true, invisible, noHorizonBefore ) );

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
    //         std::make_shared<VisibilityElevationDifferenceToGlobalHorizon>( false, invisible, noHorizonBefore ) );
    //     algs->push_back(
    //         std::make_shared<VisibilityElevationDifferenceToGlobalHorizon>( true, invisible, noHorizonBefore ) );

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

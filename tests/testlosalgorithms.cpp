#include "QObject"
#include "QTest"

#include "qgis.h"

#include "testsettings.h"

#include "enums.h"
#include "event.h"
#include "iviewshedalgorithm.h"
#include "losevaluator.h"
#include "memoryraster.h"
#include "points.h"
#include "statusnode.h"
#include "viewshedangledifferencetoglobalhorizon.h"
#include "viewshedangledifferencetolocalhorizon.h"
#include "viewshedelevationdifferencetoglobalhorizon.h"
#include "viewshedelevationdifferencetolocalhorizon.h"

#include "viewshedhorizons.h"
#include "viewshedvisibility.h"

class TestLosAlgorithms : public QObject
{
    Q_OBJECT

  private:
    std::vector<Event> eventList;
    std::vector<StatusNode> statusList;
    std::shared_ptr<ViewPoint> vp = std::make_shared<ViewPoint>( 0, 0, 0, 0.001 );
    LoSEvaluator losEval;
    std::vector<std::shared_ptr<IViewshedAlgorithm>> algs;

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
        eventList.push_back( Event( CellPosition::CENTER, 0, 1, 1.0, 0.0, elevs ) );
        resetArray( elevs, 2.0 - 0.1 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 2, 2.0, 0.0, elevs ) );
        resetArray( elevs, 3.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 3, 3.0, 0.0, elevs ) );
        resetArray( elevs, 1.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 4, 4.0, 0.0, elevs ) );
        eventList.push_back( Event( CellPosition::CENTER, 0, 5, 5.0, 0.0, elevs ) );
        resetArray( elevs, 8.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 6, 6.0, 0.0, elevs ) );
        resetArray( elevs, 2.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 7, 7.0, 0.0, elevs ) );
        resetArray( elevs, 4.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 8, 8.0, 0.0, elevs ) );
        resetArray( elevs, 15.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 9, 9.0, 0.0, elevs ) );
        resetArray( elevs, 10.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 10, 10.0, 0.0, elevs ) );

        double cellSize = 1.0;
        for ( int i = 0; i < eventList.size(); i++ )
        {
            Event e = eventList.at( i );
            statusList.push_back( StatusNode( vp, &e, cellSize ) );
        }
    }

    std::shared_ptr<StatusNode> getPointLoS( int i ) { return std::make_shared<StatusNode>( statusList.at( i ) ); }

    void checkValues()
    {
        std::shared_ptr<StatusNode> p = getPointLoS( 0 );
        QVERIFY( p->centreDistance() == 1.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 0.8, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 38.6, 0.1 ) );

        p = getPointLoS( 1 );
        QVERIFY( p->centreDistance() == 2.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 1.9, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 43.5, 0.1 ) );

        p = getPointLoS( 2 );
        QVERIFY( p->centreDistance() == 3.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 3, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 44.9, 0.1 ) );

        p = getPointLoS( 3 );
        QVERIFY( p->centreDistance() == 4.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 1, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 14.0, 0.1 ) );

        p = getPointLoS( 4 );
        QVERIFY( p->centreDistance() == 5.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 1, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 11.2, 0.1 ) );

        p = getPointLoS( 5 );
        QVERIFY( p->centreDistance() == 6.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 8, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 53.1, 0.1 ) );

        p = getPointLoS( 6 );
        QVERIFY( p->centreDistance() == 7.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 2, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 15.9, 0.1 ) );

        p = getPointLoS( 7 );
        QVERIFY( p->centreDistance() == 8.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 4, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 26.5, 0.1 ) );

        p = getPointLoS( 8 );
        QVERIFY( p->centreDistance() == 9.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 15, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 59.0, 0.1 ) );

        p = getPointLoS( 9 );
        QVERIFY( p->centreDistance() == 10.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 10, 0.1 ) );
        QVERIFY( qgsDoubleNear( p->centreGradient(), 44.9, 0.1 ) );
    }

    void isVisibile()
    {
        losEval = LoSEvaluator();

        algs.clear();

        algs.push_back( std::make_shared<ViewshedVisibility>() );

        losEval.calculate( algs, statusList, getPointLoS( 2 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        losEval.calculate( algs, statusList, getPointLoS( 3 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 4 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 5 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        losEval.calculate( algs, statusList, getPointLoS( 6 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 8 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        losEval.calculate( algs, statusList, getPointLoS( 9 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );
    }

    void isHorizon()
    {
        losEval = LoSEvaluator();

        algs.clear();

        algs.push_back( std::make_shared<ViewshedHorizons>() );

        losEval.calculate( algs, statusList, getPointLoS( 0 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 1 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 2 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        losEval.calculate( algs, statusList, getPointLoS( 3 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 4 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 5 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        losEval.calculate( algs, statusList, getPointLoS( 6 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 7 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        losEval.calculate( algs, statusList, getPointLoS( 8 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        losEval.calculate( algs, statusList, getPointLoS( 9 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );
    }

    void differenceLocalHorizon()
    {
        losEval = LoSEvaluator();

        algs.clear();

        algs.push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( false ) );
        algs.push_back( std::make_shared<ViewshedAngleDifferenceToLocalHorizon>( true ) );

        losEval.calculate( algs, statusList, getPointLoS( 0 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 128.6, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 128.6, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 1 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 133.5, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 133.5, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 2 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 135, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 135, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 3 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -30.9, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 4 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -33.6, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 5 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 8.1, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 8.1, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 6 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -37.1, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 7 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -26.56, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 8 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 5.9, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 5.9, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 9 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -14.0, 0.1 ) );
    }

    void differenceGloballHorizon()
    {
        losEval = LoSEvaluator();

        algs.clear();

        algs.push_back( std::make_shared<ViewshedAngleDifferenceToGlobalHorizon>( false ) );
        algs.push_back( std::make_shared<ViewshedAngleDifferenceToGlobalHorizon>( true ) );

        losEval.calculate( algs, statusList, getPointLoS( 0 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -20.4, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -20.4, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 3 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -45.0, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 7 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -32.4, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 8 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 0, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 0.0, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 9 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -91, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -14.0, 0.1 ) );
    }

    void differenceElevationLocalHorizon()
    {
        losEval = LoSEvaluator();

        algs.clear();

        double noHorizonBefore = -99;
        double invisible = -9999;

        algs.push_back(
            std::make_shared<ViewshedElevationDifferenceToLocalHorizon>( false, invisible, noHorizonBefore ) );
        algs.push_back(
            std::make_shared<ViewshedElevationDifferenceToLocalHorizon>( true, invisible, noHorizonBefore ) );

        losEval.calculate( algs, statusList, getPointLoS( 0 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), noHorizonBefore, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 1 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), noHorizonBefore, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 2 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), noHorizonBefore, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 3 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -2.9, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 4 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -3.9, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 5 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 2, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 2, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 6 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -7.3, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 7 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -6.6, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 8 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 3, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 3, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 9 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -6.6, 0.1 ) );
    }

    void differenceElevationGlobalHorizon()
    {
        losEval = LoSEvaluator();

        algs.clear();

        double noHorizonBefore = -99;
        double invisible = -9999;

        algs.push_back(
            std::make_shared<ViewshedElevationDifferenceToGlobalHorizon>( false, invisible, noHorizonBefore ) );
        algs.push_back(
            std::make_shared<ViewshedElevationDifferenceToGlobalHorizon>( true, invisible, noHorizonBefore ) );

        losEval.calculate( algs, statusList, getPointLoS( 0 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), noHorizonBefore, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 1 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), noHorizonBefore, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 2 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), noHorizonBefore, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 3 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -5.6, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 4 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -7.3, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 5 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), -2.0, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -2.0, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 6 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -9.6, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 7 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -9.3, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 8 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), 0, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), 0, 0.1 ) );

        losEval.calculate( algs, statusList, getPointLoS( 9 ), vp );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEval.resultAt( 1 ), -6.6, 0.1 ) );
    }
};

QTEST_MAIN( TestLosAlgorithms )
#include "testlosalgorithms.moc"

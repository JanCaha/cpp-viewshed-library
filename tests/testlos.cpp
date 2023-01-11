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
#include "viewshedangledifferencetolocalhorizon.h"

#include "viewshedhorizons.h"
#include "viewshedvisibility.h"

class TestLos : public QObject
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

    void isVisibile()
    {
        losEval = LoSEvaluator();

        algs.clear();

        algs.push_back( std::make_shared<ViewshedVisibility>() );

        QVERIFY( getPointLoS( 2 )->centreElevation() == 3.0 );
        QVERIFY( getPointLoS( 2 )->centreDistance() == 3.0 );
        losEval.calculate( algs, statusList, getPointLoS( 2 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        QVERIFY( getPointLoS( 3 )->centreElevation() == 1.0 );
        QVERIFY( getPointLoS( 3 )->centreDistance() == 4.0 );
        losEval.calculate( algs, statusList, getPointLoS( 3 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        QVERIFY( getPointLoS( 4 )->centreElevation() == 1.0 );
        QVERIFY( getPointLoS( 4 )->centreDistance() == 5.0 );
        losEval.calculate( algs, statusList, getPointLoS( 4 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        QVERIFY( getPointLoS( 5 )->centreElevation() == 8.0 );
        QVERIFY( getPointLoS( 5 )->centreDistance() == 6.0 );
        losEval.calculate( algs, statusList, getPointLoS( 5 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        QVERIFY( getPointLoS( 6 )->centreElevation() == 2.0 );
        QVERIFY( getPointLoS( 6 )->centreDistance() == 7.0 );
        losEval.calculate( algs, statusList, getPointLoS( 6 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 0.0 );

        QVERIFY( getPointLoS( 8 )->centreElevation() == 15.0 );
        QVERIFY( getPointLoS( 8 )->centreDistance() == 9.0 );
        losEval.calculate( algs, statusList, getPointLoS( 8 ), vp );
        QVERIFY( losEval.resultAt( 0 ) == 1.0 );

        QVERIFY( getPointLoS( 9 )->centreElevation() == 10.0 );
        QVERIFY( getPointLoS( 9 )->centreDistance() == 10.0 );
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
};

QTEST_MAIN( TestLos )
#include "testlos.moc"

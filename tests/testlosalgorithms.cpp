#include "QObject"
#include "QTest"

#include "qgis.h"

#include "testsettings.h"

#include "abstractviewshedalgorithm.h"
#include "cellevent.h"
#include "enums.h"
#include "los.h"
#include "losevaluator.h"
#include "losnode.h"
#include "memoryraster.h"
#include "point.h"
#include "visibilityangledifferencetoglobalhorizon.h"
#include "visibilityangledifferencetolocalhorizon.h"
#include "visibilityboolean.h"
#include "visibilityelevationdifferencetoglobalhorizon.h"
#include "visibilityelevationdifferencetolocalhorizon.h"
#include "visibilityhorizons.h"
#include "visibilityslopetoviewangle.h"
#include "visibilityviewangle.h"

using namespace viewshed;
using namespace viewshed::visibilityalgorithm;

class TestLosAlgorithms : public QObject
{
    Q_OBJECT

  private:
    std::vector<CellEvent> eventList;
    std::shared_ptr<LoS> los = std::make_shared<LoS>();
    std::shared_ptr<Point> vp = std::make_shared<Point>( 0, 0, 0, 0.001, 1 );
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

        double cellSize = 1.0;
        for ( int i = 0; i < eventList.size(); i++ )
        {
            CellEvent e = eventList.at( i );
            los->push_back( LoSNode( vp, &e, cellSize ) );
        }

        los->setViewPoint( vp );
    }

    std::shared_ptr<LoSNode> getPointLoS( int i ) { return std::make_shared<LoSNode>( los->at( i ) ); }

    void setTargetPointLoS( int i ) { los->setTargetPoint( std::make_shared<LoSNode>( los->at( i ) ) ); }

    void checkValues()
    {
        std::shared_ptr<LoSNode> p = getPointLoS( 0 );
        QVERIFY( p->centreDistance() == 1.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 0.8, 0.1 ) );

        p = getPointLoS( 1 );
        QVERIFY( p->centreDistance() == 2.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 1.9, 0.1 ) );

        p = getPointLoS( 2 );
        QVERIFY( p->centreDistance() == 3.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 3, 0.1 ) );

        p = getPointLoS( 3 );
        QVERIFY( p->centreDistance() == 4.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 1, 0.1 ) );

        p = getPointLoS( 4 );
        QVERIFY( p->centreDistance() == 5.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 1, 0.1 ) );

        p = getPointLoS( 5 );
        QVERIFY( p->centreDistance() == 6.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 8, 0.1 ) );

        p = getPointLoS( 6 );
        QVERIFY( p->centreDistance() == 7.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 2, 0.1 ) );

        p = getPointLoS( 7 );
        QVERIFY( p->centreDistance() == 8.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 4, 0.1 ) );

        p = getPointLoS( 8 );
        QVERIFY( p->centreDistance() == 9.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 15, 0.1 ) );

        p = getPointLoS( 9 );
        QVERIFY( p->centreDistance() == 10.0 );
        QVERIFY( qgsDoubleNear( p->centreElevation(), 10, 0.1 ) );
    }

    double losEvalForPoint( int position, int result = 0 )
    {
        setTargetPointLoS( position );
        losEval.calculate();
        return losEval.resultAt( result );
    }

    void isVisibile()
    {
        algs->clear();

        algs->push_back( std::make_shared<Boolean>() );

        losEval = LoSEvaluator( los, algs );

        los->setViewPoint( vp );

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

    void isHorizon()
    {
        algs->clear();

        double noHorizon = 0;
        double horizon = 1;
        double globalHorizon = 2;

        algs->push_back( std::make_shared<Horizons>( horizon, noHorizon, globalHorizon ) );

        losEval = LoSEvaluator( los, algs );

        QVERIFY( losEvalForPoint( 0 ) == noHorizon );

        QVERIFY( losEvalForPoint( 1 ) == noHorizon );

        QVERIFY( losEvalForPoint( 2 ) == horizon );

        QVERIFY( losEvalForPoint( 3 ) == noHorizon );

        QVERIFY( losEvalForPoint( 4 ) == noHorizon );

        QVERIFY( losEvalForPoint( 5 ) == horizon );

        QVERIFY( losEvalForPoint( 6 ) == noHorizon );

        QVERIFY( losEvalForPoint( 7 ) == noHorizon );

        QVERIFY( losEvalForPoint( 8 ) == globalHorizon );

        QVERIFY( losEvalForPoint( 9 ) == noHorizon );
    }

    void viewAngle()
    {
        algs->clear();

        algs->push_back( std::make_shared<ViewAngle>() );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), 38.6, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), 43.5, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), 44.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), 14, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), 11.2, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), 53.1, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), 15.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), 26.5, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 59.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), 44.9, 0.1 ) );
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
        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -30.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 1 ), -33.6, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), 8.1, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 1 ), 8.1, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 1 ), -37.1, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -26.56, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 5.9, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 5.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -14.0, 0.1 ) );
    }

    void differenceGlobalHorizon()
    {
        algs->clear();

        double invisible = -91;

        algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( false, invisible ) );
        algs->push_back( std::make_shared<AngleDifferenceToGlobalHorizon>( true, invisible ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), -20.4, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), -20.4, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -45.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -32.4, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 0, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 0.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -14.0, 0.1 ) );
    }

    void differenceElevationLocalHorizon()
    {
        algs->clear();

        double noHorizonBefore = -99;
        double invisible = -9999;

        algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( false, invisible, noHorizonBefore ) );
        algs->push_back( std::make_shared<ElevationDifferenceToLocalHorizon>( true, invisible, noHorizonBefore ) );

        LoSEvaluator losEval = LoSEvaluator( los, algs );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), noHorizonBefore, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 1 ), noHorizonBefore, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), noHorizonBefore, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 1 ), noHorizonBefore, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -2.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 1 ), -3.9, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), 2, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 1 ), 2, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 1 ), -7.3, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -6.6, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 3, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 3, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -6.6, 0.1 ) );
    }

    void differenceElevationGlobalHorizon()
    {
        algs->clear();

        double noHorizonBefore = -99;
        double invisible = -9999;

        algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( false, invisible, noHorizonBefore ) );
        algs->push_back( std::make_shared<ElevationDifferenceToGlobalHorizon>( true, invisible, noHorizonBefore ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), -0.8, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 1 ), -0.8, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), -1.4, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 1 ), -1.4, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), -2.0, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 1 ), -2.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 1 ), -5.6, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 4, 1 ), -7.3, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), -2.0, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 1 ), -2.0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 6, 1 ), -9.6, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 1 ), -9.3, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 0, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 1 ), 0, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 1 ), -6.6, 0.1 ) );
    }

    void losSlopeToViewAngle()
    {
        algs->clear();

        double invisible = -1;
        LoSSlopeToViewAngle a = LoSSlopeToViewAngle();

        algs->push_back( std::make_shared<LoSSlopeToViewAngle>( invisible ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 0, 0 ), 0.0349, 0.0001 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 1, 0 ), 4.2, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 2, 0 ), 2.7, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 3, 0 ), invisible, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 5, 0 ), 28.7, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 7, 0 ), invisible, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 8, 0 ), 25.7, 0.1 ) );

        QVERIFY( qgsDoubleNear( losEvalForPoint( 9, 0 ), invisible, 0.1 ) );
    }
};

QTEST_MAIN( TestLosAlgorithms )
#include "testlosalgorithms.moc"

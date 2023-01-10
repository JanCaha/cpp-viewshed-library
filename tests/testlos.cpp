#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "enums.h"
#include "event.h"
#include "memoryraster.h"
#include "points.h"
#include "statusnode.h"

class TestLos : public QObject
{
    Q_OBJECT

  private:
    std::vector<Event> eventList;
    std::vector<StatusNode> statusList;
    std::shared_ptr<ViewPoint> vp = std::make_shared<ViewPoint>( 0, 0, 0, 0.001 );

  private slots:

    void arrayValue( double[] * arr, double value )
    {
        arr[0] = value;
        arr[1] = value;
        arr[2] = value;
        return arr;
    }

    void initTestCase()
    {
        double elevs[3] = { 1.0, 1.0, 1.0 };

        resetArray( elevs, 1.0 );
        eventList.push_back( Event( CellPosition::CENTER, 0, 1, 1.0, 0.0, elevs ) );
        resetArray( elevs, 2.0 );
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
            Event *eptr = &e;
            statusList.push_back( StatusNode( vp, &e, cellSize ) );
        }
    }

    void constructWithDefaultSettings() {}
};

QTEST_MAIN( TestLos )
#include "testlos.moc"

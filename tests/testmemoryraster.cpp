#include "QObject"
#include "QTest"

#include "qgsrasterlayer.h"

#include "testsettings.h"

#include "memoryraster.h"

class TestMemoryRaster : public QObject
{
    Q_OBJECT

  private:
    std::shared_ptr<QgsRasterLayer> dem;

  private slots:

    void initTestCase() { dem = std::make_shared<QgsRasterLayer>( TEST_DATA_DSM, "dsm", "gdal" ); }

    void constructWithDefaultSettings()
    {
        MemoryRaster mr( dem );
        QVERIFY( mr.isValid() );
        QVERIFY( mr.crs() == dem->crs() );
        QVERIFY( mr.extent() == dem->extent() );
        QVERIFY( mr.height() == dem->height() );
        QVERIFY( mr.width() == dem->width() );
        QVERIFY( mr.dataType() == Qgis::DataType::Float32 );
        QVERIFY( mr.defaultBand() == 1 );
        QVERIFY( mr.dataProvider()->isValid() );
        QVERIFY( mr.dataProvider()->dataSourceUri().contains( QString( "/vsimem/" ) ) );
        QVERIFY( mr.dataProvider()->block( 1, mr.extent(), mr.width(), mr.height() )->isValid() );
        QVERIFY( mr.dataProvider()
                     ->block( 1, mr.extent(), mr.width(), mr.height() )
                     ->value( (int)mr.height() / 2, (int)mr.width() / 2 ) == 0.0 );
    }

    void constructWithSettings()
    {
        MemoryRaster mr( dem, Qgis::DataType::Int32, -1 );
        QVERIFY( mr.isValid() );
        QVERIFY( mr.dataType() == Qgis::DataType::Int32 );
        QVERIFY( mr.dataProvider()->isValid() );
        QVERIFY( mr.dataProvider()->dataSourceUri().contains( QString( "/vsimem/" ) ) );
        QVERIFY( mr.dataProvider()
                     ->block( 1, mr.extent(), mr.width(), mr.height() )
                     ->value( (int)mr.height() / 2, (int)mr.width() / 2 ) == -1 );
    }

    void setValue()
    {
        MemoryRaster mr( dem );
        mr.setValue( -100, mr.width() / 2, mr.height() / 2 );
        QVERIFY( mr.dataProvider()
                     ->block( 1, mr.extent(), mr.width(), mr.height() )
                     ->value( (int)mr.height() / 2, (int)mr.width() / 2 ) == -100 );
    }

    void setValues()
    {
        MemoryRaster mr( dem );

        std::shared_ptr<QgsRasterBlock> values =
            std::make_shared<QgsRasterBlock>( mr.dataType(), mr.width(), mr.height() );

        for ( int i = 0; i < values->height(); i++ )
        {
            for ( int j = 0; j < values->width(); j++ )
            {
                values->setValue( i, j, -1000 );
            }
        }

        bool result = mr.setValues( values );

        QVERIFY( result );

        QVERIFY( mr.dataProvider()
                     ->block( 1, mr.extent(), mr.width(), mr.height() )
                     ->value( (int)mr.height() / 2, (int)mr.width() / 2 ) == -1000 );

        QVERIFY( mr.dataProvider()
                     ->block( 1, mr.extent(), mr.width(), mr.height() )
                     ->value( (int)mr.height() / 4, (int)mr.width() / 4 ) == -1000 );

        QVERIFY( mr.dataProvider()
                     ->block( 1, mr.extent(), mr.width(), mr.height() )
                     ->value( (int)mr.height() / 10, (int)mr.width() / 10 ) == -1000 );
    }
};

QTEST_MAIN( TestMemoryRaster )
#include "testmemoryraster.moc"

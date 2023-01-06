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
        QVERIFY( mr.error().isEmpty() );
        QVERIFY( mr.crs() == dem->crs() );
        QVERIFY( mr.extent() == dem->extent() );
        QVERIFY( mr.height() == dem->height() );
        QVERIFY( mr.width() == dem->width() );
        QVERIFY( mr.dataType() == Qgis::DataType::Float32 );
        QVERIFY( mr.defaultBand() == 1 );

        std::unique_ptr<QgsRasterDataProvider> dp = mr.dataProvider();
        QVERIFY( dp->isValid() );
        QVERIFY( dp->dataSourceUri().contains( QString( "/vsimem/" ) ) );
        QVERIFY( dp->block( 1, mr.extent(), mr.width(), mr.height() )->isValid() );

        std::shared_ptr<QgsRasterBlock> rb( dp->block( 1, mr.extent(), mr.width(), mr.height() ) );
        QVERIFY( rb->value( (int)mr.height() / 2, (int)mr.width() / 2 ) ==
                 dem->dataProvider()->sourceNoDataValue( 1 ) );
    }

    void constructWithSettings()
    {
        double defaultValue = -1;
        MemoryRaster mr( dem, Qgis::DataType::Int32, defaultValue );
        QVERIFY( mr.isValid() );
        QVERIFY( mr.dataType() == Qgis::DataType::Int32 );

        std::unique_ptr<QgsRasterDataProvider> dp = mr.dataProvider();
        QVERIFY( dp->isValid() );
        QVERIFY( dp->dataSourceUri().contains( QString( "/vsimem/" ) ) );

        std::shared_ptr<QgsRasterBlock> rb( dp->block( 1, mr.extent(), mr.width(), mr.height() ) );
        double value = rb->value( (int)mr.height() / 2, (int)mr.width() / 2 );
        QVERIFY( rb->value( (int)mr.height() / 2, (int)mr.width() / 2 ) == defaultValue );
    }

    void setValue()
    {
        MemoryRaster mr( dem );
        mr.setValue( -100, mr.width() / 2, mr.height() / 2 );
        QVERIFY( mr.dataProvider()
                     ->block( 1, mr.extent(), mr.width(), mr.height() )
                     ->value( (int)mr.height() / 2, (int)mr.width() / 2 ) == -100 );
    }
};

QTEST_MAIN( TestMemoryRaster )
#include "testmemoryraster.moc"

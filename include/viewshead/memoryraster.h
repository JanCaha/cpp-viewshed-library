#ifndef MEMORYRASTER_H
#define MEMORYRASTER_H

#include "qgis.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsrasterdataprovider.h"
#include "qgsrasterfilewriter.h"
#include "qgsrasterlayer.h"
#include "qgsrectangle.h"

class MemoryRaster
{

  public:
    MemoryRaster( std::shared_ptr<QgsRasterLayer> rasterTemplate, Qgis::DataType dataType = Qgis::DataType::Float32,
                  double defaultValue = 0 );

    std::shared_ptr<QgsRasterDataProvider> dataProvider();

    bool save( QString fileName );
    const int height();
    const int width();
    const QgsCoordinateReferenceSystem crs();
    const QgsRectangle extent();
    const Qgis::DataType dataType();
    const int defaultBand();

    bool isValid();
    bool setValue( const double &value, const int &col, const int &row );
    bool setValues( std::shared_ptr<QgsRasterBlock> values, const int rowOffset = 0, const int colOffset = 0 );

  private:
    std::shared_ptr<QgsRasterDataProvider> mDataProvider;
    std::unique_ptr<QgsRasterBlock> mRasterBlock;
    QgsCoordinateReferenceSystem mCrs;
    QgsRectangle mExtent;
    Qgis::DataType mDataType;
    QString mRasterFilename;
    int mHeight;
    int mWidth;
    double mNoDataValue;
    int mDefaultBand = 1;
    bool mValid;

    QString randomName( int length = 16 );
};

#endif
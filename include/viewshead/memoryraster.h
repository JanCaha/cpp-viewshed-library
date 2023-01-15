#ifndef VIEWSHEDLIB_MEMORYRASTER_H
#define VIEWSHEDLIB_MEMORYRASTER_H

#include <cmath>
#include <limits>

#include "qgis.h"
#include "qgscoordinatereferencesystem.h"
#include "qgsrasterdataprovider.h"
#include "qgsrasterfilewriter.h"
#include "qgsrasterlayer.h"
#include "qgsrectangle.h"

namespace viewshed
{
    class MemoryRaster
    {

      public:
        MemoryRaster( std::shared_ptr<QgsRasterLayer> rasterTemplate, Qgis::DataType dataType = Qgis::DataType::Float32,
                      double defaultValue = std::numeric_limits<double>::quiet_NaN() );

        std::unique_ptr<QgsRasterDataProvider> dataProvider();

        bool save( QString fileName );
        const int height();
        const int width();
        const QgsCoordinateReferenceSystem crs();
        const QgsRectangle extent();
        const Qgis::DataType dataType();
        const int defaultBand();

        bool isValid();
        QString error();
        bool setValue( double value, int col, int row );

      private:
        std::unique_ptr<QgsRasterBlock> mRasterData;
        QgsCoordinateReferenceSystem mCrs;
        QgsRectangle mExtent;
        Qgis::DataType mDataType;
        int mHeight;
        int mWidth;
        double mNoDataValue;
        int mDefaultBand = 1;
        bool mValid;
        QString mError;

        QString randomName( int length = 16 );
        void prefillValues( double value );
    };
} // namespace viewshed

#endif
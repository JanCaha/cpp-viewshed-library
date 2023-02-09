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
    /**
     * @brief Class that creates raster with single band purely in memory. The overall representation is very
     * simplistic, basically a two dimensional array with some properties, that allow creating real QgsRasterLayer
     * representation from this object.
     *
     */
    class MemoryRaster
    {

      public:
        /**
         * @brief Construct a new Memory Raster object based on template raster, provided data type and default value.
         * The raster will be prefilled with the default value.
         *
         * @param rasterTemplate The memory representation gets number of columns and rows, coordinate system, extent
         * and no data value based on this QgsRasterLayer.
         * @param dataType type of data to be stored. Default Qgis::DataType::Float32.
         * @param defaultValue default value to prefill the object with. Default is
         * std::numeric_limits<double>::quiet_NaN().
         */
        MemoryRaster( std::shared_ptr<QgsRasterLayer> rasterTemplate, Qgis::DataType dataType = Qgis::DataType::Float32,
                      double defaultValue = std::numeric_limits<double>::quiet_NaN() );

        /**
         * @brief Create QgsRasterDataProvider from the class.
         *
         * @return std::unique_ptr<QgsRasterDataProvider>
         */
        std::unique_ptr<QgsRasterDataProvider> dataProvider();

        /**
         * @brief Save this object as Raster in given file name.
         *
         * @param fileName
         * @return true
         * @return false
         */
        bool save( QString fileName );

        /**
         * @brief Height of the raster. Number of rows.
         *
         * @return const int
         */
        const int height();

        /**
         * @brief Width of the raster. Number of columns.
         *
         * @return const int
         */
        const int width();

        /**
         * @brief Coordinate reference system of the data.
         *
         * @return const QgsCoordinateReferenceSystem
         */
        const QgsCoordinateReferenceSystem crs();

        /**
         * @brief Extent of the raster.
         *
         * @return const QgsRectangle
         */
        const QgsRectangle extent();

        /**
         * @brief Data type of the data.
         *
         * @return const Qgis::DataType
         */
        const Qgis::DataType dataType();

        const int defaultBand();

        /**
         * @brief Is the data set valid?
         *
         * @return true
         * @return false
         */
        bool isValid();

        /**
         * @brief Last error message.
         *
         * @return QString
         */
        QString error();

        /**
         * @brief Set the value at given row and column.
         *
         * @param value
         * @param col
         * @param row
         * @return true
         * @return false
         */
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
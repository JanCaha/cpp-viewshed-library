#include <QDebug>

#include "qgsraster.h"

#include "memoryraster.h"

MemoryRaster::MemoryRaster( std::shared_ptr<QgsRasterLayer> rasterTemplate, Qgis::DataType dataType,
                            double defaultValue )
{
    mDataType = dataType;

    mWidth = rasterTemplate->width();
    mHeight = rasterTemplate->height();
    mExtent = rasterTemplate->extent();
    mCrs = rasterTemplate->crs();

    mValid = false;

    mNoDataValue = rasterTemplate->dataProvider()->sourceNoDataValue( mDefaultBand );
    mNoDataValue = QgsRaster::representableValue( mNoDataValue, mDataType );

    defaultValue = QgsRaster::representableValue( defaultValue, mDataType );

    mRasterFilename = QString( "/vsimem/%1" ).arg( randomName() );

    QgsRasterFileWriter rw = QgsRasterFileWriter( mRasterFilename );
    rw.setOutputProviderKey( "gdal" );

    std::unique_ptr<QgsRasterInterface> rInterface;
    rInterface.reset( rasterTemplate->dataProvider()->clone() );

    mRasterBlock = std::make_unique<QgsRasterBlock>( mDataType, 1, 1 );

    mDataProvider.reset(
        rw.createOneBandRaster( dataType, rInterface.get()->xSize(), rInterface.get()->ySize(), mExtent, mCrs ) );

    if ( !mDataProvider )
    {
        return;
    }

    if ( !mDataProvider->isValid() )
    {
        return;
    }

    mDataProvider->setNoDataValue( mDefaultBand, mNoDataValue );

    QgsRasterIterator iter( rInterface.get() );
    iter.startRasterRead( mDefaultBand, rasterTemplate->width(), rasterTemplate->height(), rasterTemplate->extent() );
    int iterLeft = 0;
    int iterTop = 0;
    int iterCols = 0;
    int iterRows = 0;
    bool isNoData = false;

    std::unique_ptr<QgsRasterBlock> rasterBlock;

    mDataProvider->setEditable( true );

    while ( iter.readNextRasterPart( mDefaultBand, iterCols, iterRows, rasterBlock, iterLeft, iterTop ) )
    {
        std::unique_ptr<QgsRasterBlock> editedBlock = std::make_unique<QgsRasterBlock>( mDataType, iterCols, iterRows );

        for ( int row = 0; row < iterRows; row++ )
        {
            for ( int column = 0; column < iterCols; column++ )
            {
                const double value = rasterBlock->valueAndNoData( row, column, isNoData );
                double resultValue;

                if ( isNoData )
                {
                    resultValue = mNoDataValue;
                }
                else
                {
                    resultValue = defaultValue;
                }

                editedBlock->setValue( row, column, resultValue );
            }
        }

        mDataProvider->writeBlock( editedBlock.get(), mDefaultBand, iterLeft, iterTop );
    }

    mDataProvider->setEditable( false );

    mValid = true;
}

bool MemoryRaster::isValid() { return mValid; }

bool MemoryRaster::save( QString fileName )
{
    QgsRasterFileWriter rw = QgsRasterFileWriter( fileName );
    std::unique_ptr<QgsRasterDataProvider> saveRasterDp(
        rw.createOneBandRaster( mDataType, mWidth, mHeight, mExtent, mCrs ) );

    saveRasterDp->setNoDataValue( mDefaultBand, mNoDataValue );

    if ( !saveRasterDp )
    {
        return false;
    }

    if ( !saveRasterDp->isValid() )
    {
        return false;
    }

    saveRasterDp->setEditable( true );

    if ( !saveRasterDp->isEditable() )
    {
        return false;
    }

    std::unique_ptr<QgsRasterInterface> rInterface;
    rInterface.reset( mDataProvider->clone() );

    QgsRasterIterator iter( rInterface.get() );
    iter.startRasterRead( mDefaultBand, mWidth, mHeight, mExtent );
    int iterLeft = 0;
    int iterTop = 0;
    int iterCols = 0;
    int iterRows = 0;
    bool isNoData = false;
    std::unique_ptr<QgsRasterBlock> rasterBlock;
    while ( iter.readNextRasterPart( mDefaultBand, iterCols, iterRows, rasterBlock, iterLeft, iterTop ) )
    {
        bool writeBlockResult = saveRasterDp->writeBlock( rasterBlock.get(), mDefaultBand, iterLeft, iterTop );

        if ( !writeBlockResult )
        {
            return false;
        }
    }

    return true;
}

std::shared_ptr<QgsRasterDataProvider> MemoryRaster::dataProvider() { return mDataProvider; }

const int MemoryRaster::height() { return mHeight; }

const int MemoryRaster::width() { return mWidth; }

const QgsCoordinateReferenceSystem MemoryRaster::crs() { return mCrs; }

const QgsRectangle MemoryRaster::extent() { return mExtent; }

const Qgis::DataType MemoryRaster::dataType() { return mDataType; }

const int MemoryRaster::defaultBand() { return 1; }

bool MemoryRaster::setValue( const double &value, const int &col, const int &row )
{
    double preparedValue = QgsRaster::representableValue( value, mDataType );

    mDataProvider->setEditable( true );

    mRasterBlock->setValue( 0, 0, preparedValue );
    bool success = mDataProvider->writeBlock( mRasterBlock.get(), defaultBand(), col, row );

    mDataProvider->setEditable( false );

    return success;
}

bool MemoryRaster::setValues( std::shared_ptr<QgsRasterBlock> values, const int rowOffset, const int colOffset )
{

    for ( int i = 0; i < values->height(); i++ )
    {
        for ( int j = 0; j < values->width(); j++ )
        {
            double blockValue = values->value( i, j );

            if ( !QgsRaster::isRepresentableValue( blockValue, mDataType ) )
            {
                values->setValue( i, j, QgsRaster::representableValue( blockValue, mDataType ) );
            }
        }
    }

    mDataProvider->setEditable( true );

    bool success = mDataProvider->writeBlock( values.get(), defaultBand(), colOffset, rowOffset );

    mDataProvider->setEditable( false );

    return success;
}

QString MemoryRaster::randomName( int length )
{
    static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                   "abcdefghijklmnopqrstuvwxyz";
    std::string randomStr;
    randomStr.reserve( length );

    for ( int i = 0; i < length; ++i )
    {
        randomStr += alphanum[rand() % ( sizeof( alphanum ) - 1 )];
    }

    return QString( "%1.tif" ).arg( QString::fromStdString( randomStr ) );
}
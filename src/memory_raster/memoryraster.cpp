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

    mRasterData = std::make_unique<QgsRasterBlock>( mDataType, mWidth, mHeight );
    mRasterData->setNoDataValue( mNoDataValue );

    if ( !mRasterData->isValid() )
    {
        mError = mRasterData->error().message();
        return;
    }

    if ( std::isnan( defaultValue ) )
    {
        prefillValues( mNoDataValue );
    }
    else
    {
        defaultValue = QgsRaster::representableValue( defaultValue, mDataType );
        prefillValues( defaultValue );
    }

    if ( !mRasterData->isValid() )
    {
        mError = mRasterData->error().message();
        return;
    }

    mValid = true;
}

QString MemoryRaster::error() { return mError; }

void MemoryRaster::prefillValues( double value )
{
    mValid = false;

    if ( mRasterData )
    {
        bool ok;
        for ( int i = 0; i < mWidth * mHeight; i++ )
        {
            ok = mRasterData->setValue( i, value );
            if ( !ok )
            {
                mError = mRasterData->error().message();
                break;
            }
        }
        mError = QString();
        mValid = true;
    }
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

    saveRasterDp->writeBlock( mRasterData.get(), mDefaultBand, 0, 0 );

    return true;
}

std::unique_ptr<QgsRasterDataProvider> MemoryRaster::dataProvider()
{

    std::unique_ptr<QgsRasterDataProvider> dataProvider;

    QString rasterFilename = QString( "/vsimem/%1" ).arg( randomName() );

    QgsRasterFileWriter rw = QgsRasterFileWriter( rasterFilename );
    rw.setOutputProviderKey( "gdal" );

    dataProvider.reset( rw.createOneBandRaster( mDataType, mWidth, mHeight, mExtent, mCrs ) );

    if ( !dataProvider )
    {
        return nullptr;
    }

    if ( !dataProvider->isValid() )
    {
        return nullptr;
    }

    dataProvider->setNoDataValue( mDefaultBand, mNoDataValue );

    dataProvider->setEditable( true );

    dataProvider->writeBlock( mRasterData.get(), mDefaultBand, 0, 0 );

    dataProvider->setEditable( false );

    return dataProvider;
}

const int MemoryRaster::height() { return mHeight; }

const int MemoryRaster::width() { return mWidth; }

const QgsCoordinateReferenceSystem MemoryRaster::crs() { return mCrs; }

const QgsRectangle MemoryRaster::extent() { return mExtent; }

const Qgis::DataType MemoryRaster::dataType() { return mDataType; }

const int MemoryRaster::defaultBand() { return 1; }

bool MemoryRaster::setValue( double value, int col, int row )
{
    bool success = mRasterData->setValue( row, col, value );

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
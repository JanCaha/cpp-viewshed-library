#include "fileselectorwidget.h"

#include <QApplication>
#include <QFileDialog>
#include <QIcon>
#include <QStyle>

using ViewshedBinaries::FileSelectorWidget;

FileSelectorWidget::FileSelectorWidget( QWidget *parent ) : QWidget( parent )
{

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins( 0, 0, 0, 0 );
    setLayout( layout );

    mButton = new QToolButton( this );
    mButton->setText( QChar( 0x2026 ) );
    mButton->setToolTip( tr( "Browse" ) );
    connect( mButton, &QAbstractButton::clicked, this, &FileSelectorWidget::openFileDialog );

    mText = new QLineEdit( this );
    mText->setToolTip( tr( "Full path to the file(s), including name and extension" ) );
    connect( mText, &QLineEdit::textChanged, this, &FileSelectorWidget::textEdited );

    layout->addWidget( mText );
    layout->addWidget( mButton );

    QStyle *style = QApplication::style();
    QIcon icon = style->standardIcon( QStyle::SP_DialogCloseButton );
    mClearAction = new QAction( icon, "Clear Selection", this );
    mClearAction->setCheckable( false );

    connect( mClearAction, &QAction::triggered, this, &FileSelectorWidget::clearValue );
}

void FileSelectorWidget::setStorageMode( StorageMode mode ) { mStorageMode = mode; }

void FileSelectorWidget::openFileDialog()
{
    QString fileName;
    switch ( mStorageMode )
    {
        case GetFile:
            fileName = QFileDialog::getOpenFileName( this, "Select a file", "", mFilter, &mSelectedFilter,
                                                     QFileDialog::HideNameFilterDetails );
            break;
        case SaveFile:
        {

            fileName = QFileDialog::getSaveFileName( this, "Create or select a file", "", mFilter, &mSelectedFilter,
                                                     QFileDialog::HideNameFilterDetails );

            // fileName = QgsFileUtils::addExtensionFromFilter( fileName, mSelectedFilter );
        }
        case GetDirectory:
        {
            fileName = QFileDialog::getExistingDirectory( this, "Select a directory", "" );
            break;
        }
    }

    if ( fileName.isEmpty() )
        return;

    mText->setText( fileName );
}

void FileSelectorWidget::textEdited( const QString &path )
{
    mText->addAction( mClearAction, QLineEdit::ActionPosition::TrailingPosition );

    QFileInfo fileInfo( path );

    if ( fileInfo.exists() )
    {
        mFilePath = path;
        emit fileChanged( mFilePath );
    }
}

void FileSelectorWidget::clearValue()
{
    mFilePath = QString();
    mText->clear();
    mText->removeAction( mClearAction );

    emit fileChanged( mFilePath );
}

void FileSelectorWidget::setFilePath( QString path )
{
    mText->setText( path );
    if ( !mText->actions().empty() && path.isEmpty() )
    {
        mText->removeAction( mClearAction );
    }
}

QString FileSelectorWidget::filePath() { return mFilePath; }

void FileSelectorWidget::setFilter( std::string filter ) { mFilter = QString::fromStdString( filter ); }
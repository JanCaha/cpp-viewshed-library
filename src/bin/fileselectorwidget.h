#pragma once

#include <QAction>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

namespace ViewshedBinaries
{
    class FileSelectorWidget : public QWidget
    {
        Q_OBJECT

      public:
        enum StorageMode
        {
            GetFile,
            SaveFile,
            GetDirectory
        };
        Q_ENUM( StorageMode )

        FileSelectorWidget( QWidget *parent = nullptr );

        void setStorageMode( StorageMode mode );
        void setFilePath( QString path );
        QString filePath();

        void setFilter( std::string filter );

      signals:
        void fileChanged( const QString &path );

      private slots:
        void openFileDialog();
        void textEdited( const QString &path );

      private:
        QToolButton *mButton = nullptr;
        QLineEdit *mText = nullptr;
        QLabel *mLabel = nullptr;

        QString mFilePath;

        StorageMode mStorageMode = GetFile;

        QString mFilter;
        QString mSelectedFilter;

        QAction *mClearAction = nullptr;

        void clearValue();
    };
}
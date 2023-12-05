#include <QApplication>

#include "viewshedcalculatorwindow.h"

using ViewshedBinaries::MainCalculatorWindow;

int main( int argc, char *argv[] )
{
    QApplication app( argc, argv );
    QApplication::setApplicationName( QStringLiteral( "Viewshed Calculator" ) );
    QApplication::setApplicationVersion( QStringLiteral( "0.5" ) );
    QCoreApplication::setOrganizationName( QStringLiteral( "JanCaha" ) );
    QCoreApplication::setOrganizationDomain( QStringLiteral( "cahik.cz" ) );
    QSettings::setDefaultFormat( QSettings::Format::IniFormat );

    MainCalculatorWindow mw = MainCalculatorWindow();
    mw.show();
    return app.exec();
}

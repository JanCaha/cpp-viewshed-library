#include "iostream"
#include "ostream"
#include <cmath>

#include <QCommandLineParser>
#include <QDebug>

int exitWithError( const char *error, QCommandLineParser &parser )
{
    fprintf( stderr, "%s\n", qPrintable( error ) );
    parser.showHelp( 1 );
}

void printTimeInfo( std::string text, double time ) { std::cout << text << time << " seconds." << std::endl; }

void printProgressInfo( int size, int i )
{
    if ( i % 1000 == 0 )
    {
        double percent = std::round( ( ( i / (double)size ) * 100 ) * 100 ) / 100;

        std::cout << "Evaluated: " << percent << "%"
                  << "     "
                  << "\r" << std::flush;
    }
}

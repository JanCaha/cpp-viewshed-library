#include "doublevalidator.h"

#include <QLocale>
#include <QRegularExpression>
#include <QRegularExpressionValidator>

const QString PERMISSIVE_DOUBLE = R"(-?[\d]{0,1000}([\.%1][\d]{0,1000})?(e[+-]?[\d]{0,%2})?)";

using ViewshedBinaries::DoubleValidator;

DoubleValidator::DoubleValidator( QObject *parent ) : QRegularExpressionValidator( parent )
{
    const QRegularExpression reg( PERMISSIVE_DOUBLE.arg( locale().decimalPoint() ).arg( 1000 ) );
    setRegularExpression( reg );
}

QValidator::State DoubleValidator::validate( QString &input, int & ) const
{
    if ( input.isEmpty() )
        return Intermediate;

    bool ok = false;
    const double entered = DoubleValidator::toDouble( input, &ok );
    if ( !ok )
    {
        if ( regularExpression().match( input ).captured( 0 ) == input )
            return Intermediate;
        else
            return Invalid;
    }

    if ( regularExpression().match( input ).captured( 0 ) == input )
        return Acceptable;
    else
        return Intermediate;
}

QValidator::State DoubleValidator::validate( QString &input ) const
{
    if ( input.isEmpty() )
        return Intermediate;

    bool ok = false;
    const double entered = DoubleValidator::toDouble( input, &ok );
    if ( !ok )
    {
        if ( regularExpression().match( input ).captured( 0 ) == input )
            return Intermediate;
        else
            return Invalid;
    }

    if ( regularExpression().match( input ).captured( 0 ) == input )
        return Acceptable;
    else
        return Intermediate;
}

double DoubleValidator::toDouble( const QString &input )
{
    bool ok = false;
    return toDouble( input, &ok );
}

double DoubleValidator::toDouble( const QString &input, bool *ok )
{
    double value = QLocale().toDouble( input, ok );

    if ( !*ok )
    {
        value = QLocale( QLocale::C ).toDouble( input, ok );
    }

    if ( !*ok && !( QLocale().numberOptions() & QLocale::NumberOption::OmitGroupSeparator ) )
    {
        value = QLocale().toDouble( QString( input ).replace( QLocale().groupSeparator(), QString() ), ok );
    }
    return value;
}

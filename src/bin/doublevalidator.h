#pragma once

#include <QLocale>
#include <QRegularExpressionValidator>

class DoubleValidator : public QRegularExpressionValidator
{
  public:
    DoubleValidator( QObject *parent );

    QValidator::State validate( QString &input ) const;
    QValidator::State validate( QString &input, int & ) const override;

    static double toDouble( const QString &input, bool *ok );
    static double toDouble( const QString &input );
};
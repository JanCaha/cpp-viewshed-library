#include "qgsdoublespinbox.h"

#define MIN 0
#define MAX 999999

class IntegerSpinBox : public QgsDoubleSpinBox
{
  public:
    IntegerSpinBox( int defaultValue, QWidget *parent = nullptr )
        : IntegerSpinBox( defaultValue, false, MIN, MAX, false, parent )
    {
    }

    IntegerSpinBox( bool noDataDefault = true, int min = MIN, int max = MAX, QWidget *parent = nullptr )
        : IntegerSpinBox( MIN, noDataDefault, min, max, noDataDefault, parent )
    {
    }

    IntegerSpinBox( int defaultValue, bool useNoData = false, int min = MIN, int max = MAX, bool noDataDefault = false,
                    QWidget *parent = nullptr )
        : QgsDoubleSpinBox( parent )
    {
        double clearValue = min;

        if ( useNoData )
        {
            setClearValueMode( QgsDoubleSpinBox::ClearValueMode::CustomValue );
            clearValue = min - 1;
            setRange( clearValue, max );
            setClearValue( clearValue, "No Data" );
            mNoData = true;
        }
        else
        {
            setClearValueMode( QgsDoubleSpinBox::ClearValueMode::CustomValue );
            setRange( min, max );
            setClearValue( defaultValue );
        }

        setDecimals( 0 );

        setValue( defaultValue );

        if ( useNoData && noDataDefault )
        {
            setValue( clearValue );
        }
    }

    double isValueNoData()
    {
        if ( mNoData )
        {
            if ( qgsDoubleNear( clearValue(), value() ) )
            {
                return true;
            }
        }

        return false;
    }

  private:
    bool mNoData = false;
};
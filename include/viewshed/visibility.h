#pragma once
#include "viewshed_export.h"

#include "constants.h"
#include "enums.h"

using viewshed::CellEventPositionType;

namespace viewshed
{
    class Point;
    class RasterPosition;
    class CellEventPosition;

    /**
     * @brief Class that provides static functions to assist with viewshed calculation.
     *
     */
    class DLL_API Visibility
    {
      public:
        /**
         * @brief Calculate distance.
         *
         * @param x1
         * @param y1
         * @param x2
         * @param y2
         * @param cellSize
         * @return double
         */
        static double distance( const double &x1, const double &y1, const double &x2, const double &y2,
                                const double &cellSize );

        /**
         * @brief Calculate gradient.
         *
         * @param elevationDiff
         * @param distance
         * @return double
         */
        static double gradient( double elevationDiff, double distance );

        /**
         * @brief Calculate elevation change based on distance, atmospheric refraction and earth curvature.
         *
         * @param distance
         * @param refractionCoeff Refraction coefficient.
         * @param earthDiameter Diameter of Earth to use.
         * @return double
         */
        static double curvatureCorrections( const double &distance, const double &refractionCoeff,
                                            const double &earthDiameter );

        /**
         * @brief Calculate elevation change based on distance, atmospheric refraction and earth curvature. This version
         * uses default values of refraction coefficient 0.142860 and earth diameter 12740000.
         *
         * @param distance
         * @return double
         */
        static double curvatureCorrections( double &distance )
        {
            return curvatureCorrections( distance, viewshed::REFRACTION_COEFFICIENT, viewshed::EARTH_DIAMETER );
        };

        /**
         * @brief Obtain cell event postion based on cell event position type, row, column and important point.
         *
         * @param eventType
         * @param row
         * @param col
         * @param pointRow
         * @param pointCol
         * @return CellEventPosition
         */
        static CellEventPosition eventPosition( const CellEventPositionType &eventType, const int &row, const int &col,
                                                const int &pointRow, const int &pointCol );

        /**
         * @brief Calculate horizontal angle.
         *
         * @param row
         * @param column
         * @param pointRow
         * @param pointCol
         * @return double
         */
        static double angle( const double &row, const double &column, const int &pointRow, const int &pointCol );
    };
} // namespace viewshed

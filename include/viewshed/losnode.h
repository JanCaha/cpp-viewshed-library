#pragma once
#include "viewshed_export.h"

#include <memory>

#include "enums.h"
#include "rasterposition.h"

namespace viewshed
{
    class Point;
    class CellEvent;

    /**
     * @brief Representation of single cell that creates a point on LoS.
     *
     */
    class DLL_API LoSNode : public RasterPosition
    {
      public:
        double mAngle[3];
        double mElevs[3];
        double mDistances[3];
        bool mInverseLoSBehindTarget = false;

        /**
         * @brief Construct a new empty LoSNode object.
         *
         */
        LoSNode();

        /**
         * @brief Construct a new LoSNode object at specific row and column.
         *
         * @param row
         * @param col
         */
        LoSNode( int row, int col );

        /**
         * @brief @brief Construct a new LoSNode object build from Point position, CellEvent and cell size.
         *
         * @param pointRow Important point row of viewshed (either view point for viewshed or target point for
         * inverseviewshed).
         * @param pointCol Important point column of viewshed (either view point for viewshed or target point for
         * inverseviewshed).
         * @param e Cell event with all the necessary information.
         * @param cellSize Cell size of raster from which the information was derived.
         */
        LoSNode( const int &pointRow, const int &pointCol, const CellEvent *e, const double &cellSize );

        bool operator==( const LoSNode &other ) const;
        bool operator!=( const LoSNode &other ) const;
        bool operator<( const LoSNode other ) const;

        /**
         * @brief Extract value of ValueType at specified angle.
         *
         * @param angle
         * @param valueType
         * @return double
         */
        double valueAtAngle( const double &angle, ValueType valueType = ValueType::Elevation ) const;

        /**
         * @brief Extract value of ValueType at specific cell event position.
         *
         * @param position
         * @param valueType
         * @return double
         */
        double value( CellEventPositionType position, ValueType valueType = ValueType::Elevation ) const;

        /**
         * @brief Shortcut call to obtain horizontal angle at center of raster cell that this LoSNode represents.
         *
         * @return double
         */
        double centreAngle() const;

        /**
         * @brief Shortcut call to obtain elevation angle at center of raster cell that this LoSNode represents.
         *
         * @return double
         */
        double centreElevation() const;

        /**
         * @brief Shortcut call to obtain distance angle at center of raster cell that this LoSNode represents.
         *
         * @return double
         */
        double centreDistance() const;

        double elevationAtAngle( const double &angle ) const;

        double distanceAtAngle( const double &angle ) const;
    };
} // namespace viewshed

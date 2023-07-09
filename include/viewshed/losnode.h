#ifndef VIEWSHEDLIB_LOSNODE_H
#define VIEWSHEDLIB_LOSNODE_H

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
    class LoSNode : public RasterPosition
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
         * @brief Construct a new LoSNode object build from Point, CellEvent and cell size.
         *
         * @param point Impotant point of viewshed (either view point for viewshed or target point for inver viewshed).
         * @param e Cell event with all the necessary information.
         * @param cellSize Cell size of raster from which the information was derived.
         */
        LoSNode( std::shared_ptr<Point> point, CellEvent *e, double &cellSize );

        bool operator==( const LoSNode &other );
        bool operator!=( const LoSNode &other );
        bool operator<( const LoSNode other );

        /**
         * @brief Extract value of ValueType at specified angle.
         *
         * @param angle
         * @param valueType
         * @return double
         */
        double valueAtAngle( const double &angle, ValueType valueType = ValueType::Elevation );

        /**
         * @brief Extract value of ValueType at specific cell event position.
         *
         * @param position
         * @param valueType
         * @return double
         */
        double value( CellEventPositionType position, ValueType valueType = ValueType::Elevation );

        /**
         * @brief Shortcut call to obtain horizontal angle at center of raster cell that this LoSNode represents.
         *
         * @return double
         */
        double centreAngle();

        /**
         * @brief Shortcut call to obtain elevation angle at center of raster cell that this LoSNode represents.
         *
         * @return double
         */
        double centreElevation();

        /**
         * @brief Shortcut call to obtain distance angle at center of raster cell that this LoSNode represents.
         *
         * @return double
         */
        double centreDistance();
    };
} // namespace viewshed

#endif
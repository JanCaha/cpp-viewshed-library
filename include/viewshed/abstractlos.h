#ifndef VIEWSHEDLIB_ABSTRACTLOS_H
#define VIEWSHEDLIB_ABSTRACTLOS_H

#include "cellevent.h"
#include "enums.h"
#include "losnode.h"
#include "point.h"
#include "visibility.h"

using viewshed::CellEvent;
using viewshed::LoSNode;
using viewshed::Point;
using viewshed::ValueType;

// using viewshed::Visibility;

namespace viewshed
{
    /**
     * @brief Abstract class that represent line-of-sight (LoS). Consists of LoSNodes, view point and target point (both
     * with potential offset from the surface).
     *
     */
    class AbstractLoS
    {
      public:
        /**
         * @brief Set the View Point object directly from Point with offset specified as part of this object.
         *
         * @param vp
         */
        void setViewPoint( std::shared_ptr<Point> vp );
        /**
         * @brief Set the View Point object from LoSNode point, specifying offset of the observer.
         *
         * @param poi
         * @param observerOffset
         */
        void setViewPoint( std::shared_ptr<LoSNode> poi, double observerOffset = 0 );
        /**
         * @brief Set the Target Point object directly from Point with offset specified as part of this object.
         *
         * @param vp
         */
        void setTargetPoint( std::shared_ptr<Point> tp );
        /**
         * @brief Set the Target Point object from LoSNode point, specifying offset of the target.
         *
         * @param poi
         * @param observerOffset
         */
        void setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset = 0 );
        /**
         * @brief Set the horizontal angle for extracting elevations, distances and gradients from line-of-sight.
         *
         * @param angle
         */
        void setAngle( double angle );

        /**
         * @brief Prepare line-of-sight for calculation. For example inverse LoS needs some preprocessing before being
         * able to be analysed.
         *
         */
        virtual void prepareForCalculation() = 0;

        /**
         * @brief Extract gradient value for line-of-sight LoSNode at position `i`.
         *
         * @param i
         * @return double
         */
        virtual double gradient( int i ) = 0;

        /**
         * @brief Extract distance for line-of-sight LoSNode at position `i`.
         *
         * @param i
         * @return double
         */
        virtual double distance( int i ) = 0;

        /**
         * @brief Extract elevation for line-of-sight LoSNode at position `i`.
         *
         * @param i
         * @return double
         */
        virtual double elevation( int i ) = 0;

        virtual bool isValid() = 0;

        /**
         * @brief Number of LoSNodes in this line-of-sight.
         *
         * @return int
         */
        virtual int numberOfNodes() = 0;

        /**
         * @brief Index of target point in the line-of-sight.
         *
         * @return int
         */
        virtual int targetPointIndex() = 0;

        /**
         * @brief Row number where the outcome of LoS analysis should be stored.
         *
         * @return int
         */
        virtual int resultRow() = 0;

        /**
         * @brief Column number where the outcome of LoS analysis should be stored.
         *
         * @return int
         */
        virtual int resultCol() = 0;

        /**
         * @brief LoSNode at specific index.
         *
         * @param i
         * @return LoSNode
         */
        virtual LoSNode nodeAt( int i ) = 0;

        /**
         * @brief Horizontal angle for this line-of-sight.
         *
         * @return double
         */
        double horizontalAngle();

        /**
         * @brief Row number for the target of LoS.
         *
         * @return int
         */
        int targetRow();

        /**
         * @brief Column number for the target of LoS.
         *
         * @return int
         */
        int targetCol();

        /**
         * @brief Distance of target.
         *
         * @return double
         */
        double targetDistance();

        /**
         * @brief Gradient of target.
         *
         * @return double
         */
        double targetGradient();

        /**
         * @brief Elevation of target.
         *
         * @return double
         */
        double targetElevation();

        /**
         * @brief View point for this line-of-sight.
         *
         * @return std::shared_ptr<Point>
         */
        std::shared_ptr<Point> vp();

        void applyCurvatureCorrections( bool apply, double refractionCoeff, double earthDiameter )
        {
            mCurvatureCorrections = apply;
            mRefractionCoefficient = refractionCoeff;
            mEarthDiameter = earthDiameter;
        };

        void applyCurvatureCorrections( bool apply ) { mCurvatureCorrections = apply; };

        void setRefractionCoeficient( double refractionCoeff ) { mRefractionCoefficient = refractionCoeff; };

        void setEarthDiameter( double earthDiameter ) { mEarthDiameter = earthDiameter; };

      protected:
        AbstractLoS();

        /**
         * @brief Retrieve index of target point in this LoS.
         *
         */
        virtual void findTargetPointIndex() = 0;

        /**
         * @brief Horizontal angle between view point and target point.
         *
         */
        double mAngleHorizontal;

        /**
         * @brief Distance between observer and target point.
         *
         */
        double mPointDistance;

        /**
         * @brief View point.
         *
         */
        std::shared_ptr<Point> mVp = std::make_shared<Point>();

        /**
         * @brief Target point.
         *
         */
        std::shared_ptr<Point> mTp = std::make_shared<Point>();

        /**
         * @brief Index of target point.
         *
         */
        int mTargetIndex;

        bool mCurvatureCorrections = false;
        double mEarthDiameter = EARTH_DIAMETER;
        double mRefractionCoefficient = REFRACTION_COEFFICIENT;

        /**
         * @brief Sort LoSNodes by distance from View point.
         *
         */
        virtual void sort() = 0;
    };

} // namespace viewshed

#endif
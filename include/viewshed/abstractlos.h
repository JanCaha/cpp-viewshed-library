#ifndef VIEWSHEDLIB_ABSTRACTLOS_H
#define VIEWSHEDLIB_ABSTRACTLOS_H

#include <chrono>

#include "cellevent.h"
#include <algorithm>
#include <memory>
#include <vector>

#include "enums.h"
#include "visibility.h"

namespace viewshed
{
    class CellEvent;
    class LoSNode;

    /**
     * @brief Abstract class that represent line-of-sight (LoS). Consists of LoSNodes, view point and target point (both
     * with potential offset from the surface).
     *
     */
    class AbstractLoS : public std::vector<LoSNode>
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
        virtual double gradient( int i );

        /**
         * @brief Extract distance for line-of-sight LoSNode at position `i`.
         *
         * @param i
         * @return double
         */
        virtual double distance( int i );

        /**
         * @brief Extract elevation for line-of-sight LoSNode at position `i`.
         *
         * @param i
         * @return double
         */
        virtual double elevation( int i );

        virtual bool isValid() = 0;

        /**
         * @brief Number of LoSNodes in this line-of-sight.
         *
         * @return int
         */
        virtual int numberOfNodes();

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
        virtual LoSNode nodeAt( int i );

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

        /**
         * @brief Specify curvature corrections settings.
         *
         * @param apply
         * @param refractionCoeff
         * @param earthDiameter
         */
        void applyCurvatureCorrections( bool apply, double refractionCoeff, double earthDiameter )
        {
            mCurvatureCorrections = apply;
            mRefractionCoefficient = refractionCoeff;
            mEarthDiameter = earthDiameter;
        };

        /**
         * @brief Set curvature corrections on/off.
         *
         * @param apply
         */
        void applyCurvatureCorrections( bool apply ) { mCurvatureCorrections = apply; };

        /**
         * @brief Set the Refraction Coeficient for curvature corrections.
         *
         * @param refractionCoeff
         */
        void setRefractionCoeficient( double refractionCoeff ) { mRefractionCoefficient = refractionCoeff; };

        /**
         * @brief Set the Earth Diameter for curvature corrections.
         *
         * @param earthDiameter
         */
        void setEarthDiameter( double earthDiameter ) { mEarthDiameter = earthDiameter; };

        std::chrono::nanoseconds timeToCopy;
        std::chrono::nanoseconds timeToEval;
        std::chrono::nanoseconds timeToPrepare;
        unsigned long long originalNodesCount;

        /**
         * @brief Get index of target point in the current LoS.
         *
         * @return int
         */

        int targetIndex() { return mTargetIndex; }

        /**
         * @brief Surface elevation of viewpoint;
         *
         * @return double
         */
        double viewPointElevation();

        /**
         * @brief Surface elevation of viewpoint with offset.
         *
         * @return double
         */
        double viewPointTotalElevation();

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

        /**
         * @brief Use curvature corrections for this LoS.
         *
         */
        bool mCurvatureCorrections = false;

        /**
         * @brief Earth diameter for curvature corrections.
         *
         */
        double mEarthDiameter = EARTH_DIAMETER;

        /**
         * @brief Refraction coefficient for curvature corrections.
         *
         */
        double mRefractionCoefficient = REFRACTION_COEFFICIENT;

        /**
         * @brief Sort LoSNodes by distance from View point.
         *
         */
        virtual void sort();

        /**
         * @brief Validity of LoS.
         *
         */
        bool mValid;
    };

} // namespace viewshed

#endif
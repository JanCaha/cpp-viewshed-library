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

    class AbstractLoS
    {
      public:
        void setViewPoint( std::shared_ptr<Point> vp );
        void setViewPoint( std::shared_ptr<LoSNode> poi, double observerOffset = 0 );
        void setTargetPoint( std::shared_ptr<Point> tp );
        void setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset = 0 );
        void setAngle( double angle );

        virtual void prepareForCalculation() = 0;

        virtual double gradient( int i ) = 0;
        virtual double distance( int i ) = 0;
        virtual double elevation( int i ) = 0;
        virtual bool isValid() = 0;

        virtual int numberOfNodes() = 0;
        virtual int targetPointIndex() = 0;
        virtual int resultRow() = 0;
        virtual int resultCol() = 0;

        virtual LoSNode nodeAt( int i ) = 0;

        double horizontalAngle();

        int targetRow();
        int targetCol();

        double targetDistance();
        double targetGradient();
        double targetElevation();

        std::shared_ptr<Point> vp();

      protected:
        AbstractLoS();

        virtual void findTargetPointIndex() = 0;

        double mAngleHorizontal;
        double mPointDistance;
        std::shared_ptr<Point> mVp = std::make_shared<Point>();
        std::shared_ptr<Point> mTp = std::make_shared<Point>();
        int mTargetIndex;

        virtual void sort() = 0;
    };

} // namespace viewshed

#endif
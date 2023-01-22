#ifndef VIEWSHEDLIB_LOS_H
#define VIEWSHEDLIB_LOS_H

#include "cellevent.h"
#include "enums.h"
#include "losnode.h"
#include "points.h"
#include "visibility.h"

using viewshed::CellEvent;
using viewshed::LoSNode;
using viewshed::Point;
using viewshed::ValueType;

// using viewshed::Visibility;

namespace viewshed
{

    class ILoS
    {
      public:
        virtual double gradient( int i ) = 0;
        virtual double distance( int i ) = 0;
        virtual double elevation( int i ) = 0;
        virtual bool isValid() = 0;

        void setViewPoint( std::shared_ptr<Point> vp );

        void setViewPoint( std::shared_ptr<LoSNode> poi, double observerOffset = 0 );

        void setTargetPoint( std::shared_ptr<Point> tp );

        void setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset = 0 );

        void setAngle( double angle );

        void prepareForCalculation();

        double horizontalAngle();
        double targetDistance();
        double targetGradient();
        double targetElevation();

        std::shared_ptr<Point> vp();

        int targetRow();
        int targetCol();

      protected:
        ILoS();
        double mAngleHorizontal;
        double mPointDistance;
        std::shared_ptr<Point> mVp = std::make_shared<Point>();
        std::shared_ptr<Point> mTp = std::make_shared<Point>();
        int mTargetIndex;
    };

    class LoS : public std::vector<LoSNode>, public ILoS
    {
      public:
        LoS();
        LoS( std::vector<LoSNode> losNodes );

        void setLoSNodes( std::vector<LoSNode> losNodes );

        double gradient( int i ) override;

        double distance( int i ) override;

        double elevation( int i ) override;

        bool isValid() override;

        void setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset = 0 );

        int targetPointIndex();

        void sort();
    };

    class InverseLoS : public LoS
    {
      public:
        InverseLoS();
        InverseLoS( std::vector<LoSNode> losNodes );

        void setViewPoint( std::shared_ptr<Point> vp ) = delete;
        void setTargetPoint( std::shared_ptr<Point> tp ) = delete;

        void setTargetPoint( std::shared_ptr<Point> tp, double targetOffset = 0 );

        void setViewPoint( std::shared_ptr<LoSNode> vp, double observerOffset );

        double distance( int i ) override;

        double gradient( int i ) override;

        double elevation( int i ) override;
    };

} // namespace viewshed

#endif
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
        virtual int numberOfNodes() = 0;
        virtual int targetPointIndex() = 0;
        virtual void sort() = 0;
        virtual LoSNode nodeAt( int i ) = 0;
        virtual void prepareForCalculation() = 0;

        void setViewPoint( std::shared_ptr<Point> vp );

        void setViewPoint( std::shared_ptr<LoSNode> poi, double observerOffset = 0 );

        void setTargetPoint( std::shared_ptr<Point> tp );

        void setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset = 0 );

        void setAngle( double angle );

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

        LoSNode nodeAt( int i ) override;

        void setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset = 0 );

        int targetPointIndex() override;

        int numberOfNodes() override;

        void prepareForCalculation() override;

        void sort() override;
    };

    class InverseLoS : public std::vector<LoSNode>, public ILoS
    {
      public:
        InverseLoS();
        InverseLoS( std::vector<LoSNode> losNodes );

        void setLoSNodes( std::vector<LoSNode> losNodes );
        void setLoSNodes( LoS los );

        void setViewPoint( std::shared_ptr<Point> vp ) = delete;
        void setTargetPoint( std::shared_ptr<Point> tp ) = delete;

        void setTargetPoint( std::shared_ptr<Point> tp, double targetOffset = 0 );

        void setViewPoint( std::shared_ptr<LoSNode> vp, double observerOffset );

        double distance( int i ) override;

        double gradient( int i ) override;

        double elevation( int i ) override;

        LoSNode nodeAt( int i ) override;

        int numberOfNodes() override;

        int targetPointIndex() override;

        void prepareForCalculation() override;

        void sort() override;

        bool isValid() override;

      private:
        void removePointsAfterViewPoint();
    };

} // namespace viewshed

#endif
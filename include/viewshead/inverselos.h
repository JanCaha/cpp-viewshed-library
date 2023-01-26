#ifndef VIEWSHEDLIB_INVERSELOS_H
#define VIEWSHEDLIB_INVERSELOS_H

#include "abstractlos.h"
#include "cellevent.h"
#include "enums.h"
#include "los.h"
#include "losnode.h"
#include "point.h"
#include "visibility.h"

using viewshed::CellEvent;
using viewshed::LoS;
using viewshed::LoSNode;
using viewshed::Point;
using viewshed::ValueType;

// using viewshed::Visibility;

namespace viewshed
{
    class InverseLoS : public std::vector<LoSNode>, public AbstractLoS
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
        void prepareForCalculation() override;

        bool isValid() override;

        int numberOfNodes() override;
        int resultRow() override;
        int resultCol() override;
        int targetPointIndex() override;

        double distance( int i ) override;
        double gradient( int i ) override;
        double elevation( int i ) override;

        LoSNode nodeAt( int i ) override;

      protected:
        void sort() override;

      private:
        void removePointsAfterViewPoint();
        void fixDistancesAngles();
    };

} // namespace viewshed

#endif
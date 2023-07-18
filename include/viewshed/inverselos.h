#ifndef VIEWSHEDLIB_INVERSELOS_H
#define VIEWSHEDLIB_INVERSELOS_H

#include <vector>

#include "abstractlos.h"
#include "enums.h"

using viewshed::ValueType;

// using viewshed::Visibility;

namespace viewshed
{
    class LoSNode;
    class LoS;
    class Point;
    class CellEvent;

    /**
     * @brief Class that represents InverseLoS, that is LoS used in Inverse Viewshed calculation.
     *
     */
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
        void setRemovePointsAfterTarget( bool remove );
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
        void findTargetPointIndex() override;

      private:
        void removePointsAfterViewPoint();
        void fixDistancesAngles();
        void removePointsAfterTarget();
        bool mRemovePointsAfterTarget = false;
        void setUpTargetLoSNode();
    };

} // namespace viewshed

#endif
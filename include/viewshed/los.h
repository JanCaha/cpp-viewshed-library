#ifndef VIEWSHEDLIB_LOS_H
#define VIEWSHEDLIB_LOS_H

#include <memory>
#include <vector>

#include "abstractlos.h"
#include "enums.h"

using viewshed::ValueType;

// using viewshed::Visibility;

namespace viewshed
{
    class CellEvent;
    class LosNode;
    class Point;

    /**
     * @brief Class that represents LoS for classic Viewshed calculation.
     *
     */
    class LoS : public AbstractLoS
    {
      public:
        LoS();
        LoS( std::vector<LoSNode> losNodes );

        void setLoSNodes( std::vector<LoSNode> losNodes );
        void setTargetPoint( std::shared_ptr<LoSNode> poi, double targetOffset = 0 );
        void prepareForCalculation() override;

        bool isValid() override;

        int targetPointIndex() override;
        int numberOfNodes() override;
        int resultRow() override;
        int resultCol() override;

        LoSNode nodeAt( std::size_t i ) override;
        void removePointsAfterTarget();

      protected:
        void sort() override;
        void findTargetPointIndex() override;
    };

} // namespace viewshed

#endif
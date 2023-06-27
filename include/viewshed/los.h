#ifndef VIEWSHEDLIB_LOS_H
#define VIEWSHEDLIB_LOS_H

#include <memory>
#include <vector>

#include "abstractlos.h"
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
     * @brief Class that represents LoS for classic Viewshed calculation.
     *
     */
    class LoS : public std::vector<LoSNode>, public AbstractLoS
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

        double gradient( int i ) override;
        double distance( int i ) override;
        double elevation( int i ) override;

        LoSNode nodeAt( int i ) override;
        void removePointsAfterTarget();

      protected:
        void sort() override;
        void findTargetPointIndex() override;
    };

} // namespace viewshed

#endif
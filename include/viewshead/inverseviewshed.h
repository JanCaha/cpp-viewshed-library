#ifndef VIEWSHEDLIB_INVERSEVIEWSHED_H
#define VIEWSHEDLIB_INVERSEVIEWSHED_H

#include <functional>
#include <limits>

#include "enums.h"
#include "iviewshed.h"
#include "losevaluator.h"
#include "points.h"

namespace viewshed
{
    class InverseViewshed : public IViewshed
    {
      public:
        InverseViewshed( std::shared_ptr<Point> targetPoint, std::shared_ptr<QgsRasterLayer> dem,
                         std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs,
                         double observerOffset );

        void calculate(
            std::function<void( std::string, double )> stepsTimingCallback = []( std::string text, double time )
            { qDebug() << QString::fromStdString( text ) << time; },
            std::function<void( int, int )> progressCallback = []( int, int ) {} );

        void parseEventList( std::function<void( int, int )> progressCallback = []( int, int ) {} );
        void submitToThreadpool( CellEvent &e ) override;

        std::shared_ptr<std::vector<LoSNode>> LoSToPoint( QgsPoint point, bool onlyToPoint = false );

        std::shared_ptr<std::vector<LoSNode>> prepareLoS( std::shared_ptr<Point> vp );

      private:
        double mObserverOffset;
        //     std::shared_ptr<std::vector<LoSNode>> getLoS( LoSNode poi, bool onlyToPoi = false );
    };

} // namespace viewshed

#endif
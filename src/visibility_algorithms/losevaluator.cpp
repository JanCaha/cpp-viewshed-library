#include "losevaluator.h"

LoSEvaluator::LoSEvaluator( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs ) : mAlgs( algs ) {}

int LoSEvaluator::size() { return mAlgs.size(); }

std::shared_ptr<IViewshedAlgorithm> LoSEvaluator::algorithmAt( int i ) { return mAlgs.at( i ); }

void LoSEvaluator::parseNodes( std::vector<StatusNode> &statusNodes, StatusNode &poi )
{
    StatusNode sn;
    StatusNode snNext;
    double snGradient;

    for ( int i = 0; i < statusNodes.size(); i++ )
    {
        sn = statusNodes.at( i );

        if ( sn == poi )
        {
            mIndexPoi = i;
        }

        snGradient = sn.valueAtAngle( poi.centreAngle(), ValueType::Gradient );

        if ( i + 1 < statusNodes.size() )
        {
            snNext = statusNodes.at( i + 1 );

            if ( snNext.centreDistance() < poi.centreDistance() )
            {
                if ( mMaxGradientBefore < snGradient &&
                     snNext.valueAtAngle( poi.centreAngle(), ValueType::Gradient ) < snGradient )
                {
                    mIndexHorizonBefore = i;
                    mCountHorizonBefore++;
                }
            }

            if ( mMaxGradient < snGradient &&
                 snNext.valueAtAngle( poi.centreAngle(), ValueType::Gradient ) < snGradient )
            {
                mIndexHorizon = i;
                mCountHorizon++;
            }
        }

        if ( sn.centreDistance() < poi.centreDistance() )
        {
            if ( mMaxGradientBefore < snGradient )
            {
                mMaxGradientBefore = snGradient;
                mIndexMaxGradientBefore = i;
            }
        }

        if ( mMaxGradient < snGradient )
        {
            mMaxGradient = snGradient;
            mIndexMaxGradient = i;
        }

        for ( int j = 0; j < mAlgs.size(); j++ )
        {
            mAlgs.at( j )->extractValues( sn, poi, i );
        }
    }
}

void LoSEvaluator::calculate( std::vector<StatusNode> &statusNodes, StatusNode &poi, std::shared_ptr<ViewPoint> vp )
{
    parseNodes( statusNodes, poi );

    for ( int i = 0; i < mAlgs.size(); i++ )
    {
        mResults.push_back( mAlgs.at( i )->result( this, statusNodes, poi, vp ) );
    }
}

double LoSEvaluator::resultAt( int i ) { return mResults.at( i ); }

void LoSEvaluator::reset()
{
    mMaxGradientBefore = std::numeric_limits<double>::max() * ( -1 );
    mMaxGradient = std::numeric_limits<double>::max() * ( -1 );
    mIndexPoi = 0;
    mIndexMaxGradientBefore = 0;
    mIndexMaxGradient = 0;
    mIndexHorizonBefore = 0;
    mIndexHorizon = 0;
    mCountHorizonBefore = 0;
    mCountHorizon = 0;
    mResults.clear();
}
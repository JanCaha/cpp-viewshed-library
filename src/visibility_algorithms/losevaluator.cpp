#include "losevaluator.h"

using viewshed::IViewshedAlgorithm;
using viewshed::LoSEvaluator;

int LoSEvaluator::size() { return mAlgs.size(); }

std::shared_ptr<IViewshedAlgorithm> LoSEvaluator::algorithmAt( int i ) { return mAlgs.at( i ); }

void LoSEvaluator::parseNodes( std::vector<StatusNode> &statusNodes, std::shared_ptr<StatusNode> poi )
{
    StatusNode sn;
    StatusNode snNext;
    double snGradient;

    for ( int i = 0; i < statusNodes.size(); i++ )
    {
        sn = statusNodes.at( i );

        if ( sn == *poi.get() )
        {
            mIndexPoi = i;
        }

        snGradient = sn.valueAtAngle( poi->centreAngle(), ValueType::Gradient );

        if ( i + 1 < statusNodes.size() )
        {
            snNext = statusNodes.at( i + 1 );

            if ( snNext.centreDistance() <= poi->centreDistance() )
            {
                if ( mMaxGradientBefore < snGradient &&
                     snNext.valueAtAngle( poi->centreAngle(), ValueType::Gradient ) < snGradient )
                {
                    mIndexHorizonBefore = i;
                    mCountHorizonBefore++;
                }
            }

            if ( mMaxGradient < snGradient &&
                 snNext.valueAtAngle( poi->centreAngle(), ValueType::Gradient ) < snGradient )
            {
                mIndexHorizon = i;
                mCountHorizon++;
            }
        }

        if ( sn.centreDistance() < poi->centreDistance() )
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
            mAlgs.at( j )->extractValues( sn, *poi.get(), i );
        }
    }

    mAlreadyParsed = true;
}

void LoSEvaluator::calculate( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs,
                              std::vector<StatusNode> &statusNodes, std::shared_ptr<StatusNode> poi,
                              std::shared_ptr<IPoint> point )
{

    mAlgs = algs;

    if ( mAlreadyParsed )
        reset();

    parseNodes( statusNodes, poi );

    mResultValues = ViewshedValues( poi->row, poi->col );

    for ( int i = 0; i < mAlgs.size(); i++ )
    {
        mResultValues.values.push_back( mAlgs.at( i )->result( this, statusNodes, *poi.get(), point ) );
    }
}

void LoSEvaluator::reset()
{
    mMaxGradientBefore = -180;
    mMaxGradient = -180;
    mIndexPoi = 0;
    mIndexMaxGradientBefore = 0;
    mIndexMaxGradient = 0;
    mIndexHorizonBefore = 0;
    mIndexHorizon = 0;
    mCountHorizonBefore = 0;
    mCountHorizon = 0;
    mResultValues = ViewshedValues();
}

double LoSEvaluator::resultAt( int i ) { return mResultValues.values[i]; }
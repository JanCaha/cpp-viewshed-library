#include "losevaluator.h"
#include "iviewshed.h"

using viewshed::IViewshedAlgorithm;
using viewshed::LoSEvaluator;

int LoSEvaluator::size() { return mAlgs.size(); }

std::shared_ptr<IViewshedAlgorithm> LoSEvaluator::algorithmAt( int i ) { return mAlgs.at( i ); }

void LoSEvaluator::parseNodes( std::vector<LoSNode> &statusNodes, std::shared_ptr<LoSNode> poi )
{
    LoSNode ln;
    LoSNode lnNext;
    double snGradient;

    for ( int i = 0; i < statusNodes.size(); i++ )
    {
        ln = statusNodes.at( i );

        if ( ln == *poi.get() )
        {
            mLosValues->mIndexPoi = i;
        }

        snGradient = ln.valueAtAngle( poi->centreAngle(), ValueType::Gradient );

        if ( i + 1 < statusNodes.size() )
        {
            lnNext = statusNodes.at( i + 1 );

            if ( lnNext.centreDistance() <= poi->centreDistance() )
            {
                if ( mLosValues->mMaxGradientBefore < snGradient &&
                     lnNext.valueAtAngle( poi->centreAngle(), ValueType::Gradient ) < snGradient )
                {
                    mLosValues->mIndexHorizonBefore = i;
                    mLosValues->mCountHorizonBefore++;
                }
            }

            if ( mLosValues->mMaxGradient < snGradient &&
                 lnNext.valueAtAngle( poi->centreAngle(), ValueType::Gradient ) < snGradient )
            {
                mLosValues->mIndexHorizon = i;
                mLosValues->mCountHorizon++;
            }
        }

        if ( ln.centreDistance() < poi->centreDistance() )
        {
            if ( mLosValues->mMaxGradientBefore < snGradient )
            {
                mLosValues->mMaxGradientBefore = snGradient;
                mLosValues->mIndexMaxGradientBefore = i;
            }
        }

        if ( mLosValues->mMaxGradient < snGradient )
        {
            mLosValues->mMaxGradient = snGradient;
            mLosValues->mIndexMaxGradient = i;
        }
    }
    mAlreadyParsed = true;
}

void LoSEvaluator::calculate( std::vector<std::shared_ptr<IViewshedAlgorithm>> algs, std::vector<LoSNode> &statusNodes,
                              std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> point )
{

    mAlgs = algs;

    if ( mAlreadyParsed )
        reset();

    parseNodes( statusNodes, poi );

    mResultValues = ViewshedValues( poi->row, poi->col );

    for ( int i = 0; i < mAlgs.size(); i++ )
    {
        mResultValues.values.push_back( mAlgs.at( i )->result( mLosValues, statusNodes, *poi.get(), point ) );
    }
}

void LoSEvaluator::reset()
{
    mResultValues = ViewshedValues();
    mLosValues->reset();
}

double LoSEvaluator::resultAt( int i ) { return mResultValues.values[i]; }

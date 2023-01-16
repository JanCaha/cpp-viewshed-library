#include "losevaluator.h"
#include "iviewshed.h"

using viewshed::IViewshedAlgorithm;
using viewshed::LoSEvaluator;

LoSEvaluator::LoSEvaluator( std::shared_ptr<std::vector<LoSNode>> los,
                            std::vector<std::shared_ptr<IViewshedAlgorithm>> algs )
    : mLos( los ), mAlgs( algs )
{
}

int LoSEvaluator::size() { return mAlgs.size(); }

std::shared_ptr<IViewshedAlgorithm> LoSEvaluator::algorithmAt( int i ) { return mAlgs.at( i ); }

void LoSEvaluator::parseNodes( std::shared_ptr<LoSNode> poi )
{
    LoSNode ln;
    LoSNode lnNext;
    double snGradient;

    for ( int i = 0; i < mLos->size(); i++ )
    {
        ln = mLos->at( i );

        if ( ln == *poi.get() )
        {
            mLosValues->mIndexPoi = i;
        }

        snGradient = ln.valueAtAngle( poi->centreAngle(), ValueType::Gradient );

        if ( i + 1 < mLos->size() )
        {
            lnNext = mLos->at( i + 1 );

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

void LoSEvaluator::calculate( std::shared_ptr<LoSNode> poi, std::shared_ptr<IPoint> point )
{

    if ( mAlreadyParsed )
        reset();

    parseNodes( poi );

    mResultValues = ViewshedValues( poi->row, poi->col );

    for ( int i = 0; i < mAlgs.size(); i++ )
    {
        mResultValues.values.push_back( mAlgs.at( i )->result( mLosValues, mLos, poi, point ) );
    }
}

void LoSEvaluator::reset()
{
    mResultValues = ViewshedValues();
    mLosValues->reset();
}

double LoSEvaluator::resultAt( int i ) { return mResultValues.values[i]; }

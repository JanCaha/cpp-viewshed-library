#include "losevaluator.h"
#include "iviewshed.h"
#include "visibility.h"

using viewshed::IViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::Visibility;

LoSEvaluator::LoSEvaluator( std::shared_ptr<LoS> los,
                            std::shared_ptr<std::vector<std::shared_ptr<IViewshedAlgorithm>>> algs )
    : mLos( los ), mAlgs( algs )
{
    std::sort( mLos->begin(), mLos->end() );
}

int LoSEvaluator::size() { return mAlgs->size(); }

std::shared_ptr<IViewshedAlgorithm> LoSEvaluator::algorithmAt( int i ) { return mAlgs->at( i ); }

void LoSEvaluator::parseNodes()
{

    double snGradient;

    for ( int i = 0; i < mLos->size(); i++ )
    {
        mLosValues->mIndexPoi = mLos->targetPointIndex();

        snGradient = mLos->gradient( i );

        if ( i + 1 < mLos->size() )
        {
            if ( mLos->distance( i + 1 ) <= mLos->targetDistance() )
            {
                if ( mLosValues->mMaxGradientBefore < snGradient && mLos->gradient( i + 1 ) < snGradient )
                {
                    mLosValues->mIndexHorizonBefore = i;
                    mLosValues->mCountHorizonBefore++;
                }
            }

            if ( mLosValues->mMaxGradient < snGradient && mLos->gradient( i + 1 ) < snGradient )
            {
                mLosValues->mIndexHorizon = i;
                mLosValues->mCountHorizon++;
            }
        }

        if ( mLos->distance( i ) < mLos->targetDistance() )
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

void LoSEvaluator::calculate()
{

    if ( mAlreadyParsed )
        reset();

    parseNodes();

    mResultValues = ViewshedValues( mLos->targetRow(), mLos->targetCol() );

    for ( int i = 0; i < mAlgs->size(); i++ )
    {
        mResultValues.values.push_back( mAlgs->at( i )->result( mLosValues, mLos ) );
    }
}

void LoSEvaluator::reset()
{
    mResultValues = ViewshedValues();
    mLosValues->reset();
}

double LoSEvaluator::resultAt( int i ) { return mResultValues.values[i]; }
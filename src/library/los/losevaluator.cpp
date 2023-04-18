#include "losevaluator.h"
#include "abstractviewshed.h"
#include "visibility.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;
using viewshed::Visibility;

LoSEvaluator::LoSEvaluator( std::shared_ptr<AbstractLoS> los,
                            std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> algs )
    : mLos( los ), mAlgs( algs )
{
}

int LoSEvaluator::size() { return mAlgs->size(); }

std::shared_ptr<AbstractViewshedAlgorithm> LoSEvaluator::algorithmAt( int i ) { return mAlgs->at( i ); }

void LoSEvaluator::parseNodes()
{

    mLosValues->targetIndex = mLos->targetIndex();

    double snGradient;

    for ( int i = 0; i < mLos->numberOfNodes(); i++ )
    {
        snGradient = mLos->gradient( i );

        if ( i + 1 < mLos->numberOfNodes() )
        {
            // is current PoI horizon?
            if ( i == mLos->targetPointIndex() )
            {
                if ( mLos->gradient( i + 1 ) < snGradient && mLosValues->maxGradientBefore < snGradient )
                {
                    mLosValues->targetHorizon = true;
                }
            }

            // is LoSNode horizon before PoI?
            if ( mLos->distance( i + 1 ) <= mLos->targetDistance() )
            {
                if ( mLosValues->maxGradientBefore < snGradient && mLos->gradient( i + 1 ) < snGradient )
                {
                    mLosValues->indexHorizonBefore = i;
                    mLosValues->countHorizonBefore++;
                }
            }

            // is this LoSNode horizon?
            if ( mLosValues->maxGradient < snGradient && mLos->gradient( i + 1 ) < snGradient )
            {
                mLosValues->indexHorizon = i;
                mLosValues->countHorizon++;
            }
        }

        if ( mLos->distance( i ) < mLos->targetDistance() )
        {
            if ( mLosValues->maxGradientBefore < snGradient )
            {
                mLosValues->maxGradientBefore = snGradient;
                mLosValues->indexMaxGradientBefore = i;
            }
        }

        if ( mLosValues->maxGradient < snGradient )
        {
            mLosValues->maxGradient = snGradient;
            mLosValues->indexMaxGradient = i;
        }
    }

    // if max gradient is behind global horizon, move global horizon
    if ( mLosValues->indexHorizon < mLosValues->indexMaxGradient )
    {
        mLosValues->indexHorizon = mLosValues->indexMaxGradient;
    }

    mAlreadyParsed = true;
}

void LoSEvaluator::calculate()
{

    if ( mAlreadyParsed )
        reset();

    mLos->prepareForCalculation();

    parseNodes();

    mResultValues = ViewshedValues( mLos->resultRow(), mLos->resultCol() );

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

ViewshedValues LoSEvaluator::results() { return mResultValues; }

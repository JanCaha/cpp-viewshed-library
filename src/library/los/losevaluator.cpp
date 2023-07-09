#include "losevaluator.h"
#include "abstractviewshed.h"
#include "abstractviewshedalgorithm.h"
#include "losnode.h"
#include "point.h"
#include "viewshedvalues.h"
#include "visibility.h"

using viewshed::AbstractViewshedAlgorithm;
using viewshed::LoSEvaluator;
using viewshed::ViewshedValues;
using viewshed::Visibility;

LoSEvaluator::LoSEvaluator( std::shared_ptr<AbstractLoS> los,
                            std::shared_ptr<std::vector<std::shared_ptr<AbstractViewshedAlgorithm>>> visibilityIndices )
    : mLos( los ), mVisibilityIndices( visibilityIndices )
{
}

int LoSEvaluator::size() { return mVisibilityIndices->size(); }

std::shared_ptr<AbstractViewshedAlgorithm> LoSEvaluator::algorithmAt( int i ) { return mVisibilityIndices->at( i ); }

void LoSEvaluator::parseNodes()
{

    mLosValues->targetIndex = mLos->targetIndex();

    double snGradient;
    double nextGradient;

    for ( std::size_t i = 0; i < mLos->numberOfNodes(); i++ )
    {
        mLos->setCurrentLoSNode( i );

        snGradient = mLos->currentGradient();

        if ( i + 1 < mLos->numberOfNodes() )
        {

            nextGradient = mLos->gradient( i + 1 );

            // is current PoI horizon?
            if ( i == mLos->targetPointIndex() )
            {
                if ( nextGradient < snGradient && mLosValues->maxGradientBefore < snGradient )
                {
                    mLosValues->targetHorizon = true;
                }
            }

            // is LoSNode horizon before PoI?
            if ( mLos->distance( i + 1 ) <= mLos->targetDistance() )
            {
                if ( mLosValues->maxGradientBefore < snGradient && nextGradient < snGradient )
                {
                    mLosValues->indexHorizonBefore = i;
                    mLosValues->countHorizonBefore++;
                }
            }

            // is this LoSNode horizon?
            if ( mLosValues->maxGradient < snGradient && nextGradient < snGradient )
            {
                mLosValues->indexHorizon = i;
                mLosValues->countHorizon++;
            }
        }

        if ( mLos->currentDistance() < mLos->targetDistance() )
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

    // if last point on LoS is visible and higher then global horizon, then move global horizon to this point
    if ( mLos->gradient( mLos->numberOfNodes() - 1 ) > mLos->gradient( mLosValues->indexHorizon ) &&
         mLos->gradient( mLos->numberOfNodes() - 1 ) > mLosValues->maxGradient )
    {
        mLosValues->indexHorizon = mLos->numberOfNodes() - 1;
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

    for ( int i = 0; i < mVisibilityIndices->size(); i++ )
    {
        mResultValues.values.push_back( mVisibilityIndices->at( i )->result( mLosValues, mLos ) );
    }
}

void LoSEvaluator::reset()
{
    mResultValues = ViewshedValues();
    mLosValues->reset();
}

double LoSEvaluator::resultAt( int i ) { return mResultValues.values[i]; }

ViewshedValues LoSEvaluator::results() { return mResultValues; }

/* -*- Mode: C++; -*- */
/* VER: $Id: Distribution.h,v 1.3 2006/11/06 15:48:53 cdimitrakakis Exp cdimitrakakis $*/
// copyright (c) 2006 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef MAKE_MAIN
#include "PolicyEvaluation.h"
#include "BetaDistribution.h"



/// A toy UCT stopping problem
class BeliefExpansionAlgorithm 
{
public:
    std::vector<ConjugatePrior> prior; /// prior for all actions
    uint n_actions;
    BeliefExpansionAlgorithm(std::vector<ConjugatePrior> prior_)
	: prior(prior_)
    {
	n_actions = prior.size();
    }
    virtual ~BeliefExpansionAlgorithm
    {
    }
    void Observe(int action, real reward)
    {
	assert (action>= 0 && action < n_actions);
	real x = (real) (((int) reward) * 2 - 1);
	prior[action].calculatePosterior(x);
    }
};

/// A toy UCT stopping problem
class UCTBeliefExpansion : BeliefExpansionAlgorithm
{
public:
    BeliefExpansionAlgorithm()

    virtual ~UCTBeliefExpansion
    {
    }
    int Act()
    {
	
    }
    void Observe(reward);
};



int main (int argc, char** argv)
{
    real alpha = 0;
    real beta = 0;
    ConjugatePrior* prior = new BetaDistribution(alpha, beta);

    real mean_r = urandom(-1, 1);
    
    EvaluateAlgorithm(algorithm, mean_r);
    
    return 0;
}

void EvaluateAlgorithm(UCTAlgorithm* algorithm, real mean_r)
{
    algorith
}

#endif

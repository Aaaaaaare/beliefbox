// -*- Mode: c++ -*-
// copyright (c) 2009 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
// $Revision$
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ContextBandit.h"


ContextBandit::ContextBandit(uint n_actions_,
                             uint n_features,
                             uint values_per_feature,
                             RandomNumberGenerator* rng): n_actions(n_actions_)
{ 
    printf ("Making bandit!\n");
    this->rng = rng;
    // this bandit is binary
    n_states = (int) round(pow(values_per_feature, n_features));
    NormalDistribution* positive_reward = new NormalDistribution(1.0, 1.0);
    NormalDistribution* negative_reward = new NormalDistribution(-1.0, 1.0);
    rewards.push_back(positive_reward);
    rewards.push_back(negative_reward);

    mdp = new DiscreteMDP (n_states, n_actions, NULL);

    // setup rewards
    for (uint s=0; s<n_states; ++s) {
        for (uint a=0; a<n_actions; a++) {
            state = s;
            if (urandom() < 0.5) {
                mdp->setRewardDistribution(s, a, positive_reward);
            } else {
                mdp->setRewardDistribution(s, a, negative_reward);
            }
        }
    }

    // set up transitions
    real Pr = 1.0 / (real) n_states;
    for (uint s=0; s<n_states; s++) {
        for (uint a=0; a<n_actions; a++) {
            for (uint s2=0; s2<n_states; s2++) {
                //printf ("%d %d %d / %d", s, a, s2, n_states);
                mdp->setTransitionProbability(s, a, s2, Pr);
            }
        }
    }
    mdp->Check();
}


/// put the environment in its natural state
void ContextBandit::Reset()
{
    state = (int) rng->discrete_uniform(n_states);
    reward = 0;
}


/// returns true if the action succeeds, false if we are in a terminal state
bool ContextBandit::Act(int action)
{
    reward = mdp->Act(action);

    return true;  // we continue
}

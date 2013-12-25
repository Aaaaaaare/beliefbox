// -*- Mode: c++ -*-
// copyright (c) 2013 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DIRICHLET_TRANSITION_H
#define DIRICHLET_TRANSITION_H

#include "TransitionDistribution.h"
#include "Dirichlet.h"
#include "DirichletFiniteOutcomes.h"

/** Discrete transition distribution that is Dirichlet

	Here the prior mass is distributed uniformly over the state space.
	
	By default, an unvisited state-action pair has a uniform distribution state. This behaviour may not be ideal.
 */
class DirichletTransitions 
{
public:
	int n_states; ///< number of states
	int n_actions; ///< number of actions
	real prior_mass; ///< total prior mass to place
	bool uniform_unknown; ///< whether to use a uniform distribution for unknown states
	/// The set of Dirichlet distributions
	std::unordered_map<DiscreteStateAction, DirichletDistribution> P;
	//std::unordered_map<DiscreteStateAction, DirichletFiniteOutcomes> P;
	DirichletTransitions(int n_states_, int n_actions_, real prior_mass_ = 1, bool uniform_unknown_ = true)
		: n_states(n_states_),
		  n_actions(n_actions_),
		  prior_mass(prior_mass_),
		  uniform_unknown(uniform_unknown_)
	{
	}
	virtual ~DirichletTransitions() {}

	/// Set a state transition
	virtual real Observe(int state, int action, int next_state);

	/// Generate a next state
	virtual int marginal_generate(int state, int action) const;

	/// Get the marginal over next states.
	virtual Vector getMarginal(int state, int action) const;

	/// Get the parameters over next states.
	virtual Vector getParameters(int state, int action) const;

	/// Generate a multinomial distribution
	virtual Vector generate(int state, int action) const;
	/// Get the marginal probability of the next state
	virtual real marginal_pdf(int state, int action, int next_state) const;

	int getCounts(int state, int action) const
	{
		DiscreteStateAction SA(state, action);
		auto got = P.find(SA);
		if (got == P.end()) {
			return 0;
		} else {
			return got->second.getCounts();
		}
	}

};

typedef TransitionDistribution<int, int> DiscreteTransitionDistribution;
#endif
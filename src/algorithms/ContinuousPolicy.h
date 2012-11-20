// -*- Mode: c++ -*-
// copyright (c) 2012 by Nikolaos Tziortziotis <ntziorzi@gmail.com>
// $Revision$
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef CONTINUES_POLICY_H
#define CONTINUES_POLICY_H

#include "MultinomialDistribution.h"
#include "AbstractPolicy.h"
#include "BasisSet.h"
#include "Matrix.h"
#include "Vector.h"

/** A policy in a continuous state space.

	Abstract class.
*/
class ContinuousPolicy: public AbstractPolicy<Vector,int>
{
protected:
	int n_dimension;
	int n_actions;
	RBFBasisSet* bfs;
	Vector weights;
public:
	ContinuousPolicy( int n_dimension_, int n_actions_, RBFBasisSet* bfs_)
		: n_dimension(n_dimension_),n_actions(n_actions_),bfs(bfs_)
	{}
	ContinuousPolicy( int n_dimension_, int n_actions_, RBFBasisSet* bfs_, Vector& weights_)
		: n_dimension(n_dimension_),n_actions(n_actions_),bfs(bfs_),weights(weights_)
	{}
	virtual ~ContinuousPolicy() {}
	virtual int SelectAction() = 0;
	virtual int SelectAction(Vector next_state) = 0;
	virtual void Observe(Vector& previous_state, int& action, real r, Vector& next_state) = 0;
	virtual void Observe(real r, Vector& next_state) = 0;
	virtual void Reset() = 0;
	virtual void Reset(const Vector start_state) = 0;
	virtual real getActionProbability(int& action) const = 0;
	virtual real getActionProbability(Vector state, int& action) = 0;
	virtual void Show() = 0;
	virtual void Update(Vector weights_) = 0;
};

class FixedContinuousPolicy : public ContinuousPolicy
{
public:
	Vector p;
	Vector state;
	FixedContinuousPolicy(int n_dimension_, int n_actions_, RBFBasisSet* bfs_);
	FixedContinuousPolicy(int n_dimension_, int n_actions_, RBFBasisSet* bfs_, Vector& weights_);
	virtual ~FixedContinuousPolicy();
	virtual int SelectAction();
	virtual int SelectAction(Vector next_state);
	virtual void Observe(Vector& previous_state, int& action, real r, Vector& next_state);
	virtual void Observe(real r, Vector& next_state);
	virtual void Reset()
	{
		Vector reset_state(n_dimension);
		Reset(reset_state);
	}
	virtual void Reset(const Vector start_state);
	virtual real getActionProbability(int& action) const;
	virtual real getActionProbability(Vector state, int& action);
	virtual void Show();
	virtual void Update(Vector weights_)
	{
		assert(weights_.Size()==weights.Size());
		weights = weights_;
	}
	virtual Vector getWeights()
	{
		return weights;
	}
	inline void StatePolicy();
};

#endif
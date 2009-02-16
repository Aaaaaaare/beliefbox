/* -*- Mode: c++;  -*- */
// copyright (c) 2007 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef SPARSE_MARKOVCHAIN_H
#define SPARSE_MARKOVCHAIN_H

#include "MarkovChain.h"
#include <map>
#include <vector>

/**
   \ingroup StatisticsGroup
 */
/*@{*/



typedef std::map<int, std::vector<float>, std::greater<int> > SourceMap;
typedef SourceMap::iterator SourceMapIterator;

class SparseTransitions 
{
protected:
	int n_sources;
	int n_destinations;
	std::map<int, std::vector<float>, std::greater<int> > sources;
public:
	SparseTransitions(int n_sources, int n_destinations)
	{
		this->n_sources = n_sources;
		this->n_destinations = n_destinations;
	}
	float get_weight(int src, int dst)
	{
		SourceMapIterator i = sources.find(src);
		if (i==sources.end()) {
			return 0.0;
		} else {
			return i->destinations??
		}
	}
	int nof_destinations()
	{
		return n_destinations;
	}

	float AddTransition(int src, int dst)
	{
		sources.find(src);
	}
};

/// A sparse implementation of a Markov chain
class SparseMarkovChain : public MarkovChain
{
protected:
    int CalculateStateID ();
    int n_transitions; ///< total number of transitions
	SparseTransitions transitions; ///< history-wide transition table

    float threshold;
public:
    SparseMarkovChain (int n_states, int mem_size);
    virtual ~SparseMarkovChain ();

    /* probabilities */
    virtual float getTransition (int src, int dst);
    virtual float getProbability (int src, int dst);
    virtual float pdf(int src, Vector q);
    virtual void setTransition (int src, int dst, float value) ;
    virtual void setThreshold (float threshold);


    /* Training and generation */
    virtual int PushState (int state);
    virtual float ObserveNextState (int state);
    virtual float NextStateProbability (int state);
    virtual void Reset ();
    virtual int GenerateStatic ();
    virtual int generate ();

    /* Debug functions */
    virtual int ShowTransitions ();
};
/*@}*/
#endif
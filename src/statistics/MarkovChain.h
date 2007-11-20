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
#ifndef MARKOVCHAIN_H
#define MARKOVCHAIN_H

#include "Vector.h"
#include <vector>


/**
   \ingroup StatisticsGroup
 */
/*@{*/

/// A Markov Chain
class MarkovChain {
protected:
	int n_states; ///< number of distinct states
	int mem_size; ///< history size for new transitions
	int curr_state; ///< current address in history
	int tot_states; ///< total number of representable states (mem_size*n_states)
	std::vector<int> memory; ///< hold history in here
public:
    MarkovChain (int n_states, int mem_size);
    virtual ~MarkovChain ();

    /* probabilities */
    virtual float getTransition (int src, int dst) = 0;
    virtual float getProbability (int src, int dst) = 0;
    virtual float pdf(int src, Vector q) = 0;
    virtual void setTransition (int src, int dst, float value) = 0;
    virtual void setThreshold (float threshold) = 0;

    /* Training and generation */
    virtual int PushState (int state) = 0;
    virtual float ObserveNextState (int state) = 0;
    virtual float NextStateProbability (int state) = 0;
    virtual void Reset () = 0;
    virtual int GenerateStatic () = 0;
    virtual int generate () = 0;

    /* misc */
    int getTotalStates() {return tot_states;}
    /* Debug functions */
    virtual int ShowTransitions () = 0;
    void ShowMemory();
};




/*@}*/
#endif

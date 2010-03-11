/* -*- Mode: c++;  -*- */
// copyright (c) 2010 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONTEXT_TREE_RL_H
#define CONTEXT_TREE_RL_H

#include <vector>
#include "real.h"
#include "Vector.h"
#include "Ring.h"
#include "BetaDistribution.h"


/** A context tree implementation.
    
    Uses rewards.
*/
class ContextTreeRL
{
public:
    // public classes
    struct Node
    {
        int n_branches;
        int n_outcomes;
        int depth; ///< depth
        Node* prev; ///< previous node
        std::vector<Node*> next; ///< pointers to next nodes
        Vector P; ///< probability of next symbols
        Vector alpha; ///< parameters of next symbols
        const real prior_alpha; ///< implicit prior value of alpha
        real w; ///< backoff weight
        real log_w; ///< log of w
        real log_w_prior; ///< initial value
        BetaDistribution reward_prior;
        Node(int n_branches_,
             int n_outcomes_);
        Node(Node* prev_);
        ~Node();
        real Q;
        real Observe(Ring<int>& history,
                     Ring<int>::iterator x,
                     int y,
                     real r,
                     real probability);
        void Show();
        int NChildren();    
    };
    
    // public methods
    ContextTreeRL(int n_branches_, int n_symbols_, int max_depth_= 0);
    ~ContextTreeRL();
    real Observe(int x, int y, real r);
    void Show();
    int NChildren();
    void QLearning(real step_size, int action, int observation, real reward);
protected: 
    int n_branches;
    int n_symbols;
    int max_depth;
    Node* root;
    Ring<int> history;
};



#endif
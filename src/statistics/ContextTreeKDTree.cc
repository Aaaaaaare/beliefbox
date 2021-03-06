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

#include "ContextTreeKDTree.h"
#include "BetaDistribution.h"
#include <cmath>
#include  "Random.h"

#undef LOG_CALCULATIONS

/** Create the root node of the tree */
ContextTreeKDTree::Node::Node(ContextTreeKDTree& tree_,
							  const Vector& lower_bound_,
							  const Vector& upper_bound_)
    : tree(tree_),
#ifdef USE_GAUSSIAN_MIX
      gaussian((upper_bound_ + lower_bound_) * 0.5,
               1.0, 1.0,
               Matrix::Unity(upper_bound_.Size(), upper_bound_.Size())),
      //gaussian(lower_bound_, upper_bound_),
      w_gaussian(0.5),
#endif
      //beta_product(lower_bound_, upper_bound_),
	  lower_bound(lower_bound_),
      upper_bound(upper_bound_),
      volume(Volume(upper_bound - lower_bound)),
      depth(0),
      prev(NULL),
      next(tree.n_branches),
      alpha(tree.n_branches),
      w(0.5), log_w(0), log_w_prior(-log(2)),
      S(0)
{
    assert(lower_bound < upper_bound);
	splitting_dimension = ArgMax(upper_bound - lower_bound);
#ifdef RANDOM_SPLITS
	real phi = 0.1 + 0.8 * urandom();
	mid_point = phi * upper_bound[splitting_dimension] + 
		(1 - phi) * lower_bound[splitting_dimension];
#else
	mid_point = (upper_bound[splitting_dimension] + lower_bound[splitting_dimension]) / 2.0;
#endif
}

/** Make a new leaf node.
    
    In order to avoid overfitting, the weights are fixed.
    That way, the contribution of the leaf nodes is small.
 */
ContextTreeKDTree::Node::Node(ContextTreeKDTree::Node* prev_,
							  const Vector& lower_bound_,
							  const Vector& upper_bound_)
    : tree(prev_->tree), 
#ifdef USE_GAUSSIAN_MIX
      gaussian((upper_bound_ + lower_bound_) * 0.5,
               1.0, 1.0,
               prev_->gaussian.T_n),
      //Matrix::Unity(upper_bound_.Size(), upper_bound_.Size())),
      //gaussian(lower_bound_, upper_bound_),
      w_gaussian(0.5),
#endif
      //beta_product(lower_bound_, upper_bound_),
	  lower_bound(lower_bound_),
      upper_bound(upper_bound_),
      volume(Volume(upper_bound - lower_bound)),
      depth(prev_->depth + 1),
      prev(prev_),
      next(tree.n_branches),
      alpha(tree.n_branches),
      log_w(0),
      //log_w_prior(-(real) depth * log(2)),
      log_w_prior(-log(2)),
      S(0)
{
    assert(lower_bound < upper_bound);
	splitting_dimension = ArgMax(upper_bound - lower_bound);    
#ifdef RANDOM_SPLITS
	real phi = 0.1 + 0.8 * urandom();
	mid_point = phi * upper_bound[splitting_dimension] + 
		(1 - phi) * lower_bound[splitting_dimension];
#else
	mid_point = (upper_bound[splitting_dimension] + lower_bound[splitting_dimension]) / 2.0;
#endif

    w = exp(log_w_prior);
    for (int i=0; i<tree.n_branches; ++i) {
        next[i] = NULL;
    }
    
}

/// make sure to kill all
ContextTreeKDTree::Node::~Node()
{
    for (int i=0; i<tree.n_branches; ++i) {
        delete next[i];
    }
}

/** Observe new data, adapt parameters.
    
    Each node corresponds to an interval C.
    The probability of \f$x\f$ is
    \f[
    P(X | C_k) = U(C_k) w_k + (1 - w_k) [a_{k,0} / n_k P(X | C_{k,0}) + a_{k,1} / n_k P(X | C_{k,1})]
    \f]
    of course, if \f$X \in C_k^i\f$ then
    \f[
    P(X | C_k) = U(C_k) w_k + (1 - w_k) [a_{k,i} / n_k P(X | C_{k,i}]
    \f]

    Or in long hand
    \f[
    P(X | C) = P(U | C) P(X | U, C) + [1 - P(U | C)] \sum_{C'} P(X | C') P(C' | C, \not U)
    \f]
    That means that
    \f[
    P(U | X, C) = P(X | U, C) P(U | C) / P(X | C).
    \f]
    
    This algorithm uses the same structure of uniform / non-uniform mixture
    as in Marcus Hutter's work Bayesian Infinite Trees.
    
    However, it is significantly simplified by using an explicit
    online posterior recursion.
*/
real ContextTreeKDTree::Node::Observe(const Vector& x,
									  real probability)
{
    // Which interval is the x lying at
    int k;
    if ( x[splitting_dimension] < mid_point) {
        k = 0;
    } else {
        k = 1;
    }
    
    // Calculate the local distribution
    real P_uniform = 1.0 / volume; //Volume (upper_bound - lower_bound);
    //P_uniform *= beta_product.Observe(x);
#ifdef USE_GAUSSIAN_MIX
    real P_gaussian = gaussian.Observe(x);
    assert(P_gaussian >= 0);
    real P_local = w_gaussian * P_gaussian + (1 - w_gaussian) * P_uniform;
    assert(P_local >= 0);
    w_gaussian *= P_gaussian / P_local;
#else
    real P_local = P_uniform;
#endif
	//printf ("P_u = %f\n", P_uniform);
    // probability of recursion
    // P is the probability in the remainder of the chain.
    P =  (1.0 + alpha[k]) / (2.0 + S);

    // adapt parameters
    alpha[k] ++;
    S++;

    //real threshold = 1; //log(depth);
    real threshold = pow(1.1, (real) depth); 
    if ((tree.max_depth==0 || depth < tree.max_depth) && S >  threshold) {
        if (!next[k]) {
            if (k == 0) {
				Vector new_bound = upper_bound;
				new_bound(splitting_dimension) = mid_point;
                next[k] = new Node(this, lower_bound, new_bound);
            } else {
				Vector new_bound = lower_bound;
				new_bound(splitting_dimension) = mid_point;
                next[k] = new Node(this, new_bound, upper_bound);
            }
        }
        P *= next[k]->Observe(x, P);
    } else {
        // There are four options when there are no further
        // children. 

        // The first is to assume a uniform distribution on the k-th
        // interval. Since there are two intervals, multiple by two.
        //P *= 2 * P_local;

        // The second is to just stop.
         P  = P_local; 

        // The third is to use the interval probability
        //P = P;
        
        // The final is to set the next probability to zero.
        //P  = 0;
    }

#ifdef LOG_CALCULATIONS
    w = exp(log_w_prior + log_w); 
#endif

    real total_probability = P_local * w + (1 - w) * P;

    // posterior weight
#ifdef LOG_CALCULATIONS
    log_w = log(w * P_local / total_probability) - log_w_prior;
#else
    w *= P_local / total_probability;
#endif
    assert (w >= 0 && w <= 1);
    
#if 0
    std::cout << depth << ": P(y|h_k)=" << P
              << ", P(h_k|B_k)=" << w 
              << ", P(y|B_{k-1})="<< probability
              << ", P(y|B_k)=" << total_probability
              << std::endl;
#endif

    return total_probability;
}

/** Observe new data, adapt parameters.

    We again ignore the probability given by the previous model,
    since this is a much simpler case.

    We only care about the fact that the previous model posits a
    uniform distribution for the interval of the current model, while
    the current model posits a mixture of two uniform distributions.

*/
real ContextTreeKDTree::Node::pdf(const Vector& x,
                                  real probability)
{
    int k;
    if ( x[splitting_dimension] < mid_point) {
        k = 0;
    } else {
        k = 1;
    }
    real P_uniform = 1.0 / volume; //Volume(upper_bound - lower_bound);

#ifdef USE_GAUSSIAN_MIX
    real P_gaussian = gaussian.pdf(x);
    real P_local = w_gaussian * P_gaussian + (1 - w_gaussian) * P_uniform;
#else
    real P_local = P_uniform;
#endif

    P =  (1.0 + alpha[k]) / (2.0 + S);

    //real threshold = 1;
    real threshold = pow(1.1, (real) depth); 
    if (S >  threshold && next[k]) {
        P *= next[k]->pdf(x, P);
    } else {
        //     P *= 2 * P_local;
        P = P_local;
    }

#ifdef LOG_CALCULATIONS
    w = exp(log_w_prior + log_w); 
#endif
    real total_probability = P_local * w + (1 - w) * P;
	return total_probability;
}


void ContextTreeKDTree::Node::Show()
{
    printf ("%d %f %f\n", depth, w, volume);
    for (int k=0; k<tree.n_branches; ++k) {
        if (next[k]) {
            next[k]->Show();
        }
    }
	return;
}

int ContextTreeKDTree::Node::NChildren()
{
    int my_children = 0;
    for (int k=0; k<tree.n_branches; ++k) {
        if (next[k]) {
            my_children++;
            my_children += next[k]->NChildren();
        }
    }
    return my_children;
}


/// n_branches is a bit of a silly thing, deprecated
ContextTreeKDTree::ContextTreeKDTree(int n_branches_,
                                         int max_depth_,
									 const Vector& lower_bound,
									 const Vector& upper_bound)
    : n_branches(n_branches_),
      max_depth(max_depth_)
{
    root = new Node(*this, lower_bound, upper_bound);
}

ContextTreeKDTree::~ContextTreeKDTree()
{
    delete root;
}

real ContextTreeKDTree::Observe(const Vector& x)
{
    return root->Observe(x, 1);
}


real ContextTreeKDTree::pdf(const Vector& x)
{
    return root->pdf(x, 1);
}

void ContextTreeKDTree::Show()
{
    root->Show();
    std::cout << "Total contexts: " << NChildren() << std::endl;
}

int ContextTreeKDTree::NChildren()
{
    return root->NChildren();
}

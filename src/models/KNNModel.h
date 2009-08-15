/* -*- Mode: C++; -*- */
// copyright (c) 2009 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef KNN_MODEL_H
#define KNN_MODEL_H


class TrajectorySample
{
    Vector s;
    int a;
    real r;
    Vector s2;
    TrajectorySample(Vector s_, int a_, real r_, Vector s2_)
        : s(s_), a(a_), r(r_), s2(s2_)
    {
    }
        
    
};

class KNNModel
{
protected:
    int n_actions;
    int n_dim;
    std::vector<KDTree<TrajectorySample> > kd_tree_list;
    //RBFBasisSet basis;
    std::list<TrajectorySample> pairs;
public:	
    KNNModel(int n_actions, int n_dim);
    void AddElement(TrajectorySample p);
    void Evaluate(Vector&x, Vector& y, int K);
};




#endif

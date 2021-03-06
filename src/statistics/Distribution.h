/* -*- Mode: C++; -*- */
// copyright (c) 2004-2009 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DISTRIBUTION_H
#define DISTRIBUTION_H

#include "real.h"
#include <cmath>
#include <iostream>
#include "Object.h"
#include "Vector.h"
#include "MathFunctions.h"
#include "RandomNumberGenerator.h"

/**
   \defgroup StatisticsGroup Statistics and probability
*/
/*@{*/
template <typename T>
class AbstractDistribution : public Object {
public:
	AbstractDistribution()  {}
    virtual ~AbstractDistribution() {};
    virtual void generate(T& x) const = 0;
    virtual T generate() const = 0;
    virtual real pdf(const T& x) const =0;
    virtual real log_pdf(const T& x) const
    {
        return log(pdf(x));
    }
};

/// Vector distribtuions
typedef AbstractDistribution<Vector> VectorDistribution;

/// Probability distribution
class Distribution : public Object {
public:
    Distribution() {}
    virtual ~Distribution() {}
    virtual real generate() const = 0; ///< generate a value from this distribution
    virtual real pdf(real x) const = 0; ///< return the density at point x
    virtual real log_pdf(real x) const
    {
        return log(pdf(x));
    }
    virtual real log_pdf(const std::vector<real> & x) const;
    virtual real log_pdf(const std::vector<real> & x);
    virtual Distribution* clone() { 
		Serror ("Not implemented\n");
		return NULL; /*  does nothing */}
    virtual real getMean() const 
	{
		Serror("Not implemented\n");
		return 0;
	}
    virtual void setMean (real mean) ///< set the mean
    {
        Serror ("Cannot set mean\n");
    }
};

/// \brief Parametric distribution.
/// For distribution simply defined via moments-related parameters.
class ParametricDistribution : public Distribution
{
public:
    ParametricDistribution() {}
    virtual ~ParametricDistribution() {}
    virtual void setVariance (real var) ///< set the variance
    {
        Serror ("Cannot set variance\n");
    }
};


/// A Conjugate prior is special in the sense that a posterior can be calculated analytically.
class ConjugatePrior : public ParametricDistribution
{
public:
    ConjugatePrior() {}
    virtual ~ConjugatePrior() {}
    /// Calculate posterior from some observation x.
    virtual void calculatePosterior(real x) = 0; 
    /// Observe a value, calculate posterior, return prior marginal
    /// likelihood of x.
    virtual real Observe(real x) = 0; 
    /// get marginal_pdf
    virtual real marginal_pdf(real x) const  = 0;
};


class BernoulliDistribution : public ParametricDistribution
{
public:
    real p;
    BernoulliDistribution(){p=0.5f;}
    BernoulliDistribution(real p);
    virtual ~BernoulliDistribution() {}
    virtual void setMean(real mean) {p = mean;}
    virtual void setVariance(real var) {} ///< set variance in a magic way
    virtual real generate() const;
    virtual real pdf(real x) const;
    virtual real getMean() const {return p;}
};

/// Discrete probability distribution
class DiscreteDistribution : public Distribution {
public:
    int n_outcomes; ///< number of possible outcomes
    real* p; ///< probabilities of outcomes
    DiscreteDistribution();
    /// Make a discrete distribution with N outcomes
    DiscreteDistribution(int N); 
    virtual ~DiscreteDistribution();
    virtual real generate() const;
    virtual real pdf(real x) const;
    virtual Distribution* clone();
    virtual real getMean() const;
    static int generate(const std::vector<real>& x);
    static int generate(const Vector& x);
};



/// Uniform probability distribution
class UniformDistribution : public ParametricDistribution {
public:
    real min;
    real range;
    UniformDistribution() {min=0.0; range=1.0;}
    /// Create a uniform distribution with mean \c mean and standard deviation \c std
    UniformDistribution(real a, real b)
    {
        this->min = a;
        this->range = b-a;
    }
    virtual ~UniformDistribution() {}
    virtual real generate() const;
    virtual real pdf(real x) const;
    virtual void setVariance (real var) 
    {
        range = sqrt(12.0f * var);
    } 
    virtual void setMean (real mean)
    {
        min = mean - 0.5*range;
    }
    virtual real getMean () const
    {
        return (min + 0.5*range);
    }
};


/// Laplacian probability distribution
class LaplacianDistribution : public ParametricDistribution {
public:
    real l; ///< lambda
    real m; ///< mean
    LaplacianDistribution() {m=0.0;l=1.0;}
    /// Create a Laplacian distribution with parameter \c lambda
    LaplacianDistribution(real lambda)
    {
        m = 0.0;
        l = lambda;
    }
    LaplacianDistribution(real mean, real var)
    {
        setMean (mean);
        setVariance (var);
    }
    virtual ~LaplacianDistribution() {}
    virtual real generate() const;
    virtual real pdf(real x) const;
    virtual void setVariance (real var)
    {l = sqrt(0.5f / var);}
    virtual void setMean (real mean)
    {m = mean;}
    virtual real getMean () const
    {
        return m;
    }
};


/*@}*/

#endif

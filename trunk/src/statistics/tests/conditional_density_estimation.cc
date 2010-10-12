/* -*- Mode: C++; -*- */
// copyright (c) 2010 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef MAKE_MAIN
#include "ContextTreeKDTree.h"
#include "ConditionalKDContextTree.h"
#include "Random.h"
#include <vector>
#include "EasyClock.h"
#include "NormalDistribution.h"
#include "BetaDistribution.h"


int main (int argc, char** argv)
{
    if (argc != 7) {
        Serror ("Usage: density_estimation T D D_c Alpha Beta 0/1\n");
        exit(-1);
    }
    int T = atoi(argv[1]);
    if (T < 0) {
        Serror("T should be >= 0\n");
        exit(-1);
    }

    int max_depth = atoi(argv[2]);
    if (max_depth <= 0) {
        Serror("max_depth should be >= 0\n");
        exit(-1);
    }

    int max_depth_cond = atoi(argv[3]);
    if (max_depth <= 0) {
        Serror("max_depth should be >= 0\n");
        exit(-1);
    }

    real Alpha = atof(argv[4]);
    if (Alpha < 0) {
        Serror("Alpha should be >= 0\n");
        exit(-1);
    }

    real Beta = atof(argv[5]);
    if (Beta < 0) {
        Serror("Beta should be >= 0\n");
        exit(-1);
    }

    bool joint = atoi(argv[6]);
    printf ("Joint: %d\n", joint);

    //BetaDistribution distribution(Alpha,Beta);
    //BetaDistribution distribution2(2*Beta,Alpha);
    NormalDistribution distribution(0,Alpha);
    NormalDistribution distribution2(0,Beta);

	Vector lower_bound(2);
	Vector upper_bound(2);
	Vector lower_bound_x(1);
	Vector upper_bound_x(1);
	for (int i=0; i<2; ++i) {
		lower_bound(i) = -100;
		upper_bound(i) = 100;
	}
	lower_bound_x(0) = -100;
	upper_bound_x(0) = 100;
    ContextTreeKDTree pdf(2, max_depth, lower_bound, upper_bound);
    ConditionalKDContextTree cpdf(2, max_depth, max_depth_cond,
								  lower_bound_x, upper_bound_x,
								  lower_bound_x, upper_bound_x);
    //NormalUnknownMeanPrecision pdf;

    int randomise = urandom()*100;
    for (int i=0; i<randomise; i++) {
        distribution.generate();
    }

	Vector z(2);
	z(0) = 0;
	z(1) = 1;

    for (int t=0; t<T; ++t) {
		real a = distribution.generate();
		real b = distribution2.generate();

#if 0
          if (urandom() < 0.99) {
			z(0) = a + 0.1*b;
			z(1) = b - 0.1*a ;
		} else {
			z(0) = a + 2;
			z(1) = b + 3;
		}
#else
          z(0) = 5*sin(0.1*t) + a + 0.5*b;
          z(1) = 5*cos(0.1*t) + b  - 0.2*a;
#endif
          if (joint) {
              real p = pdf.Observe(z);
          } else {
              Vector x(1);
              Vector y(1);
              x(0) = z(0);
              y(0) = z(1);
              real p2 = cpdf.Observe(x, y);
          }
          //printf ("%f %f %f %f #Pr \n", z(0), z(1), p, p2);
    }
#if 1
	real step = 0.1;
    if (joint) {
        for (real y=-100; y<100; y+=step) {
            for (real x=-100; x<100; x+=step) {
                Vector v(2);
                v[0] = x;
                v[1] = y;
                printf ("%f ", pdf.pdf(v));// distribution.pdf(x)*distribution2.pdf(y));
            }
		printf(" # P_XY\n");
        }
    } else {
        for (real y=-100; y<100; y+=step) {
            for (real x=-100; x<100; x+=step) {
                Vector X(1);
                Vector Y(1);
                X(0) = x;
                Y(0) = y;
                printf (" %f ", cpdf.pdf(X, Y));// distribution.pdf(x)*distribution2.pdf(y));
            }
            printf(" # P_Y_X\n");
        }
    }
	printf ("PDF model\n");
    pdf.Show();
	printf ("CPDF model\n");
	cpdf.Show();

#endif
    return 0;
}

#endif
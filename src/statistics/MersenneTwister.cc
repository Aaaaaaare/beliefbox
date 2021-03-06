/* -*- Mode: C++; -*- */
// VER: $Id: Distribution.c,v 1.3 2006/11/06 15:48:53 cdimitrakakis Exp cdimitrakakis $
// copyright (c) 2004 by Christos Dimitrakakis <christos.dimitrakakis@gmail.com>
// Adapted from Torch, by Ronan Cololbert et al.
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "MersenneTwister.h"
#include <ctime>

// The initial seed.
unsigned long MersenneTwister::initial_seed;

///// Code for the Mersenne Twister random generator....
const int MersenneTwister::n = 624;
const int MersenneTwister::m = 397;
int MersenneTwister::left = 1;
int MersenneTwister::initf = 0;
unsigned long *MersenneTwister::next;
unsigned long MersenneTwister::state[MersenneTwister::n]; /* the array for the state vector  */
////////////////////////////////////////////////////////
void MersenneTwister::seed()
{
  time_t ltime;
  struct tm *today;
  time(&ltime);
  today = localtime(&ltime);
  manualSeed((unsigned long)today->tm_sec);
}

///////////// The next 4 methods are taken from http://www.math.keio.ac.jp/matumoto/emt.html
///////////// Here is the copyright:
/*
   A C-program for MT19937, with initialization improved 2002/2/10.
   Coded by Takuji Nishimura and Makoto Matsumoto.
   This is a faster version by taking Shawn Cokus's optimization,
   Matthe Bellew's simplification, Isaku Wada's real version.

   Before using, initialize the state by using init_genrand(seed)
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.keio.ac.jp/matumoto/emt.html
   email: matumoto@math.keio.ac.jp
*/

////////////////// Macros for the Mersenne Twister random generator...
/* Period parameters */  
//#define n 624
//#define m 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UMASK 0x80000000UL /* most significant w-r bits */
#define LMASK 0x7fffffffUL /* least significant r bits */
#define MIXBITS(u,v) ( ((u) & UMASK) | ((v) & LMASK) )
#define TWIST(u,v) ((MIXBITS(u,v) >> 1) ^ ((v)&1UL ? MATRIX_A : 0UL))
/////////////////////////////////////////////////////////// That's it.

void MersenneTwister::manualSeed(unsigned long the_seed_)
{
  initial_seed = the_seed_;
  state[0]= initial_seed & 0xffffffffUL;
  for(int j = 1; j < n; j++)
  {
    state[j] = (1812433253UL * (state[j-1] ^ (state[j-1] >> 30)) + j); 
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, mSBs of the seed affect   */
    /* only mSBs of the array state[].                        */
    /* 2002/01/09 modified by makoto matsumoto             */
    state[j] &= 0xffffffffUL;  /* for >32 bit machines */
  }
  left = 1;
  initf = 1;
}

unsigned long MersenneTwister::getInitialSeed()
{
  if(initf == 0)
  {
    seed();
  }

  return initial_seed;
}

void MersenneTwister::nextState()
{
  unsigned long *p=state;
  
  /* if init_genrand() has not been called, */
  /* a default initial seed is used         */
  if(initf == 0)
    seed();
//    manualSeed(5489UL);

  left = n;
  next = state;
    
  for(int j = n-m+1; --j; p++) 
    *p = p[m] ^ TWIST(p[0], p[1]);

  for(int j = m; --j; p++) 
    *p = p[m-n] ^ TWIST(p[0], p[1]);

  *p = p[m-n] ^ TWIST(p[0], state[0]);
}

unsigned long MersenneTwister::random()
{
  unsigned long y;

  if (--left == 0)
    nextState();
  y = *next++;
  
  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

/* generates a random number on [0,1)-real-interval */
real MersenneTwister::uniform()
{
  unsigned long y;

  if(--left == 0)
    nextState();
  y = *next++;

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);
  
  return (real)y * (1.0/4294967296.0); 
  /* divided by 2^32 */
}

MersenneTwisterRNG::MersenneTwisterRNG() : n(624), m(397)
{
    left = 1;
    initf = 0;
    state = new unsigned long [n];
}
MersenneTwisterRNG::~MersenneTwisterRNG()
{
    delete [] state;
}
void MersenneTwisterRNG::seed()
{
  time_t ltime;
  struct tm *today;
  time(&ltime);
  today = localtime(&ltime);
  manualSeed((unsigned long)today->tm_sec);
}

void MersenneTwisterRNG::manualSeed(unsigned long the_seed_)
{
  initial_seed = the_seed_;
  state[0]= initial_seed & 0xffffffffUL;
  for(int j = 1; j < n; j++)
  {
    state[j] = (1812433253UL * (state[j-1] ^ (state[j-1] >> 30)) + j); 
    /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
    /* In the previous versions, mSBs of the seed affect   */
    /* only mSBs of the array state[].                        */
    /* 2002/01/09 modified by makoto matsumoto             */
    state[j] &= 0xffffffffUL;  /* for >32 bit machines */
  }
  left = 1;
  initf = 1;
}

unsigned long MersenneTwisterRNG::getInitialSeed()
{
  if(initf == 0)
  {
    seed();
  }

  return initial_seed;
}

void MersenneTwisterRNG::nextState()
{
  unsigned long *p=state;
  
  /* if init_genrand() has not been called, */
  /* a default initial seed is used         */
  if(initf == 0)
    seed();
//    manualSeed(5489UL);

  left = n;
  next = state;
    
  for(int j = n-m+1; --j; p++) 
    *p = p[m] ^ TWIST(p[0], p[1]);

  for(int j = m; --j; p++) 
    *p = p[m-n] ^ TWIST(p[0], p[1]);

  *p = p[m-n] ^ TWIST(p[0], state[0]);
}

unsigned long MersenneTwisterRNG::random()
{
  unsigned long y;

  if (--left == 0)
    nextState();
  y = *next++;
  
  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);

  return y;
}

/* generates a random number on [0,1)-real-interval */
real MersenneTwisterRNG::uniform()
{
  unsigned long y;

  if(--left == 0)
    nextState();
  y = *next++;

  /* Tempering */
  y ^= (y >> 11);
  y ^= (y << 7) & 0x9d2c5680UL;
  y ^= (y << 15) & 0xefc60000UL;
  y ^= (y >> 18);
  
  return (real)y * (1.0/4294967296.0); 
  /* divided by 2^32 */
}

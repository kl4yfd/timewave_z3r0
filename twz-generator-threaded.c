//  twz-generator-threaded.c
// 
// Based on source code by the original author: Peter Meyer
//  Calculate the value of the timewave, using multiple threads

// Ported to Linux
// 4 Oct 2009
// John A Phelps
// kl4yfd@gmail.com


// Extended to multithreaded (1 thread per data set + 1 main thread)
// 08 Dec 2012
// John A Phelps
// kl4yfd@gmail.com

/*

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>

09 Dec 2012
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <stdbool.h>
#include <pthread.h>


#define FALSE 0
#define TRUE  1
#define NUM_POWERS 128
#define PREC 16 // long double (80 bit) numbers have about 16 significant digits  INTEL / AMD / x86_64
//#define PREC 32 // long double (128 bit) numbers have about 32 significant digits (QUAD PRECISION)
#define NUM_SETS 4
#define NUM_DATA_POINTS 384
#define CALC_PREC       1000000  //  precision in calculation of wave values



struct ThreadStruct 
{
  
  volatile bool _lock1;
  
  volatile bool _lock2;
  
  volatile bool _lock3;
  
  volatile bool _lock4;
  
  
  volatile long double x1;
  
  volatile long double x2;
  
  volatile long double x3;
  
  volatile long double x4;
  
  
  volatile long double ans1;
  
  volatile long double ans2;
  
  volatile long double ans3;
  
  volatile long double ans4;
  
};


/// Create a global lock structure to ease programming (4 static threads, so this is OK)
struct ThreadStruct lockstruct;


long double NegativeBailout = -2.0;



long double powers[NUM_POWERS];

//  Powers of (normally) 64.
//  Due to the limitations of double precision
//  floating point arithmetic these values are
//  exact only up to powers[8] for powers of 64.

int64_t wave_factor = 2;		//  default wave factor 
int64_t number_set, stringchar;


char *usage = "\nUsage: twz [dtz] [neg] [step] [wf]." 
"\n dtz = days to zero-point (12/21/2012)" 
"\n neg = days to calcualte into negative-time (past zero)" 
"\n step = steps in which to decrement time (in minutes)" 
"\n wf = wave factor (default 2, range 2-10000)" 
"\n\nThis program calculates the value of the timewave at a given point relative to the zero point.\n";



char *set_name[NUM_SETS] =
{ "Kelley", "Watkins", "Sheliak", "Huang Ti" };


char *title = "Days to Zero (DTZ), Kelley, Watkins, Sheliak, Huang Ti";


//  The number sets.
int64_t w[NUM_SETS][NUM_DATA_POINTS] = 
{ 
  {
    #include "DATA/DATA.TW1"		//  half-twist
  }, 
  {
    #include "DATA/DATA.TW2"		//  no half-twist
  }, 
  {
    #include "DATA/DATA.TW3"		//  Sheliak 
  }, 
  {
    #include "DATA/DATA.TW4"		//  HuangTi (no half-twist)
  } 
};



void inputerror (void);

void get_dtzp (void);

void get_NegBailout (void);

void get_step (void);

void get_wave_factor (void);

void set_powers (void);

long double f (long double x, int64_t number_set);

void *fONE (void);

void *fTWO (void);

void *fTHREE (void);

void *fFOUR (void);



long double v (long double y, int64_t number_set);

long double mult_power (long double x, int64_t i);

long double div_power (long double x, int64_t i);


long double dtzp, step;


/*-----------------------------*/ 
int
main (int argc, char *argv[]) 
{
  
  
  int64_t i, j, ch;
    
  if (argc != 5 && argc != 1)
    
  {
    
    printf ("%s", usage);
    
    inputerror ();
    
  }
  
  
  if (argc == 5)
    
  {
    
    dtzp = atof (&argv[1][0]);
    
    
    NegativeBailout = atof (&argv[2][0]);
    
    NegativeBailout *= -1;
    
    
    step = atof (&argv[3][0]);
    
    step /= 60;		// Convert to 60 minute hours 
    step /= 24;			// Convert to 24 hour days 
    
    wave_factor = atoi (&argv[4][0]);
    
    
    if (wave_factor < 2 || wave_factor > 10000)
      
    {
      
      printf ("%s", usage);
      
      inputerror ();
      
    }
    
  }
  
  
  if (argc == 1)		// If no commandline inputs
  {
    
    get_dtzp ();
    
    get_NegBailout ();
    
    get_step ();
    
    get_wave_factor ();
    
    
  }
  
  // Pre-lock all the data locks before spawning the threads, so the threads do not run yet.
  lockstruct._lock1 = lockstruct._lock2 = lockstruct._lock3 = lockstruct._lock4 = false;
  
  // Declare the thread pointers & start
  pthread_t tONE, tTWO, tTHREE, tFOUR;
  
  pthread_create (&tONE, NULL, (void *) &fONE, NULL);
  
  pthread_create (&tTWO, NULL, (void *) &fTWO, NULL);
  
  pthread_create (&tTHREE, NULL, (void *) &fTHREE, NULL);
  
  pthread_create (&tFOUR, NULL, (void *) &fFOUR, NULL);
  
  
  
  
  set_powers ();
  
  
  
  
  
  
  
  //printf("\n\ndtzp: %lfstep: %lfwave_factor: %d",dtzp, step, wave_factor);
  printf ("\n%s\n", title);
  
  
  while (dtzp >= NegativeBailout)
    
  {
    
    
    printf ("\n%.*Lf ,", PREC, dtzp);
    
    
    
    // populate with the requested date
    lockstruct.x1 = lockstruct.x2 = lockstruct.x3 = lockstruct.x4 = dtzp;
    
    // Set all the data locks
    lockstruct._lock1 = lockstruct._lock2 = lockstruct._lock3 =
    lockstruct._lock4 = true;
    
    // Wait for calculated signal
    while (true == lockstruct._lock1 || true == lockstruct._lock2
      || true == lockstruct._lock3 || true == lockstruct._lock4) {
      };
    
    
    printf ("%.*Lf ,%.*Lf ,%.*Lf ,%.*Lf ,", PREC, lockstruct.ans1, PREC,
	    lockstruct.ans2, PREC, lockstruct.ans3, PREC, lockstruct.ans4);
    
    
    
    dtzp -= step;
    
    
  }
  
  
}





//  wave_factor is a global variable
/*-----------------*/ 
void
set_powers (void) 
{
  
  uint64_t j;
  
  
  /*  put powers[j] = wave_factor^j  */ 
  
  powers[0] = (long double) 1;
  
  for (j = 1; j < NUM_POWERS; j++)
    
    powers[j] = wave_factor * powers[j - 1];
  
}



/*  x is number of days to zero date  */ 
/*--------------*/ 
long double
f (long double x, 
   int64_t number_set) 
{
  
  uint64_t i;
  
  long double sum = 0.0, last_sum = 0.0;
  
  
  if (x)
    
  {
    
    for (i = 0; x >= powers[i]; i++)
      
      sum += mult_power (v (div_power (x, i), number_set), i);
    
    
    i = 0;
    
    do
      
    {
      
      if (++i > CALC_PREC + 2)
	
	break;
      
      last_sum = sum;
      
      sum += div_power (v (mult_power (x, i), number_set), i);
      
    }
    while ((sum == 0.0) || (sum > last_sum));
    
  }
  
  
  /*  dividing by 64^3 gives values consistent with the Apple // version
   *  and provides more convenient y-axis labels
   */ 
  sum = div_power (sum, 3);
  
  
  return (sum);
  
}



/*  x is number of days to zero date  */ 
/*--------------*/ 
void *
fONE (void) 
{
  onestart:
  ;			// This semicolon is required on GCC and ignored elsewhere
  // Wait until the lock is un-set, then run (for first run)
  do {
    
  }
  while (false == lockstruct._lock1);
  
  
  int64_t number_set = 0;
  
  uint64_t i;
  
  long double sum = 0.0, last_sum = 0.0;
  
  
  long double x = lockstruct.x1;
  
  
  if (x)
    
  {
    
    for (i = 0; x >= powers[i]; i++)
      
      sum += mult_power (v (div_power (x, i), number_set), i);
    
    
    i = 0;
    
    do
      
    {
      
      if (++i > CALC_PREC + 2)
	
	break;
      
      last_sum = sum;
      
      sum += div_power (v (mult_power (x, i), number_set), i);
      
    }
    while ((sum == 0.0) || (sum > last_sum));
    
  }
  
  
  /*  dividing by 64^3 gives values consistent with the Apple // version
   *  and provides more convenient y-axis labels
   */ 
  sum = div_power (sum, 3);
  
  
  lockstruct.ans1 = sum;
  
  lockstruct._lock1 = false;
  
  
  goto onestart;
  
  
  return 0;
  
}


/*  x is number of days to zero date  */ 
/*--------------*/ 
void *
fTWO (void) 
{
  twostart:
  ;			// This semicolon is required on GCC and ignored elsewhere
  // Wait until the lock is un-set, then run (for first run)
  do {
    
  }
  while (false == lockstruct._lock2);
  

  int64_t number_set = 1;
  
  uint64_t i;
  
  long double sum = 0.0, last_sum = 0.0;
  
  
  long double x = lockstruct.x2;
  
  
  if (x)
    
  {
    
    for (i = 0; x >= powers[i]; i++)
      
      sum += mult_power (v (div_power (x, i), number_set), i);
    
    
    i = 0;
    
    do
      
    {
      
      if (++i > CALC_PREC + 2)
	
	break;
      
      last_sum = sum;
      
      sum += div_power (v (mult_power (x, i), number_set), i);
      
    }
    while ((sum == 0.0) || (sum > last_sum));
    
  }
  
  
  /*  dividing by 64^3 gives values consistent with the Apple // version
   *  and provides more convenient y-axis labels
   */ 
  sum = div_power (sum, 3);
  
  
  lockstruct.ans2 = sum;
  
  lockstruct._lock2 = false;
  
  
  goto twostart;
  
  
  return 0;
  
}


/*  x is number of days to zero date  */ 
/*--------------*/ 
void *
fTHREE (void) 
{
  threestart:
  ;			// This semicolon is required on GCC and ignored elsewhere
  // Wait until the lock is un-set, then run (for first run)
  do {
    
  }
  while (false == lockstruct._lock3);
  
  
  int64_t number_set = 2;
  
  uint64_t i;
  
  long double sum = 0.0, last_sum = 0.0;
  
  
  long double x = lockstruct.x3;
  
  
  if (x)
    
  {
    
    for (i = 0; x >= powers[i]; i++)
      
      sum += mult_power (v (div_power (x, i), number_set), i);
    
    
    i = 0;
    
    do
      
    {
      
      if (++i > CALC_PREC + 2)
	
	break;
      
      last_sum = sum;
      
      sum += div_power (v (mult_power (x, i), number_set), i);
      
    }
    while ((sum == 0.0) || (sum > last_sum));
    
  }
  
  
  /*  dividing by 64^3 gives values consistent with the Apple // version
   *  and provides more convenient y-axis labels
   */ 
  sum = div_power (sum, 3);
  
  
  lockstruct.ans3 = sum;
  
  lockstruct._lock3 = false;
  
  
  goto threestart;
  
  
  return 0;
  
}



/*  x is number of days to zero date  */ 
/*--------------*/ 
void *
fFOUR (void) 
{
  fourstart:
  ;			// This semicolon is required on GCC and ignored elsewhere
  // Wait until the lock is un-set, then run (for first run)
  do {
    
  }
  while (false == lockstruct._lock4);
  
  

  int64_t number_set = 3;
  
  uint64_t i;
  
  long double sum = 0.0, last_sum = 0.0;
  
  
  long double x = lockstruct.x4;
  
  
  if (x)
    
  {
    
    for (i = 0; x >= powers[i]; i++)
      
      sum += mult_power (v (div_power (x, i), number_set), i);
    
    
    i = 0;
    
    do
      
    {
      
      if (++i > CALC_PREC + 2)
	
	break;
      
      last_sum = sum;
      
      sum += div_power (v (mult_power (x, i), number_set), i);
      
    }
    while ((sum == 0.0) || (sum > last_sum));
    
  }
  
  
  /*  dividing by 64^3 gives values consistent with the Apple // version
   *  and provides more convenient y-axis labels
   */ 
  sum = div_power (sum, 3);
  
  
  lockstruct.ans4 = sum;
  
  lockstruct._lock4 = false;
  
  
  goto fourstart;
  
  
  return 0;
  
}



/*--------------*/ 
long double
v (long double y, 
   int64_t number_set) 
{
  
  int64_t i = (int64_t) (fmod (y, (long double) NUM_DATA_POINTS));
  
  int64_t j = (i + 1) % NUM_DATA_POINTS;
  
  long double z = y - floor (y);
  
  
  return (z == 0.0 ? (long double) w[number_set][i] : 
  (w[number_set][j] - w[number_set][i]) * z + w[number_set][i]);
  
} 


/*  in order to speed up the calculation, if wave factor = 64
 *  then instead of using multiplication or division operation
 *  we act directly on the floating point representation;
 *  multiplying by 64^i is accomplished by adding i*0x60
 *  to the exponent (the last 2 bytes of the 8-byte representation);
 *  dividing by 64^i is accomplished by subtracting i*0x60
 *  from the exponent
 */ 

/*-----------------------*/ 
long double
mult_power (long double x, 
	    int64_t i) 
{
  
  int64_t *exponent = (int64_t *) &x + 3;
  
  
  if (wave_factor == 64)
    
    *exponent += i * 0x60;	/*  measurably faster  */
    
    else
      
      x *= powers[i];
    
    
    return (x);
  
}



/*----------------------*/ 
long double
div_power (long double x, 
	   int64_t i) 
{
  
  int64_t *exponent = (int64_t *) &x + 3;
  
  
  if ((wave_factor == 64) && (*exponent > i * 0x60))
    
    *exponent -= i * 0x60;
  
  else
    
    x /= powers[i];
  
  
  return (x);
  
}



void
get_dtzp (void) 
{
  
  printf ("Enter the number of days before zero point:  ");
  
  int64_t temp = scanf ("%Lf", &dtzp);
  
} 

void

get_NegBailout (void) 
{
  
  printf ("Enter the number of days to calculate after zero point:  ");
  
  int64_t temp = scanf ("%Lf", &NegativeBailout);
  
  NegativeBailout *= -1;	// Set to negative for internal use
  
} 

void

get_step (void) 
{
  
  printf ("Enter the time step in minutes ( >= 0 ):  ");
  
  int64_t temp = scanf ("%Lf", &step);
  
  step /= 60;			// Convert to 60 minute hours
  step /= 24;			// Convert to fractions of 24-hour days for internal calculations...
  if (step > dtzp || step < 0)
    inputerror ();		// The step cannot be larger than dtz as this would cause no values to be calculated.
}



void
get_wave_factor (void) 
{
  
  printf ("Enter the wave factor (2-10000): ");
  
  int64_t temp = scanf ("%li", &wave_factor);
  
  
  //  if ( wave_factor < 2 || wave_factor > 10000 ) inputerror(); 
} 

int64_t

doublecheck (void) 
{
  
  char answer;
  
  printf
  ("\nThe combination you have chosen will create %Lf data points. \nDo you wish to continue? (Y/N) ", 1 + (int) dtzp / step);
  
  answer = getchar ();
  
} 

void

inputerror (void) 
{
  
  printf ("\nError: Invalid input, exiting.\n\n");
  
  exit (EXIT_SUCCESS);
  
} 


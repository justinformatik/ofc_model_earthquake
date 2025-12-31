#include <stdio.h>
#include <stdlib.h>
#include "my_numerics.h"
#include <gsl/gsl_rng.h>


gsl_rng* initGSL(){
    gsl_rng* myGenerator = gsl_rng_alloc(gsl_rng_mt19937); //Verwende Mersenne Twister 19937
    gsl_rng_set(myGenerator, time(NULL));                  //Verwende als Seed Zeit
    return myGenerator;
}

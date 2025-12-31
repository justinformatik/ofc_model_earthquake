#include <stdio.h>
#include <stdlib.h>
#include "my_numerics.h"
#include <gsl/gsl_rng.h>

//Dies ist die einzige Funktion, die für die Abgabe notwendig war. Diese hatte ich bereits vor der Prüfung geschrieben.
gsl_rng* initGSL(){
    gsl_rng* myGenerator = gsl_rng_alloc(gsl_rng_mt19937); //Verwende Mersenne Twister 19937
    gsl_rng_set(myGenerator, time(NULL));                  //Verwende als Seed Zeit
    return myGenerator;
}
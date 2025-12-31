#include "my_numerics.h"
int main(void){  
    gsl_rng *myGenerator = initGSL();

    FILE* data_MT = fopen("MersenneTwister.csv", "w");
    int N = 99999; 

    for(int i = 0; i < N; i++){
        if((i+1) % 3 == 0){
            
            fprintf(data_MT, "%.14e\n", gsl_rng_uniform(myGenerator));
        }else{
            
            fprintf(data_MT, "%.14e, ", gsl_rng_uniform(myGenerator));
        }
    }
    gsl_rng_free(myGenerator);
    fclose(data_MT);         
    return 0;
}

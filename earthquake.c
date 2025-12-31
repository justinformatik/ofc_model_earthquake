#include "my_numerics.h"

typedef struct {
    int i, j;
} Cords;


void allocate_grid(double*** grid, int L){             
    *grid = (double**)calloc(L, sizeof(double*));       
    if (*grid == NULL) { 
        fprintf(stderr, "Fehler: Speicherreservierung für die Zeilen des Gitters fehlgeschlagen.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < L; i++){                       
        (*grid)[i] = (double*)calloc(L, sizeof(double));
        if ((*grid)[i] == NULL) { 
            fprintf(stderr, "Fehler: Speicherreservierung für die Spalte %d des Gitters fehlgeschlagen.\n", i);
            exit(EXIT_FAILURE);
        }
    }
}

void free_grid(double*** grid, int L){ 
    for(int i = 0; i < L; i++){
        free((*grid)[i]);
    }
    free(*grid);
}


void initialize_grid(double** grid, gsl_rng* myGenerator, int L, double F_c){ 
    for(int i = 0; i < L; i++){                    
        for(int j = 0; j < L; j++){    
            grid[i][j] = gsl_rng_uniform(myGenerator) * F_c;
        }
    }
}




void distribute_force(double** grid, int i, int j, double f, int L){ 
    if(i > 0) grid[i - 1][j] += f;     
    if(i < L - 1) grid[i + 1][j] += f;  
    if(j > 0) grid[i][j - 1] += f;
    if(j < L - 1) grid[i][j + 1] += f;
}


void trigger_earthquake(double** grid, int L, FILE* data, int run_index, double F_c, double Fmin, double alpha, int cascade_id, int timestep, Cords* cords){
    cords = (Cords*) malloc(L * L * sizeof(Cords));                         
    double *forces_to_distribute = (double*) malloc(L * L * sizeof(double)); 
    int ongoing_bool = 1;                                                    
    while(ongoing_bool){                                                    
        int cords_count = 0; 
        ongoing_bool = 0;   
        for(int i = 0; i < L; i++){
            for(int j = 0; j < L; j++){
                if(grid[i][j] >= F_c) {
                    cords[cords_count].i = i;
                    cords[cords_count].j = j;
                    forces_to_distribute[cords_count] = grid[i][j] * alpha;
                    cords_count += 1;
                    ongoing_bool = 1; 
                }
            }
        }
        for(int index = 0; index < cords_count; index++){ 
            int i = cords[index].i;
            int j = cords[index].j;
            double f = forces_to_distribute[index];
            if(run_index != -1){ 
                fprintf(data, "%d,%d,%d,%d,%f,%d\n", run_index, cascade_id, i, j, alpha, timestep); 
            } else {
                fprintf(data, "%d,%d,%d,%f,%d\n", cascade_id, i, j, alpha, timestep);
            }
            distribute_force(grid, i, j, f, L); 
        }
        for(int index = 0; index < cords_count; index++){
            grid[cords[index].i][cords[index].j] = Fmin; 
        }
        if(ongoing_bool) timestep += 1; 
    }
    free(cords);
    free(forces_to_distribute);
}


void disturb_grid(double** grid, int L, double F_c, double Fmin){
    double Fmax = Fmin; 
    for(int i = 0; i < L; i++){
        for(int j = 0; j < L; j++) {
            if(grid[i][j] > Fmax) Fmax = grid[i][j]; 
        }
    }
    double difference = F_c - Fmax;   
    for(int i = 0; i < L; i++) {
        for(int j = 0; j < L; j++) {
            grid[i][j] += difference; 
        }
    }
}

int main() {
    gsl_rng *myGenerator = initGSL(); 
    double **grid;         
    Cords* cords = NULL;    
    double F_c = 1.0;       
    double Fmin =  0.0;    
    double L1 =  20;        
    double L2 =  30;        
    double alpha = 0.20;    
    double N = 1000;        
    int cascade_id = 0;     
    int timestep = 0;       
    int number_of_runs = 21;
    
    
    {
        allocate_grid(&grid, L1);
        initialize_grid(grid, myGenerator, L1, F_c);       
        FILE *data1 = fopen("1.csv", "w"); 
        if(data1 == NULL){
            printf("Could not open file\n");
            exit(EXIT_FAILURE);
        }
        for(int run = 0; run < N; run++){ 
            trigger_earthquake(grid, L1, data1, -1, F_c, Fmin, alpha, cascade_id, timestep, cords);
            disturb_grid(grid, L1, F_c, Fmin);
            cascade_id += 1; 
            timestep = 0;    
        }
        fclose(data1); 
        free_grid(&grid, L1); 
        cascade_id = 0; 
    }

    
    N =  5000; 
    double N2 = 50000; 
    char* data_filenames[] = {"2_1.csv", "2_2.csv", "2_3.csv", "2_4.csv"};  
    int L_values[] = {L1, L2, L1, L2}; 
    double N_values[] = {N, N, N2, N2};
    {
        for(int task_index = 0; task_index < 4; task_index++) { 
            allocate_grid(&grid, L_values[task_index]);

            FILE *data = fopen(data_filenames[task_index], "w");
            if(data == NULL) {
                printf("Could not open file\n");
                exit(EXIT_FAILURE);
            }
            for(int run_index = 0; run_index < number_of_runs; run_index++) { 
                initialize_grid(grid, myGenerator, L_values[task_index], F_c); 
                for(int run = 0; run < N_values[task_index]; run++) {
                    trigger_earthquake(grid, L_values[task_index], data, run_index, F_c, Fmin, alpha, cascade_id, timestep, cords);
                    disturb_grid(grid, L_values[task_index], F_c, Fmin);
                    cascade_id += 1; 
                    timestep = 0;
                }
            }
            fclose(data);
            free_grid(&grid, L_values[task_index]);
            cascade_id = 0; 
        }
    }


    double alpha_step = 0.01; 
    double alpha_start = 0.13;
    double alpha_end = 0.23;  
    {
        FILE *data3 = fopen("3.csv", "w");
        if(data3 == NULL) {
            printf("Could not open file\n");
            exit(EXIT_FAILURE);
        }
        allocate_grid(&grid, L1);

        for(double new_alpha = alpha_start; new_alpha <= alpha_end; new_alpha += alpha_step){
            alpha = new_alpha; 
            for(int run_index = 0; run_index < number_of_runs; run_index++) {
                initialize_grid(grid, myGenerator, L1, F_c); 
                for(int run = 0; run < N; run++) {
                    trigger_earthquake(grid, L1, data3, run_index, F_c, Fmin, alpha, cascade_id, timestep, cords);
                    disturb_grid(grid, L1, F_c, Fmin);
                    cascade_id += 1;
                    timestep = 0;
                }
            }
        }
    }
    return 0;
}

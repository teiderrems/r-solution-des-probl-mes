/**
 * @file evaluation.c
 * @brief Implémentation des fonctions d'évaluation
 */

#include "evaluation.h"
#include <stdlib.h>
#include <time.h>

int calculer_makespan(Instance *instance, int *permutation) {
    int nb_jobs = instance->nb_jobs;
    int nb_machines = instance->nb_machines;
    
    int **completion_times = (int**)malloc(nb_jobs * sizeof(int*));
    for (int i = 0; i < nb_jobs; i++) {
        completion_times[i] = (int*)malloc(nb_machines * sizeof(int));
    }
    
    // Initialisation première job
    completion_times[0][0] = instance->jobs[permutation[0]].processing_times[0];
    for (int m = 1; m < nb_machines; m++) {
        completion_times[0][m] = completion_times[0][m-1] + instance->jobs[permutation[0]].processing_times[m];
    }
    
    // Calcul pour les autres jobs
    for (int i = 1; i < nb_jobs; i++) {
        completion_times[i][0] = completion_times[i-1][0] + instance->jobs[permutation[i]].processing_times[0];
        
        for (int m = 1; m < nb_machines; m++) {
            int max_val = (completion_times[i-1][m] > completion_times[i][m-1]) ? 
                          completion_times[i-1][m] : completion_times[i][m-1];
            completion_times[i][m] = max_val + instance->jobs[permutation[i]].processing_times[m];
        }
    }
    
    int makespan = completion_times[nb_jobs-1][nb_machines-1];
    
    for (int i = 0; i < nb_jobs; i++) {
        free(completion_times[i]);
    }
    free(completion_times);
    
    return makespan;
}

int calculer_tardiness(Instance *instance, int *permutation) {
    int nb_jobs = instance->nb_jobs;
    int nb_machines = instance->nb_machines;
    
    int **completion_times = (int**)malloc(nb_jobs * sizeof(int*));
    for (int i = 0; i < nb_jobs; i++) {
        completion_times[i] = (int*)malloc(nb_machines * sizeof(int));
    }
    
    // Calcul des dates de fin
    completion_times[0][0] = instance->jobs[permutation[0]].processing_times[0];
    for (int m = 1; m < nb_machines; m++) {
        completion_times[0][m] = completion_times[0][m-1] + instance->jobs[permutation[0]].processing_times[m];
    }
    
    for (int i = 1; i < nb_jobs; i++) {
        completion_times[i][0] = completion_times[i-1][0] + instance->jobs[permutation[i]].processing_times[0];
        
        for (int m = 1; m < nb_machines; m++) {
            int max_val = (completion_times[i-1][m] > completion_times[i][m-1]) ? 
                          completion_times[i-1][m] : completion_times[i][m-1];
            completion_times[i][m] = max_val + instance->jobs[permutation[i]].processing_times[m];
        }
    }
    
    // Calcul de la tardiness
    int total_tardiness = 0;
    for (int i = 0; i < nb_jobs; i++) {
        int completion_time = completion_times[i][nb_machines-1];
        int due_date = instance->jobs[permutation[i]].due_date;
        if (completion_time > due_date) {
            total_tardiness += (completion_time - due_date);
        }
    }
    
    for (int i = 0; i < nb_jobs; i++) {
        free(completion_times[i]);
    }
    free(completion_times);
    
    return total_tardiness;
}

ObjectiveVector evaluer_solution(Instance *instance, int *permutation) {
    ObjectiveVector result;
    result.makespan = calculer_makespan(instance, permutation);
    result.tardiness = calculer_tardiness(instance, permutation);
    return result;
}

int* generer_solution_aleatoire(Instance *instance) {
    int nb_jobs = instance->nb_jobs;
    int *permutation = (int*)malloc(nb_jobs * sizeof(int));
    
    for (int i = 0; i < nb_jobs; i++) {
        permutation[i] = i;
    }
    
    // Mélange Fisher-Yates
    for (int i = nb_jobs - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = temp;
    }
    
    return permutation;
}
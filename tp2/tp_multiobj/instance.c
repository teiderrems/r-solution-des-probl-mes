/**
 * @file instance.c
 * @brief Implémentation de la gestion des instances
 */

#include "instance.h"
#include <stdio.h>
#include <stdlib.h>

Instance* lire_instance(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }
    
    Instance *instance = (Instance*)malloc(sizeof(Instance));
    
    fscanf(file, "%d", &instance->nb_jobs);
    fscanf(file, "%d", &instance->nb_machines);
    
    int seed;
    fscanf(file, "%d", &seed); // Seed qu'on ignore
    
    instance->jobs = (Job*)malloc(instance->nb_jobs * sizeof(Job));
    
    for (int i = 0; i < instance->nb_jobs; i++) {
        instance->jobs[i].processing_times = (int*)malloc(instance->nb_machines * sizeof(int));
        
        int id;
        fscanf(file, "%d", &id);
        fscanf(file, "%d", &instance->jobs[i].due_date);
        
        for (int j = 0; j < instance->nb_machines; j++) {
            fscanf(file, "%d", &instance->jobs[i].processing_times[j]);
        }
    }
    
    fclose(file);
    return instance;
}

void liberer_instance(Instance *instance) {
    if (instance) {
        for (int i = 0; i < instance->nb_jobs; i++) {
            free(instance->jobs[i].processing_times);
        }
        free(instance->jobs);
        free(instance);
    }
}
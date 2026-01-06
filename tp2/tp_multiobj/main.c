/**
 * @file main.c
 * @brief Point d'entrée principal du programme d'optimisation multi-objectif
 */

#include "config.h"
#include "instance.h"
#include "evaluation.h"
#include "archive.h"
#include "algorithms.h"
#include "analysis.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * @brief Fonction principale du programme
 */
int main(int argc, char *argv[]) {
    srand(time(NULL));
    
    // Parse des arguments
    Config config;
    if (parse_arguments(argc, argv, &config) != 0) {
        return EXIT_FAILURE;
    }
    
    afficher_configuration(&config);
    
    // Chargement de l'instance
    Instance *instance = lire_instance(config.instance_file);
    if (!instance) {
        return EXIT_FAILURE;
    }
    
    printf("Instance chargée: %d jobs, %d machines\n\n", 
           instance->nb_jobs, instance->nb_machines);
    
    // Exécution
    executer_experimentation(instance, &config);
    
    // Libération mémoire
    liberer_instance(instance);
    
    printf("=== PROGRAMME TERMINÉ AVEC SUCCÈS ===\n");
    printf("Résultats sauvegardés dans: %s/\n", config.output_dir);
    
    return EXIT_SUCCESS;
}
/**
 * @file config.c
 * @brief Implémentation de la gestion de la configuration
 */

#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void afficher_aide() {
    printf("Usage: ./tp_multiobj [OPTIONS] fichier_instance\n\n");
    printf("Options:\n");
    printf("  -h, --help                 Affiche cette aide\n");
    printf("  -o, --offline N            Nombre de solutions pour le filtrage offline (defaut: 500)\n");
    printf("  -p, --poids N              Nombre de poids pour l'approche scalaire (defaut: 10)\n");
    printf("  -i, --iterations N         Nombre maximal d'iterations (defaut: 1000)\n");
    printf("  -s, --size N               Taille initiale pour l'approche Pareto (defaut: 20)\n");
    printf("  -r, --runs N               Nombre d'executions pour les statistiques (defaut: 1)\n");
    printf("  -a, --algo TYPE            Algorithme a executer: all, scalar, pareto (defaut: all)\n");
    printf("  -v, --verbose              Mode verbeux pour plus de details\n");
    printf("  --no-analyze               Desactive l'analyse comparative\n");
    printf("  --output-dir DIR           Repertoire de sortie pour les resultats (defaut: .)\n");
    printf("\nExemples:\n");
    printf("  ./tp_multiobj instance.txt\n");
    printf("  ./tp_multiobj -o 1000 -p 20 -i 5000 instance.txt\n");
    printf("  ./tp_multiobj --algo scalar --iterations 2000 instance.txt\n");
}

int parse_arguments(int argc, char *argv[], Config *config) {
    // Valeurs par défaut
    strcpy(config->instance_file, "");
    config->offline_solutions = 500;
    config->nb_poids = 10;
    config->max_iterations = 1000;
    config->pareto_size = 20;
    config->nb_runs = 1;
    config->verbose = 0;
    config->no_analyze = 0;
    strcpy(config->output_dir, ".");
    strcpy(config->algo_type, "all");
    
    // Vérification des arguments
    if (argc < 2) {
        afficher_aide();
        return 1;
    }
    
    // Parse des arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            afficher_aide();
            return 1;
        }
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--offline") == 0) {
            if (i + 1 < argc) {
                config->offline_solutions = atoi(argv[++i]);
            } else {
                printf("Erreur: option -o require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--poids") == 0) {
            if (i + 1 < argc) {
                config->nb_poids = atoi(argv[++i]);
            } else {
                printf("Erreur: option -p require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations") == 0) {
            if (i + 1 < argc) {
                config->max_iterations = atoi(argv[++i]);
            } else {
                printf("Erreur: option -i require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0) {
            if (i + 1 < argc) {
                config->pareto_size = atoi(argv[++i]);
            } else {
                printf("Erreur: option -s require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--runs") == 0) {
            if (i + 1 < argc) {
                config->nb_runs = atoi(argv[++i]);
            } else {
                printf("Erreur: option -r require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algo") == 0) {
            if (i + 1 < argc) {
                strcpy(config->algo_type, argv[++i]);
            } else {
                printf("Erreur: option -a require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            config->verbose = 1;
        }
        else if (strcmp(argv[i], "--no-analyze") == 0) {
            config->no_analyze = 1;
        }
        else if (strcmp(argv[i], "--output-dir") == 0) {
            if (i + 1 < argc) {
                strcpy(config->output_dir, argv[++i]);
            } else {
                printf("Erreur: option --output-dir require une valeur\n");
                return 1;
            }
        }
        else {
            // C'est probablement le nom du fichier d'instance
            strcpy(config->instance_file, argv[i]);
        }
    }
    
    // Vérification du fichier d'instance
    if (strlen(config->instance_file) == 0) {
        printf("Erreur: nom de fichier d'instance manquant\n");
        afficher_aide();
        return 1;
    }
    
    // Vérification des valeurs
    if (config->offline_solutions <= 0) {
        printf("Erreur: nombre de solutions offline doit etre > 0\n");
        return 1;
    }
    if (config->nb_poids <= 0) {
        printf("Erreur: nombre de poids doit etre > 0\n");
        return 1;
    }
    if (config->max_iterations <= 0) {
        printf("Erreur: nombre d'iterations doit etre > 0\n");
        return 1;
    }
    if (config->pareto_size <= 0) {
        printf("Erreur: taille initiale doit etre > 0\n");
        return 1;
    }
    if (config->nb_runs <= 0) {
        printf("Erreur: nombre d'executions doit etre > 0\n");
        return 1;
    }
    
    // Vérification du répertoire de sortie
    struct stat st = {0};
    if (stat(config->output_dir, &st) == -1) {
        // Création du répertoire
        if (mkdir(config->output_dir, 0777) != 0) {
            printf("Erreur: impossible de creer le repertoire %s\n", config->output_dir);
            return 1;
        }
    }
    
    return 0;
}

void afficher_configuration(Config *config) {
    printf("=== CONFIGURATION ===\n");
    printf("Fichier d'instance: %s\n", config->instance_file);
    printf("Solutions offline: %d\n", config->offline_solutions);
    printf("Nombre de poids: %d\n", config->nb_poids);
    printf("Iterations maximales: %d\n", config->max_iterations);
    printf("Taille initiale Pareto: %d\n", config->pareto_size);
    printf("Nombre d'executions: %d\n", config->nb_runs);
    printf("Algorithme: %s\n", config->algo_type);
    printf("Mode verbeux: %s\n", config->verbose ? "oui" : "non");
    printf("Analyse: %s\n", config->no_analyze ? "non" : "oui");
    printf("Repertoire sortie: %s\n", config->output_dir);
    printf("=====================\n\n");
}
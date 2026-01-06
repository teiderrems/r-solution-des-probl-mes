/**
 * @file algorithms.c
 * @brief Implémentation des algorithmes d'optimisation
 */

#include "algorithms.h"
#include "evaluation.h"
#include "analysis.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Archive* approche_scalaire(Instance *instance, int nb_poids, int max_iterations, Config *config) {
    Archive *archive = creer_archive(100);
    
    clock_t debut = clock();
    
    for (int p = 0; p < nb_poids; p++) {
        double alpha = (double)p / (nb_poids - 1);
        double poids_makespan = 1.0 - alpha;
        double poids_tardiness = alpha;
        
        if (config->verbose && p % 5 == 0) {
            printf("  Poids %d/%d (alpha=%.2f, makespan=%.2f, tardiness=%.2f)\n", 
                   p+1, nb_poids, alpha, poids_makespan, poids_tardiness);
        }
        
        // Générer solution initiale aléatoire
        int *permutation = generer_solution_aleatoire(instance);
        ObjectiveVector current = evaluer_solution(instance, permutation);
        
        for (int iter = 0; iter < max_iterations; iter++) {
            // Voisinage: swap de deux jobs
            int meilleur_i = -1, meilleur_j = -1;
            double meilleur_score = poids_makespan * current.makespan + 
                                   poids_tardiness * current.tardiness;
            
            for (int i = 0; i < instance->nb_jobs; i++) {
                for (int j = i + 1; j < instance->nb_jobs; j++) {
                    // Essayer le swap
                    int temp = permutation[i];
                    permutation[i] = permutation[j];
                    permutation[j] = temp;
                    
                    ObjectiveVector voisin = evaluer_solution(instance, permutation);
                    double score_voisin = poids_makespan * voisin.makespan + 
                                         poids_tardiness * voisin.tardiness;
                    
                    if (score_voisin < meilleur_score) {
                        meilleur_score = score_voisin;
                        meilleur_i = i;
                        meilleur_j = j;
                    }
                    
                    // Annuler le swap
                    temp = permutation[i];
                    permutation[i] = permutation[j];
                    permutation[j] = temp;
                }
            }
            
            if (meilleur_i != -1) {
                // Appliquer le meilleur swap
                int temp = permutation[meilleur_i];
                permutation[meilleur_i] = permutation[meilleur_j];
                permutation[meilleur_j] = temp;
                current = evaluer_solution(instance, permutation);
            } else {
                if (config->verbose && iter < max_iterations - 1) {
                    printf("    Convergence atteinte à l'itération %d\n", iter);
                }
                break;
            }
        }
        
        // Ajouter à l'archive
        filtrage_online(archive, current);
        free(permutation);
    }
    
    clock_t fin = clock();
    
    if (config->verbose) {
        printf("  Temps d'execution: %.3f secondes\n", (double)(fin - debut) / CLOCKS_PER_SEC);
    }
    
    return archive;
}

Archive* approche_pareto(Instance *instance, int taille_initiale, int max_iterations, Config *config) {
    Archive *archive = creer_archive(100);
    
    clock_t debut = clock();
    
    // Initialisation de l'archive avec des solutions aléatoires
    if (config->verbose) {
        printf("  Initialisation avec %d solutions aléatoires\n", taille_initiale);
    }
    
    for (int i = 0; i < taille_initiale; i++) {
        int *permutation = generer_solution_aleatoire(instance);
        ObjectiveVector sol = evaluer_solution(instance, permutation);
        filtrage_online(archive, sol);
        free(permutation);
    }
    
    for (int iter = 0; iter < max_iterations; iter++) {
        // Pour chaque solution dans l'archive, explorer son voisinage
        int archive_size = archive->size;
        
        if (config->verbose && iter % 100 == 0) {
            printf("  Iteration %d/%d, archive: %d solutions\n", 
                   iter+1, max_iterations, archive_size);
        }
        
        for (int idx = 0; idx < archive_size; idx++) {
            int *permutation = generer_solution_aleatoire(instance);
            
            // Explorer le voisinage (swap)
            for (int i = 0; i < instance->nb_jobs; i++) {
                for (int j = i + 1; j < instance->nb_jobs; j++) {
                    // Essayer le swap
                    int temp = permutation[i];
                    permutation[i] = permutation[j];
                    permutation[j] = temp;
                    
                    ObjectiveVector voisin = evaluer_solution(instance, permutation);
                    filtrage_online(archive, voisin);
                    
                    // Annuler le swap
                    temp = permutation[i];
                    permutation[i] = permutation[j];
                    permutation[j] = temp;
                }
            }
            
            free(permutation);
        }
    }
    
    clock_t fin = clock();
    
    if (config->verbose) {
        printf("  Temps d'execution: %.3f secondes\n", (double)(fin - debut) / CLOCKS_PER_SEC);
    }
    
    return archive;
}

void executer_experimentation(Instance *instance, Config *config) {
    // Tableaux pour les statistiques
    Statistics *all_stats = NULL;
    if (config->nb_runs > 1) {
        all_stats = (Statistics*)malloc(config->nb_runs * sizeof(Statistics));
    }
    
    // Exécution multiple
    for (int run = 0; run < config->nb_runs; run++) {
        printf("=== EXECUTION %d/%d ===\n", run+1, config->nb_runs);
        
        if (config->nb_runs > 1) {
            // Nouvelle seed pour chaque run
            srand(time(NULL) + run);
        }
        
        Archive *archive_scalaire = NULL;
        Archive *archive_pareto = NULL;
        Statistics stats = {0};
        
        clock_t debut_total = clock();
        
        // Question 4: Solution aléatoire (uniquement au premier run)
        if (run == 0) {
            printf("\n--- Question 4: Solution aleatoire ---\n");
            int *perm_aleatoire = generer_solution_aleatoire(instance);
            ObjectiveVector eval_aleatoire = evaluer_solution(instance, perm_aleatoire);
            printf("Solution aleatoire: makespan=%d, tardiness=%d\n", 
                   eval_aleatoire.makespan, eval_aleatoire.tardiness);
            free(perm_aleatoire);
        }
        
        // Question 5: Filtrage offline (uniquement au premier run)
        if (run == 0) {
            printf("\n--- Question 5: Filtrage offline ---\n");
            printf("Generation de %d solutions aleatoires...\n", config->offline_solutions);
            
            ObjectiveVector *solutions_offline = (ObjectiveVector*)malloc(config->offline_solutions * sizeof(ObjectiveVector));
            
            for (int i = 0; i < config->offline_solutions; i++) {
                int *perm = generer_solution_aleatoire(instance);
                solutions_offline[i] = evaluer_solution(instance, perm);
                free(perm);
            }
            
            Archive *archive_offline = filtrage_offline(solutions_offline, config->offline_solutions);
            printf("Resultat: %d solutions non-dominees\n", archive_offline->size);
            
            sauvegarder_front("front_offline.dat", archive_offline, config);
            
            free(solutions_offline);
            liberer_archive(archive_offline);
        }
        
        // Question 6: Filtrage online (uniquement au premier run)
        if (run == 0) {
            printf("\n--- Question 6: Filtrage online ---\n");
            Archive *archive_online = creer_archive(100);
            
            clock_t debut_online = clock();
            int nb_comparaisons = 0;
            
            for (int i = 0; i < config->offline_solutions; i++) {
                int *perm = generer_solution_aleatoire(instance);
                ObjectiveVector sol = evaluer_solution(instance, perm);
                nb_comparaisons += archive_online->size + 1;
                filtrage_online(archive_online, sol);
                free(perm);
            }
            
            clock_t fin_online = clock();
            double temps_online = (double)(fin_online - debut_online) / CLOCKS_PER_SEC;
            
            printf("Archive finale: %d solutions non-dominees\n", archive_online->size);
            printf("Temps d'execution: %.4f secondes\n", temps_online);
            printf("Nombre de comparaisons: %d\n", nb_comparaisons);
            
            liberer_archive(archive_online);
        }
        
        // Exécution des algorithmes selon la configuration
        if (strcmp(config->algo_type, "all") == 0 || strcmp(config->algo_type, "scalar") == 0) {
            printf("\n--- Question 7: Approche scalaire ---\n");
            printf("Parametres: %d poids, %d iterations\n", config->nb_poids, config->max_iterations);
            
            clock_t debut_scalaire = clock();
            archive_scalaire = approche_scalaire(instance, config->nb_poids, config->max_iterations, config);
            clock_t fin_scalaire = clock();
            
            stats.time_scalar = (double)(fin_scalaire - debut_scalaire) / CLOCKS_PER_SEC;
            stats.solutions_scalar = archive_scalaire->size;
            
            printf("Solutions trouvees: %d (temps: %.3f s)\n", 
                   stats.solutions_scalar, stats.time_scalar);
            
            // Sauvegarde du front
            char filename[256];
            snprintf(filename, sizeof(filename), "front_scalaire_run_%d.dat", run+1);
            sauvegarder_front(filename, archive_scalaire, config);
        }
        
        if (strcmp(config->algo_type, "all") == 0 || strcmp(config->algo_type, "pareto") == 0) {
            printf("\n--- Question 8: Approche Pareto ---\n");
            printf("Parametres: taille initiale %d, %d iterations\n", 
                   config->pareto_size, config->max_iterations);
            
            clock_t debut_pareto = clock();
            archive_pareto = approche_pareto(instance, config->pareto_size, config->max_iterations, config);
            clock_t fin_pareto = clock();
            
            stats.time_pareto = (double)(fin_pareto - debut_pareto) / CLOCKS_PER_SEC;
            stats.solutions_pareto = archive_pareto->size;
            
            printf("Solutions trouvees: %d (temps: %.3f s)\n", 
                   stats.solutions_pareto, stats.time_pareto);
            
            // Sauvegarde du front
            char filename[256];
            snprintf(filename, sizeof(filename), "front_pareto_run_%d.dat", run+1);
            sauvegarder_front(filename, archive_pareto, config);
        }
        
        // Analyse comparative
        if (!config->no_analyze && (strcmp(config->algo_type, "all") == 0)) {
            printf("\n--- Analyse comparative ---\n");
            
            // Calcul des points de référence
            double ref_makespan = 0, ref_tardiness = 0;
            for (int i = 0; i < 100; i++) {
                int *perm = generer_solution_aleatoire(instance);
                ObjectiveVector sol = evaluer_solution(instance, perm);
                if (sol.makespan > ref_makespan) ref_makespan = sol.makespan;
                if (sol.tardiness > ref_tardiness) ref_tardiness = sol.tardiness;
                free(perm);
            }
            
            ref_makespan *= 1.1;
            ref_tardiness *= 1.1;
            
            if (archive_scalaire) {
                stats.hypervolume_scalar = calculer_hypervolume(archive_scalaire, ref_makespan, ref_tardiness);
                printf("Hypervolume scalaire: %.2f\n", stats.hypervolume_scalar);
            }
            
            if (archive_pareto) {
                stats.hypervolume_pareto = calculer_hypervolume(archive_pareto, ref_makespan, ref_tardiness);
                printf("Hypervolume Pareto:   %.2f\n", stats.hypervolume_pareto);
            }
            
            if (archive_scalaire && archive_pareto) {
                if (stats.hypervolume_scalar > stats.hypervolume_pareto) {
                    printf("→ Approche scalaire meilleure (+%.1f%%)\n", 
                           (stats.hypervolume_scalar - stats.hypervolume_pareto) / stats.hypervolume_pareto * 100);
                } else if (stats.hypervolume_pareto > stats.hypervolume_scalar) {
                    printf("→ Approche Pareto meilleure (+%.1f%%)\n", 
                           (stats.hypervolume_pareto - stats.hypervolume_scalar) / stats.hypervolume_scalar * 100);
                } else {
                    printf("→ Approches equivalentes\n");
                }
            }
        }
        
        // Sauvegarde des statistiques
        if (config->nb_runs > 1) {
            all_stats[run] = stats;
            sauvegarder_statistiques(&stats, run, config);
        }
        
        // Libération mémoire pour cette run
        if (archive_scalaire) {
            liberer_archive(archive_scalaire);
        }
        if (archive_pareto) {
            liberer_archive(archive_pareto);
        }
        
        clock_t fin_total = clock();
        double temps_total = (double)(fin_total - debut_total) / CLOCKS_PER_SEC;
        printf("\nTemps total execution %d: %.3f secondes\n", run+1, temps_total);
        printf("============================\n\n");
    }
    
    // Statistiques globales si plusieurs runs
    if (config->nb_runs > 1 && !config->no_analyze && strcmp(config->algo_type, "all") == 0) {
        afficher_statistiques_globales(all_stats, config->nb_runs, config);
        free(all_stats);
    }
}
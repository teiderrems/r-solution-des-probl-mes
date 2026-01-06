/**
 * @file analysis.c
 * @brief Implémentation des fonctions d'analyse
 */

#include "analysis.h"
#include <stdio.h>
#include <stdlib.h>

double calculer_hypervolume(Archive *archive, double ref_makespan, double ref_tardiness) {
    if (archive->size == 0) return 0.0;
    
    // Trier par makespan croissant
    for (int i = 0; i < archive->size - 1; i++) {
        for (int j = i + 1; j < archive->size; j++) {
            if (archive->vectors[i].makespan > archive->vectors[j].makespan) {
                ObjectiveVector temp = archive->vectors[i];
                archive->vectors[i] = archive->vectors[j];
                archive->vectors[j] = temp;
            }
        }
    }
    
    double hypervolume = 0.0;
    double dernier_tardiness = ref_tardiness;
    
    for (int i = 0; i < archive->size; i++) {
        double largeur = (i == 0) ? 
                        (ref_makespan - archive->vectors[i].makespan) :
                        (archive->vectors[i-1].makespan - archive->vectors[i].makespan);
        double hauteur = dernier_tardiness - archive->vectors[i].tardiness;
        
        hypervolume += largeur * hauteur;
        dernier_tardiness = archive->vectors[i].tardiness;
    }
    
    return hypervolume;
}

void sauvegarder_front(char *filename, Archive *archive, Config *config) {
    char chemin[512];
    snprintf(chemin, sizeof(chemin), "%s/%s", config->output_dir, filename);
    
    FILE *file = fopen(chemin, "w");
    if (!file) {
        printf("Erreur: impossible de creer le fichier %s\n", chemin);
        return;
    }
    
    fprintf(file, "# makespan tardiness\n");
    for (int i = 0; i < archive->size; i++) {
        fprintf(file, "%d %d\n", archive->vectors[i].makespan, archive->vectors[i].tardiness);
    }
    
    fclose(file);
}

void sauvegarder_statistiques(Statistics *stats, int run, Config *config) {
    char chemin[512];
    snprintf(chemin, sizeof(chemin), "%s/statistiques_run_%d.txt", config->output_dir, run+1);
    
    FILE *file = fopen(chemin, "w");
    if (!file) {
        printf("Erreur: impossible de creer le fichier %s\n", chemin);
        return;
    }
    
    fprintf(file, "# Statistiques d'execution - Run %d\n", run+1);
    fprintf(file, "Hypervolume_scalaire: %.2f\n", stats->hypervolume_scalar);
    fprintf(file, "Hypervolume_pareto: %.2f\n", stats->hypervolume_pareto);
    fprintf(file, "Solutions_scalaire: %d\n", stats->solutions_scalar);
    fprintf(file, "Solutions_pareto: %d\n", stats->solutions_pareto);
    fprintf(file, "Temps_scalaire: %.3f\n", stats->time_scalar);
    fprintf(file, "Temps_pareto: %.3f\n", stats->time_pareto);
    
    fclose(file);
}

void afficher_statistiques_globales(Statistics *all_stats, int nb_runs, Config *config) {
    printf("=== STATISTIQUES GLOBALES (%d runs) ===\n", nb_runs);
    
    double avg_hyper_scalar = 0, avg_hyper_pareto = 0;
    double avg_time_scalar = 0, avg_time_pareto = 0;
    double avg_sol_scalar = 0, avg_sol_pareto = 0;
    
    for (int i = 0; i < nb_runs; i++) {
        avg_hyper_scalar += all_stats[i].hypervolume_scalar;
        avg_hyper_pareto += all_stats[i].hypervolume_pareto;
        avg_time_scalar += all_stats[i].time_scalar;
        avg_time_pareto += all_stats[i].time_pareto;
        avg_sol_scalar += all_stats[i].solutions_scalar;
        avg_sol_pareto += all_stats[i].solutions_pareto;
    }
    
    avg_hyper_scalar /= nb_runs;
    avg_hyper_pareto /= nb_runs;
    avg_time_scalar /= nb_runs;
    avg_time_pareto /= nb_runs;
    avg_sol_scalar /= nb_runs;
    avg_sol_pareto /= nb_runs;
    
    printf("Moyenne hypervolume scalaire: %.2f\n", avg_hyper_scalar);
    printf("Moyenne hypervolume Pareto:   %.2f\n", avg_hyper_pareto);
    printf("Moyenne temps scalaire:       %.3f s\n", avg_time_scalar);
    printf("Moyenne temps Pareto:         %.3f s\n", avg_time_pareto);
    printf("Moyenne solutions scalaire:   %.1f\n", avg_sol_scalar);
    printf("Moyenne solutions Pareto:     %.1f\n", avg_sol_pareto);
    
    if (avg_hyper_scalar > avg_hyper_pareto) {
        printf("\nConclusion: Approche scalaire en moyenne meilleure de %.1f%%\n", 
               (avg_hyper_scalar - avg_hyper_pareto) / avg_hyper_pareto * 100);
    } else {
        printf("\nConclusion: Approche Pareto en moyenne meilleure de %.1f%%\n", 
               (avg_hyper_pareto - avg_hyper_scalar) / avg_hyper_scalar * 100);
    }
    
    // Sauvegarde des statistiques globales
    char chemin[512];
    snprintf(chemin, sizeof(chemin), "%s/statistiques_globales.txt", config->output_dir);
    
    FILE *file = fopen(chemin, "w");
    if (file) {
        fprintf(file, "# Statistiques globales (%d runs)\n", nb_runs);
        fprintf(file, "Hypervolume_scalaire_moyen: %.2f\n", avg_hyper_scalar);
        fprintf(file, "Hypervolume_pareto_moyen: %.2f\n", avg_hyper_pareto);
        fprintf(file, "Temps_scalaire_moyen: %.3f\n", avg_time_scalar);
        fprintf(file, "Temps_pareto_moyen: %.3f\n", avg_time_pareto);
        fprintf(file, "Solutions_scalaire_moyen: %.1f\n", avg_sol_scalar);
        fprintf(file, "Solutions_pareto_moyen: %.1f\n", avg_sol_pareto);
        fclose(file);
    }
}
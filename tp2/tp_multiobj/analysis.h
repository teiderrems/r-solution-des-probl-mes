/**
 * @file analysis.h
 * @brief Fonctions d'analyse et de statistiques
 */

#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "archive.h"
#include "config.h"

/**
 * @struct Statistics
 * @brief Structure pour stocker les statistiques d'exécution
 */
typedef struct {
    double hypervolume_scalar;
    double hypervolume_pareto;
    int solutions_scalar;
    int solutions_pareto;
    double time_scalar;
    double time_pareto;
} Statistics;

/**
 * @brief Calcule l'hypervolume d'un front d'approximation
 * 
 * @param archive Archive contenant le front d'approximation
 * @param ref_makespan Valeur de référence pour le makespan
 * @param ref_tardiness Valeur de référence pour la tardiness
 * @return double Valeur de l'hypervolume
 */
double calculer_hypervolume(Archive *archive, double ref_makespan, double ref_tardiness);

/**
 * @brief Sauvegarde un front Pareto dans un fichier
 * 
 * @param filename Nom du fichier de sortie
 * @param archive Archive contenant le front à sauvegarder
 * @param config Pointeur vers la configuration du programme
 */
void sauvegarder_front(char *filename, Archive *archive, Config *config);

/**
 * @brief Sauvegarde les statistiques d'une exécution dans un fichier
 * 
 * @param stats Pointeur vers la structure de statistiques
 * @param run Numéro de l'exécution (pour le nom de fichier)
 * @param config Pointeur vers la configuration du programme
 */
void sauvegarder_statistiques(Statistics *stats, int run, Config *config);

/**
 * @brief Affiche les statistiques globales de plusieurs exécutions
 * 
 * @param all_stats Tableau de statistiques
 * @param nb_runs Nombre d'exécutions
 * @param config Pointeur vers la configuration du programme
 */
void afficher_statistiques_globales(Statistics *all_stats, int nb_runs, Config *config);

#endif // ANALYSIS_H
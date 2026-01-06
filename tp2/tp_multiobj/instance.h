/**
 * @file instance.h
 * @brief Structures et fonctions pour la gestion des instances
 */

#ifndef INSTANCE_H
#define INSTANCE_H

/**
 * @struct Job
 * @brief Structure représentant un job avec ses caractéristiques
 */
typedef struct {
    int id;
    int due_date;
    int *processing_times;
} Job;

/**
 * @struct Instance
 * @brief Structure représentant une instance complète du problème
 */
typedef struct {
    int nb_jobs;
    int nb_machines;
    Job *jobs;
} Instance;

/**
 * @brief Lit une instance de problème depuis un fichier
 * 
 * @param filename Chemin vers le fichier d'instance
 * @return Instance* Pointeur vers l'instance créée, NULL en cas d'erreur
 */
Instance* lire_instance(const char *filename);

/**
 * @brief Libère la mémoire allouée pour une instance
 * 
 * @param instance Pointeur vers l'instance à libérer
 */
void liberer_instance(Instance *instance);

#endif // INSTANCE_H
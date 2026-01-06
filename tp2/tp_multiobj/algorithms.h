/**
 * @file algorithms.h
 * @brief Algorithmes d'optimisation multi-objectif
 */

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "instance.h"
#include "archive.h"
#include "config.h"
#include<string.h>

/**
 * @brief Approche scalaire par somme pondérée
 * 
 * @param instance Pointeur vers l'instance du problème
 * @param nb_poids Nombre de vecteurs de poids à tester
 * @param max_iterations Nombre maximal d'itérations par recherche locale
 * @param config Pointeur vers la configuration du programme
 * @return Archive* Archive contenant les solutions trouvées
 */
Archive* approche_scalaire(Instance *instance, int nb_poids, int max_iterations, Config *config);

/**
 * @brief Approche Pareto par recherche locale
 * 
 * @param instance Pointeur vers l'instance du problème
 * @param taille_initiale Nombre de solutions initiales aléatoires
 * @param max_iterations Nombre maximal d'itérations de recherche
 * @param config Pointeur vers la configuration du programme
 * @return Archive* Archive contenant les solutions trouvées
 */
Archive* approche_pareto(Instance *instance, int taille_initiale, int max_iterations, Config *config);

/**
 * @brief Exécute une expérimentation complète
 * 
 * @param instance Pointeur vers l'instance
 * @param config Pointeur vers la configuration
 */
void executer_experimentation(Instance *instance, Config *config);

#endif // ALGORITHMS_H
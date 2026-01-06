/**
 * @file evaluation.h
 * @brief Fonctions d'évaluation des solutions
 */

#ifndef EVALUATION_H
#define EVALUATION_H

#include "instance.h"

/**
 * @struct ObjectiveVector
 * @brief Structure représentant un vecteur d'objectifs
 */
typedef struct {
    int makespan;
    int tardiness;
} ObjectiveVector;

/**
 * @brief Calcule le makespan (Cmax) d'une permutation de jobs
 * 
 * @param instance Pointeur vers l'instance du problème
 * @param permutation Tableau représentant l'ordre de traitement des jobs
 * @return int Valeur du makespan
 */
int calculer_makespan(Instance *instance, int *permutation);

/**
 * @brief Calcule la somme des retards (tardiness) d'une permutation
 * 
 * @param instance Pointeur vers l'instance du problème
 * @param permutation Tableau représentant l'ordre de traitement des jobs
 * @return int Somme des retards
 */
int calculer_tardiness(Instance *instance, int *permutation);

/**
 * @brief Évalue une solution pour les deux objectifs
 * 
 * @param instance Pointeur vers l'instance du problème
 * @param permutation Tableau représentant l'ordre de traitement des jobs
 * @return ObjectiveVector Structure contenant les deux valeurs d'objectif
 */
ObjectiveVector evaluer_solution(Instance *instance, int *permutation);

/**
 * @brief Génère une permutation aléatoire des jobs
 * 
 * @param instance Pointeur vers l'instance du problème
 * @return int* Tableau alloué dynamiquement contenant la permutation
 */
int* generer_solution_aleatoire(Instance *instance);

#endif // EVALUATION_H
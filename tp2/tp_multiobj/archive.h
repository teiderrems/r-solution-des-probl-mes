/**
 * @file archive.h
 * @brief Structures et fonctions pour la gestion des archives
 */

#ifndef ARCHIVE_H
#define ARCHIVE_H

#include "evaluation.h"

/**
 * @struct Archive
 * @brief Structure représentant une archive de solutions non-dominées
 */
typedef struct {
    ObjectiveVector *vectors;
    int size;
    int capacity;
} Archive;

/**
 * @brief Teste si un vecteur domine un autre au sens de Pareto
 * 
 * @param a Premier vecteur d'objectifs
 * @param b Second vecteur d'objectifs
 * @return int 1 si a domine b, 0 sinon
 */
int domine(ObjectiveVector a, ObjectiveVector b);

/**
 * @brief Filtrage offline d'un ensemble de solutions
 * 
 * @param solutions Tableau de vecteurs d'objectifs à filtrer
 * @param nb_solutions Nombre de solutions dans le tableau
 * @return Archive* Archive contenant les solutions non-dominées
 */
Archive* filtrage_offline(ObjectiveVector *solutions, int nb_solutions);

/**
 * @brief Filtrage online pour mise à jour d'archive
 * 
 * @param archive Pointeur vers l'archive à mettre à jour
 * @param nouvelle Nouveau vecteur d'objectifs à considérer
 */
void filtrage_online(Archive *archive, ObjectiveVector nouvelle);

/**
 * @brief Crée une nouvelle archive vide
 * 
 * @param capacity Capacité initiale de l'archive
 * @return Archive* Nouvelle archive vide
 */
Archive* creer_archive(int capacity);

/**
 * @brief Libère la mémoire allouée pour une archive
 * 
 * @param archive Pointeur vers l'archive à libérer
 */
void liberer_archive(Archive *archive);

#endif // ARCHIVE_H
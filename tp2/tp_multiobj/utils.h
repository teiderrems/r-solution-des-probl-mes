/**
 * @file utils.h
 * @brief Fonctions utilitaires diverses
 */

#ifndef UTILS_H
#define UTILS_H

/**
 * @brief Échange deux éléments dans un tableau
 * 
 * @param a Premier élément
 * @param b Second élément
 */
void swap(int *a, int *b);

/**
 * @brief Copie une permutation
 * 
 * @param source Permutation source
 * @param taille Taille de la permutation
 * @return int* Nouvelle permutation copiée
 */
int* copier_permutation(int *source, int taille);

/**
 * @brief Affiche une permutation
 * 
 * @param permutation Permutation à afficher
 * @param taille Taille de la permutation
 */
void afficher_permutation(int *permutation, int taille);

#endif // UTILS_H
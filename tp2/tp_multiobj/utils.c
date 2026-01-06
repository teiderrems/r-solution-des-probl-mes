/**
 * @file utils.c
 * @brief Implémentation des fonctions utilitaires
 */

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int* copier_permutation(int *source, int taille) {
    int *copie = (int*)malloc(taille * sizeof(int));
    for (int i = 0; i < taille; i++) {
        copie[i] = source[i];
    }
    return copie;
}

void afficher_permutation(int *permutation, int taille) {
    printf("[");
    for (int i = 0; i < taille; i++) {
        printf("%d", permutation[i]);
        if (i < taille - 1) printf(", ");
    }
    printf("]\n");
}
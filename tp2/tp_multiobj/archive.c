/**
 * @file archive.c
 * @brief Implémentation de la gestion des archives
 */

#include "archive.h"
#include <stdlib.h>

int domine(ObjectiveVector a, ObjectiveVector b) {
    return (a.makespan <= b.makespan && a.tardiness <= b.tardiness) &&
           (a.makespan < b.makespan || a.tardiness < b.tardiness);
}

Archive* filtrage_offline(ObjectiveVector *solutions, int nb_solutions) {
    Archive *archive = (Archive*)malloc(sizeof(Archive));
    archive->capacity = nb_solutions;
    archive->vectors = (ObjectiveVector*)malloc(nb_solutions * sizeof(ObjectiveVector));
    archive->size = 0;
    
    int *est_domine = (int*)calloc(nb_solutions, sizeof(int));
    
    for (int i = 0; i < nb_solutions; i++) {
        if (est_domine[i]) continue;
        
        for (int j = 0; j < nb_solutions; j++) {
            if (i == j || est_domine[j]) continue;
            
            if (domine(solutions[i], solutions[j])) {
                est_domine[j] = 1;
            } else if (domine(solutions[j], solutions[i])) {
                est_domine[i] = 1;
                break;
            }
        }
        
        if (!est_domine[i]) {
            archive->vectors[archive->size] = solutions[i];
            archive->size++;
        }
    }
    
    free(est_domine);
    return archive;
}

void filtrage_online(Archive *archive, ObjectiveVector nouvelle) {
    // Vérifier si la nouvelle solution est dominée
    for (int i = 0; i < archive->size; i++) {
        if (domine(archive->vectors[i], nouvelle)) {
            return;
        }
    }
    
    // Supprimer les solutions dominées par la nouvelle
    int i = 0;
    while (i < archive->size) {
        if (domine(nouvelle, archive->vectors[i])) {
            for (int j = i; j < archive->size - 1; j++) {
                archive->vectors[j] = archive->vectors[j + 1];
            }
            archive->size--;
        } else {
            i++;
        }
    }
    
    // Ajouter la nouvelle solution
    if (archive->size >= archive->capacity) {
        archive->capacity *= 2;
        archive->vectors = (ObjectiveVector*)realloc(archive->vectors, 
                                                    archive->capacity * sizeof(ObjectiveVector));
    }
    archive->vectors[archive->size] = nouvelle;
    archive->size++;
}

Archive* creer_archive(int capacity) {
    Archive *archive = (Archive*)malloc(sizeof(Archive));
    archive->capacity = capacity;
    archive->size = 0;
    archive->vectors = (ObjectiveVector*)malloc(capacity * sizeof(ObjectiveVector));
    return archive;
}

void liberer_archive(Archive *archive) {
    if (archive) {
        free(archive->vectors);
        free(archive);
    }
}
#ifndef ___FLOWSHOP___
#define ___FLOWSHOP___

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

/**
 * @brief Structure représentant une instance du problème Flow Shop de permutation.
 * 
 * - nb_job : nombre de jobs (n)
 * - nb_machine : nombre de machines (m)
 * - seed : graine utilisée pour la génération (à ignorer, mais présente dans le fichier)
 * - processing_times[i][j] : temps de traitement du job i sur la machine j
 */
typedef struct {
    long int seed;
    int nb_job;
    int nb_machine;
    int **processing_times;
} Instance;



typedef struct {
    int a, b;
} Pair;

/**
 * @brief Structure de configuration pour le programme.
 */
typedef struct {
    char instance_file[256];
    int k_executions;
    int max_evals;
    char output_dir[256];
    long seed;
    int verbose;
} Config;

/* =============== Chargement et gestion mémoire =============== */

/**
 * @brief Charge une instance depuis un fichier au format Taillard.
 * 
 * Le format attendu est :
 *   nb_job
 *   nb_machine
 *   seed
 *   job_id
 *   valeur_à_ignorer
 *   t_i0 t_i1 ... t_i(m-1)
 *   ...
 * 
 * @param path Chemin vers le fichier d'instance.
 * @return Pointeur vers l'instance chargée, ou NULL en cas d'erreur.
 */
Instance* load_instance(const char* path);

/**
 * @brief Affiche une instance (utile pour le débogage).
 * 
 * @param inst Pointeur vers l'instance à afficher.
 */
void afficher(Instance* inst);

/**
 * @brief Affiche une solution (utile pour voir l'aperçu de la solution).
 * 
 * @param inst Pointeur vers une solution.
 */
void afficher_solution(int* sol,int n,int cost);

/**
 * @brief Libère toute la mémoire allouée pour une instance.
 * 
 * @param inst Pointeur vers l'instance à libérer.
 */
void free_memory(Instance* inst);

/* =============== Génération de solutions =============== */

/**
 * @brief Génère une solution valide aléatoire (permutation des jobs 0..n-1).
 * 
 * @param n Nombre de jobs.
 * @return Tableau de n entiers représentant une permutation aléatoire, ou NULL si échec.
 */
int* generate_valid_solution(int n);

/* =============== Évaluation =============== */

/**
 * @brief Calcule le makespan (Cmax) d'une solution donnée.
 * 
 * Le makespan est la date de fin du dernier job sur la dernière machine.
 * 
 * @param inst Instance du problème.
 * @param solution Permutation des jobs (tableau de taille inst->nb_job).
 * @return Valeur du makespan.
 */
int compute_makespan(Instance* inst, int* solution);

/* =============== Opérateurs de voisinage =============== */

/**
 * @brief Échange deux jobs aux positions i et j dans la solution.
 * 
 * @param sol Tableau de jobs (permutation).
 * @param i Première position.
 * @param j Deuxième position.
 */
void echange(int* sol, int i, int j);

/**
 * @brief Insère le job à la position 'from' à la position 'to'.
 * 
 * Tous les jobs entre les deux positions sont décalés.
 * 
 * @param sol Tableau de jobs (permutation).
 * @param n Taille de la solution (nombre de jobs).
 * @param from Position initiale du job à déplacer.
 * @param to Nouvelle position du job.
 */
void insere(int* sol, int n, int from, int to);

/* =============== Algorithmes de recherche =============== */

/**
 * @brief Marche aléatoire avec un opérateur de voisinage donné.
 * 
 * À chaque étape, un voisin est généré aléatoirement (swap ou insert selon 'use_insert').
 * La meilleure solution rencontrée est retournée.
 * 
 * @param inst Instance du problème.
 * @param max_evals Nombre maximal d'évaluations de solutions.
 * @param best_cost Pointeur pour retourner le coût de la meilleure solution.
 * @param use_insert Si 1, utilise 'insere', sinon utilise 'echange'.
 * @return Meilleure solution trouvée.
 */
int* marche_aleatoire(Instance* inst, int max_evals, int* best_cost, int use_insert);

/**
 * @brief Hill Climber "first improvement" avec voisinage aléatoire.
 * 
 * Génère les voisins dans un ordre aléatoire (sans remise) et s'arrête dès qu'une amélioration est trouvée.
 * 
 * @param inst Instance du problème.
 * @param max_evals Nombre maximal d'évaluations.
 * @param best_cost Pointeur pour retourner le coût final.
 * @param use_insert Si 1, utilise 'insere', sinon 'echange'.
 * @return Solution locale optimale.
 */
int* climber_first(Instance* inst, int max_evals, int* best_cost, int use_insert);

/**
 * @brief Hill Climber "best improvement".
 * 
 * Évalue tous les voisins possibles (dans un ordre aléatoire) et choisit le meilleur.
 * 
 * @param inst Instance du problème.
 * @param max_evals Nombre maximal d'évaluations.
 * @param best_cost Pointeur pour retourner le coût final.
 * @param use_insert Si 1, utilise 'insere', sinon 'echange'.
 * @return Solution locale optimale.
 */
int* climber_best(Instance* inst, int max_evals, int* best_cost, int use_insert);

/**
 * @brief Algorithme personnalisé : Random Restart Hill Climbing (first improvement).
 * 
 * Relance le hill climber plusieurs fois jusqu'à épuisement du budget d'évaluations.
 * 
 * @param inst Instance du problème.
 * @param max_evals Nombre maximal d'évaluations.
 * @param best_cost Pointeur pour retourner le coût de la meilleure solution.
 * @param use_insert Si 1, utilise 'insere', sinon 'echange'.
 * @return Meilleure solution trouvée.
 */
int* algorithme_perso(Instance* inst, int max_evals, int* best_cost, int use_insert);

/**
 * @brief Algorithme personnalisé : Réutilisation de la solution Hill Climbing (first improvement).
 * 
 * Relance le hill climber plusieurs fois jusqu'à épuisement du budget d'évaluations.
 * 
 * @param inst Instance du problème.
 * @param max_evals Nombre maximal d'évaluations.
 * @param best_cost Pointeur pour retourner le coût de la meilleure solution.
 * @param use_insert Si 1, utilise 'insere', sinon 'echange'.
 * @return Meilleure solution trouvée.
 */
int* algorithme_perso_2(Instance* inst,int max_evals, int* best_cost, int use_insert);

#endif
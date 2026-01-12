/**
 * @file multi_objective_fsp.h
 * @brief Header principal pour le TP d'optimisation multi-objectif FSP
 * @details Contient toutes les déclarations de structures et fonctions
 */

#ifndef MULTI_OBJECTIVE_FSP_H
#define MULTI_OBJECTIVE_FSP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <linux/limits.h>
#include <dirent.h>

/* === CONSTANTES === */

#define DEFAULT_OFFLINE_SOLUTIONS 500    /**< Nombre par défaut de solutions pour le filtrage offline */
#define DEFAULT_NB_POIDS 10              /**< Nombre par défaut de poids pour l'approche scalaire */
#define DEFAULT_MAX_ITERATIONS 500       /**< Nombre maximal d'itérations par défaut */
#define DEFAULT_PARETO_SIZE 10           /**< Taille initiale par défaut pour l'approche Pareto */
#define DEFAULT_NB_RUNS 1                /**< Nombre d'exécutions par défaut */
#define DEFAULT_ARCHIVE_CAPACITY 100     /**< Capacité initiale par défaut des archives */

/* === STRUCTURES DE DONNÉES === */

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
 * @struct ObjectiveVector
 * @brief Structure représentant un vecteur d'objectifs
 */
typedef struct {
    int makespan;
    int tardiness;
} ObjectiveVector;

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
 * @struct Config
 * @brief Structure de configuration du programme
 */
typedef struct {
    char instance_file[256];
    int offline_solutions;
    int nb_poids;
    int max_iterations;
    int pareto_size;
    int nb_runs;
    int verbose;
    int no_analyze;
    char output_dir[256];
    char algo_type[10];
    int seed;
    char input_dir[256];
    int experiment;
} Config;

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

/* === DÉCLARATIONS DES FONCTIONS === */

/* Gestion des instances */
/**
 * @brief Lit une instance depuis un fichier.
 * @param filename Chemin du fichier d'instance.
 * @return Pointeur vers l'instance allouée, ou NULL en cas d'erreur.
 */
Instance* lire_instance(const char *filename);

/**
 * @brief Libère la mémoire allouée pour une instance.
 * @param instance Pointeur vers l'instance à libérer.
 */
void liberer_instance(Instance *instance);

/* Évaluation des solutions */
/**
 * @brief Calcule le makespan (temps de fin maximal) d'une permutation.
 * @param instance Pointeur vers l'instance du problème.
 * @param permutation Tableau représentant l'ordre des jobs.
 * @return Valeur du makespan.
 */
int calculer_makespan(Instance *instance, int *permutation);

/**
 * @brief Calcule le retard total (tardiness) d'une permutation.
 * @param instance Pointeur vers l'instance du problème.
 * @param permutation Tableau représentant l'ordre des jobs.
 * @return Valeur du retard total.
 */
int calculer_tardiness(Instance *instance, int *permutation);

/**
 * @brief Évalue une solution en retournant son vecteur d'objectifs.
 * @param instance Pointeur vers l'instance du problème.
 * @param permutation Tableau représentant l'ordre des jobs.
 * @return Vecteur d'objectifs (makespan, tardiness).
 */
ObjectiveVector evaluer_solution(Instance *instance, int *permutation);

/**
 * @brief Génère une permutation aléatoire de jobs.
 * @param nb_jobs Nombre de jobs.
 * @return Tableau alloué contenant la permutation (à libérer par l'appelant).
 */
int* generer_solution_aleatoire(int nb_jobs);

/* Gestion des archives */
/**
 * @brief Teste si le vecteur d'objectifs @p a domine @p b.
 * @param a Premier vecteur d'objectifs.
 * @param b Deuxième vecteur d'objectifs.
 * @return 1 si a domine b, 0 sinon.
 */
int domine(ObjectiveVector a, ObjectiveVector b);

/**
 * @brief Filtre un ensemble de solutions pour ne garder que les non-dominées (offline).
 * @param solutions Tableau de vecteurs d'objectifs.
 * @param nb_solutions Nombre d'éléments dans @p solutions.
 * @return Archive contenant les solutions non-dominées (allouée).
 */
Archive* filtrage_offline(ObjectiveVector *solutions, int nb_solutions);

/**
 * @brief Ajoute une nouvelle solution dans une archive en filtrant en ligne (online).
 * @param archive Pointeur vers l'archive à mettre à jour.
 * @param nouvelle Vecteur d'objectifs de la nouvelle solution.
 */
void filtrage_online(Archive *archive, ObjectiveVector nouvelle);

/**
 * @brief Crée une archive vide avec une capacité initiale donnée.
 * @param capacity Capacité initiale.
 * @return Pointeur vers l'archive allouée.
 */
Archive* creer_archive(int capacity);

/**
 * @brief Libère la mémoire associée à une archive.
 * @param archive Pointeur vers l'archive à libérer.
 */
void liberer_archive(Archive *archive);

/* Algorithmes d'optimisation */
/**
 * @brief Approche scalaire multipondérée pour générer un front approximatif.
 * @param instance Pointeur vers l'instance du problème.
 * @param nb_poids Nombre de poids scalaires à tester.
 * @param max_iterations Nombre maximal d'itérations par poids.
 * @param config Pointeur vers la configuration d'exécution.
 * @return Archive contenant les solutions trouvées.
 */
Archive* approche_scalaire(Instance *instance, int nb_poids, int max_iterations, Config *config);

/**
 * @brief Approche basée sur un maintien d'une archive Pareto (multi-objectif).
 * @param instance Pointeur vers l'instance du problème.
 * @param taille_initiale Taille initiale de la population/archive.
 * @param max_iterations Nombre maximal d'itérations.
 * @param config Pointeur vers la configuration d'exécution.
 * @return Archive contenant les solutions non-dominées trouvées.
 */
Archive* approche_pareto(Instance *instance, int taille_initiale, int max_iterations, Config *config);

/**
 * @brief Lance les expérimentations (exécutions multiples, sauvegardes, analyses).
 * @param instance Pointeur vers l'instance du problème.
 * @param config Pointeur vers la configuration d'exécution.
 */
void executer_experimentation(Instance *instance, Config *config);

/* Analyse et statistiques */
/**
 * @brief Calcule l'hypervolume d'une archive par rapport à une référence.
 * @param archive Archive contenant les points sur le front.
 * @param ref_makespan Valeur de référence pour le makespan.
 * @param ref_tardiness Valeur de référence pour le tardiness.
 * @return Valeur de l'hypervolume.
 */
double calculer_hypervolume(Archive *archive, double ref_makespan, double ref_tardiness);

/**
 * @brief Sauvegarde un front (archive) dans un fichier.
 * @param filename Chemin du fichier de sortie.
 * @param archive Archive contenant le front.
 * @param config Pointeur vers la configuration d'exécution.
 */
void sauvegarder_front(char *filename, Archive *archive, Config *config);

/**
 * @brief Sauvegarde les statistiques d'une exécution.
 * @param stats Pointeur vers la structure de statistiques.
 * @param run Indice de l'exécution (run).
 * @param config Pointeur vers la configuration d'exécution.
 */
void sauvegarder_statistiques(Statistics *stats, int run, Config *config);

/**
 * @brief Affiche des statistiques agrégées pour plusieurs runs.
 * @param all_stats Tableau de statistiques pour chaque run.
 * @param nb_runs Nombre de runs.
 * @param config Pointeur vers la configuration d'exécution.
 */
void afficher_statistiques_globales(Statistics *all_stats, int nb_runs, Config *config);

/* Configuration */
/**
 * @brief Affiche l'aide/usage du programme sur stdout.
 */
void afficher_aide();

/**
 * @brief Parse les arguments passés en ligne de commande.
 * @param argc Nombre d'arguments.
 * @param argv Tableau d'arguments.
 * @param config Pointeur vers la structure de configuration à remplir.
 * @return 0 si succès, non-zéro sinon.
 */
int parse_arguments(int argc, char *argv[], Config *config);

/**
 * @brief Affiche la configuration courante.
 * @param config Pointeur vers la configuration à afficher.
 */
void afficher_configuration(Config *config);

/* Utilitaires */
/**
 * @brief Échange deux entiers.
 * @param a Pointeur vers le premier entier.
 * @param b Pointeur vers le second entier.
 */
void swap(int *a, int *b);

/**
 * @brief Copie une permutation d'entiers.
 * @param source Tableau source.
 * @param taille Taille du tableau.
 * @return Nouveau tableau alloué contenant la copie (à libérer par l'appelant).
 */
int* copier_permutation(int *source, int taille);

/**
 * @brief Affiche une permutation sur stdout.
 * @param permutation Tableau de la permutation.
 * @param taille Taille du tableau.
 */
void afficher_permutation(int *permutation, int taille);

/**
 * @brief Teste si un fichier existe.
 * @param chemin Chemin vers le fichier.
 * @return 1 si le fichier existe, 0 sinon.
 */
int fichier_existe(const char *chemin);

/* Utilitaires pour les noms de fichiers */
/**
 * @brief Extrait le nom de base d'un fichier (sans chemin et sans extension).
 * @param chemin Chemin complet du fichier.
 * @return Pointeur vers le nom de base.
 */
char* extraire_nom_base(const char *chemin);

/**
 * @brief Construit un préfixe pour les noms de fichiers de résultats.
 * @param instance_file Nom du fichier d'instance.
 * @param iterations Nombre d'itérations.
 * @param pareto_size Taille de l'approche Pareto.
 * @param prefix Tableau où stocker le préfixe.
 * @param size Taille du tableau.
 */
void construire_prefixe_fichier(const char *instance_file, int iterations, 
                               int pareto_size, char *prefix, size_t size);

/* Analyse graphique */
/**
 * @brief Génère les scripts Gnuplot nécessaires pour visualiser les résultats.
 * @param config Pointeur vers la configuration d'exécution.
 * @param nb_runs Nombre de runs à inclure.
 */
void generer_scripts_gnuplot(Config *config, int nb_runs);

/**
 * @brief Lit un front (fichier) et le convertit en Archive.
 * @param filename Chemin du fichier contenant le front.
 * @return Archive allouée contenant les points lus.
 */
Archive* lire_front(const char *filename);

/**
 * @brief Génère un graphique d'évolution de l'hypervolume.
 * @param stats Tableau de statistiques contenant les hypervolumes.
 * @param nb_runs Nombre de runs.
 * @param config Pointeur vers la configuration d'exécution.
 */
void generer_graphique_hypervolume(Statistics *stats, int nb_runs, Config *config);

/**
 * @brief Procédure d'analyse visuelle des fronts produits.
 * @param config Pointeur vers la configuration d'exécution.
 * @param nb_runs Nombre de runs à analyser.
 * @param all_stats Tableau de statistiques pour chaque run.
 */
void analyser_fronts_visuellement(Config *config, int nb_runs, Statistics *all_stats);

#endif // MULTI_OBJECTIVE_FSP_H
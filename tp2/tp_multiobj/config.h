/**
 * @file config.h
 * @brief Gestion de la configuration du programme
 */

#ifndef CONFIG_H
#define CONFIG_H

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
} Config;

/**
 * @brief Affiche le message d'aide avec les options disponibles
 */
void afficher_aide();

/**
 * @brief Parse les arguments de la ligne de commande
 * 
 * @param argc Nombre d'arguments
 * @param argv Tableau des arguments (comme dans main)
 * @param config Pointeur vers la structure de configuration à remplir
 * @return int 0 en cas de succès, 1 en cas d'erreur (arguments invalides)
 */
int parse_arguments(int argc, char *argv[], Config *config);

/**
 * @brief Affiche la configuration actuelle du programme
 * 
 * @param config Pointeur vers la structure de configuration
 */
void afficher_configuration(Config *config);

#endif // CONFIG_H
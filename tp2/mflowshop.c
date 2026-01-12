/**
 * @file main.c
 * @brief Programme principal d'optimisation multi-objectif FSP
 * @details Contient toutes les implémentations des fonctions du TP
 */

#include "mflowshop.h"

/* === FONCTIONS UTILITAIRES POUR LES NOMS DE FICHIERS === */

char *extraire_nom_base(const char *chemin)
{
    char *nom_complet = strrchr(chemin, '/');
    if (nom_complet == NULL)
    {
        nom_complet = (char *)chemin;
    }
    else
    {
        nom_complet++; // Passer le '/'
    }

    // Supprimer l'extension .txt si présente
    char *point = strrchr(nom_complet, '.');
    if (point != NULL && strcmp(point, ".txt") == 0)
    {
        *point = '\0';
    }

    // Supprimer d'autres extensions courantes
    point = strrchr(nom_complet, '.');
    if (point != NULL && (strcmp(point, ".dat") == 0 || strcmp(point, ".out") == 0))
    {
        *point = '\0';
    }

    return nom_complet;
}

void construire_prefixe_fichier(const char *instance_file, int iterations,
                                int pareto_size, char *prefix, size_t size)
{
    char nom_base[256];
    strncpy(nom_base, extraire_nom_base(instance_file), sizeof(nom_base) - 1);
    nom_base[sizeof(nom_base) - 1] = '\0';

    // Remplacer les caractères spéciaux par des underscores
    for (int i = 0; nom_base[i]; i++)
    {
        if (nom_base[i] == '/' || nom_base[i] == '\\' || nom_base[i] == ':' ||
            nom_base[i] == '*' || nom_base[i] == '?' || nom_base[i] == '"' ||
            nom_base[i] == '<' || nom_base[i] == '>' || nom_base[i] == '|')
        {
            nom_base[i] = '_';
        }
    }

    snprintf(prefix, size, "%s_i%d_p%d", nom_base, iterations, pareto_size);
}

/* === FONCTIONS DE GESTION DES INSTANCES === */

Instance *lire_instance(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    Instance *instance = (Instance *)malloc(sizeof(Instance));
    if (!instance)
    {
        fclose(file);
        printf("Erreur: allocation mémoire échouée\n");
        return NULL;
    }

    // Lire le nombre de jobs
    if (fscanf(file, "%d", &instance->nb_jobs) != 1)
    {
        printf("Erreur: lecture du nombre de jobs échouée\n");
        fclose(file);
        free(instance);
        return NULL;
    }

    // Lire le nombre de machines
    if (fscanf(file, "%d", &instance->nb_machines) != 1)
    {
        printf("Erreur: lecture du nombre de machines échouée\n");
        fclose(file);
        free(instance);
        return NULL;
    }

    int seed;
    if (fscanf(file, "%d", &seed) != 1)
    { // Seed qu'on ignore
        printf("Erreur: lecture de la seed échouée\n");
        fclose(file);
        free(instance);
        return NULL;
    }

    // Allouer le tableau de jobs
    instance->jobs = (Job *)malloc(instance->nb_jobs * sizeof(Job));
    if (!instance->jobs)
    {
        printf("Erreur: allocation mémoire pour les jobs échouée\n");
        fclose(file);
        free(instance);
        return NULL;
    }

    // Initialiser tous les pointeurs à NULL pour pouvoir les libérer en cas d'erreur
    for (int i = 0; i < instance->nb_jobs; i++)
    {
        instance->jobs[i].processing_times = NULL;
    }

    // Lire les données pour chaque job
    for (int i = 0; i < instance->nb_jobs; i++)
    {
        instance->jobs[i].processing_times = (int *)malloc(instance->nb_machines * sizeof(int));
        if (!instance->jobs[i].processing_times)
        {
            printf("Erreur: allocation mémoire pour les temps de traitement échouée\n");
            // Libérer la mémoire allouée jusqu'ici
            for (int j = 0; j <= i; j++)
            {
                if (instance->jobs[j].processing_times)
                {
                    free(instance->jobs[j].processing_times);
                }
            }
            free(instance->jobs);
            fclose(file);
            free(instance);
            return NULL;
        }

        int id;
        if (fscanf(file, "%d", &id) != 1)
        {
            printf("Erreur: lecture de l'ID du job %d échouée\n", i);
            // Libérer la mémoire
            for (int j = 0; j <= i; j++)
            {
                free(instance->jobs[j].processing_times);
            }
            free(instance->jobs);
            fclose(file);
            free(instance);
            return NULL;
        }

        if (fscanf(file, "%d", &instance->jobs[i].due_date) != 1)
        {
            printf("Erreur: lecture de la due date du job %d échouée\n", i);
            // Libérer la mémoire
            for (int j = 0; j <= i; j++)
            {
                free(instance->jobs[j].processing_times);
            }
            free(instance->jobs);
            fclose(file);
            free(instance);
            return NULL;
        }

        for (int j = 0; j < instance->nb_machines; j++)
        {
            if (fscanf(file, "%d", &instance->jobs[i].processing_times[j]) != 1)
            {
                printf("Erreur: lecture du temps de traitement %d du job %d échouée\n", j, i);
                // Libérer la mémoire
                for (int k = 0; k <= i; k++)
                {
                    free(instance->jobs[k].processing_times);
                }
                free(instance->jobs);
                fclose(file);
                free(instance);
                return NULL;
            }
        }
    }

    fclose(file);
    return instance;
}

void liberer_instance(Instance *instance)
{
    if (instance)
    {
        for (int i = 0; i < instance->nb_jobs; i++)
        {
            free(instance->jobs[i].processing_times);
        }
        free(instance->jobs);
        free(instance);
    }
}

/* === FONCTIONS D'ÉVALUATION === */

int calculer_makespan(Instance *instance, int *permutation)
{
    int nb_jobs = instance->nb_jobs;
    int nb_machines = instance->nb_machines;

    // Allouer et initialiser le tableau à 0
    int **completion_times = (int **)calloc(nb_jobs, sizeof(int *));
    if (!completion_times)
    {
        printf("Erreur: allocation mémoire pour completion_times échouée\n");
        return -1;
    }

    for (int i = 0; i < nb_jobs; i++)
    {
        completion_times[i] = (int *)calloc(nb_machines, sizeof(int));
        if (!completion_times[i])
        {
            printf("Erreur: allocation mémoire pour completion_times[%d] échouée\n", i);
            // Libérer la mémoire allouée jusqu'ici
            for (int j = 0; j < i; j++)
            {
                free(completion_times[j]);
            }
            free(completion_times);
            return -1;
        }
    }

    // Initialisation première job
    completion_times[0][0] = instance->jobs[permutation[0]].processing_times[0];
    for (int m = 1; m < nb_machines; m++)
    {
        completion_times[0][m] = completion_times[0][m - 1] + instance->jobs[permutation[0]].processing_times[m];
    }

    // Calcul pour les autres jobs
    for (int i = 1; i < nb_jobs; i++)
    {
        completion_times[i][0] = completion_times[i - 1][0] + instance->jobs[permutation[i]].processing_times[0];

        for (int m = 1; m < nb_machines; m++)
        {
            int max_val = (completion_times[i - 1][m] > completion_times[i][m - 1]) ? completion_times[i - 1][m] : completion_times[i][m - 1];
            completion_times[i][m] = max_val + instance->jobs[permutation[i]].processing_times[m];
        }
    }

    int makespan = completion_times[nb_jobs - 1][nb_machines - 1];

    // Libération mémoire
    for (int i = 0; i < nb_jobs; i++)
    {
        free(completion_times[i]);
    }
    free(completion_times);

    return makespan;
}

int calculer_tardiness(Instance *instance, int *permutation)
{
    int nb_jobs = instance->nb_jobs;
    int nb_machines = instance->nb_machines;

    // Allouer et initialiser le tableau à 0
    int **completion_times = (int **)calloc(nb_jobs, sizeof(int *));
    if (!completion_times)
    {
        printf("Erreur: allocation mémoire pour completion_times échouée\n");
        return -1;
    }

    for (int i = 0; i < nb_jobs; i++)
    {
        completion_times[i] = (int *)calloc(nb_machines, sizeof(int));
        if (!completion_times[i])
        {
            printf("Erreur: allocation mémoire pour completion_times[%d] échouée\n", i);
            // Libérer la mémoire allouée jusqu'ici
            for (int j = 0; j < i; j++)
            {
                free(completion_times[j]);
            }
            free(completion_times);
            return -1;
        }
    }

    // Calcul des dates de fin
    completion_times[0][0] = instance->jobs[permutation[0]].processing_times[0];
    for (int m = 1; m < nb_machines; m++)
    {
        completion_times[0][m] = completion_times[0][m - 1] + instance->jobs[permutation[0]].processing_times[m];
    }

    for (int i = 1; i < nb_jobs; i++)
    {
        completion_times[i][0] = completion_times[i - 1][0] + instance->jobs[permutation[i]].processing_times[0];

        for (int m = 1; m < nb_machines; m++)
        {
            int max_val = (completion_times[i - 1][m] > completion_times[i][m - 1]) ? completion_times[i - 1][m] : completion_times[i][m - 1];
            completion_times[i][m] = max_val + instance->jobs[permutation[i]].processing_times[m];
        }
    }

    // Calcul de la tardiness
    int total_tardiness = 0;
    for (int i = 0; i < nb_jobs; i++)
    {
        int completion_time = completion_times[i][nb_machines - 1];
        int due_date = instance->jobs[permutation[i]].due_date;
        if (completion_time > due_date)
        {
            total_tardiness += (completion_time - due_date);
        }
    }

    // Libération mémoire
    for (int i = 0; i < nb_jobs; i++)
    {
        free(completion_times[i]);
    }
    free(completion_times);

    return total_tardiness;
}

ObjectiveVector evaluer_solution(Instance *instance, int *permutation)
{
    ObjectiveVector result = {0, 0};
    int nb_jobs = instance->nb_jobs;
    int nb_machines = instance->nb_machines;

    // Allouer et initialiser le tableau à 0
    int **completion_times = (int **)calloc(nb_jobs, sizeof(int *));
    if (!completion_times)
    {
        printf("Erreur: allocation mémoire pour completion_times échouée\n");
        result.makespan = -1;
        result.tardiness = -1;
        return result;
    }

    for (int i = 0; i < nb_jobs; i++)
    {
        completion_times[i] = (int *)calloc(nb_machines, sizeof(int));
        if (!completion_times[i])
        {
            printf("Erreur: allocation mémoire pour completion_times[%d] échouée\n", i);
            // Libérer la mémoire allouée jusqu'ici
            for (int j = 0; j < i; j++)
            {
                free(completion_times[j]);
            }
            free(completion_times);
            result.makespan = -1;
            result.tardiness = -1;
            return result;
        }
    }

    // Calculer les completion times
    completion_times[0][0] = instance->jobs[permutation[0]].processing_times[0];
    for (int m = 1; m < nb_machines; m++)
    {
        completion_times[0][m] = completion_times[0][m - 1] + instance->jobs[permutation[0]].processing_times[m];
    }

    for (int i = 1; i < nb_jobs; i++)
    {
        completion_times[i][0] = completion_times[i - 1][0] + instance->jobs[permutation[i]].processing_times[0];

        for (int m = 1; m < nb_machines; m++)
        {
            int max_val = (completion_times[i - 1][m] > completion_times[i][m - 1]) ? completion_times[i - 1][m] : completion_times[i][m - 1];
            completion_times[i][m] = max_val + instance->jobs[permutation[i]].processing_times[m];
        }
    }

    // Calculer le makespan (dernier job, dernière machine)
    result.makespan = completion_times[nb_jobs - 1][nb_machines - 1];

    // Calculer la tardiness
    result.tardiness = 0;
    for (int i = 0; i < nb_jobs; i++)
    {
        int completion_time = completion_times[i][nb_machines - 1];
        int due_date = instance->jobs[permutation[i]].due_date;
        if (completion_time > due_date)
        {
            result.tardiness += (completion_time - due_date);
        }
    }

    // Libération mémoire
    for (int i = 0; i < nb_jobs; i++)
    {
        free(completion_times[i]);
    }
    free(completion_times);

    return result;
}

int *generer_solution_aleatoire(int nb_jobs)
{
    int *permutation = (int *)malloc(nb_jobs * sizeof(int));

    for (int i = 0; i < nb_jobs; i++)
    {
        permutation[i] = i;
    }

    // Mélange Fisher-Yates
    for (int i = nb_jobs - 1; i > 0; i--)
    {
        int j = rand() % (i + 1);
        int temp = permutation[i];
        permutation[i] = permutation[j];
        permutation[j] = temp;
    }

    return permutation;
}

/* === GESTION DES ARCHIVES === */

int domine(ObjectiveVector a, ObjectiveVector b)
{
    return (a.makespan <= b.makespan && a.tardiness <= b.tardiness) &&
           (a.makespan < b.makespan || a.tardiness < b.tardiness);
}

Archive *filtrage_offline(ObjectiveVector *solutions, int nb_solutions)
{
    Archive *archive = (Archive *)malloc(sizeof(Archive));
    archive->capacity = nb_solutions;
    archive->vectors = (ObjectiveVector *)malloc(nb_solutions * sizeof(ObjectiveVector));
    archive->size = 0;

    int *est_domine = (int *)calloc(nb_solutions, sizeof(int));

    for (int i = 0; i < nb_solutions; i++)
    {
        if (est_domine[i])
            continue;

        for (int j = 0; j < nb_solutions; j++)
        {
            if (i == j || est_domine[j])
                continue;

            if (domine(solutions[i], solutions[j]))
            {
                est_domine[j] = 1;
            }
            else if (domine(solutions[j], solutions[i]))
            {
                est_domine[i] = 1;
                break;
            }
        }

        if (!est_domine[i])
        {
            archive->vectors[archive->size] = solutions[i];
            archive->size++;
        }
    }

    free(est_domine);
    return archive;
}

void filtrage_online(Archive *archive, ObjectiveVector nouvelle)
{
    // Vérifier si la nouvelle solution est dominée
    for (int i = 0; i < archive->size; i++)
    {
        if (domine(archive->vectors[i], nouvelle))
        {
            return;
        }
    }

    // Supprimer les solutions dominées par la nouvelle
    int i = 0;
    while (i < archive->size)
    {
        if (domine(nouvelle, archive->vectors[i]))
        {
            for (int j = i; j < archive->size - 1; j++)
            {
                archive->vectors[j] = archive->vectors[j + 1];
            }
            archive->size--;
        }
        else
        {
            i++;
        }
    }

    // Ajouter la nouvelle solution
    if (archive->size >= archive->capacity)
    {
        archive->capacity *= 2;
        archive->vectors = (ObjectiveVector *)realloc(archive->vectors,
                                                      archive->capacity * sizeof(ObjectiveVector));
    }
    archive->vectors[archive->size] = nouvelle;
    archive->size++;
}

Archive *creer_archive(int capacity)
{
    Archive *archive = (Archive *)malloc(sizeof(Archive));
    archive->capacity = capacity;
    archive->size = 0;
    archive->vectors = (ObjectiveVector *)malloc(capacity * sizeof(ObjectiveVector));
    return archive;
}

void liberer_archive(Archive *archive)
{
    if (archive)
    {
        free(archive->vectors);
        free(archive);
    }
}

/* === FONCTIONS UTILITAIRES === */

void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

int *copier_permutation(int *source, int taille)
{
    int *copie = (int *)malloc(taille * sizeof(int));
    for (int i = 0; i < taille; i++)
    {
        copie[i] = source[i];
    }
    return copie;
}

void afficher_permutation(int *permutation, int taille)
{
    printf("[");
    for (int i = 0; i < taille; i++)
    {
        printf("%d", permutation[i]);
        if (i < taille - 1)
            printf(", ");
    }
    printf("]\n");
}

int fichier_existe(const char *chemin)
{
    FILE *file = fopen(chemin, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

/* === ALGORITHMES D'OPTIMISATION === */

/* Prototypes des fonctions internes */
ObjectiveVector recherche_locale_scalaire(Instance *instance, int *permutation,
                                          double poids_makespan, double poids_tardiness,
                                          int max_iterations);

/**
 * @brief Recherche locale pour une fonction scalaire
 */
ObjectiveVector recherche_locale_scalaire(Instance *instance, int *permutation,
                                          double poids_makespan, double poids_tardiness,
                                          int max_iterations)
{
    ObjectiveVector current = evaluer_solution(instance, permutation);

    for (int iter = 0; iter < max_iterations; iter++)
    {
        int meilleur_i = -1, meilleur_j = -1;
        double meilleur_score = poids_makespan * current.makespan +
                                poids_tardiness * current.tardiness;

        // Explorer tout le voisinage swap
        for (int i = 0; i < instance->nb_jobs; i++)
        {
            for (int j = i + 1; j < instance->nb_jobs; j++)
            {
                // Essayer le swap
                swap(&permutation[i], &permutation[j]);

                ObjectiveVector voisin = evaluer_solution(instance, permutation);
                double score_voisin = poids_makespan * voisin.makespan +
                                      poids_tardiness * voisin.tardiness;

                if (score_voisin < meilleur_score)
                {
                    meilleur_score = score_voisin;
                    meilleur_i = i;
                    meilleur_j = j;
                }

                // Annuler le swap
                swap(&permutation[i], &permutation[j]);
            }
        }

        // Appliquer le meilleur swap trouvé
        if (meilleur_i != -1)
        {
            swap(&permutation[meilleur_i], &permutation[meilleur_j]);
            current = evaluer_solution(instance, permutation);
        }
        else
        {
            // Convergence : aucun swap n'améliore la solution
            break;
        }
    }

    return current;
}

Archive *approche_scalaire(Instance *instance, int nb_poids, int max_iterations, Config *config)
{
    Archive *archive = creer_archive(100);

    clock_t debut = clock();

    for (int p = 0; p < nb_poids; p++)
    {
        double alpha = (double)p / (nb_poids - 1);
        double poids_makespan = 1.0 - alpha;
        double poids_tardiness = alpha;

        if (config->verbose && p % 5 == 0)
        {
            printf("  Poids %d/%d (alpha=%.2f, makespan=%.2f, tardiness=%.2f)\n",
                   p + 1, nb_poids, alpha, poids_makespan, poids_tardiness);
        }

        // Générer solution initiale aléatoire
        int *permutation = generer_solution_aleatoire(instance->nb_jobs);

        // Recherche locale avec les poids actuels
        ObjectiveVector meilleure = recherche_locale_scalaire(instance, permutation,
                                                              poids_makespan, poids_tardiness,
                                                              max_iterations);

        // Ajouter à l'archive
        filtrage_online(archive, meilleure);
        free(permutation);
    }

    clock_t fin = clock();

    if (config->verbose)
    {
        printf("  Temps d'execution: %.3f secondes\n", (double)(fin - debut) / CLOCKS_PER_SEC);
    }

    return archive;
}

Archive *approche_pareto(Instance *instance, int taille_initiale, int max_iterations, Config *config)
{
    Archive *archive = creer_archive(100);
    clock_t debut = clock();

    if (config->verbose)
    {
        printf("  Début de l'approche Pareto avec %d solutions initiales et %d itérations\n",
               taille_initiale, max_iterations);
    }

    // Phase 1 : Initialisation de l'archive avec des solutions aléatoires
    if (config->verbose)
    {
        printf("  Initialisation avec %d solutions aléatoires\n", taille_initiale);
    }

    for (int i = 0; i < taille_initiale; i++)
    {
        int *permutation = generer_solution_aleatoire(instance->nb_jobs);
        ObjectiveVector sol = evaluer_solution(instance, permutation);
        filtrage_online(archive, sol);
        free(permutation);
    }

    // Phase 2 : Recherche itérative améliorée
    int nb_jobs = instance->nb_jobs;
    int total_améliorations = 0;

    for (int iter = 0; iter < max_iterations; iter++)
    {
        int archive_size_avant = archive->size;

        if (config->verbose && iter % 20 == 0)
        {
            printf("  Itération %d/%d, archive: %d solutions\n",
                   iter + 1, max_iterations, archive->size);
        }

        // Générer une nouvelle solution aléatoire
        int *permutation = generer_solution_aleatoire(nb_jobs);

        // Explorer un nombre limité de swaps aléatoires
        int nb_swaps_a_tester = 50; // Limite fixe raisonnable
        if (nb_swaps_a_tester > nb_jobs * (nb_jobs - 1) / 2)
        {
            nb_swaps_a_tester = nb_jobs * (nb_jobs - 1) / 2;
        }

        for (int s = 0; s < nb_swaps_a_tester; s++)
        {
            int i = rand() % nb_jobs;
            int j = rand() % nb_jobs;
            if (i == j)
            {
                s--; // pour s'assurer qu'on teste un swap valide
                continue;
            }

            // Appliquer le swap
            swap(&permutation[i], &permutation[j]);

            ObjectiveVector voisin = evaluer_solution(instance, permutation);
            filtrage_online(archive, voisin);

            // Annuler le swap pour la prochaine itération
            swap(&permutation[i], &permutation[j]);
        }

        free(permutation);

        // Compter les améliorations
        if (archive->size > archive_size_avant)
        {
            total_améliorations++;
        }

        // Critère d'arrêt prématuré : si pas d'amélioration depuis un certain temps
        if (iter > 100 && total_améliorations == 0)
        {
            if (config->verbose)
            {
                printf("  Stagnation détectée, arrêt prématuré à l'itération %d\n", iter);
            }
            break;
        }
    }

    clock_t fin = clock();

    if (config->verbose)
    {
        printf("  Temps d'exécution: %.3f secondes\n", (double)(fin - debut) / CLOCKS_PER_SEC);
        printf("  Solutions trouvées: %d\n", archive->size);
        printf("  Améliorations totales: %d\n", total_améliorations);
    }

    return archive;
}

/* === FONCTIONS D'ANALYSE === */

double calculer_hypervolume(Archive *archive, double ref_makespan, double ref_tardiness)
{
    if (archive->size == 0)
        return 0.0;

    // Trier par makespan croissant
    for (int i = 0; i < archive->size - 1; i++)
    {
        for (int j = i + 1; j < archive->size; j++)
        {
            if (archive->vectors[i].makespan > archive->vectors[j].makespan)
            {
                ObjectiveVector temp = archive->vectors[i];
                archive->vectors[i] = archive->vectors[j];
                archive->vectors[j] = temp;
            }
        }
    }

    double hypervolume = 0.0;
    double dernier_tardiness = ref_tardiness;

    for (int i = 0; i < archive->size; i++)
    {
        double largeur = (i == 0) ? (ref_makespan - archive->vectors[i].makespan) : (archive->vectors[i - 1].makespan - archive->vectors[i].makespan);
        double hauteur = dernier_tardiness - archive->vectors[i].tardiness;

        hypervolume += largeur * hauteur;
        dernier_tardiness = archive->vectors[i].tardiness;
    }

    return hypervolume;
}

void sauvegarder_front(char *filename, Archive *archive, Config *config)
{
    char chemin[512];
    snprintf(chemin, sizeof(chemin), "%s/%s", config->output_dir, filename);

    FILE *file = fopen(chemin, "w");
    if (!file)
    {
        printf("Erreur: impossible de creer le fichier %s\n", chemin);
        return;
    }

    fprintf(file, "# makespan tardiness\n");
    fprintf(file, "# Instance: %s\n", config->instance_file);
    fprintf(file, "# Iterations: %d, Pareto size: %d\n", config->max_iterations, config->pareto_size);
    fprintf(file, "# Nombre de solutions: %d\n", archive->size);
    fprintf(file, "# Date: %s", ctime(&(time_t){time(NULL)}));

    for (int i = 0; i < archive->size; i++)
    {
        fprintf(file, "%d %d\n", archive->vectors[i].makespan, archive->vectors[i].tardiness);
    }

    fclose(file);
}

void sauvegarder_statistiques(Statistics *stats, int run, Config *config)
{
    char prefix[512]; // Augmenté de 256 à 512
    construire_prefixe_fichier(config->instance_file, config->max_iterations,
                               config->pareto_size, prefix, sizeof(prefix));

    char chemin[1024]; // Augmenté de 512 à 1024
    snprintf(chemin, sizeof(chemin), "%s/%s_statistiques_run_%d.txt",
             config->output_dir, prefix, run + 1);

    FILE *file = fopen(chemin, "w");
    if (!file)
    {
        printf("Erreur: impossible de creer le fichier %s\n", chemin);
        return;
    }

    fprintf(file, "# Statistiques d'execution - Run %d\n", run + 1);
    fprintf(file, "# Instance: %s\n", config->instance_file);
    fprintf(file, "# Iterations: %d, Pareto size: %d\n", config->max_iterations, config->pareto_size);
    fprintf(file, "# Date: %s", ctime(&(time_t){time(NULL)}));
    fprintf(file, "Hypervolume_scalaire: %.2f\n", stats->hypervolume_scalar);
    fprintf(file, "Hypervolume_pareto: %.2f\n", stats->hypervolume_pareto);
    fprintf(file, "Solutions_scalaire: %d\n", stats->solutions_scalar);
    fprintf(file, "Solutions_pareto: %d\n", stats->solutions_pareto);
    fprintf(file, "Temps_scalaire: %.3f\n", stats->time_scalar);
    fprintf(file, "Temps_pareto: %.3f\n", stats->time_pareto);

    fclose(file);
}

void afficher_statistiques_globales(Statistics *all_stats, int nb_runs, Config *config)
{
    char prefix[256];
    construire_prefixe_fichier(config->instance_file, config->max_iterations,
                               config->pareto_size, prefix, sizeof(prefix));

    printf("=== STATISTIQUES GLOBALES (%d runs) ===\n", nb_runs);
    printf("Instance: %s\n", extraire_nom_base(config->instance_file));
    printf("Iterations: %d, Pareto size: %d\n", config->max_iterations, config->pareto_size);

    double avg_hyper_scalar = 0, avg_hyper_pareto = 0;
    double avg_time_scalar = 0, avg_time_pareto = 0;
    double avg_sol_scalar = 0, avg_sol_pareto = 0;

    for (int i = 0; i < nb_runs; i++)
    {
        avg_hyper_scalar += all_stats[i].hypervolume_scalar;
        avg_hyper_pareto += all_stats[i].hypervolume_pareto;
        avg_time_scalar += all_stats[i].time_scalar;
        avg_time_pareto += all_stats[i].time_pareto;
        avg_sol_scalar += all_stats[i].solutions_scalar;
        avg_sol_pareto += all_stats[i].solutions_pareto;
    }

    avg_hyper_scalar /= nb_runs;
    avg_hyper_pareto /= nb_runs;
    avg_time_scalar /= nb_runs;
    avg_time_pareto /= nb_runs;
    avg_sol_scalar /= nb_runs;
    avg_sol_pareto /= nb_runs;

    printf("Moyenne hypervolume scalaire: %.2f\n", avg_hyper_scalar);
    printf("Moyenne hypervolume Pareto:   %.2f\n", avg_hyper_pareto);
    printf("Moyenne temps scalaire:       %.3f s\n", avg_time_scalar);
    printf("Moyenne temps Pareto:         %.3f s\n", avg_time_pareto);
    printf("Moyenne solutions scalaire:   %.1f\n", avg_sol_scalar);
    printf("Moyenne solutions Pareto:     %.1f\n", avg_sol_pareto);

    if (avg_hyper_scalar > avg_hyper_pareto)
    {
        printf("\nConclusion: Approche scalaire en moyenne meilleure de %.1f%%\n",
               (avg_hyper_scalar - avg_hyper_pareto) / avg_hyper_pareto * 100);
    }
    else if (avg_hyper_pareto > avg_hyper_scalar)
    {
        printf("\nConclusion: Approche Pareto en moyenne meilleure de %.1f%%\n",
               (avg_hyper_pareto - avg_hyper_scalar) / avg_hyper_scalar * 100);
    }
    else
    {
        printf("\n→ Approches equivalentes\n");
    }

    // Sauvegarde des statistiques globales
    char chemin[512];
    snprintf(chemin, sizeof(chemin), "%s/%s_statistiques_globales.txt",
             config->output_dir, prefix);

    FILE *file = fopen(chemin, "w");
    if (file)
    {
        fprintf(file, "# Statistiques globales (%d runs)\n", nb_runs);
        fprintf(file, "# Instance: %s\n", config->instance_file);
        fprintf(file, "# Iterations: %d, Pareto size: %d\n", config->max_iterations, config->pareto_size);
        fprintf(file, "# Date: %s", ctime(&(time_t){time(NULL)}));
        fprintf(file, "Hypervolume_scalaire_moyen: %.2f\n", avg_hyper_scalar);
        fprintf(file, "Hypervolume_pareto_moyen: %.2f\n", avg_hyper_pareto);
        fprintf(file, "Temps_scalaire_moyen: %.3f\n", avg_time_scalar);
        fprintf(file, "Temps_pareto_moyen: %.3f\n", avg_time_pareto);
        fprintf(file, "Solutions_scalaire_moyen: %.1f\n", avg_sol_scalar);
        fprintf(file, "Solutions_pareto_moyen: %.1f\n", avg_sol_pareto);
        fclose(file);
    }
}

/* === FONCTIONS DE CONFIGURATION === */

void afficher_aide()
{
    printf("Usage: ./tp_multiobj [OPTIONS] fichier_instance\n\n");
    printf("Options:\n");
    printf("  -h, --help                 Affiche cette aide\n");
    printf("  -o, --offline N            Nombre de solutions pour le filtrage offline (defaut: 500)\n");
    printf("  -p, --poids N              Nombre de poids pour l'approche scalaire (defaut: 10)\n");
    printf("  -i, --iterations N         Nombre maximal d'iterations (defaut: 500)\n");
    printf("  -id, --input-dir <dir>     Repertoire d'entree pour les instances (defaut: .)\n");
    printf("  -f, --instance-file <file> Fichier d'instance (defaut: 7_5_01.txt)\n");
    printf("  -s, --size N               Taille initiale pour l'approche Pareto (defaut: 10)\n");
    printf("  -r, --runs N               Nombre d'executions pour les statistiques (defaut: 1)\n");
    printf("  -a, --algo TYPE            Algorithme a executer: all, scalar, pareto (defaut: all)\n");
    printf("  -v, --verbose              Mode verbeux pour plus de details\n");
    printf("  --no-analyze               Desactive l'analyse comparative\n");
    printf("  -od, --output-dir DIR           Repertoire de sortie pour les resultats (defaut: .)\n");
    printf("  --experiment               Executer l'experimentation sur toutes les instances\n");
    printf("\nExemples:\n");
    printf("  ./tp_multiobj instance.txt\n");
    printf("  ./tp_multiobj -o 1000 -p 20 -i 500 -f instance.txt\n");
    printf("  ./tp_multiobj --algo scalar --iterations 2000 -f instance.txt\n");
    printf("  ./tp_multiobj --experiment -od results\n");
}

int parse_arguments(int argc, char *argv[], Config *config)
{
    // Valeurs par défaut MODIFIÉES pour être plus raisonnables
    strcpy(config->instance_file, "");
    strcpy(config->input_dir, "");
    config->offline_solutions = 500;
    config->nb_poids = 10;
    config->max_iterations = 500; // Réduit de 1000 à 500
    config->pareto_size = 10;     // Réduit de 20 à 10
    config->nb_runs = 1;
    config->verbose = 0;
    config->no_analyze = 0;
    strcpy(config->output_dir, ".");
    strcpy(config->algo_type, "all");
    config->seed = 0;
    config->experiment = 0;

    // Vérification des arguments
    if (argc < 2)
    {
        afficher_aide();
        return 1;
    }

    // Parse des arguments
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
        {
            afficher_aide();
            return 1;
        }
        else if (strcmp(argv[i],"-f")==0 || strcmp(argv[i],"--instance-file")==0)
        {
            if (i + 1 < argc)
            {
                strcpy(config->instance_file, argv[++i]);
            }
            else
            {
                printf("Erreur: option -f ou --instance-file require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-id") == 0 || strcmp(argv[i], "--input-dir") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(config->input_dir, argv[++i]);
            }
            else
            {
                printf("Erreur: option -id ou --input-dir require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--offline") == 0)
        {
            if (i + 1 < argc)
            {
                config->offline_solutions = atoi(argv[++i]);
            }
            else
            {
                printf("Erreur: option -o ou --offline require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--poids") == 0)
        {
            if (i + 1 < argc)
            {
                config->nb_poids = atoi(argv[++i]);
            }
            else
            {
                printf("Erreur: option -p ou --poids require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--iterations") == 0)
        {
            if (i + 1 < argc)
            {
                config->max_iterations = atoi(argv[++i]);
            }
            else
            {
                printf("Erreur: option -i ou --iterations require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0)
        {
            if (i + 1 < argc)
            {
                config->pareto_size = atoi(argv[++i]);
            }
            else
            {
                printf("Erreur: option -s ou --size require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--runs") == 0)
        {
            if (i + 1 < argc)
            {
                config->nb_runs = atoi(argv[++i]);
            }
            else
            {
                printf("Erreur: option -r ou --runs require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--algo") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(config->algo_type, argv[++i]);
            }
            else
            {
                printf("Erreur: option -a ou --algo require une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0)
        {
            config->verbose = 1;
        }
        else if (strcmp(argv[i], "--no-analyze") == 0)
        {
            config->no_analyze = 1;
        }
        else if (strcmp(argv[i], "--experiment") == 0)
        {
            config->experiment = 1;
        }
        else if (strcmp(argv[i], "--output-dir") == 0 || strcmp(argv[i], "-od") == 0)
        {
            if (i + 1 < argc)
            {
                strcpy(config->output_dir, argv[++i]);
            }
            else
            {
                printf("Erreur: option --output-dir ou -od require une valeur\n");
                return 1;
            }
        }
        else
        {
            // C'est probablement le nom du fichier d'instance
            strcpy(config->instance_file, argv[i]);
        }
    }

    // Vérification du fichier d'instance
    if (!config->experiment && strlen(config->instance_file) == 0)
    {
        printf("Erreur: nom de fichier d'instance manquant\n");
        afficher_aide();
        return 1;
    }

    // Vérification des valeurs
    if (config->offline_solutions <= 0)
    {
        printf("Erreur: nombre de solutions offline doit etre > 0\n");
        return 1;
    }
    if (config->nb_poids <= 0)
    {
        printf("Erreur: nombre de poids doit etre > 0\n");
        return 1;
    }
    if (config->max_iterations <= 0)
    {
        printf("Erreur: nombre d'iterations doit etre > 0\n");
        return 1;
    }
    if (config->pareto_size <= 0)
    {
        printf("Erreur: taille initiale doit etre > 0\n");
        return 1;
    }
    if (config->nb_runs <= 0)
    {
        printf("Erreur: nombre d'executions doit etre > 0\n");
        return 1;
    }

    // Vérification du répertoire de sortie
    struct stat st = {0};
    if (stat(config->output_dir, &st) == -1)
    {
        // Création du répertoire
        if (mkdir(config->output_dir, 0777) != 0)
        {
            printf("Erreur: impossible de creer le repertoire %s\n", config->output_dir);
            return 1;
        }
    }

    return 0;
}

void afficher_configuration(Config *config)
{
    printf("=== CONFIGURATION ===\n");
    printf("Fichier d'instance: %s\n", config->instance_file);
    printf("Solutions offline: %d\n", config->offline_solutions);
    printf("Nombre de poids: %d\n", config->nb_poids);
    printf("Iterations maximales: %d\n", config->max_iterations);
    printf("Taille initiale Pareto: %d\n", config->pareto_size);
    printf("Nombre d'executions: %d\n", config->nb_runs);
    printf("Algorithme: %s\n", config->algo_type);
    printf("Mode verbeux: %s\n", config->verbose ? "oui" : "non");
    printf("Analyse: %s\n", config->no_analyze ? "non" : "oui");
    printf("Repertoire sortie: %s\n", config->output_dir);
    printf("=====================\n\n");
}

/* === ANALYSE GRAPHIQUE === */

void generer_scripts_gnuplot(Config *config, int nb_runs)
{
    char prefix[512];
    construire_prefixe_fichier(config->instance_file, config->max_iterations,
                               config->pareto_size, prefix, sizeof(prefix));

    char chemin_script[1024];

    // Chemin complet pour le script principal
    snprintf(chemin_script, sizeof(chemin_script), "%s/%s_front_comparaison.gnuplot",
             config->output_dir, prefix);
    FILE *script = fopen(chemin_script, "w");

    if (!script)
    {
        printf("Erreur: impossible de créer le script gnuplot\n");
        return;
    }

    char abs_path[PATH_MAX];
    if (realpath(config->output_dir, abs_path) == NULL) {
        printf("Erreur: impossible de résoudre le chemin %s\n", config->output_dir);
        fclose(script);
        return;
    }

    // En-tête du script gnuplot
    fprintf(script, "# ============================================\n");
    fprintf(script, "# Script gnuplot généré automatiquement\n");
    fprintf(script, "# TP Multi-objectif FSP - Analyse graphique\n");
    fprintf(script, "# Instance: %s\n", config->instance_file);
    fprintf(script, "# Iterations: %d, Pareto size: %d\n", config->max_iterations, config->pareto_size);
    fprintf(script, "# Préfixe: %s\n", prefix);
    fprintf(script, "# ============================================\n\n");

    // Configuration générale
    fprintf(script, "# Configuration générale\n");
    fprintf(script, "set terminal pngcairo enhanced size 1200,800\n");
    fprintf(script, "set encoding utf8\n");
    fprintf(script, "set datafile separator whitespace\n");
    fprintf(script, "set datafile commentschars \"#\"\n");
    fprintf(script, "set key top right\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set style line 1 lc rgb '#0060ad' lt 1 lw 2 pt 7 ps 1.5   # Scalaire\n");
    fprintf(script, "set style line 2 lc rgb '#dd181f' lt 1 lw 2 pt 9 ps 1.5   # Pareto\n");
    fprintf(script, "set style line 3 lc rgb '#00ad60' lt 2 lw 2 pt 5 ps 1.0   # Offline\n");
    fprintf(script, "\n");

    // Titre et labels
    fprintf(script, "# Titre et labels\n");
    fprintf(script, "set title \"Comparaison des fronts Pareto\\nInstance: %s (Iterations: %d, Pareto size: %d)\"\n",
            extraire_nom_base(config->instance_file), config->max_iterations, config->pareto_size);
    fprintf(script, "set xlabel 'Makespan (Cmax)'\n");
    fprintf(script, "set ylabel 'Tardiness (Tsum)'\n");
    fprintf(script, "set logscale y\n");
    fprintf(script, "\n");

    // CORRECTION ICI : Supprimer le répertoire du chemin de sortie
    fprintf(script, "# Fichier de sortie\n");
    fprintf(script, "set output '%s_front_comparaison.png'\n", prefix);
    fprintf(script, "\n");

    // Commande de plot
    fprintf(script, "# Tracé des données\n");
    fprintf(script, "plot ");

    // Ajout des fronts (si disponibles)
    int first_plot = 1;

    // Front offline (référence)
    if (!first_plot)
        fprintf(script, ", ");
    fprintf(script, "'%s_front_offline.dat' using 1:2 with points ls 3 title 'Filtrage Offline (référence)'", prefix);
    first_plot = 0;

    // Fronts par run (scalaire et Pareto)
    for (int run = 1; run <= nb_runs; run++)
    {
        // Front scalaire
        fprintf(script, ", '%s_front_scalaire_run_%d.dat' using 1:2 with points ls 1 title 'Scalaire Run %d'",
                prefix, run, run);

        // Front Pareto
        fprintf(script, ", '%s_front_pareto_run_%d.dat' using 1:2 with points ls 2 title 'Pareto Run %d'",
                prefix, run, run);
    }

    fprintf(script, "\n\n");

    // Fermeture du script
    fprintf(script, "# Fin du script\n");
    fprintf(script, "print 'Script exécuté avec succès. Graphique généré: %s_front_comparaison.png'\n", prefix);
    fclose(script);

    printf("Script gnuplot généré: %s\n", chemin_script);
    printf("Pour générer les graphiques:\n");
    printf("  cd %s\n", config->output_dir);
    printf("  gnuplot \"%s_front_comparaison.gnuplot\"\n", prefix);
}

Archive *lire_front(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Erreur: impossible d'ouvrir le fichier %s\n", filename);
        return NULL;
    }

    // Créer une archive temporaire
    Archive *archive = creer_archive(100);

    char ligne[256];
    int makespan, tardiness;
    int count = 0;

    // Lire ligne par ligne
    while (fgets(ligne, sizeof(ligne), file))
    {
        // Ignorer les lignes de commentaire
        if (ligne[0] == '#')
            continue;

        // Parser les valeurs
        if (sscanf(ligne, "%d %d", &makespan, &tardiness) == 2)
        {
            ObjectiveVector sol = {makespan, tardiness};
            filtrage_online(archive, sol);
            count++;
        }
    }

    fclose(file);

    printf("  Fichier %s lu: %d solutions chargées, %d non-dominées\n",
           filename, count, archive->size);

    return archive;
}

void generer_graphique_hypervolume(Statistics *stats, int nb_runs, Config *config)
{
    if (!stats || nb_runs <= 0)
    {
        printf("Données insuffisantes pour générer le graphique d'hypervolume\n");
        return;
    }

    char prefix[512];
    construire_prefixe_fichier(config->instance_file, config->max_iterations,
                               config->pareto_size, prefix, sizeof(prefix));

    char chemin_data[1024];
    char chemin_script[1024];

    // Créer le fichier de données
    snprintf(chemin_data, sizeof(chemin_data), "%s/%s_hypervolume_evolution.dat",
             config->output_dir, prefix);
    FILE *data_file = fopen(chemin_data, "w");

    if (!data_file)
    {
        printf("Erreur: impossible de créer le fichier de données\n");
        return;
    }

    // En-tête du fichier de données
    fprintf(data_file, "# run hypervolume_scalaire hypervolume_pareto\n");
    fprintf(data_file, "# Instance: %s\n", config->instance_file);
    fprintf(data_file, "# Iterations: %d, Pareto size: %d\n", config->max_iterations, config->pareto_size);

    // Écrire les données
    for (int run = 0; run < nb_runs; run++)
    {
        fprintf(data_file, "%d %.2f %.2f\n",
                run + 1,
                stats[run].hypervolume_scalar,
                stats[run].hypervolume_pareto);
    }

    fclose(data_file);

    // Créer le script gnuplot
    snprintf(chemin_script, sizeof(chemin_script), "%s/%s_hypervolume_evolution.gnuplot",
             config->output_dir, prefix);
    FILE *script = fopen(chemin_script, "w");

    if (!script)
    {
        printf("Erreur: impossible de créer le script gnuplot\n");
        return;
    }

    char abs_path[PATH_MAX];
    if (realpath(config->output_dir, abs_path) == NULL) {
        printf("Erreur: impossible de résoudre le chemin %s\n", config->output_dir);
        fclose(script);
        return;
    }

    fprintf(script, "set terminal pngcairo enhanced size 1200,600\n");
    fprintf(script, "set output '%s/%s_hypervolume_evolution.png'\n", abs_path, prefix);
    fprintf(script, "set title \"Évolution de l'hypervolume\\nInstance: %s (Iterations: %d, Pareto size: %d, %d runs)\"\n",
            extraire_nom_base(config->instance_file), config->max_iterations, config->pareto_size, nb_runs);
    fprintf(script, "set xlabel 'Run'\n");
    fprintf(script, "set ylabel 'Hypervolume'\n");
    fprintf(script, "set grid\n");
    fprintf(script, "set key top left\n");
    fprintf(script, "set style line 1 lc rgb '#0060ad' lw 2 pt 7 ps 1.5\n");
    fprintf(script, "set style line 2 lc rgb '#dd181f' lw 2 pt 9 ps 1.5\n");
    fprintf(script, "set style line 3 lc rgb '#00ad60' lw 1 dt 2\n");
    fprintf(script, "\n");
    fprintf(script, "plot '%s/%s_hypervolume_evolution.dat' using 1:2 with linespoints ls 1 title 'Approche Scalaire', \\\n", abs_path, prefix);
    fprintf(script, "     '%s/%s_hypervolume_evolution.dat' using 1:3 with linespoints ls 2 title 'Approche Pareto'\n", abs_path, prefix);

    fclose(script);

    printf("Graphique d'évolution de l'hypervolume généré\n");
    printf("Pour générer: cd %s && gnuplot \"%s_hypervolume_evolution.gnuplot\"\n",
           config->output_dir, prefix);
}
void analyser_fronts_visuellement(Config *config, int nb_runs, Statistics *all_stats)
{
    printf("\n=== ANALYSE GRAPHIQUE ===\n");

    char prefix[256];
    construire_prefixe_fichier(config->instance_file, config->max_iterations,
                               config->pareto_size, prefix, sizeof(prefix));

    printf("Préfixe utilisé: %s\n", prefix);

    // 1. Générer les scripts gnuplot principaux
    generer_scripts_gnuplot(config, nb_runs);

    // 2. Si nous avons des statistiques, générer le graphique d'hypervolume
    if (all_stats && nb_runs > 1)
    {
        generer_graphique_hypervolume(all_stats, nb_runs, config);
    }

    // 3. Comparer les fronts si possible (premier run)
    if (nb_runs >= 1)
    {
        char chemin_front1[512];
        char chemin_front2[512];

        snprintf(chemin_front1, sizeof(chemin_front1), "%s/%s_front_scalaire_run_1.dat",
                 config->output_dir, prefix);
        snprintf(chemin_front2, sizeof(chemin_front2), "%s/%s_front_pareto_run_1.dat",
                 config->output_dir, prefix);

        // Vérifier si les fichiers existent
        if (fichier_existe(chemin_front1) && fichier_existe(chemin_front2))
        {
            Archive *front_scalaire = lire_front(chemin_front1);
            Archive *front_pareto = lire_front(chemin_front2);

            if (front_scalaire && front_pareto)
            {
                printf("  Fronts lus avec succès\n");
                printf("  Scalaire: %d solutions, Pareto: %d solutions\n",
                       front_scalaire->size, front_pareto->size);

                liberer_archive(front_scalaire);
                liberer_archive(front_pareto);
            }
        }
    }

    printf("\n=== INSTRUCTIONS POUR LA VISUALISATION ===\n");
    printf("1. Installer gnuplot si nécessaire:\n");
    printf("   Ubuntu/Debian: sudo apt-get install gnuplot\n");
    printf("   macOS: brew install gnuplot\n");
    printf("   Windows: Télécharger depuis http://www.gnuplot.info/\n\n");

    printf("2. Générer les graphiques:\n");
    printf("   cd %s\n", config->output_dir);
    printf("   gnuplot %s_front_comparaison.gnuplot\n", prefix);

    if (all_stats && nb_runs > 1)
    {
        printf("   gnuplot %s_hypervolume_evolution.gnuplot\n", prefix);
    }

    printf("\n3. Visualiser les images PNG générées\n");
    printf("   ls %s/%s_*.png\n", config->output_dir, prefix);
    printf("========================================\n");
}

/* === EXPÉRIMENTATION COMPLÈTE === */

void executer_experimentation(Instance *instance, Config *config)
{
    char prefix[256];
    construire_prefixe_fichier(config->instance_file, config->max_iterations,
                               config->pareto_size, prefix, sizeof(prefix));

    printf("Prefixe des fichiers: %s\n\n", prefix);

    // Tableaux pour les statistiques
    Statistics *all_stats = NULL;
    if (config->nb_runs > 1)
    {
        all_stats = (Statistics *)malloc(config->nb_runs * sizeof(Statistics));
    }

    // Exécution multiple
    for (int run = 0; run < config->nb_runs; run++)
    {
        printf("=== EXECUTION %d/%d ===\n", run + 1, config->nb_runs);
        printf("Prefixe: %s\n", prefix);

        if (config->nb_runs > 1)
        {
            // Nouvelle seed pour chaque run
            srand(time(NULL) + run);
        }

        Archive *archive_scalaire = NULL;
        Archive *archive_pareto = NULL;
        Statistics stats = {0};

        clock_t debut_total = clock();

        // Question 4: Solution aléatoire (uniquement au premier run)
        if (run == 0)
        {
            printf("\n--- Question 4: Solution aleatoire ---\n");
            int *perm_aleatoire = generer_solution_aleatoire(instance->nb_jobs);
            ObjectiveVector eval_aleatoire = evaluer_solution(instance, perm_aleatoire);
            printf("Solution aleatoire: makespan=%d, tardiness=%d\n",
                   eval_aleatoire.makespan, eval_aleatoire.tardiness);
            free(perm_aleatoire);
        }

        // Question 5: Filtrage offline (uniquement au premier run)
        if (run == 0)
        {
            printf("\n--- Question 5: Filtrage offline ---\n");
            printf("Generation de %d solutions aleatoires...\n", config->offline_solutions);

            ObjectiveVector *solutions_offline = (ObjectiveVector *)malloc(config->offline_solutions * sizeof(ObjectiveVector));

            for (int i = 0; i < config->offline_solutions; i++)
            {
                int *perm = generer_solution_aleatoire(instance->nb_jobs);
                solutions_offline[i] = evaluer_solution(instance, perm);
                free(perm);
            }

            Archive *archive_offline = filtrage_offline(solutions_offline, config->offline_solutions);
            printf("Resultat: %d solutions non-dominees\n", archive_offline->size);

            char filename[256];
            snprintf(filename, sizeof(filename), "%s_front_offline.dat", prefix);
            sauvegarder_front(filename, archive_offline, config);

            free(solutions_offline);
            liberer_archive(archive_offline);
        }

        // Question 6: Filtrage online (uniquement au premier run)
        if (run == 0)
        {
            printf("\n--- Question 6: Filtrage online ---\n");
            Archive *archive_online = creer_archive(100);

            clock_t debut_online = clock();
            int nb_comparaisons = 0;

            for (int i = 0; i < config->offline_solutions; i++)
            {
                int *perm = generer_solution_aleatoire(instance->nb_jobs);
                ObjectiveVector sol = evaluer_solution(instance, perm);
                nb_comparaisons += archive_online->size + 1;
                filtrage_online(archive_online, sol);
                free(perm);
            }

            clock_t fin_online = clock();
            double temps_online = (double)(fin_online - debut_online) / CLOCKS_PER_SEC;

            printf("Archive finale: %d solutions non-dominees\n", archive_online->size);
            printf("Temps d'execution: %.4f secondes\n", temps_online);
            printf("Nombre de comparaisons: %d\n", nb_comparaisons);

            liberer_archive(archive_online);
        }

        // Exécution des algorithmes selon la configuration
        if (strcmp(config->algo_type, "all") == 0 || strcmp(config->algo_type, "scalar") == 0)
        {
            printf("\n--- Question 7: Approche scalaire ---\n");
            printf("Parametres: %d poids, %d iterations\n", config->nb_poids, config->max_iterations);

            clock_t debut_scalaire = clock();
            archive_scalaire = approche_scalaire(instance, config->nb_poids, config->max_iterations, config);
            clock_t fin_scalaire = clock();

            stats.time_scalar = (double)(fin_scalaire - debut_scalaire) / CLOCKS_PER_SEC;
            stats.solutions_scalar = archive_scalaire->size;

            printf("Solutions trouvees: %d (temps: %.3f s)\n",
                   stats.solutions_scalar, stats.time_scalar);

            // Sauvegarde du front
            char filename[256];
            snprintf(filename, sizeof(filename), "%s_front_scalaire_run_%d.dat", prefix, run + 1);
            sauvegarder_front(filename, archive_scalaire, config);
        }

        if (strcmp(config->algo_type, "all") == 0 || strcmp(config->algo_type, "pareto") == 0)
        {
            printf("\n--- Question 8: Approche Pareto ---\n");

            // Ajuster automatiquement les paramètres en fonction de la taille de l'instance
            int iterations_pareto = config->max_iterations;
            int taille_initiale_pareto = config->pareto_size;

            if (instance->nb_jobs > 50)
            {
                // Pour les grandes instances, réduire drastiquement
                iterations_pareto = config->max_iterations / 5;
                taille_initiale_pareto = config->pareto_size / 2;
                printf("  Instance grande (%d jobs): paramètres réduits automatiquement\n", instance->nb_jobs);
            }
            else if (instance->nb_jobs > 20)
            {
                // Pour les instances moyennes, réduire modérément
                iterations_pareto = config->max_iterations / 2;
                printf("  Instance moyenne (%d jobs): itérations réduites\n", instance->nb_jobs);
            }

            printf("Parametres ajustés: taille initiale %d, %d iterations\n",
                   taille_initiale_pareto, iterations_pareto);

            clock_t debut_pareto = clock();
            archive_pareto = approche_pareto(instance, taille_initiale_pareto, iterations_pareto, config);
            clock_t fin_pareto = clock();

            stats.time_pareto = (double)(fin_pareto - debut_pareto) / CLOCKS_PER_SEC;
            stats.solutions_pareto = archive_pareto->size;

            printf("Solutions trouvees: %d (temps: %.3f s)\n",
                   stats.solutions_pareto, stats.time_pareto);

            // Sauvegarde du front
            char filename[256];
            snprintf(filename, sizeof(filename), "%s_front_pareto_run_%d.dat", prefix, run + 1);
            sauvegarder_front(filename, archive_pareto, config);
        }

        // Analyse comparative
        if (!config->no_analyze && (strcmp(config->algo_type, "all") == 0))
        {
            printf("\n--- Analyse comparative ---\n");

            // Calcul des points de référence
            double ref_makespan = 0, ref_tardiness = 0;
            for (int i = 0; i < 100; i++)
            {
                int *perm = generer_solution_aleatoire(instance->nb_jobs);
                ObjectiveVector sol = evaluer_solution(instance, perm);
                if (sol.makespan > ref_makespan)
                    ref_makespan = sol.makespan;
                if (sol.tardiness > ref_tardiness)
                    ref_tardiness = sol.tardiness;
                free(perm);
            }

            ref_makespan *= 1.1;
            ref_tardiness *= 1.1;

            if (archive_scalaire)
            {
                stats.hypervolume_scalar = calculer_hypervolume(archive_scalaire, ref_makespan, ref_tardiness);
                printf("Hypervolume scalaire: %.2f\n", stats.hypervolume_scalar);
            }

            if (archive_pareto)
            {
                stats.hypervolume_pareto = calculer_hypervolume(archive_pareto, ref_makespan, ref_tardiness);
                printf("Hypervolume Pareto:   %.2f\n", stats.hypervolume_pareto);
            }

            if (archive_scalaire && archive_pareto)
            {
                if (stats.hypervolume_scalar > stats.hypervolume_pareto)
                {
                    printf("→ Approche scalaire meilleure (+%.1f%%)\n",
                           (stats.hypervolume_scalar - stats.hypervolume_pareto) / stats.hypervolume_pareto * 100);
                }
                else if (stats.hypervolume_pareto > stats.hypervolume_scalar)
                {
                    printf("→ Approche Pareto meilleure (+%.1f%%)\n",
                           (stats.hypervolume_pareto - stats.hypervolume_scalar) / stats.hypervolume_scalar * 100);
                }
                else
                {
                    printf("→ Approches equivalentes\n");
                }
            }
        }

        // Sauvegarde des statistiques
        if (config->nb_runs > 1)
        {
            all_stats[run] = stats;
            sauvegarder_statistiques(&stats, run, config);
        }

        // Libération mémoire pour cette run
        if (archive_scalaire)
        {
            liberer_archive(archive_scalaire);
        }
        if (archive_pareto)
        {
            liberer_archive(archive_pareto);
        }

        clock_t fin_total = clock();
        double temps_total = (double)(fin_total - debut_total) / CLOCKS_PER_SEC;
        printf("\nTemps total execution %d: %.3f secondes\n", run + 1, temps_total);
        printf("============================\n\n");
    }

    // Statistiques globales si plusieurs runs
    if (config->nb_runs > 1 && !config->no_analyze && strcmp(config->algo_type, "all") == 0)
    {
        afficher_statistiques_globales(all_stats, config->nb_runs, config);
    }

    // Analyse graphique des résultats
    if (!config->no_analyze)
    {
        analyser_fronts_visuellement(config, config->nb_runs, all_stats);
    }

    // Libération de la mémoire
    if (config->nb_runs > 1 && !config->no_analyze && strcmp(config->algo_type, "all") == 0)
    {
        free(all_stats);
    }
}

/* =============== Fonction d'expérimentation =============== */

void experiment_all_instances(Config *config) {
    DIR *dir;
    struct dirent *entry;
    char instance_dir[512];
    if (strlen(config->input_dir) > 0) {
        strcpy(instance_dir, config->input_dir);
    } else {
        strcpy(instance_dir, "instances");
    }

    printf("Instance dir: '%s'\n", instance_dir);

    dir = opendir(instance_dir);
    if (!dir) {
        fprintf(stderr, "Erreur: impossible d'ouvrir le répertoire %s\n", instance_dir);
        return;
    }

    // Ouvrir le fichier de résultats
    char result_file[512];
    sprintf(result_file, "%s/experiment_results_tp2.txt", config->output_dir);
    FILE *f = fopen(result_file, "w");
    if (!f) {
        fprintf(stderr, "Erreur: impossible de créer le fichier %s\n", result_file);
        closedir(dir);
        return;
    }

    fprintf(f, "# Résultats de l'expérimentation TP2\n");
    fprintf(f, "# Instance\tMaxIter\tParetoSize\tAlgo\tHypervolume\tSolutions\tTemps(s)\n");

    // Liste pour stocker les instances traitées
    char **instances_traitees = NULL;
    int nb_instances_traitees = 0;
    int capacite_instances = 10;
    instances_traitees = (char **)malloc(capacite_instances * sizeof(char *));

    // Paramètres à tester
    int max_iter_list[] = {100, 500, 1000};
    int pareto_size_list[] = {5, 10, 20};
    int num_max_iter = 3;
    int num_pareto_size = 3;
    int runs = config->nb_runs; // utiliser la valeur de config

    const char* algos[] = {"scalar", "pareto"};

    while ((entry = readdir(dir)) != NULL) {
        if (strstr(entry->d_name, ".txt") != NULL) {
            char path[512];
            sprintf(path, "%s/%s", instance_dir, entry->d_name);

            Instance* inst = lire_instance(path);
            if (!inst) {
                fprintf(stderr, "Erreur: impossible de charger %s\n", path);
                continue;
            }

            printf("Traitement de l'instance %s\n", entry->d_name);

            // Ajouter à la liste des instances traitées
            if (nb_instances_traitees >= capacite_instances) {
                capacite_instances *= 2;
                instances_traitees = (char **)realloc(instances_traitees, capacite_instances * sizeof(char *));
            }
            instances_traitees[nb_instances_traitees] = (char *)malloc(strlen(entry->d_name) + 1);
            strcpy(instances_traitees[nb_instances_traitees], entry->d_name);
            nb_instances_traitees++;

            srand(config->seed == 0 ? time(NULL) : config->seed);

            for (int m = 0; m < num_max_iter; m++) {
                int max_iter = max_iter_list[m];
                for (int p = 0; p < num_pareto_size; p++) {
                    int pareto_size = pareto_size_list[p];

                    for (int a = 0; a < 2; a++) {
                        const char* algo = algos[a];
                        double sum_hyper = 0.0;
                        int sum_sol = 0;
                        double sum_time = 0.0;
                        int valid_runs = 0;

                        for (int run = 0; run < runs; run++) {
                            // Créer des archives temporaires
                            Archive *archive = NULL;
                            Statistics stats = {0};
                            clock_t debut = clock();

                            if (strcmp(algo, "scalar") == 0) {
                                archive = approche_scalaire(inst, config->nb_poids, max_iter, config);
                                stats.solutions_scalar = archive->size;
                                stats.time_scalar = (double)(clock() - debut) / CLOCKS_PER_SEC;
                                // Calculer hypervolume
                                double ref_makespan = 0, ref_tardiness = 0;
                                for (int i = 0; i < 50; i++) { // moins pour vitesse
                                    int *perm = generer_solution_aleatoire(inst->nb_jobs);
                                    ObjectiveVector sol = evaluer_solution(inst, perm);
                                    if (sol.makespan > ref_makespan) ref_makespan = sol.makespan;
                                    if (sol.tardiness > ref_tardiness) ref_tardiness = sol.tardiness;
                                    free(perm);
                                }
                                ref_makespan *= 1.1;
                                ref_tardiness *= 1.1;
                                stats.hypervolume_scalar = calculer_hypervolume(archive, ref_makespan, ref_tardiness);

                                // Sauvegarder le front pour le premier run
                                if (run == 0) {
                                    char prefix[256];
                                    construire_prefixe_fichier(entry->d_name, max_iter, pareto_size, prefix, sizeof(prefix));
                                    char filename[256];
                                    snprintf(filename, sizeof(filename), "%s_front_scalaire_run_1.dat", prefix);
                                    sauvegarder_front(filename, archive, config);
                                }
                            } else if (strcmp(algo, "pareto") == 0) {
                                archive = approche_pareto(inst, pareto_size, max_iter, config);
                                stats.solutions_pareto = archive->size;
                                stats.time_pareto = (double)(clock() - debut) / CLOCKS_PER_SEC;
                                // Calculer hypervolume
                                double ref_makespan = 0, ref_tardiness = 0;
                                for (int i = 0; i < 50; i++) {
                                    int *perm = generer_solution_aleatoire(inst->nb_jobs);
                                    ObjectiveVector sol = evaluer_solution(inst, perm);
                                    if (sol.makespan > ref_makespan) ref_makespan = sol.makespan;
                                    if (sol.tardiness > ref_tardiness) ref_tardiness = sol.tardiness;
                                    free(perm);
                                }
                                ref_makespan *= 1.1;
                                ref_tardiness *= 1.1;
                                stats.hypervolume_pareto = calculer_hypervolume(archive, ref_makespan, ref_tardiness);

                                // Sauvegarder le front pour le premier run
                                if (run == 0) {
                                    char prefix[256];
                                    construire_prefixe_fichier(entry->d_name, max_iter, pareto_size, prefix, sizeof(prefix));
                                    char filename[256];
                                    snprintf(filename, sizeof(filename), "%s_front_pareto_run_1.dat", prefix);
                                    sauvegarder_front(filename, archive, config);
                                }
                            }

                            if (archive) {
                                liberer_archive(archive);
                            }

                            // Accumuler
                            if (strcmp(algo, "scalar") == 0) {
                                sum_hyper += stats.hypervolume_scalar;
                                sum_sol += stats.solutions_scalar;
                                sum_time += stats.time_scalar;
                            } else {
                                sum_hyper += stats.hypervolume_pareto;
                                sum_sol += stats.solutions_pareto;
                                sum_time += stats.time_pareto;
                            }
                            valid_runs++;
                        }

                        double avg_hyper = sum_hyper / valid_runs;
                        double avg_sol = (double)sum_sol / valid_runs;
                        double avg_time = sum_time / valid_runs;

                        fprintf(f, "%s\t%d\t%d\t%s\t%.2f\t%.1f\t%.3f\n", entry->d_name, max_iter, pareto_size, algo, avg_hyper, avg_sol, avg_time);
                    }
                }
            }

            liberer_instance(inst);

            // Générer le script gnuplot pour cette instance
            const char* algos[] = {"scalar", "pareto"};
            char instance_gnuplot[512];
            sprintf(instance_gnuplot, "%s/%s_plot.gnuplot", config->output_dir, entry->d_name);
            FILE *ig = fopen(instance_gnuplot, "w");
            if (ig) {
                fprintf(ig, "set terminal png size 800,600\n");
                fprintf(ig, "set output '%s_results.png'\n", entry->d_name);
                fprintf(ig, "set title 'Instance %s - Hypervolume vs Max Iterations'\n", entry->d_name);
                fprintf(ig, "set xlabel 'Max Iterations'\n");
                fprintf(ig, "set ylabel 'Hypervolume moyen'\n");
                fprintf(ig, "set key outside\n");
                fprintf(ig, "set datafile separator \"\\t\"\n");
                fprintf(ig, "set datafile commentschars \"#\"\n");
                fprintf(ig, "set xrange [0:1200]\n");
                fprintf(ig, "set yrange [0:*]\n");
                fprintf(ig, "plot ");
                int plot_count = 0;
                for (int p = 0; p < 3; p++) {
                    int pareto_size = (p == 0 ? 5 : (p == 1 ? 10 : 20));
                    for (int a = 0; a < 2; a++) {
                        const char* algo = algos[a];
                        fprintf(ig, "'experiment_results_tp2.txt' using (stringcolumn(1) eq \"%s\" && stringcolumn(4) eq \"%s\" && column(3) == %d ? column(2) : 1/0):5 with linespoints lc %d title \"%s ParetoSize %d\"", entry->d_name, algo, pareto_size, (p*2 + a + 1), algo, pareto_size);
                        plot_count++;
                        if (plot_count < 6) fprintf(ig, ", ");
                    }
                }
                fprintf(ig, "\n");
                fclose(ig);
                printf("Script gnuplot pour %s créé : %s\n", entry->d_name, instance_gnuplot);
            }
        }
    }

    // Générer des scripts gnuplot pour chaque configuration de paramètres
    const char* instances[] = {"7_5_01.txt", "20_10_01.txt", "20_20_01.txt", "30_10_01.txt", "30_20_01.txt", "50_10_01.txt", "50_20_01.txt"};
    int num_instances = 7;
    for (int m = 0; m < 3; m++) {
        int max_iter = max_iter_list[m];
        for (int p = 0; p < 3; p++) {
            int pareto_size = pareto_size_list[p];
            char config_gnuplot[512];
            sprintf(config_gnuplot, "%s/config_%d_%d_plot.gnuplot", config->output_dir, max_iter, pareto_size);
            FILE *cg = fopen(config_gnuplot, "w");
            if (cg) {
                fprintf(cg, "set terminal png size 800,600\n");
                fprintf(cg, "set output 'hypervolume_vs_instance_max_iter_%d_pareto_size_%d.png'\n", max_iter, pareto_size);
                fprintf(cg, "set title 'Config MaxIter %d, ParetoSize %d - Hypervolume vs Instance'\n", max_iter, pareto_size);
                fprintf(cg, "set xlabel 'Instance'\n");
                fprintf(cg, "set ylabel 'Hypervolume moyen'\n");
                fprintf(cg, "set key outside\n");
                fprintf(cg, "set datafile separator \"\\t\"\n");
                fprintf(cg, "set datafile commentschars \"#\"\n");
                fprintf(cg, "set xtics rotate by -45\n");
                fprintf(cg, "set xtics (\"7_5_01.txt\" 1, \"20_10_01.txt\" 2, \"20_20_01.txt\" 3, \"30_10_01.txt\" 4, \"30_20_01.txt\" 5, \"50_10_01.txt\" 6, \"50_20_01.txt\" 7)\n");
                fprintf(cg, "set yrange [0:*]\n");
                fprintf(cg, "plot ");
                int plot_count = 0;
                for (int a = 0; a < 2; a++) {
                    const char* algo = algos[a];
                    fprintf(cg, "'experiment_results_tp2.txt' using (column(2) == %d && column(3) == %d && stringcolumn(4) eq \"%s\" ? (stringcolumn(1) eq \"7_5_01.txt\" ? 1 : stringcolumn(1) eq \"20_10_01.txt\" ? 2 : stringcolumn(1) eq \"20_20_01.txt\" ? 3 : stringcolumn(1) eq \"30_10_01.txt\" ? 4 : stringcolumn(1) eq \"30_20_01.txt\" ? 5 : stringcolumn(1) eq \"50_10_01.txt\" ? 6 : stringcolumn(1) eq \"50_20_01.txt\" ? 7 : 1/0) : 1/0):5 with linespoints lc %d title \"%s\"", max_iter, pareto_size, algo, (a + 1), algo);
                    plot_count++;
                    if (plot_count < 2) fprintf(cg, ", ");
                }
                fprintf(cg, "\n");
                fclose(cg);
                printf("Script gnuplot pour config %d %d créé : %s\n", max_iter, pareto_size, config_gnuplot);
            }
        }
    }

    // Générer scripts pour chaque instance et config
    for (int i = 0; i < num_instances; i++) {
        const char* instance = instances[i];
        for (int m = 0; m < 3; m++) {
            int max_iter = (m == 0 ? 100 : (m == 1 ? 500 : 1000));
            for (int p = 0; p < 3; p++) {
                int pareto_size = (p == 0 ? 5 : (p == 1 ? 10 : 20));
                char instance_config_gnuplot[1024];
                sprintf(instance_config_gnuplot, "%s/%s_max_iter_%d_pareto_size_%d_plot.gnuplot", config->output_dir, instance, max_iter, pareto_size);
                FILE *icg = fopen(instance_config_gnuplot, "w");
                if (icg) {
                    fprintf(icg, "set terminal png size 800,600\n");
                    fprintf(icg, "set output '%s_max_iter_%d_pareto_size_%d.png'\n", instance, max_iter, pareto_size);
                    fprintf(icg, "set title 'Instance %s, MaxIter %d, ParetoSize %d - Hypervolume'\n", instance, max_iter, pareto_size);
                    fprintf(icg, "set xlabel 'Algorithm'\n");
                    fprintf(icg, "set ylabel 'Hypervolume'\n");
                    fprintf(icg, "set xtics ('scalar' 1, 'pareto' 2)\n");
                    fprintf(icg, "plot 'experiment_results_tp2.txt' using (stringcolumn(1) eq \"%s\" && $2 == %d && $3 == %d && stringcolumn(4) eq \"scalar\" ? 1 : 1/0):5 with points pt 7 lc 1 title \"scalar\", 'experiment_results_tp2.txt' using (stringcolumn(1) eq \"%s\" && $2 == %d && $3 == %d && stringcolumn(4) eq \"pareto\" ? 2 : 1/0):5 with points pt 7 lc 2 title \"pareto\"\n", instance, max_iter, pareto_size, instance, max_iter, pareto_size);
                    fclose(icg);
                    printf("Script gnuplot pour instance %s, config %d %d créé : %s\n", instance, max_iter, pareto_size, instance_config_gnuplot);
                }
            }
        }
    }

    fclose(f);
    closedir(dir);
    printf("Expérimentation terminée. Résultats dans %s\n", result_file);

    // Générer les scripts de visualisation des fronts pour chaque instance et configuration
    printf("\n=== GÉNÉRATION DES SCRIPTS DE VISUALISATION DES FRONTS ===\n");
    for (int i = 0; i < nb_instances_traitees; i++) {
        char instance_path[512];
        sprintf(instance_path, "%s/%s", instance_dir, instances_traitees[i]);
        
        // Sauvegarder la config originale
        char original_instance_file[256];
        strcpy(original_instance_file, config->instance_file);
        int original_max_iter = config->max_iterations;
        int original_pareto_size = config->pareto_size;
        
        // Charger l'instance pour vérifier qu'elle existe
        Instance* inst = lire_instance(instance_path);
        if (!inst) continue;
        liberer_instance(inst);
        
        // Pour chaque configuration
        for (int m = 0; m < num_max_iter; m++) {
            int max_iter = max_iter_list[m];
            for (int p = 0; p < num_pareto_size; p++) {
                int pareto_size = pareto_size_list[p];
                
                // Mettre à jour la config temporairement
                strcpy(config->instance_file, instance_path);
                config->max_iterations = max_iter;
                config->pareto_size = pareto_size;
                
                // Générer les scripts pour cette configuration
                analyser_fronts_visuellement(config, config->nb_runs, NULL);
            }
        }
        
        // Restaurer la config originale
        strcpy(config->instance_file, original_instance_file);
        config->max_iterations = original_max_iter;
        config->pareto_size = original_pareto_size;
    }

    // Libérer la mémoire des instances
    for (int i = 0; i < nb_instances_traitees; i++) {
        free(instances_traitees[i]);
    }
    free(instances_traitees);

    // Générer le script gnuplot
    char gnuplot_file[512];
    sprintf(gnuplot_file, "%s/experiment_plot_tp2.gnuplot", config->output_dir);
    FILE *g = fopen(gnuplot_file, "w");
    if (g) {
        fprintf(g, "set terminal png size 1200,800\n");
        fprintf(g, "set output 'experiment_results_tp2.png'\n");
        fprintf(g, "set title 'Expérimentation TP2 - Hypervolume vs Max Iterations'\n");
        fprintf(g, "set xlabel 'Max Iterations'\n");
        fprintf(g, "set ylabel 'Hypervolume moyen'\n");
        fprintf(g, "set key outside\n");
        fprintf(g, "set datafile separator \"\\t\"\n");
        fprintf(g, "set datafile commentschars \"#\"\n");
        fprintf(g, "set xrange [0:1200]\n");
        fprintf(g, "set yrange [0:*]\n");
        fprintf(g, "plot ");
        int plot_count = 0;
        for (int p = 0; p < num_pareto_size; p++) {
            int pareto_size = pareto_size_list[p];
            for (int a = 0; a < 2; a++) {
                const char* algo = algos[a];
                fprintf(g, "'experiment_results_tp2.txt' using (stringcolumn(4) eq \"%s\" && column(3) == %d ? column(2) : 1/0):5 with linespoints lc %d title \"%s ParetoSize %d\"", algo, pareto_size, (p*2 + a + 1), algo, pareto_size);
                plot_count++;
                if (plot_count < 6) fprintf(g, ", ");
            }
        }
        fprintf(g, "\n");
        fclose(g);
        printf("Script gnuplot créé : %s\n", gnuplot_file);
    }
}

/* === POINT D'ENTRÉE PRINCIPAL === */

int main(int argc, char *argv[])
{
    srand(time(NULL));

    // Parse des arguments
    Config config;
    if (parse_arguments(argc, argv, &config) != 0)
    {
        return EXIT_FAILURE;
    }

    if (config.experiment) {
        experiment_all_instances(&config);
        return EXIT_SUCCESS;
    }

    afficher_configuration(&config);

    char instance_path[512];
    // Construire le chemin complet de l'instance
    if (strlen(config.input_dir) > 0)
    {
        snprintf(instance_path, sizeof(instance_path), "%s/%s", config.input_dir, config.instance_file);
        strcpy(config.instance_file, instance_path);
    }
    else
    {
        strcpy(instance_path, config.instance_file);
    }
    // Chargement de l'instance
    Instance *instance = lire_instance(config.instance_file);
    if (!instance)
    {
        return EXIT_FAILURE;
    }

    printf("Instance chargée: %d jobs, %d machines\n\n",
           instance->nb_jobs, instance->nb_machines);

    // Exécution
    executer_experimentation(instance, &config);

    // Libération mémoire
    liberer_instance(instance);

    printf("=== PROGRAMME TERMINÉ AVEC SUCCÈS ===\n");
    printf("Résultats sauvegardés dans: %s/\n", config.output_dir);
    printf("Préfixe des fichiers: %s_i%d_p%d\n",
           extraire_nom_base(config.instance_file), config.max_iterations, config.pareto_size);

    return EXIT_SUCCESS;
}
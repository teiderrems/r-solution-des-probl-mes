#include "header.h"
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>

int verbose = 0;

/* =============== Fonctions de configuration =============== */

void afficher_aide() {
    printf("Usage: fsp [OPTIONS] <instance_file>\n");
    printf("Solveur pour le problème Flow Shop de permutation (mono-objectif).\n\n");
    printf("Options:\n");
    printf("  -k, --executions <n>    Nombre d'exécutions à moyenner (défaut: 10)\n");
    printf("  -m, --max-evals <n>     Budget maximum d'évaluations (défaut: 10000)\n");
    printf("  -o, --output-dir <dir>  Répertoire de sortie (défaut: .)\n");
    printf("  -s, --seed <n>          Graine pour le générateur aléatoire (défaut: basée sur l'heure)\n");
    printf("  -v, --verbose           Mode verbeux\n");
    printf("  -h, --help              Afficher cette aide\n\n");
    printf("Exemples:\n");
    printf("  fsp instances/20_10_01.txt\n");
    printf("  fsp -k 5 -m 50000 -o results instances/20_10_01.txt\n");
    printf("  fsp -v -s 12345 instances/7_5_01.txt\n");
}

int parse_arguments(int argc, char *argv[], Config *config) {
    // Valeurs par défaut
    strcpy(config->instance_file, "");
    config->k_executions = 10;
    config->max_evals = 10000;
    config->verbose = 0;
    strcpy(config->output_dir, ".");
    config->seed = -1;

    // Vérification des arguments
    if (argc < 2) {
        afficher_aide();
        return 1;
    }

    // Parse des arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            afficher_aide();
            return 1;
        }
        else if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--executions") == 0) {
            if (i + 1 < argc) {
                config->k_executions = atoi(argv[++i]);
            } else {
                printf("Erreur: option -k requiert une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max-evals") == 0) {
            if (i + 1 < argc) {
                config->max_evals = atoi(argv[++i]);
            } else {
                printf("Erreur: option -m requiert une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output-dir") == 0) {
            if (i + 1 < argc) {
                strcpy(config->output_dir, argv[++i]);
            } else {
                printf("Erreur: option -o requiert une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--seed") == 0) {
            if (i + 1 < argc) {
                config->seed = atol(argv[++i]);
            } else {
                printf("Erreur: option -s requiert une valeur\n");
                return 1;
            }
        }
        else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            config->verbose = 1;
        }
        else {
            // C'est probablement le nom du fichier d'instance
            strcpy(config->instance_file, argv[i]);
        }
    }

    // Vérification du fichier d'instance
    if (strlen(config->instance_file) == 0) {
        printf("Erreur: nom de fichier d'instance manquant\n");
        afficher_aide();
        return 1;
    }

    // Vérification des valeurs
    if (config->k_executions <= 0) {
        printf("Erreur: nombre d'exécutions doit être > 0\n");
        return 1;
    }
    if (config->max_evals <= 0) {
        printf("Erreur: nombre maximum d'évaluations doit être > 0\n");
        return 1;
    }

    // Vérification du répertoire de sortie
    struct stat st = {0};
    if (stat(config->output_dir, &st) == -1) {
        // Création du répertoire
        if (mkdir(config->output_dir, 0777) != 0) {
            printf("Erreur: impossible de créer le répertoire %s\n", config->output_dir);
            return 1;
        }
    }

    return 0;
}

/* =============== Question 1 : Chargement =============== */

Instance* load_instance(const char* path) {
    int test=0;
    if (!path) return NULL;
    FILE* file = fopen(path, "r");
    if (!file) return NULL;

    Instance* inst = malloc(sizeof(Instance));
    if (!inst) { fclose(file); return NULL; }

    test=fscanf(file, "%d", &inst->nb_job);
    test=fscanf(file, "%d", &inst->nb_machine);
    test=fscanf(file, "%ld", &inst->seed);

    inst->processing_times = malloc(inst->nb_job * sizeof(int*));
    if (!inst->processing_times) { free(inst); fclose(file); return NULL; }

    for (int i = 0; i < inst->nb_job && test; i++) {
        int job_id, dummy;
        test=fscanf(file, "%d", &job_id);
        test=fscanf(file, "%d", &dummy); // valeur à ignorer

        inst->processing_times[job_id] = malloc(inst->nb_machine * sizeof(int));
        for (int j = 0; j < inst->nb_machine; j++) {
            test=fscanf(file, "%d", &inst->processing_times[job_id][j]);
        }
    }
    fclose(file);
    return inst;
}

/* =============== Question 1 : Affichage =============== */

void afficher(Instance* inst) {
    if (!inst) return;
    printf("Instance: %d jobs, %d machines, seed=%ld\n",
           inst->nb_job, inst->nb_machine, inst->seed);
    for (int i = 0; i < inst->nb_job; i++) {
        printf("Job %d: ", i);
        for (int j = 0; j < inst->nb_machine; j++) {
            printf("%d ", inst->processing_times[i][j]);
        }
        printf("\n");
    }
}


void affiche_paids(Pair *p,int n){
    if (p==NULL)
    {
        return;
    }
    for (int i = 0; i < n; i++)
    {
        printf("(%d,%d),",p[i].a,p[i].b);
        if (i>15)
        {
            printf("\n");
        }
        
    }
}

void afficher_solution(int* sol,int n,int cost){
    if (sol==NULL)
    {
        return;
    }
    for (int i = 0; i < n; i++)
    {
        printf("%d ",sol[i]);
    }
    printf("| cost=%d\n",cost);
}

/* =============== Question 1 : Libération =============== */

void free_memory(Instance* inst) {
    if (!inst) return;
    for (int i = 0; i < inst->nb_job; i++) {
        free(inst->processing_times[i]);
    }
    free(inst->processing_times);
    free(inst);
}

/* =============== Question 2 =============== */

int* generate_valid_solution(int n) {
    int* sol = malloc(n * sizeof(int));
    if (!sol) return NULL;
    for (int i = 0; i < n; i++) sol[i] = i;

    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        echange(sol, i, j);
    }
    return sol;
}

/* =============== Question 3 =============== */

int compute_makespan(Instance* inst, int* solution) {
    if (!inst || !solution) return 0;
    
    int n = inst->nb_job;
    int m = inst->nb_machine;
    
    // Allocation avec vérification d'erreur
    int** comp = malloc(n * sizeof(int*));
    if (!comp) return 0;
    
    for (int i = 0; i < n; i++) {
        comp[i] = calloc(m, sizeof(int));
        if (!comp[i]) {
            // Libérer la mémoire déjà allouée en cas d'erreur
            for (int k = 0; k < i; k++) free(comp[k]);
            free(comp);
            return 0;
        }
    }

    // Calcul du makespan
    for (int i = 0; i < n; i++) {
        int job = solution[i];
        for (int j = 0; j < m; j++) {
            int up = (i > 0) ? comp[i-1][j] : 0;
            int left = (j > 0) ? comp[i][j-1] : 0;
            comp[i][j] = (up > left ? up : left) + inst->processing_times[job][j];
        }
    }
    
    int makespan = comp[n-1][m-1];
    
    // Libération de la mémoire
    for (int i = 0; i < n; i++) free(comp[i]);
    free(comp);
    
    return makespan;
}
/* =============== Question 4 =============== */

void echange(int* sol, int i, int j) {
    if (i == j) return;
    int tmp = sol[i];
    sol[i] = sol[j];
    sol[j] = tmp;
}

/* =============== Question 5 =============== */

void insere(int* sol, int n, int from, int to) {
    if (from == to || from < 0 || from >= n || to < 0 || to >= n) return;
    int job = sol[from];
    if (from < to) {
        for (int i = from; i < to; i++) sol[i] = sol[i+1]; // from=2 to=5 sol[2]=sol[3]; sol[3]=sol[4]
    } else {
        for (int i = from; i > to; i--) sol[i] = sol[i-1];
    }
    sol[to] = job;
}

/* =============== Utilitaires pour les voisins =============== */

Pair* generate_all_pairs(int n, int use_insert, int* total) {
    if (use_insert) {
        *total = n * (n - 1); // from != to
        Pair* pairs = malloc((*total) * sizeof(Pair));
        int idx = 0;
        for (int from = 0; from < n; from++) {
            for (int to = 0; to < n; to++) {
                if (from != to) {
                    pairs[idx].a = from;
                    pairs[idx].b = to;
                    idx++;
                }
            }
        }
        return pairs;
    } else {
        *total = n * (n - 1) / 2;
        Pair* pairs = malloc((*total) * sizeof(Pair));
        int idx = 0;
        for (int i = 0; i < n; i++) {
            for (int j = i + 1; j < n; j++) {
                pairs[idx].a = i;
                pairs[idx].b = j;
                idx++;
            }
        }
        return pairs;
    }
}

void shuffle_pairs(Pair* pairs, int total) {
    for (int i = total - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Pair tmp = pairs[i];
        pairs[i] = pairs[j];
        pairs[j] = tmp;
    }
}

/* =============== Question 6 =============== */

int* marche_aleatoire(Instance* inst, int max_evals, int* best_cost, int use_insert) {
    int n = inst->nb_job;
    int* current = generate_valid_solution(n);
    int current_cost = compute_makespan(inst, current);
    int* best = malloc(n * sizeof(int));
    memcpy(best, current, n * sizeof(int));
    if (verbose) afficher_solution(current,n,current_cost);
    *best_cost = current_cost;
    int evals = 1;

    while (evals < max_evals) {
        int i = rand() % n;
        int j = use_insert ? (rand() % (n - 1)) : (rand() % n);
        if (use_insert && j >= i) j++; // garantir j != i
        if (!use_insert && i == j) j = (j + 1) % n;

        if (use_insert) {
            insere(current, n, i, j);
        } else {
            echange(current, i, j);
        }

        int new_cost = compute_makespan(inst, current);
        evals++;
        if (new_cost < *best_cost) {
            *best_cost = new_cost;
            if (verbose) afficher_solution(current,n,new_cost);
            memcpy(best, current, n * sizeof(int));
        }
    }

    free(current);
    return best;
}

/* =============== Question 7 =============== */

int* climber_first(Instance* inst, int max_evals, int* best_cost, int use_insert) {
    int n = inst->nb_job;
    int* current = generate_valid_solution(n);
    int current_cost = compute_makespan(inst, current);
    int evals = 1;
    *best_cost = current_cost;
    if (verbose) afficher_solution(current,n,current_cost);
    while (evals < max_evals) {
        int total;
        Pair* pairs = generate_all_pairs(n, use_insert, &total);
        shuffle_pairs(pairs, total);
        int improved = 0;

        for (int k = 0; k < total && evals < max_evals; k++) {
            int a = pairs[k].a, b = pairs[k].b;
            if (use_insert) {
                insere(current, n, a, b);
            } else {
                echange(current, a, b);
            }

            int new_cost = compute_makespan(inst, current);
            evals++;

            if (new_cost < current_cost) {
                current_cost = new_cost;
                if (verbose) afficher_solution(current,n,new_cost);
                improved = 1;
                break; // first improvement
            } else {
                // annuler le mouvement
                if (use_insert) {
                    insere(current, n, b, a); // inversion approximative (à améliorer si besoin)
                    // Meilleur : sauvegarder l'état ou recréer
                    // Pour simplicité, on recrée la solution à chaque fois dans une vraie implémentation
                    // Ici, on va plutôt recopier la solution de départ pour chaque voisin
                } else {
                    echange(current, a, b);
                }
            }
        }

        free(pairs);
        if (!improved) break;
    }

    *best_cost = current_cost;
    return current;
}

// Version robuste de climber_first (sans annulation manuelle)

int* climber_first_safe(Instance* inst, int max_evals, int* best_cost, int use_insert) {
    int n = inst->nb_job;
    int* current = generate_valid_solution(n);
    int current_cost = compute_makespan(inst, current);
    int evals = 1;
    *best_cost = current_cost;
    if (verbose) afficher_solution(current,n,current_cost);

    while (evals < max_evals) {
        int total;
        Pair* pairs = generate_all_pairs(n, use_insert, &total);
        shuffle_pairs(pairs, total);
        int improved = 0;

        for (int k = 0; k < total && evals < max_evals; k++) {
            int* candidate = malloc(n * sizeof(int));
            memcpy(candidate, current, n * sizeof(int));

            if (use_insert) {
                insere(candidate, n, pairs[k].a, pairs[k].b);
            } else {
                echange(candidate, pairs[k].a, pairs[k].b);
            }

            int new_cost = compute_makespan(inst, candidate);
            evals++;

            if (new_cost < current_cost) {
                free(current);
                current = candidate;
                current_cost = new_cost;
                if (verbose) afficher_solution(current,n,new_cost);
                improved = 1;
                break;
            } else {
                free(candidate);
            }
        }

        free(pairs);
        if (!improved) break;
    }

    *best_cost = current_cost;
    return current;
}


int* climber_first_safe_2(Instance* inst, int max_evals, int* best_cost, int use_insert,int* sol) {
    int n = inst->nb_job;
    int* current = malloc(sizeof(int)*n);
    memcpy(current,sol, n * sizeof(int));
    int current_cost = compute_makespan(inst, current);
    int evals = 1;
    *best_cost = current_cost;

    while (evals < max_evals) {
        int total;
        Pair* pairs = generate_all_pairs(n, use_insert, &total);
        shuffle_pairs(pairs, total);
        int improved = 0;

        for (int k = 0; k < total && evals < max_evals; k++) {
            int* candidate = malloc(n * sizeof(int));
            memcpy(candidate, current, n * sizeof(int));

            if (use_insert) {
                insere(candidate, n, pairs[k].a, pairs[k].b);
            } else {
                echange(candidate, pairs[k].a, pairs[k].b);
            }

            int new_cost = compute_makespan(inst, candidate);
            evals++;

            if (new_cost < current_cost) {
                free(current);
                current = candidate;
                current_cost = new_cost;
                if (verbose) afficher_solution(current,n,new_cost);
                improved = 1;
                break;
            } else {
                free(candidate);
            }
        }

        free(pairs);
        if (!improved) break;
    }

    *best_cost = current_cost;
    return current;
}

int* climber_best(Instance* inst, int max_evals, int* best_cost, int use_insert) {
    int n = inst->nb_job;
    int* current = generate_valid_solution(n);
    int current_cost = compute_makespan(inst, current);
    int evals = 1;
    *best_cost = current_cost;
    if (verbose) afficher_solution(current,n,current_cost);
    while (evals < max_evals) {
        int total;
        Pair* pairs = generate_all_pairs(n, use_insert, &total);
        shuffle_pairs(pairs, total);

        int best_local_cost = current_cost;
        int best_idx = -1;
        int* best_candidate = NULL;

        for (int k = 0; k < total && evals < max_evals; k++) {
            int* candidate = malloc(n * sizeof(int));
            memcpy(candidate, current, n * sizeof(int));

            if (use_insert) {
                insere(candidate, n, pairs[k].a, pairs[k].b);
            } else {
                echange(candidate, pairs[k].a, pairs[k].b);
            }

            int cost = compute_makespan(inst, candidate);
            evals++;

            if (cost < best_local_cost) {
                best_local_cost = cost;
                best_idx = k;
                if (best_candidate) free(best_candidate);
                best_candidate = candidate;
                if (verbose) afficher_solution(best_candidate,n,best_local_cost);
            } else {
                free(candidate);
            }
        }

        free(pairs);

        if (best_idx == -1) {
            if (best_candidate) free(best_candidate);
            break;
        } else {
            free(current);
            current = best_candidate;
            current_cost = best_local_cost;
        }
    }

    *best_cost = current_cost;
    return current;
}

/* =============== Question 8 =============== */

int* algorithme_perso(Instance* inst, int max_evals, int* best_cost, int use_insert) {
    int n = inst->nb_job;
    int* global_best = NULL;
    *best_cost = INT_MAX;
    int evals = 0;

    while (evals < max_evals) {
        int local_cost;
        int remaining = max_evals - evals;
        int budget = (remaining > 10000) ? 10000 : remaining;

        int* local_sol = climber_first_safe(inst, budget, &local_cost, use_insert);
        
        evals += budget;

        if (local_cost < *best_cost) {
            *best_cost = local_cost;
            if (global_best) free(global_best);
            global_best = local_sol;
            if (verbose) afficher_solution(global_best,n,local_cost);
        } else {
            free(local_sol);
        }
    }

    return global_best;
}


int* algorithme_perso_2(Instance* inst, int max_evals, int* best_cost, int use_insert) {
    int n = inst->nb_job;
    int evals = 0;

    // 1. Définir un budget pour la première recherche (par ex. 10% ou 10k max)
    int init_budget = (max_evals > 10000) ? 10000 : (max_evals / 10);
    if (init_budget <= 0) init_budget = 1; // Assurer un budget minimum
    if (init_budget > max_evals) init_budget = max_evals;

    // 2. Générer la première solution de départ (point de départ de l'itération)
    // Nous utilisons climber_first_safe pour garantir une bonne solution de départ
    int* current_sol = climber_first_safe(inst, init_budget, best_cost, use_insert);
    evals += init_budget; // Compter les évaluations utilisées

    // 3. Initialiser 'global_best'. 
    // global_best doit être une *copie* distincte de current_sol.
    int* global_best = malloc(n * sizeof(int));
    if (!global_best) { free(current_sol); return NULL; } // Gestion d'erreur
    memcpy(global_best, current_sol, n * sizeof(int));
    // *best_cost a déjà été défini par climber_first_safe
    
    // Afficher la première solution trouvée
    if (verbose) {
        printf("Solution initiale pour perso_2:\n");
        afficher_solution(global_best, n, *best_cost);
    }

    while (evals < max_evals) {
        int local_cost;
        int remaining = max_evals - evals;
        int budget = (remaining > 10000) ? 10000 : remaining;
        
        // 4. [FIX] Lancer la recherche locale suivante à partir de 'current_sol'
        // Le résultat est stocké dans 'next_sol' (un NOUVEAU pointeur)
        int* next_sol = climber_first_safe_2(inst, budget, &local_cost, use_insert, current_sol);
        evals += budget;

        // 5. [FIX] Nous n'avons plus besoin de l'ancien point de départ.
        free(current_sol);

        // 6. [FIX] Le résultat de cette recherche ('next_sol') 
        // devient le point de départ pour la prochaine itération.
        current_sol = next_sol;

        // 7. [FIX] Vérifier si ce nouvel optimum local est le meilleur globalement
        if (local_cost < *best_cost) {
            *best_cost = local_cost;
            // Nous devons COPIER le contenu, car 'current_sol' sera libéré
            // à la prochaine itération (étape 5).
            memcpy(global_best, current_sol, n * sizeof(int));
            
            if (verbose) {
                printf("Nouvelle meilleure solution trouvée (perso_2):\n");
                afficher_solution(global_best, n, *best_cost);
            }
        }
        // [FIX] Il n'y a PAS de 'else { free(current_sol); }' !
        // Nous avons besoin de 'current_sol' pour la prochaine itération.
    }

    // 8. [FIX] Nettoyer le dernier 'current_sol' qui n'est plus utilisé
    free(current_sol);

    // 9. Retourner la copie de la meilleure solution
    return global_best;
}

/* =============== Question 9 =============== */

int main(int argc, char* argv[]) {
    Config config;
    if (parse_arguments(argc, argv, &config) != 0) {
        return EXIT_FAILURE;
    }

    verbose = config.verbose;

    char* path = config.instance_file;
    int k = config.k_executions;
    int max_evals = config.max_evals;
    char* output_dir = config.output_dir;
    long seed = config.seed;

    Instance* inst = load_instance(path);
    if (!inst) {
        fprintf(stderr, "Erreur: impossible de charger l'instance.\n");
        return EXIT_FAILURE;
    }

    srand(seed == -1 ? time(NULL) : seed);

    if (verbose) afficher(inst);

    double avg_costs[2][5];
    double avg_times[2][5];

    struct {
        char* name;
        int* (*func)(Instance*, int, int*, int);
    } algos[] = {
        {"Marche aléatoire", marche_aleatoire},
        {"Climber First", climber_first_safe},
        {"Climber Best", climber_best},
        {"Mon algorithme", algorithme_perso},
        {"Mon algorithme v2", algorithme_perso_2}
    };

    for (int use_insert = 0; use_insert <= 1; use_insert++) {
        const char* voisinage = use_insert ? "Insertion" : "Échange";
        printf("\n=== Voisinage: %s ===\n", voisinage);

        for (int a = 0; a < 5; a++) {
            double sum_cost = 0.0;
            double sum_time = 0.0;
            for (int run = 0; run < k; run++) {
                int cost;
                clock_t start = clock();
                int* sol = algos[a].func(inst, max_evals, &cost, use_insert);
                clock_t end = clock();
                sum_time += (double)(end - start) / CLOCKS_PER_SEC;
                sum_cost += cost;
                if (verbose) afficher_solution(sol, inst->nb_job, cost);
                free(sol);
            }
            printf("%s : coût moyen = %.2f, temps moyen = %.3f s\n", algos[a].name, sum_cost / k, sum_time / k);
            avg_costs[use_insert][a] = sum_cost / k;
            avg_times[use_insert][a] = sum_time / k;
        }
    }

    // Sauvegarde des résultats si demandé
    if (strcmp(output_dir, ".") != 0) {
        mkdir(output_dir, 0755);
        const char* voisinages[2] = {"Échange", "Insertion"};
        for (int v = 0; v < 2; v++) {
            char filepath[256];
            sprintf(filepath, "%s/results_%s.txt", output_dir, v == 0 ? "echange" : "insertion");
            FILE* f = fopen(filepath, "w");
            if (f) {
                fprintf(f, "# Résultats pour l'instance %s - Voisinage %s\n", config.instance_file, voisinages[v]);
                fprintf(f, "# k_executions=%d, max_evals=%d, seed=%ld\n", config.k_executions, config.max_evals, config.seed);
                fprintf(f, "# Algorithme\tCoût moyen\tTemps moyen (s)\n");
                for (int a = 0; a < 5; a++) {
                    fprintf(f, "%s\t%.2f\t%.3f\n", algos[a].name, avg_costs[v][a], avg_times[v][a]);
                }
                fclose(f);
            }
        }

        // Générer le script gnuplot
        char filepath[256];
        sprintf(filepath, "%s/plot.gnuplot", output_dir);
        FILE* g = fopen(filepath, "w");
        if (g) {
            fprintf(g, "set terminal png size 800,600\n");
            fprintf(g, "set output '%s/results.png'\n", output_dir);
            fprintf(g, "set title 'Comparaison des algorithmes - Instance %s'\n", config.instance_file);
            fprintf(g, "set xlabel 'Algorithme'\n");
            fprintf(g, "set ylabel 'Coût moyen'\n");
            fprintf(g, "set xtics rotate by -45\n");
            fprintf(g, "set key outside\n");
            fprintf(g, "plot '%s/results_echange.txt' using 2:xtic(1) title 'Échange' with linespoints, '%s/results_insertion.txt' using 2:xtic(1) title 'Insertion' with linespoints\n", output_dir, output_dir);
            fclose(g);
        }
    }

    free_memory(inst);
    return EXIT_SUCCESS;
}
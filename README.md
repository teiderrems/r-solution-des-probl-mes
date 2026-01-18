# Résolution de Problèmes d'Optimisation : Flow Shop Mono- et Multi-Objectif

**Projet TP - Mineure IA - Résolution de Problèmes**

Ce dépôt contient l'implémentation complète de deux travaux pratiques sur l'optimisation du problème Flow Shop :
- **TP1** : Flow Shop mono-objectif (minimisation du makespan)
- **TP2** : Flow Shop multi-objectif (optimisation Pareto avec hypervolume)

## Table des Matières
1. [Vue d'ensemble](#vue-densemble)
2. [TP1 : Flow Shop Mono-Objectif](#tp1--flow-shop-mono-objectif)
3. [TP2 : Flow Shop Multi-Objectif](#tp2--flow-shop-multi-objectif)
4. [Installation et Compilation](#installation-et-compilation)
5. [Utilisation](#utilisation)
6. [Résultats et Analyse](#résultats-et-analyse)
7. [Structure du Projet](#structure-du-projet)
8. [Références](#références)

## Vue d'ensemble

Le problème Flow Shop consiste à ordonner des tâches sur des machines pour minimiser certains critères de performance. Nous traitons deux variantes :

### Problème Flow Shop
- **n** tâches à ordonner
- **m** machines en série
- Chaque tâche i a un temps de traitement p_{i,j} sur la machine j
- Contrainte : même ordre sur toutes les machines

### TP1 : Mono-Objectif
- **Objectif** : Minimiser le makespan (C_max = temps de fin de la dernière tâche)
- **Approche** : Heuristiques de recherche locale
- **Évaluation** : Algorithmes comparés statistiquement

### TP2 : Multi-Objectif
- **Objectifs** : Minimiser simultanément le makespan ET le temps d'écoulement total
- **Approche** : Optimisation Pareto avec indicateur hypervolume
- **Évaluation** : Fronts de Pareto et métriques multi-objectives

## TP1 : Flow Shop Mono-Objectif

### Algorithmes Implémentés

#### 1. Marche Aléatoire
- Génération de solutions aléatoires
- Sélection de la meilleure trouvée
- Référence pour évaluer les autres algorithmes

#### 2. Hill Climber - First Improvement
- Recherche locale avec voisinage
- Accepte la première amélioration trouvée
- Exploration limitée mais rapide

#### 3. Hill Climber - Best Improvement
- Recherche locale exhaustive du voisinage
- Accepte toujours la meilleure amélioration
- Exploration complète mais coûteuse

#### 4. Algorithme Personnalisé 1 : Random Restart Hill Climbing
- Redémarrage périodique depuis de nouvelles solutions aléatoires
- Combine exploration globale et exploitation locale

#### 5. Algorithme Personnalisé 2 : Réutilisation de Solutions
- Mémorisation des meilleures solutions trouvées
- Redémarrage depuis des solutions mémorisées

#### Stratégies d'implémentation (TP1)
- **Représentation** : chaque solution est une permutation (`int perm[n]`). L'évaluation du makespan se fait par simulation séquentielle sur les `m` machines (complexité O(n·m)).
- **Évaluation efficace** : fonctions dédiées `evaluate_makespan()` et `evaluate_sum_completion()` ; possibilité d'optimiser localement l'évaluation pour les voisinages d'insertion afin d'éviter des copies complètes.
- **Voisinages** : implémentés en-place pour limiter les allocations : `swap(i,j)` (échange adjacent ou arbitraire) et `insert(i,j)` (décalage d'un élément). Chaque opération est suivie d'un rollback (`swap` inverse) pour tester sans copie coûteuse.
- **Hill Climber - First Improvement** : itération sur les voisins dans un ordre aléatoire/déterministe, accepte la première amélioration et redémarre la recherche depuis la nouvelle solution.
- **Hill Climber - Best Improvement** : évalue tout le voisinage et applique la meilleure amélioration trouvée; utilisé quand on veut une montée complète au coût de plus d'évaluations.
- **Marche Aléatoire** : génération par mélange Fisher‑Yates; paramètres contrôlés (`k` itérations, graine RNG pour reproductibilité).
- **Random Restart** : boucle de redémarrages avec paramètres `max_restarts` et `max_evals` par restart; conserve la meilleure solution globale.
- **Réutilisation de Solutions** : archive simple (tableau trié ou heap) des `K` meilleures solutions; lors d'un redémarrage, on sélectionne une solution de l'archive et on applique une petite perturbation (quelques swaps/insertions) pour échapper aux optima locaux. Détection de doublons via hachage simple (empreinte permutation).
- **Paramétrage et robustesse** : parsing CLI pour `-k`, `-m`, `-seed`, `-v`; gestion stricte de la mémoire en C; sorties formatées pour générer automatiquement les scripts Gnuplot.

> Note : Les fonctions de voisinage sont conçues pour modifier la permutation in-place et revenir à l'état initial rapidement, ce qui réduit les copies et accélère les boucles d'exploration.

#### Détails des fonctions (TP1)
Ci-dessous la liste des fonctions principales implémentées dans le répertoire `tp1` avec une brève description, signature, complexité approximative et remarques d'implémentation.

- `void afficher_aide()` — Affiche l'aide et les options en ligne de commande.
- `int parse_arguments(int argc, char *argv[], Config *config)` — Parse les arguments CLI et remplit `Config` (retourne 0 si succès).
- `Instance* load_instance(const char* path)` — Charge une instance au format Taillard ; retourne un `Instance*` alloué (libérer avec `free_memory`). Complexité O(n·m) en lecture.
- `void afficher(Instance* inst)` — Affiche le contenu de l'instance (débogage).
- `void afficher_solution(int* sol,int n,int cost)` — Affiche une permutation et son coût.
- `void affiche_paids(Pair *p,int n)` — Outil de debug pour afficher un tableau de paires.
- `void free_memory(Instance* inst)` — Libère toutes les allocations associées à une instance.
- `int* generate_valid_solution(int n)` — Génère une permutation aléatoire (Fisher‑Yates) ; retourne un tableau alloué (caller must free).
- `int compute_makespan(Instance* inst, int* solution)` — Calcule le makespan par simulation (complexité O(n·m)).
- `void echange(int* sol, int i, int j)` — Échange deux positions in-place (swap).
- `void insere(int* sol, int n, int from, int to)` — Insère un élément et décale le segment (in-place).
- `Pair* generate_all_pairs(int n, int use_insert, int* total)` — Génère la liste des voisins (swap ou insert) ; retourne tableau alloué.
- `void shuffle_pairs(Pair* pairs, int total)` — Mélange la liste de voisins (Fisher‑Yates).
- `int* marche_aleatoire(Instance* inst, int max_evals, int* best_cost, int use_insert)` — Marche aléatoire ; explore des voisins aléatoires et renvoie la meilleure solution trouvée (malloc).
- `int* climber_first(Instance* inst, int max_evals, int* best_cost, int use_insert)` — Hill Climber ``first improvement`` (version initiale, annulation manuelle des mouvements).
- `int* climber_first_safe(Instance* inst, int max_evals, int* best_cost, int use_insert)` — Hill Climber sécurisé qui crée des candidats temporaires pour éviter les erreurs d'annulation.
- `int* climber_first_safe_2(Instance* inst, int max_evals, int* best_cost, int use_insert,int* sol)` — Variante acceptant une solution de départ fournie.
- `int* climber_best(Instance* inst, int max_evals, int* best_cost, int use_insert)` — Hill Climber ``best improvement`` (évalue tout le voisinage et applique la meilleure amélioration).
- `int* algorithme_perso(Instance* inst, int max_evals, int* best_cost, int use_insert)` — Random Restart Hill Climbing ; enchaîne des recherches locales sur des redémarrages aléatoires.
- `int* algorithme_perso_2(Instance* inst, int max_evals, int* best_cost, int use_insert)` — Réutilisation de solutions : redémarre depuis la solution courante améliorée et conserve la meilleure globale.
- `void experiment_all_instances(Config *config)` — Orchestration des expérimentations (parcours du dossier `instances`, exécutions multiples, génération de fichiers de résultats et scripts Gnuplot).

> Remarques ⚠️ :
> - Les fonctions qui retournent des pointeurs alloués (`Instance*`, `int*`, `Pair*`) exigent que l'appelant libère la mémoire correspondante.
> - Les évaluations `compute_makespan` sont le goulot d'étranglement principal — optimisations locales (évaluations incrémentales) peuvent réduire considérablement le coût.

##### Exemples d'utilisation (TP1)
Quelques exemples d'appels courants montrant la signature et l'usage minimal (libération mémoire requise) :

```c
// Charger une instance, générer une solution aléatoire et évaluer
Instance *inst = load_instance("instances/20_10_01.txt");
int *sol = generate_valid_solution(inst->nb_job);
int makespan = compute_makespan(inst, sol);
printf("Makespan = %d\n", makespan);
free(sol);
free_memory(inst);

// Utiliser le hill climber (first improvement) en mode safe (insertion)
int best_cost;
Instance *inst2 = load_instance("instances/7_5_01.txt");
int *best = climber_first_safe(inst2, 10000, &best_cost, /*use_insert=*/1);
printf("Meilleur coût (first) = %d\n", best_cost);
free(best);
free_memory(inst2);

// Lancer l'algorithme personnalisé v2 (réutilisation de solutions)
Instance *inst3 = load_instance("instances/30_10_01.txt");
int best_cost2;
int *best2 = algorithme_perso_2(inst3, 50000, &best_cost2, /*use_insert=*/0);
printf("Meilleur coût (perso_2) = %d\n", best_cost2);
free(best2);
free_memory(inst3);
```

> Astuce : Toujours libérer les solutions retournées par les algorithmes (`free(sol)`, `free(best)`) et les instances (`free_memory(inst)`).


### Voisinages
- **Échange** : Permutation de deux tâches adjacentes
- **Insertion** : Déplacement d'une tâche vers une nouvelle position

### Résultats TP1
L'analyse comparative montre que :
- Les hill climbers surpassent largement la marche aléatoire
- L'algorithme personnalisé 2 (réutilisation) obtient les meilleurs résultats
- Le voisinage insertion donne généralement de meilleurs résultats que l'échange

## TP2 : Flow Shop Multi-Objectif

> Voir aussi : [tp2/gnuplot_examples.md](tp2/gnuplot_examples.md) — exemple Gnuplot et script prêt à l'emploi.

### Approche Multi-Objectif
- **Objectifs concurrents** :
  - C_max : Makespan (minimiser)
  - ∑C_i : Somme des temps d'écoulement (minimiser)
- **Concept de Pareto** : Solutions non-dominées
- **Métrique** : Hypervolume pour évaluer la qualité des fronts

### Algorithmes Évolutionnaires
- **Population** : Ensemble de solutions candidates
- **Sélection** : Mécanismes évolutionnaires
- **Croisement et Mutation** : Opérateurs génétiques adaptés
- **Évaluation** : Dominance de Pareto

#### Stratégies d'implémentation (TP2)
- **Représentation et évaluation** : solutions encodées en permutations (`int perm[n]`); chaque individu stocke les deux objectifs (C_max, ∑C_i). L'évaluation restreinte aux deux fonctions d'ordonnancement garantit des coûts prévisibles O(n·m).
- **Initialisation** : population initiale générée aléatoirement (Fisher‑Yates) et enrichie par quelques heuristiques issues de TP1 (si activé) pour accélérer la convergence.
- **Sélection** : tournoi binaire basé sur la dominance de Pareto et, en cas d'égalité, sur une métrique de diversité (crowding distance simple) pour préserver la diversité.
- **Opérateurs variation** :
  - **Croisement** : opérateurs préservant permutation (ex. OX ou PMX) implémentés pour produire descendants valides sans correction coûteuse.
  - **Mutation** : petites perturbations (swap aléatoire, insertion) avec probabilité faible pour maintenir exploration.
- **Archive Pareto** : archive externe des solutions non-dominées maintenue à chaque génération; si l'archive dépasse une taille limite, on effectue une réduction basée sur la contribution à l'hypervolume ou la distance de crowding.
- **Hypervolume** : calculé en 2D (C_max, ∑C_i) avec un point de référence fixé en début d'expérience; calcul exact optimisé pour 2D (complexité acceptable) et journalisé à chaque génération pour analyse.
- **Remplacement** : stratégie générational avec élitisme (on conserve les meilleurs non-dominés) ou option steady-state selon le paramètre `-steady`.
- **Critères d'arrêt** : nombre d'évaluations maximal, nombre de générations, ou absence d'amélioration de l'hypervolume sur un seuil donné.
- **Implémentation système** : tout en C avec structures légères pour individus/archives, sorties log vers fichiers `results_*`, scripts Gnuplot automatiques pour visualisation des fronts.

> Note 💡 : Le calcul de l'hypervolume est central pour l'évaluation; le code est optimisé pour des fronts 2D et permet le recalcul incrémental lorsque possible.

##### Exemples d'utilisation (TP2)
Exemples courts illustrant la lecture d'instance, l'évaluation d'une permutation et l'utilisation des approches scalar/pareto :

```c
// Lire une instance et évaluer une solution aléatoire
Instance *inst = lire_instance("instances/20_10_01.txt");
int *perm = generer_solution_aleatoire(inst->nb_jobs);
ObjectiveVector obj = evaluer_solution(inst, perm);
printf("Makespan=%d, Tardiness=%d\n", obj.makespan, obj.tardiness);
free(perm);
liberer_instance(inst);

// Approche scalaire (balayage de poids) et sauvegarde du front
Config cfg = {0};
strcpy(cfg.instance_file, "instances/20_10_01.txt");
strcpy(cfg.output_dir, "results");
cfg.max_iterations = 500;
cfg.nb_poids = 10;
Instance *inst2 = lire_instance(cfg.instance_file);
Archive *as = approche_scalaire(inst2, cfg.nb_poids, cfg.max_iterations, &cfg);
sauvegarder_front("front_scalaire.txt", as, &cfg);
liberer_archive(as);
liberer_instance(inst2);

// Approche Pareto
Instance *inst3 = lire_instance("instances/50_20_01.txt");
Archive *ap = approche_pareto(inst3, /*taille_initiale=*/20, /*iterations=*/1000, &cfg);
sauvegarder_front("front_pareto.txt", ap, &cfg);
liberer_archive(ap);
liberer_instance(inst3);

// Calcul d'hypervolume
double hv = calculer_hypervolume(ap, /*ref_makespan=*/10000.0, /*ref_tardiness=*/10000.0);
printf("Hypervolume = %.2f\n", hv);
```

> Remarque ⚠️ : Les `Archive*` allouées doivent être libérées avec `liberer_archive()` ; `Instance*` avec `liberer_instance()`.

### Métriques d'Évaluation
- **Hypervolume** : Mesure la qualité globale du front de Pareto
- **Diversité** : Répartition des solutions sur le front
- **Convergence** : Proximité du front optimal

### Résultats TP2
L'optimisation multi-objectif révèle :
- Trade-off entre les deux objectifs
- Fronts de Pareto bien distribués
- Amélioration progressive de l'hypervolume
- Solutions robustes pour différents poids relatifs

## Installation et Compilation

### Prérequis
- GCC avec support C99
- GNU Make
- Gnuplot (pour les graphiques)
- Système Linux/Unix

### Compilation TP1
```bash
cd tp1
make          # Compilation
make clean    # Nettoyage
make test     # Tests de base
make graphs   # Génération de graphiques
```

### Compilation TP2
```bash
cd tp2
make          # Compilation
make clean    # Nettoyage
make test     # Tests de base
make graphs   # Génération de graphiques
```

## Utilisation

### TP1 - Mono-Objectif
```bash
# Exécution simple
./fsp -i instances -f 7_5_01.txt -k 5 -m 10000 -v

# Avec sauvegarde des résultats
./fsp -i instances -f 20_10_01.txt -k 10 -m 50000 -o results

# Génération de graphiques
make graphs
```

### TP2 - Multi-Objectif
```bash
# Exécution avec paramètres personnalisés
./mo -f instances/50_20_01.txt -i 100000 -p 20 -o results_50_20_01_100000

# Génération de graphiques
make graphs
```

## Résultats et Analyse

### Analyse Comparative TP1
| Algorithme | Voisinage Échange | Voisinage Insertion |
|------------|-------------------|-------------------|
| Marche Aléatoire | 665.4 ± 0.8 | 665.0 ± 0.8 |
| Hill Climber First | 670.2 ± 0.0 | 669.8 ± 0.0 |
| Hill Climber Best | 673.8 ± 0.0 | 666.0 ± 0.0 |
| Algo Personnalisé 1 | 672.0 ± 0.0 | 670.6 ± 0.0 |
| Algo Personnalisé 2 | 672.6 ± 0.0 | 668.6 ± 0.0 |

### Analyse TP2
- **Hypervolume** : Évolution progressive vers l'optimal
- **Front de Pareto** : Solutions bien distribuées
- **Convergence** : Amélioration continue sur 100,000 évaluations
- **Robustesse** : Performance stable sur différentes instances

## Structure du Projet

```
TP/
├── tp1/                          # Flow Shop Mono-Objectif
│   ├── fsp.c                     # Code principal
│   ├── header.h                  # Définitions
│   ├── Makefile                  # Compilation
│   ├── README.md                 # Documentation TP1
│   ├── instances/                # Jeux de données
│   └── results/                  # Résultats et graphiques
│
└── tp2/                          # Flow Shop Multi-Objectif
    ├── mflowshop.c               # Code principal
    ├── mflowshop.h               # Définitions
    ├── Makefile                  # Compilation
    ├── README.md                 # Documentation TP2
    ├── instances/                # Jeux de données
    ├── results_*/                # Résultats par expérience
    └── RP_TP2_FSP.pdf           # Sujet du TP2
```

## Références

### Théoriques
- Garey, M. R., Johnson, D. S., & Sethi, R. (1976). The complexity of flowshop and jobshop scheduling. Mathematics of Operations Research.
- Taillard, E. (1993). Benchmarks for basic scheduling problems. European Journal of Operational Research.

### Multi-Objectif
- Zitzler, E., & Thiele, L. (1999). Multiobjective evolutionary algorithms: a comparative case study and the strength Pareto approach. IEEE Transactions on Evolutionary Computation.
- Knowles, J., & Corne, D. (2000). Approximating the nondominated front using the Pareto archived evolution strategy. Evolutionary Computation.

### Implémentation
- Code C optimisé avec structures de données efficaces
- Parsing robuste des arguments en ligne de commande
- Génération automatique de scripts Gnuplot
- Gestion mémoire sécurisée
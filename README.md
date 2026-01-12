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

### Voisinages
- **Échange** : Permutation de deux tâches adjacentes
- **Insertion** : Déplacement d'une tâche vers une nouvelle position

### Résultats TP1
L'analyse comparative montre que :
- Les hill climbers surpassent largement la marche aléatoire
- L'algorithme personnalisé 2 (réutilisation) obtient les meilleurs résultats
- Le voisinage insertion donne généralement de meilleurs résultats que l'échange

## TP2 : Flow Shop Multi-Objectif

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

---

**Auteur** : Teida Noutsa  
**Date** : Janvier 2026  
**Contexte** : Mineure IA - Résolution de Problèmes
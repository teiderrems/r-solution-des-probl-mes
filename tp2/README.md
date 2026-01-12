# Flow Shop Permutation (FSP) - Solveur Multi-Objectif

Un solveur pour le problème Flow Shop Permutation (FSP) multi-objectif implémentant deux approches d'optimisation : l'approche scalaire par pondération et l'approche Pareto directe.

## 📋 Objectifs du Projet

Ce projet résout le problème FSP avec deux objectifs à minimiser :
- **Makespan (Cmax)** : Temps total pour compléter tous les jobs
- **Tardiness totale (Tsum)** : Somme des retards des jobs par rapport à leurs dates d'échéance

## ✨ Fonctionnalités

- **Lecture d'instances** : Format standard avec jobs, machines et temps de traitement
- **Évaluation de solutions** : Calcul du makespan et de la tardiness totale
- **Approche scalaire** : Optimisation par pondération des objectifs avec recherche locale
- **Approche Pareto** : Maintien d'une archive de solutions non-dominées
- **Filtrage offline/online** : Gestion d'archives de solutions optimales au sens de Pareto
- **Analyse comparative** : Calcul d'hypervolume et génération de statistiques
- **Visualisation** : Génération automatique de graphiques avec gnuplot

## 🏗️ Structure du Projet

```
.
├── Makefile              # Fichier de compilation et gestion du projet
├── mflowshop.c          # Code source principal
├── mflowshop.h          # En-tête avec déclarations
├── instances/           # Répertoire des instances de test
│   ├── 7_5_01.txt
│   ├── 20_10_01.txt
│   └── 50_10_01.txt
└── results/            # Répertoire de sortie (généré automatiquement)
```

## 📦 Installation et Compilation

### Prérequis
- GCC (GNU Compiler Collection)
- Make
- Gnuplot (optionnel, pour la visualisation)

### Installation sous Ubuntu/Debian
```bash
sudo apt update
sudo apt install gcc make gnuplot
```

### Compilation
```bash
# Configuration initiale
make setup

# Compilation standard
make

# Compilation en mode debug
make debug
```

## 🚀 Utilisation

### Commandes de base

```bash
# Afficher l'aide
make help

# Tester rapidement
make test

# Exécuter sur une instance
make run INSTANCE=instances/7_5_01.txt

# Exécuter avec paramètres personnalisés
make run INSTANCE=instances/20_10_01.txt ARGS="-o 1000 -p 20 -i 500 -v"

# Exécuter sur toutes les instances
make run-all ARGS="-o 500 -p 10 -i 200"

# Générer les graphiques (après exécution)
make graphs

# Nettoyer les fichiers de compilation
make clean

# Nettoyage complet (incluant les résultats)
make distclean
```

### Options du programme

| Option | Description | Valeur par défaut |
|--------|-------------|-------------------|
| `-h, --help` | Affiche l'aide | - |
| `-o, --offline N` | Nombre de solutions pour filtrage offline | 500 |
| `-p, --poids N` | Nombre de poids pour approche scalaire | 10 |
| `-i, --iterations N` | Nombre maximal d'itérations | 500 |
| `-s, --size N` | Taille initiale pour approche Pareto | 10 |
| `-r, --runs N` | Nombre d'exécutions pour statistiques | 1 |
| `-a, --algo TYPE` | Algorithme à exécuter : all, scalar, pareto | all |
| `-v, --verbose` | Mode verbeux pour plus de détails | désactivé |
| `--no-analyze` | Désactive l'analyse comparative | désactivé |
| `-od, --output-dir DIR` | Répertoire de sortie pour résultats | 
| `-ip, --input-dir DIR` | Répertoire des instances | 
| `-f, --instance-file FILE` | Nom de l'instance.

## 📊 Format des Instances

Le format des fichiers d'instance est le suivant :
```
<nb_jobs>
<nb_machines>
<seed>
<job_id> 
<due_date> 
<processing_time_machine_1> 
.
.
. 
<job_id> 
<due_date>
<processing_time_machine_m>
...
```

Exemple pour un problème avec 7 jobs et 5 machines :
```
7
5
123456789
0 
468 
54 79 16 66 58
1 
568
87 64 22 78 34
...
```

## 📈 Résultats et Visualisation

### Fichiers générés
Les résultats sont sauvegardés avec un nom préfixé par l'instance et les paramètres :
- `[instance]_i[iterations]_p[paretosize]_front_scalaire_run_[n].dat` : Solutions approche scalaire
- `[instance]_i[iterations]_p[paretosize]_front_pareto_run_[n].dat` : Solutions approche Pareto
- `[instance]_i[iterations]_p[paretosize]_front_offline.dat` : Solutions filtrage offline
- `[instance]_i[iterations]_p[paretosize]_statistiques_run_[n].txt` : Statistiques par exécution
- `[instance]_i[iterations]_p[paretosize]_front_comparaison.gnuplot` : Script de visualisation

### Génération des graphiques

```bash
# Après avoir exécuté le programme
make graphs

# Ou manuellement
cd results
gnuplot "50_10_01_i500_p10_front_comparaison.gnuplot"
```

Le graphique généré montrera la comparaison des fronts Pareto des deux approches.

## 📚 Développement

### Structure du code

- **Structures principales** :
  - `Instance` : Représente une instance du problème
  - `Job` : Caractéristiques d'un job
  - `ObjectiveVector` : Vecteur d'objectifs (makespan, tardiness)
  - `Archive` : Collection de solutions non-dominées
  - `Config` : Configuration d'exécution

- **Fonctions principales** :
  - `evaluer_solution()` : Évalue une permutation de jobs
  - `approche_scalaire()` : Implémente l'approche par pondération
  - `approche_pareto()` : Implémente l'approche Pareto directe
  - `filtrage_online()` / `filtrage_offline()` : Gestion d'archives

### Tests et validation

```bash
# Tests rapides
make test

# Exécution sur instance test
./mo -o 10 -p 2 -i 10 --no-analyze -f instances/7_5_01.txt

# Exécution sur instance test
./mo -o 10 -p 2 -i 10 --no-analyze -f 7_5_01.txt -ip instances
```

## 🎯 Exemples d'utilisation

### Comparaison simple
```bash
make run INSTANCE=instances/7_5_01.txt ARGS="-o 1000 -p 20 -v"
```

### Analyse approfondie
```bash
make run INSTANCE=instances/50_10_01.txt ARGS="-o 10000 -p 40 -i 1000 -r 5 -v --output-dir results_detailed"
make graphs
```

## 📊 Interprétation des résultats

### Métriques principales
1. **Hypervolume** : Mesure la qualité du front Pareto (plus élevé = meilleur)
2. **Nombre de solutions** : Combien de solutions non-dominées trouvées
3. **Temps d'exécution** : Performance des algorithmes

### Exemple de sortie
```
=== STATISTIQUES GLOBALES (5 runs) ===
Instance: 50_10_01
Iterations: 500, Pareto size: 10
Moyenne hypervolume scalaire: 60048146.12
Moyenne hypervolume Pareto:   25552257.52
Moyenne temps scalaire:       6.244 s
Moyenne temps Pareto:         0.043 s
Conclusion: Approche scalaire en moyenne meilleure de +135.0%
```

## 🤝 Contribution

1. Fork le projet
2. Créez une branche pour votre fonctionnalité (`git checkout -b feature/AmazingFeature`)
3. Commit vos changements (`git commit -m 'Add some AmazingFeature'`)
4. Push vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrez une Pull Request

## 📄 Licence

Ce projet est distribué sous licence MIT. Voir le fichier `LICENSE` pour plus de détails.

## 👥 Auteurs

- Rémi Raoul
- Basé sur le TP de la Mineure IA - Résolution de problèmes multi-objectif
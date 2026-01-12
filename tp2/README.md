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
├── Makefile              # Makefile minimaliste pour compilation
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
# Compilation simple
make

# Nettoyer les fichiers de compilation
make clean
```

## 🚀 Utilisation

### Commandes de base

```bash
# Compiler le programme
make

# Tester rapidement avec différents arguments
make test

# Générer les graphiques (dans le répertoire courant)
make graphs

# Lancer l'expérimentation sur toutes les instances
make experiment

# Nettoyer les fichiers de compilation
make clean
```

### Exécution manuelle

```bash
# Exécution simple
./mo -f instances/7_5_01.txt

# Avec tous les arguments
./mo -f 7_5_01.txt -id instances -o 1000 -p 20 -i 500 -s 10 -r 5 -v -od results
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

## 🧪 Mode Expérimentation

Le mode expérimentation permet d'exécuter automatiquement les algorithmes sur toutes les instances disponibles avec diverses configurations de paramètres (`max_iterations` et `pareto_size`).

### Utilisation
```bash
# Via Makefile
make experiment

# Ou directement
./mo --experiment
```

### Ce que fait le mode expérimentation
- Parcourt toutes les instances dans le répertoire `instances/`
- Teste différentes combinaisons de paramètres :
  - `max_iterations` : 100, 500, 1000
  - `pareto_size` : 5, 10, 20
- Exécute les approches scalaire et Pareto pour chaque configuration
- Calcule l'hypervolume moyen et génère des statistiques
- Sauvegarde les résultats dans `experiment_results_tp2.txt`
- Génère des scripts Gnuplot pour :
  - Comparaison globale des hypervolumes
  - Comparaison par instance
  - Comparaison par configuration de paramètres
  - Comparaison par instance et configuration

### Fichiers générés en mode expérimentation
- `experiment_results_tp2.txt` : Résultats tabulés avec moyennes d'hypervolume
- Scripts Gnuplot pour visualisations automatiques :
  - `experiment_plot_tp2.gnuplot` : Graphique global
  - `[instance]_plot.gnuplot` : Graphique par instance
  - `config_[max_iter]_[pareto_size]_plot.gnuplot` : Graphique par configuration
  - `[instance]_max_iter_[max_iter]_pareto_size_[pareto_size]_plot.gnuplot` : Graphique par instance et config

## 📈 Visualisation des Fronts Pareto

Le programme génère automatiquement des graphiques comparant les fronts Pareto obtenus par les deux approches (scalaire et Pareto directe).

### Graphiques de Comparaison de Fronts
Pour chaque instance et configuration testée, un script gnuplot est généré :
- `[instance]_i[max_iter]_p[pareto_size]_front_comparaison.gnuplot`

Ces scripts génèrent des graphiques PNG montrant :
- **Points bleus** : Solutions du front Pareto de l'approche scalaire
- **Points rouges** : Solutions du front Pareto de l'approche Pareto directe
- **Axes** : Makespan (X) vs Tardiness totale (Y)

### Génération des Graphiques
```bash
# Depuis le répertoire de résultats
cd results/
gnuplot 7_5_01_i1000_p20_front_comparaison.gnuplot
```

### Fichiers de Données
Les fronts sont sauvegardés dans des fichiers `.dat` :
- `[instance]_i[max_iter]_p[pareto_size]_front_scalaire_run_1.dat`
- `[instance]_i[max_iter]_p[pareto_size]_front_pareto_run_1.dat`

Format des fichiers de données :
```
# Makespan Tardiness
1234.5 567.8
2345.6 678.9
...
```

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
# Après avoir exécuté le programme dans un répertoire de résultats
cd results
make graphs

# Ou manuellement
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
# Tests rapides avec différents arguments
make test

# Exécution sur instance test
./mo -f 7_5_01.txt -id instances -o 10 -p 2 -i 10 --no-analyze
```

## 🎯 Exemples d'utilisation

### Comparaison simple
```bash
./mo -f 7_5_01.txt -id instances -o 1000 -p 20 -i 500 -v
```

### Analyse approfondie
```bash
./mo -f 50_10_01.txt -id instances -o 10000 -p 40 -i 1000 -r 5 -v -od results_detailed
cd results_detailed
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
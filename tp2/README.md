# Flow Shop Permutation (FSP) - Solveur Multi-Objectif

Un solveur pour le problème Flow Shop Permutation (FSP) multi-objectif implémentant deux approches d'optimisation : l'approche scalaire par pondération et l'approche Pareto directe.

## Objectifs du Projet

Ce projet résout le problème FSP avec deux objectifs à minimiser :
- **Makespan (Cmax)** : Temps total pour compléter tous les jobs
- **Tardiness totale (Tsum)** : Somme des retards des jobs par rapport à leurs dates d'échéance

## Fonctionnalités

- **Lecture d'instances** : Format standard avec jobs, machines et temps de traitement
- **Évaluation de solutions** : Calcul du makespan et de la tardiness totale
- **Approche scalaire** : Optimisation par pondération des objectifs avec recherche locale
- **Approche Pareto** : Maintien d'une archive de solutions non-dominées
- **Filtrage offline/online** : Gestion d'archives de solutions optimales au sens de Pareto
- **Analyse comparative** : Calcul d'hypervolume et génération de statistiques
- **Visualisation** : Génération automatique de graphiques avec gnuplot

## Structure du Projet

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

## Installation et Compilation

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

## Utilisation

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

## Mode Expérimentation

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

## Visualisation des Fronts Pareto

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

### Expérimentation : Visualisation des fronts Pareto

- Les scripts de comparaison (`[instance]_i[max_iter]_p[pareto_size]_front_comparaison.gnuplot`) génèrent un PNG par instance et configuration (`*_front_comparaison.png`) montrant les fronts obtenus par l'approche scalaire (bleu) et l'approche Pareto (rouge).

- Fichiers utiles pour l'analyse détaillée :
  - Les fichiers `.dat` (un fichier par run) permettent d'agréger plusieurs runs et d'extraire statistiques (moyennes de points projetés, densité, etc.).
  - Le fichier `test_results_front/` contient de nombreux exemples prêts à l'emploi (`20_10_01_i1000_p10_front_comparaison.png`, `30_20_01_i500_p20_front_comparaison.png`, ...).

- Commandes pratiques :
  - Générer un graphique unique :
    ```bash
    cd test_results_front
    gnuplot 20_10_01_i1000_p10_front_comparaison.gnuplot
    ```
  - Superposer plusieurs runs (ex. pour l'agrégation) : concaténer ou écrire un petit script Python qui lit tous les fichiers `*_front_pareto_run_*.dat` et trace les nuages de points avec transparence pour évaluer la variabilité.

- Points d'interprétation (style rapport) :
  1. **Recouvrement des fronts** : observer si les points de l'approche scalaire dominent ceux de l'approche Pareto (ou inversement) selon la région des objectifs. Un recouvrement partiel indique complémentarité.
  2. **Diversité vs Convergence** : la distribution des points (largeur du front) donne une idée de la diversité; la proximité au front théorique (si connu) renseigne sur la convergence.
  3. **Annotation par métriques** : il est utile d'annoter les figures avec les hypervolumes moyens (calculés dans `experiment_results_tp2.txt`) pour fournir une mesure quantitative en plus de la visualisation.

- Exemple de figure (extrait) :

  ![Comparaison front Pareto 20_10_01](test_results_front/20_10_01_i1000_p10_front_comparaison.png)
  *Légende : bleu = approche scalaire, rouge = approche Pareto directe. Les données proviennent des fichiers `.dat` associés.*

> Recommandation : Pour publications ou rapports, regrouper plusieurs runs (≥ 5) et afficher bandes d'incertitude (p.ex. enveloppe min/max ou intervalle interquartile) afin de rendre la comparaison robuste et statistiquement significative.

Pour l'agrégation et la visualisation multi-run, utilisez les scripts Gnuplot fournis dans `test_results_front/` et les fichiers `.dat` générés par l'exécution (p.ex. `[instance]_i[max_iter]_p[pareto_size]_front_comparaison.gnuplot` et leurs PNG associés).

> Recommandation : pour des comparaisons robustes, regrouper plusieurs runs (≥ 5) et afficher des bandes d'incertitude (ex. min/max ou intervalle interquartile) à partir des `.dat` existants.

## Format des Instances

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

## Résultats et Visualisation

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

##  Développement

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

## Exemples d'utilisation

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

## Interprétation des résultats

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

---

## Résultats expérimentaux (TP2)

### Données et graphiques
- Les résultats agrégés sont sauvegardés dans `test_results_front/experiment_results_tp2.txt`.
- Le graphique synthétique global est disponible : `test_results_front/experiment_results_tp2.png`.
- Graphiques complémentaires (hypervolume par instance / configuration) :
  - `test_results_front/hypervolume_vs_instance_max_iter_100_pareto_size_5.png` (ex.)
  - `test_results_front/hypervolume_vs_instance_max_iter_500_pareto_size_10.png` (ex.)
  - `test_results_front/hypervolume_vs_instance_max_iter_1000_pareto_size_20.png` (ex.)

> Voir le répertoire `test_results_front/` pour la collection complète de figures et scripts Gnuplot.

### Observations (analyse succincte)
1. **Tendance générale** : L'approche scalaire produit, en moyenne, des hypervolumes plus élevés que l'approche Pareto directe sur de nombreuses instances et configurations testées. Ceci indique une meilleure couverture de l'espace objectif pour les paramètres explorés.

2. **Dépendance à la configuration** : Les performances relatives des deux approches varient selon la **taille d'archive Pareto** et le **nombre d'itérations**. Par exemple, pour `30_20_01` et `max_iter=1000, pareto_size=20`, l'approche Pareto dépasse notablement l'approche scalaire (hypervolume Pareto supérieur), ce qui montre que l'approche Pareto profite d'une archive plus large et d'itérations suffisantes.

3. **Comportement par instance** : Sur les instances de grande taille (`50_10_01`, `50_20_01`), les hypervolumes absolus sont plus élevés (échelle) et l'approche scalaire reste souvent dominante, probablement en raison d'une exploration plus agressive permise par la scalarisation et le budget d'itérations choisis.

4. **Nombre de solutions** : Le nombre moyen de solutions non-dominées trouvé par l'approche Pareto est généralement supérieur ou égal à celui de l'approche scalaire (dépend fortement des paramètres), ce qui est attendu car l'approche Pareto vise explicitement la diversité.

5. **Temps d'exécution** : Les durées moyennes signalées montrent que l'approche scalaire peut être plus coûteuse en temps pour certaines configurations (dépend de l'implémentation et du nombre d'itérations), mais l'écart dépend fortement de la taille de l'instance et des paramètres d'exécution.

### Interprétation et recommandations
- **Conclusion principale** : Pour des comparaisons directes sous budget d'itérations modéré, l'approche scalaire tend à fournir un hypervolume moyen supérieur — utile si l'objectif est d'optimiser l'hypervolume global rapidement.
- **Recommandation pratique** : Si la diversité du front est prioritaire (p.ex. usage multi-critère réel), privilégier l'approche Pareto avec une **taille d'archive plus importante** et un **nombre d'itérations élevé**, car elle montre de meilleures performances sur certaines instances et configurations.
- **Pistes d'amélioration** : Étendre les tests (plus de runs) pour obtenir intervalles de confiance, tester d'autres opérateurs génétiques/paramètres, et comparer la contribution de la taille d'archive vs. le budget d'itérations sur la dispersion du front.

### Figures (exemples)

- Figure 1 — Synthèse des hypervolumes par configuration :

  ![Synthèse hypervolumes](test_results_front/experiment_results_tp2.png)
  *Légende : hypervolume moyen (axe Y) pour chaque configuration d'instance (axe X), comparant approches scalaire (bleu) et Pareto (rouge).* 

- Figure 2 — Hypervolume par instance (exemples) :

  ![Hypervolume par instance (ex.)](test_results_front/hypervolume_vs_instance_max_iter_1000_pareto_size_10.png)
  *Légende : évolution comparée des hypervolumes pour chaque instance sous `max_iter=1000, pareto_size=10`.*


> Remarque : Ces figures servent d'illustration. Pour une analyse robuste, produire des intervalles de confiance (p.ex. ±std) sur plusieurs runs et tester une gamme plus large de paramètres.

---


##  Contribution

1. Fork le projet
2. Créez une branche pour votre fonctionnalité (`git checkout -b feature/AmazingFeature`)
3. Commit vos changements (`git commit -m 'Add some AmazingFeature'`)
4. Push vers la branche (`git push origin feature/AmazingFeature`)
5. Ouvrez une Pull Request

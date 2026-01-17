**Projet Flow Shop (TP)**

- **Description**: Ce dépôt contient une implémentation en C d'un solveur pour le problème Flow Shop de permutation (calcul du makespan / Cmax) et plusieurs heuristiques de recherche locale (marche aléatoire, hill-climbers "first" et "best", deux variantes d'algorithmes personnalisés).

**Fichiers clés**:
- `fsp.c` : code principal contenant le chargement d'instances, l'évaluation (compute_makespan), opérateurs de voisinage (`echange`, `insere`) et les algorithmes heuristiques.
- `header.h` : définitions de structures et prototypes de fonctions.
- `instances/` : exemples d'instances (format Taillard) utilisées pour les tests.

**Compilation**:
- Depuis le répertoire `tp1/`, utiliser le Makefile minimaliste :

```bash
make          # Compile le programme
make clean    # Nettoie les fichiers compilés
make test     # Teste l'aide et exécute un exemple simple
make graphs   # Génère les données et le graphique PNG
```

Ou compiler manuellement avec GCC :
```bash
gcc -Wall -Wextra -O2 -std=c99 fsp.c -o fsp -lm
```

**Exécution**:

```bash
./fsp [OPTIONS] <instance_file>
```

**Utilisation rapide avec le Makefile**:
```bash
make test      # Test basique avec l'instance 7_5_01.txt
make graphs    # Génère et affiche les graphiques pour 7_5_01.txt
make experiment # Lance l'expérimentation sur toutes les instances avec différentes configurations
```

**Mode expérimentation**:
- Utiliser `make experiment` ou `./fsp --experiment` pour exécuter automatiquement les algorithmes sur toutes les instances disponibles avec diverses configurations de paramètres (`max_evals` et `k_executions`).
- Les résultats sont sauvegardés dans `experiment_results_tp1.txt` avec moyennes et écarts-types.
- Des scripts Gnuplot sont générés pour visualiser les comparaisons globales et par instance.
- Nécessite Gnuplot pour générer les graphiques PNG.

Options disponibles :
- `-k, --executions <n>` : nombre d'exécutions à moyenner par algorithme (défaut : 10)
- `-m, --max-evals <n>` : budget maximum d'évaluations (défaut : 10000)
- `-o, --output-dir <dir>` : répertoire de sortie pour sauvegarder les résultats (défaut : .)
- `-s, --seed <n>` : graine pour le générateur aléatoire (défaut : basée sur l'heure)
- `-v, --verbose` : active l'affichage verbeux
- `-f, --instance-file <instance_file>` : nom de l'instance
- `-i, --input-dir <dir>` : répertoire de l'instance 
- `-h, --help` : affiche l'aide

Exemples :
```
./fsp -f instances/20_10_01.txt
./fsp --instance-file instances/20_10_01.txt
./fsp -i instances -f 20_10_01.txt
./fsp --input-dir instances --instance-file 20_10_01.txt
./fsp -k 5 -m 50000 -o results -f instances/20_10_01.txt
./fsp -v -s 12345 -f instances/7_5_01.txt
```

Le programme lance les algorithmes pour les deux voisinages (`Échange` et `Insertion`) et affiche le coût moyen (makespan) et le temps d'exécution moyen pour chaque algorithme. En mode verbeux (`-v`), il affiche également l'instance et les solutions intermédiaires.

Si un répertoire de sortie est spécifié (`-o`), le programme sauvegarde les résultats dans des fichiers texte et génère un script Gnuplot pour visualiser la comparaison des algorithmes.

**Génération de graphiques**:
- Utiliser `make graphs` pour générer automatiquement un graphique PNG comparant les performances des algorithmes
- Le graphique est sauvegardé dans `results/results_[instance]_[k].png`
- Nécessite l'installation de Gnuplot : `sudo apt install gnuplot` (Ubuntu/Debian)

- Pour générer tous les PNG en une commande : depuis le répertoire `tp1/`, exécuter `./generate_plots.sh` (génère tous les PNG listés par les scripts `.gnuplot` dans `tp1/results/`).

> Voir aussi : [`tp1/gnuplot_examples.md`](tp1/gnuplot_examples.md) pour des exemples d'utilisation et des scripts prêts à l'emploi.

> Voir aussi : [../tp2/gnuplot_examples.md](../tp2/gnuplot_examples.md) — exemple Gnuplot et script prêt à l'emploi pour tracer des fronts Pareto.

**Nouvelles fonctionnalités (v1.1)**:
- **Makefile minimaliste** : Cibles `make`, `make clean`, `make test`, `make graphs`
- **Mode verbeux** (`-v`) : Affiche l'instance chargée et les solutions intermédiaires pour chaque exécution
- **Mesure du temps d'exécution** : Affiche le temps moyen d'exécution pour chaque algorithme
- **Arguments nommés modernes** : Interface avec options longues (`--executions`) et courtes (`-k`)
- **Sauvegarde automatique des résultats** : Génération de fichiers texte et scripts Gnuplot
- **Configuration structurée** : Parsing robuste avec validation, valeurs par défaut et aide intégrée
- **Création automatique de répertoires** : Le répertoire de sortie est créé si nécessaire
- **Génération automatique de graphiques** : Scripts Gnuplot intégrés pour visualisation

**Algorithmes implémentés**:
- Marche aléatoire (`marche_aleatoire`)
- Hill Climber - first improvement (version sûre : `climber_first_safe`)
- Hill Climber - best improvement (`climber_best`)
- Algorithme personnalisé 1 : Random Restart Hill Climbing (`algorithme_perso`)
- Algorithme personnalisé 2 : Réutilisation de la solution (`algorithme_perso_2`)

**Format des instances** (format Taillard attendu) :
- ligne 1 : `nb_job`
- ligne 2 : `nb_machine`
- ligne 3 : `seed` (présent mais ignoré par l'algorithme)
- puis pour chaque job : `job_id` `dummy` `t_0 t_1 ... t_(m-1)`

**Notes d'implémentation et points importants**:
- La génération d'une solution aléatoire utilise `rand()`; le programme initialise la graine avec `srand(time(NULL))` dans `main()`.
- `compute_makespan` alloue une matrice temporaire pour calculer les dates de fin; la mémoire est libérée après calcul.
- Des versions "sûres" des hill-climbers (`climber_first_safe`, `climber_first_safe_2`) sont fournies pour éviter des annulations manuelles erronées des voisinages.

**Améliorations possibles**:
- Ajouter des tests unitaires pour `compute_makespan`, `insere` et `echange`.
- Mesurer et profiler les performances (profiling, optimisation mémoire).
- Implémenter une annulation efficace (undo) pour `insere` afin d'éviter la copie intégrale des solutions.
- Ajouter un mode verbeux / logging et options en ligne de commande (par ex. choix d'algorithme, seed fixe). ✅ **Implémenté : options `-v`, `-k`, `-m`, `-o`, `-s`, `-h`, `-f`, `-i` avec parsing structuré.**
- Supporter la lecture d'instances au format différent ou la génération aléatoire d'instances pour tests.

**Conseils d'utilisation**:
- Pour expérimenter, fixer `srand` avec une valeur connue (remplacer `srand(time(NULL))` par `srand(12345)`).
- Commencer avec un petit `max_evals` (ex. `1000`) pour tester rapidement, puis augmenter.


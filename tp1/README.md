**Projet Flow Shop (TP)**

- **Description**: Ce dépôt contient une implémentation en C d'un solveur pour le problème Flow Shop de permutation (calcul du makespan / Cmax) et plusieurs heuristiques de recherche locale (marche aléatoire, hill-climbers "first" et "best", deux variantes d'algorithmes personnalisés).

**Fichiers clés**:
- `fsp.c` : code principal contenant le chargement d'instances, l'évaluation (compute_makespan), opérateurs de voisinage (`echange`, `insere`) et les algorithmes heuristiques.
- `header.h` : définitions de structures et prototypes de fonctions.
- `instances/` : exemples d'instances (format Taillard) utilisées pour les tests.

**Compilation**:
- Depuis le répertoire `tp1/`, compiler avec GCC :

```
gcc -fdiagnostics-color=always -g fsp.c -o fsp
```

(La tâche VS Code fournie dans l'espace de travail utilise une commande similaire.)

**Exécution**:

```
./fsp <chemin_instance> <k_executions> <max_evals>
```

- `<chemin_instance>` : chemin vers un fichier d'instance (ex. `../instances/20_10_01.txt`).
- `<k_executions>` : nombre d'exécutions à moyenner par algorithme (ex. `10`).
- `<max_evals>` : budget maximum d'évaluations (ex. `100000`).

Exemple :
```
./fsp ../instances/20_10_01.txt 5 10000
```

Le programme lance les algorithmes pour les deux voisinages (`Échange` et `Insertion`) et affiche le coût moyen (makespan) pour chaque algorithme.

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
- Ajouter un mode verbeux / logging et options en ligne de commande (par ex. choix d'algorithme, seed fixe).
- Supporter la lecture d'instances au format différent ou la génération aléatoire d'instances pour tests.

**Conseils d'utilisation**:
- Pour expérimenter, fixer `srand` avec une valeur connue (remplacer `srand(time(NULL))` par `srand(12345)`).
- Commencer avec un petit `max_evals` (ex. `1000`) pour tester rapidement, puis augmenter.

Si vous voulez, je peux :
- ajouter un script `run_all.sh` pour lancer plusieurs instances et collecter les résultats ;
- ajouter des tests unitaires basiques (makefile ou script de test) ;
- ou commiter ces changements dans un dépôt Git et créer un README au niveau supérieur.


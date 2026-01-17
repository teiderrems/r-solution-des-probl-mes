# Exemples Gnuplot pour TP2

Ce fichier donne de courts exemples montrant comment sauvegarder un front de Pareto avec `sauvegarder_front()` et générer un graphique avec `gnuplot`.

## Sauvegarder le front (C)

```c
// Sauvegarder le front dans results/front_pareto.txt
sauvegarder_front("front_pareto.txt", archive, &cfg);
```

## Script Gnuplot (front_pareto.gnuplot)

```gnuplot
set terminal png size 800,600
set output 'results/front_pareto.png'
set xlabel 'Makespan'
set ylabel 'Tardiness'
plot 'results/front_pareto.txt' using 1:2 with points pt 7 title 'Pareto front'
```

## Commande pour générer l'image

```bash
mkdir -p results
gnuplot front_pareto.gnuplot
# ou directement : gnuplot -e "load 'front_pareto.gnuplot'"
```

## Astuce
Utilisez `construire_prefixe_fichier()` pour générer des noms de fichier avec préfixes (ex. `instance_i100_p20`) et évitez les collisions entre runs.

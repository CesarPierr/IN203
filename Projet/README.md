Les differents fichiers de simulation se trouvent dans des branches git differentes

# Mesure de temps initiale :
| Temps passé :      | Avec affichage | Sans affichage |
| :--------------:   | :------------: | :------------: |
| Dans la simulation | 0.02881 s      | 0.01451 s      |
| Dans l'affichage   | 0.01391 s      |  0.0001 s      |

On remarque que le temps passé à l'affichage est presque aussi important que celui pour calculer et que ces 2 parties sont entierement parallelisables. On pourrait donc facilement gagner du temps en parallelisant ces 2 taches.

voici aussi quelques mesures complementaires utiles pour la suite : 

| habitants :        | 100 000 habitants | 200 000 habitants | 300 000 habitants | 400 000 habitants |
| :--------------:   | :---------------: | :--------------:  | :------------:    | :---------------: |
| temps simulation   |  0.02881 s        | 0.05957 s         | 0.08741 s         | 0.1184 s          |

# Parallelisation avec affichage contre simulation

| Temps passé :      | Avec affichage synchrone |
| :--------------:   | :----------------------: |
| Dans la simulation | 0.01775 s                | 
| Dans l'affichage   | 0.01726 s                | 

On remarque une nette diminution du temps de la simulation grace à la parallelisation de l'affichage, néanmoins celle-ci reste 0.03s plus lent que la simulation sans affichage surement du au temps de synchronisation et d'envoi des données

# Parallélisation affichage asynchrone contre simulation 

Le temps passé dans la simulation descend cette fois à 0.0156s soit 0.02 de moins qu'avec l'affichage synchrone. On est donc très proche du temps trouvé au debut sans affichage !

# Parallélisation OpenMP

Pour la population constante par processus on compare donc au temps de la premiere simulation pour une population egale à nb_processus * population_par_processus

| Speedup (par rapport à la version initiale)| nb_threads : 1 | nb_threads : 2 |nb_threads : 3 |nb_threads : 4 |
| :----------------------------------------: | :------------: | :------------: | :-----------: | :-----------: |
| population globale constante (100 000)     | 1.78           | 2.79           | 2.50          | 2.78          |
| population constant par processus (100 000)| 1.88           | 2.87           | 2.46          | 2.55          |

# Parallélisation MPI de la simulation

| Speedup (par rapport à la version initiale)| nb_proc_simu : 1 | nb_proc_simu : 2 |nb_proc_simu : 3 |
| :----------------------------------------: | :--------------: | :--------------: | :-------------: |
| population globale constante (100 000)     | 1.73             | 2.66             | 2.94            |
| population constant par processus (100 000)| 1.75             | 2.79             | 3.73            |

On remarque ici que le speedup continue a augmenter sensiblement et ne stagne pas pour une population constante par processus. La parallelisation précédente etait donc memoy bound lorsqu'on utilisait un trop grand nombre de threads.
Il faut néanmoins faire attention à la granularité

# Parallélisation finale

avec les parametres d'une parallelisation MPI maximale (3 processeurs pour la simulation) 

Sur mon ordinateur seulement :

| Speedup (par rapport à la version initiale)| nb_threads_simu : 1 | nb_threads_simu : 2 |nb_threads_simu : 3 | nb_threads_simu |
| :----------------------------------------: | :-----------------: | :-----------------: | :----------------: | :-------------: |
| population globale constante (100 000)     | 2.88                | 3.54                | 0.97               | 0.43            |
| population constant par threads (33 333)   | 1.75             | 2.79             | 3.73            |

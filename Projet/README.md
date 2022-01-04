Les differents fichiers de simulation se trouvent dans des branches git differentes

# lscpu :

Architecture:                    x86_64
CPU op-mode(s):                  32-bit, 64-bit
Byte Order:                      Little Endian
Address sizes:                   39 bits physical, 48 bits virtual
CPU(s):                          8
On-line CPU(s) list:             0-7
Thread(s) per core:              2
Core(s) per socket:              4
Socket(s):                       1
NUMA node(s):                    1
Vendor ID:                       GenuineIntel
CPU family:                      6
Model:                           126
Model name:                      Intel(R) Core(TM) i5-1035G1 CPU @ 1.00GHz
Stepping:                        5
CPU MHz:                         1200.000
CPU max MHz:                     3600,0000
CPU min MHz:                     400,0000
BogoMIPS:                        2380.80
Virtualization:                  VT-x
L1d cache:                       192 KiB
L1i cache:                       128 KiB
L2 cache:                        2 MiB
L3 cache:                        6 MiB
NUMA node0 CPU(s):               0-7

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


Sur mon ordinateur seulement :

avec les parametres d'une parallelisation MPI minimale (2 processeurs pour la simulation) 

| Speedup (par rapport à la version initiale)| nb_threads/proc : 1 | nb_threads/proc : 2 |nb_threads/proc : 3 | nb_threads/proc : 4 |
| :----------------------------------------: | :-----------------: | :-----------------: | :----------------: | :-----------------: |
| population globale constante (100 000)     | 2.75                | 3.78                | 4.89               | 1.20                |
| population constant par threads (50 000)   | 2.73                | 3.58                | 4.01               | 2.42                |

avec les parametres d'une parallelisation MPI maximale (3 processeurs pour la simulation) 

| Speedup (par rapport à la version initiale)| nb_threads/proc : 1 | nb_threads/proc : 2 |nb_threads/proc : 3 | nb_threads/proc : 4 |
| :----------------------------------------: | :-----------------: | :-----------------: | :----------------: | :-----------------: |
| population globale constante (100 000)     | 2.88                | 3.54                | 0.97               | 0.43                |
| population constant par threads (33 333)   | 2.88                | 3.65                | 1.94               | 1.38                |


# Conclusion

On remarque que la parallelisation ayant le speedup maximal est celle utilisant 3 processus pour MPI et 3 threads par processus. Cela s'explique par le fait qu'on maximise l'utilisation de la mémoire partagée sans etre trop memory bound. De plus mon processeur n'ayant que 8 threads au maximum (4 coeurs) on comprend facilement que lorsque qu'on cherche à utiliser 3 processeurs et 4 threads par coeur soit 12 threads au total, le speedup ralentit fortement car la machine ne peut operer autant d'operations en parallèle.

  Ce projet m'a donc permis de me rendre compte de façon experimentale des limites théorique de la parallélisation vues en cours. J'ai aussi via mes recherches pour le projet découvert comment déclarer de nouveaux types MPI ainsi que de nouvelles fonctions.
  
  J'ai aussi partiellement découvert la librairie boost car je n'avais pas compris dans un premier temps qu'il ne fallait pas envoyer la population dans la derniere partie et j'ai été amené à me demander (pour rien) comment envoyer une classe par MPI en la serializant avec boost

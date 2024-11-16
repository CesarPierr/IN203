# Parallélisation d'un Modèle de Transmission Virale

## Vue d'ensemble
Ce projet implémente la parallélisation des calculs d'évolution épidémiologique d'un modèle de transmission virale en utilisant OpenMP et MPI. Les différentes versions du code sont disponibles dans des branches Git distinctes.

## Configuration matérielle de test
Architecture testée : Intel(R) Core(TM) i5-1035G1 CPU @ 1.00GHz
- 8 CPUs (4 cœurs physiques, 2 threads par cœur)
- 1 socket
- Cache L3 : 6 MiB
- Fréquence : 1.00GHz - 3.60GHz

## Analyse des performances initiales

### Mesures de base
|                    | Avec affichage | Sans affichage |
|-------------------|----------------|----------------|
| Temps simulation  | 0.02881 s     | 0.01451 s     |
| Temps affichage   | 0.01391 s     | 0.0001 s      |

### Impact de la taille de population
| Habitants        | 100 000 | 200 000 | 300 000 | 400 000 |
|-----------------|----------|----------|----------|----------|
| Temps simulation| 0.02881 s| 0.05957 s| 0.08741 s| 0.1184 s |

## Stratégies de parallélisation

### 1. Parallélisation Simulation/Affichage

#### Version synchrone
- Séparation en deux boucles while distinctes
- Transmission des données de la grille au processus d'affichage
- Résultats :
  - Simulation : 0.01775 s
  - Affichage : 0.01726 s

#### Version asynchrone
- Utilisation de `Isend` pour le processus d'affichage
- Implémentation de `Iprobe` pour la transmission
- Temps de simulation : 0.0156 s

### 2. Parallélisation OpenMP
Performance relative (speedup) par rapport à la version initiale :

| Configuration                          | 1 thread | 2 threads | 3 threads | 4 threads |
|---------------------------------------|----------|-----------|-----------|-----------|
| Population globale fixe (100 000)     | 1.78     | 2.79      | 2.50      | 2.78      |
| Population fixe par processus (100 000)| 1.88     | 2.87      | 2.46      | 2.55      |

### 3. Parallélisation MPI
- Répartition de la population entre les cœurs
- Distribution des contaminés
- Utilisation de `MPI_Allgather` pour la synchronisation

| Configuration                          | 1 proc | 2 proc | 3 proc |
|---------------------------------------|---------|---------|---------|
| Population globale fixe (100 000)     | 1.73    | 2.66    | 2.94    |
| Population fixe par processus (100 000)| 1.75    | 2.79    | 3.73    |

### 4. Parallélisation hybride (OpenMP + MPI)

#### Configuration minimale (2 processus MPI)
| Configuration                        | 1 thread/proc | 2 threads/proc | 3 threads/proc | 4 threads/proc |
|-------------------------------------|---------------|----------------|----------------|----------------|
| Population globale fixe (100 000)    | 2.75         | 3.78          | 4.89          | 1.20          |
| Population fixe par thread (50 000)  | 2.73         | 3.58          | 4.01          | 2.42          |

#### Configuration maximale (3 processus MPI)
| Configuration                        | 1 thread/proc | 2 threads/proc | 3 threads/proc | 4 threads/proc |
|-------------------------------------|---------------|----------------|----------------|----------------|
| Population globale fixe (100 000)    | 2.88         | 3.54          | 0.97          | 0.43          |
| Population fixe par thread (33 333)  | 2.88         | 3.65          | 1.94          | 1.38          |

## Conclusions

### Performance optimale
La meilleure configuration obtenue utilise :
- 3 processus MPI
- 3 threads par processus
- Speedup maximal observé : 4.89

### Limites observées
1. Memory bound : La parallélisation OpenMP montre ses limites avec l'augmentation du nombre de threads
2. Ressources matérielles : Les performances se dégradent au-delà de 8 threads (limite physique du CPU)

### Apprentissages techniques
- Création de nouveaux types MPI
- Gestion de la mémoire partagée
- Optimisation des communications inter-processus
- Exploration partielle de la sérialisation avec Boost (non utilisée dans la version finale)

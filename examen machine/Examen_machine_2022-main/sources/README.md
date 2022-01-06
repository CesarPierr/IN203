# analyse des temps :

on remarque dans un premier temps que lors de l'execution, les temps d'execution sont repartis comme suit : 


| Temps passé :       | encodage par DFT | selection des 10% coefs | reconstitution de l'image compressée | temps total |
| :--------------:    | :------------: | :----------------------: | :----------------------------------: | :---------: |

| tiny_lena_gray.png  | 0.381 s      | 0.000275 s      | 0.0588 s| 0.442 s|
| small_lena_gray.png | 100.59 s      |  0.0057 s      | 15.04 s |  115.6 s|

au vu des ordres de grandeur il va falloir paralleliser la fonction discret transform fourrier ainsi que inverse partial discret. Les autres fonctions ne representent pas grand chose en temps d'execution

#parallelisation open mp :

on parallelise la boucle k2 de discret fourrier transform pour limiter les sauts memoires par rapport à k1;

temps fourier discret :
pour la small

| NB_THREADS      | 2    |    3 |    4 | 
| :--------------:| :--: | :--: | :--: |
| discret fourier transform | 58.4 s | 41.26 s | 31.1 s |
|acceleration               | 1.72 | 2.43  | 3.22   |
|inverse fourier transform  | 7.52 |  5.18 | 4.07 s |
|acceleration               | 1.98 | 2.89  | 4.65   |

on remarque que l'acces memoire semble limiter l'acceleration la transformation de fourrier discrete se faisant sur l'image mais moins sur la transformée inverse (il y a moins de coefficients)

#parallelisation MPI1

| NB_THREADS                | 2    |    3 |    4 | 
| :--------------:          | :--: | :--: | :--: |
| discret fourier transform | 51.51 s | 36.27 s | 26.76 s |
|acceleration               | 1.96 | 2.75  | 3.73   |

Ici, l'acceleration parallele de la transformée de fourier discrete n'est plus memory bound. 

#parallelisation MPI2
# Puissance4 
Tarek AINOUZ
Kévin DONIN DE ROSIERE
Hamza MEKNASSI


le jeux de puissance 4 coder en C en utilisant la librairie graphique Ncurses

Déplacez vous dans le répertoire du projet.
Exécutez la commande `make` afin de compiler.
Lancer le serveur dans un terminal en tapant `./serveur nbPort nbParties`
Ou `nbPort` est le numéro du port qu'écoute le serveur
Et `nbParties` est le nombre maximum de parties simultanées que peut gérer le serveur.

Lancer dans un autre terminal un client:
`./client nbPort`

En lancer un second dans un autre terminal pour pouvoir commencer la partie.

Fichiers:
- `client.c` -Le client
- `serveur.c` -Le serveur
- `messages.h` -Fichier synthétisant les échanges clients serveurs
- `makefile` -Fichier responsable de la compilation


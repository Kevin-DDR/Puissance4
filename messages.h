

/*******************************************************
	UDP
********************************************************/

//Demande de connexion d'un client vers le serveur
typedef struct {
	unsigned char type;
} connexion_t;

//Acceptation d'une connexion par le serveur
typedef struct {
	unsigned char type;//2
	unsigned char idPartie; 
	unsigned char idJoueur;
} connexion_accepte_t;

//Refus d'une connexion par le serveur (Limite atteinte)
typedef struct {
	unsigned char type;//3
} connexion_refus_t;


typedef struct {
	unsigned char type;//4
	unsigned char** grille;
} envoi_etat_t;

//Jouer une piece
typedef struct {
	unsigned char type;//5
	unsigned char idPartie; 
	unsigned char idJoueur;
	unsigned char** grille;
} coup_joueur_t;

//Interruption
typedef struct {
	unsigned char type;//6
	unsigned char idPartie; 
	unsigned char idJoueur;
} interruption_joueur_t;

//Interruption
typedef struct {
	unsigned char type;//7
	unsigned char idPartie; 
	unsigned char idJoueur;
	unsigned char** grille;
} Victoire_joueur_t;












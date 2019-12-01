

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


typedef struct {
	unsigned char type;
	unsigned char gagnant;
	unsigned char perdant;
} resultat_t;




















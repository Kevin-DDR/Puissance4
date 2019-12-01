#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>  /* Pour socket */
#include <arpa/inet.h>   /* Pour sockaddr_in, inet_pton */
#include <string.h>      /* Pour memset */
#include "messages.h"
#include <unistd.h>      /* Pour close */

#define HAUTEUR 6
#define LONGUEUR 7

typedef struct {
	unsigned char longueur;
	unsigned char hauteur;
	struct sockaddr_in adresses[2];
	//TODO Rajouter un champs avec les adresses

	unsigned char** grille;

} partie_t;

partie_t* creerPartie(){
	partie_t* p = (partie_t*) malloc(sizeof(partie_t));
	p->longueur = LONGUEUR;
	p->hauteur = HAUTEUR;
	p->grille = (unsigned char **) malloc(sizeof(unsigned char *)* HAUTEUR);
	for(int i = 0; i < HAUTEUR; i++ ){
		p->grille[i] = (unsigned char*) malloc(sizeof(unsigned char) * LONGUEUR);
		for(int j = 0; j < LONGUEUR; j++){
			p->grille[i][j] = 0;
		}
	}
	return p;
}

void afficherGrille(unsigned char** grille){
	for(int i = 0; i < HAUTEUR; i++){
		for(int j = 0; j < LONGUEUR; j++){
			printf("%d",grille[i][j]);
			//printf("%d %d \n", i,j);
		}
		printf("\n");
	}

}


int main(int argc, char *argv[]) {
	/* Ouvrir le socket dans un while ? Ou alors  */
	

	int sockfd;
  	struct sockaddr_in adresseServeur;
  	struct sockaddr_in adresseSlave;
  	struct sockaddr_in adresseMaster;
  	socklen_t adresseSlaveLen;

  	int nbParties = 0;
  	int full = 0;


  	char connexion_master[sizeof(unsigned char)];
  	char connexion_slave[sizeof(unsigned char)];
  	unsigned char bufferMsg[sizeof(unsigned char) * 500];
  	char adresse[16];
  	unsigned char type,tmp;
  	unsigned short port;

  	if(argc != 3) {
        fprintf(stderr, "Usage: %s portServeur nbParties\n", argv[0]);
        fprintf(stderr, "\tOu:\n");
        fprintf(stderr, "\tport : port UDP du serveur\n");
        fprintf(stderr, "\tnbParties : Le nombre de parties simultanées\n");

        exit(EXIT_FAILURE);
    }
    int maxParties = atoi(argv[2]);


    partie_t** tabParties = (partie_t**) malloc(sizeof(partie_t*) * atoi(argv[2]));



  	adresseSlaveLen = sizeof(struct sockaddr_in);


  	/* Creation de la socket */
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Erreur lors de la creation de la socket ");
		exit(EXIT_FAILURE);
	}
	/* Creation de l'adresse du serveur */
	memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
	adresseServeur.sin_family = AF_INET;
	adresseServeur.sin_port = htons(atoi(argv[1]));
	adresseServeur.sin_addr.s_addr = htonl(INADDR_ANY);


	  /* Nommage de la socket */
	if(bind(sockfd, (struct sockaddr*)&adresseServeur, sizeof(struct sockaddr_in)) == -1) {
    	perror("Erreur lors du nommage de la socket ");
    	exit(EXIT_FAILURE);
	}

	//TODO faire un handler pour les signaux



	//TODO Rajouter le while ici
	//tODO Verifier que le nombre de partie depasse pas le max;
	while(1){
		if(recvfrom(sockfd, bufferMsg, sizeof(bufferMsg), 0, (struct sockaddr*)&adresseMaster, &adresseSlaveLen) == -1) {
			perror("Erreur lors de la reception de la reception du message ");
			exit(EXIT_FAILURE);
		}

		memcpy(&type,&bufferMsg,sizeof(unsigned char));
		printf("Type : %u\n",type);

		

		switch(type){
			case 1:
				//Connexion a une partie
			if(nbParties < maxParties){
				if(full == 0){
					//La partie est vide
					//Création de la partie
					tabParties[nbParties] = creerPartie();
					//Stockage de l'adresse du client 1
					tabParties[nbParties]->adresses[0] = adresseMaster;

					//type
					tmp = 2;
					memcpy(&bufferMsg,&tmp,sizeof(tmp));
					//idPartie
					tmp = nbParties;
					memcpy(&bufferMsg[sizeof(tmp)],&tmp,sizeof(tmp));
					//Id joueur
					tmp = 1;
					memcpy(&bufferMsg[sizeof(tmp)*2],&tmp,sizeof(tmp));
					//Envoi de la confirmation


					if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0,(struct sockaddr*)&adresseMaster, adresseSlaveLen) ==-1 ){
						perror("Erreur lors de l'envoi de la réponse");
				    	exit(EXIT_FAILURE);
					}


					full++;



				}else{
					//La partie comprend déjà un joueur
					//Stockage de l'adresse du client 2
					tabParties[nbParties]->adresses[1] = adresseMaster;


					//type
					tmp = 2;
					memcpy(&bufferMsg,&tmp,sizeof(tmp));
					//idPartie
					tmp = nbParties;
					memcpy(&bufferMsg[sizeof(tmp)],&tmp,sizeof(tmp));
					//Id joueur
					tmp = 2;
					memcpy(&bufferMsg[sizeof(tmp)*2],&tmp,sizeof(tmp));

					
					//Envoi de la confirmation


					if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0,(struct sockaddr*)&adresseMaster, adresseSlaveLen) ==-1 ){
						perror("Erreur lors de l'envoi de la réponse");
				    	exit(EXIT_FAILURE);
					}


					

					//TODO Tirer au hasard quel joueur recoit le premier message, puis envoyer a l'adresse qui correspond.

					memset(&bufferMsg, 0, sizeof(bufferMsg));


					//type
					tmp = 4;
					memcpy(&bufferMsg,&tmp,sizeof(tmp));

					//Grille
					//memcpy(&bufferMsg[sizeof(unsigned char)],&tabParties[nbParties]->grille,sizeof(unsigned char) * HAUTEUR * LONGUEUR);
					//afficherGrille(tabParties[nbParties]->grille);

					//On copie le contenu de la grille ligne par ligne
					for(int i = 0; i< HAUTEUR; i++){
						for(int j = 0; j < LONGUEUR; j++){
							memcpy(&bufferMsg[sizeof(unsigned char) + ((i * LONGUEUR  + j ) * sizeof(unsigned char))],&(tabParties[nbParties]->grille[i][j]),sizeof(unsigned char));
						}
						
					}


					/*
					for(int i = 0; i< HAUTEUR * LONGUEUR + 1; i++){
						memcpy(&tmp,&bufferMsg[i * sizeof(unsigned char)],sizeof(tmp));
						//printf("%d\n",tmp );
					}
					*/


					if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0,(struct sockaddr*)&adresseMaster, adresseSlaveLen) ==-1 ){
						perror("Erreur lors de l'envoi de la grille");
				    	exit(EXIT_FAILURE);
					}



					nbParties++;
					full = 0;

				}
			}else{
				//Nombre limite de parties atteinte, refus de la connexion
					tmp = 3;
					memcpy(&bufferMsg,&tmp,sizeof(tmp));
					memcpy(&bufferMsg[sizeof(tmp)*2],&tmp,sizeof(tmp));

					if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0,(struct sockaddr*)&adresseMaster, adresseSlaveLen) ==-1 ){
						perror("Erreur lors de l'envoi de la réponse");
				    	exit(EXIT_FAILURE);
					}


			}
			
			break;

			//Reception d'un coup
			//TODO mise à jour de la grille dans la partie
			//TODO envoi de la grille à l'autre joueur



		}

		
	}

	



	/******************************************

	Connexion du deuxieme client

	*****************************************/



	if(recvfrom(sockfd, connexion_slave, sizeof(connexion_slave), 0, (struct sockaddr*)&adresseSlave, &adresseSlaveLen) == -1) {
		perror("Erreur lors de la reception de la taille de la demande de connexion ");
		exit(EXIT_FAILURE);
	}

	memcpy(&type,&connexion_slave,sizeof(unsigned char));
	printf("Type Slave: %u\n",type);

	

	return 1;

	

	inet_ntop(AF_INET, &(adresseMaster.sin_addr), adresse, INET_ADDRSTRLEN);
	

	memcpy(&port,&connexion_master[sizeof(unsigned char)],sizeof(unsigned short));
	printf("Port : %hu \n",port);


	/* Fermeture du socket maitre */
	if(close(sockfd) == -1) {
    	perror("Erreur lors de la fermeture de la socket ");
    	exit(EXIT_FAILURE);
	}



	

	if(type != 2){
		perror("Erreur, le Slave doit etre lancé en second");
		exit(EXIT_FAILURE);
	}



	printf("Datagram's IP address is: %s\n", inet_ntoa(adresseSlave.sin_addr));
   	printf("Datagram's port is: %d\n", (int) ntohs(adresseSlave.sin_port));
   	printf("%s\n", adresse);

	

	/* Fermeture du socket maitre */
	if(close(sockfd) == -1) {
    	perror("Erreur lors de la fermeture de la socket ");
    	exit(EXIT_FAILURE);
	}


	return 0;
}
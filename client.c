#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <ncurses.h>
#include "messages.h"
#include <sys/socket.h>  /* Pour socket */
#include <arpa/inet.h>   /* Pour sockaddr_in, inet_pton */
#include <string.h>      /* Pour memset */
#include <unistd.h>      /* Pour close */
#include <signal.h>

#define NB_LIGNES_SIM		12				/* Dimensions des fenetres du programme */
#define NB_COL_SIM			45
#define NB_LIGNES_MSG		27
#define NB_COL_MSG			49
#define NB_LIGNES_COL  		3
#define NB_COL_COL			45	

#define HAUTEUR 6
#define LONGUEUR 7


WINDOW *fen_sim;							/* Fenetre de simulation partagee par les lems*/
WINDOW *fen_msg;							/* Fenetre de messages partagee par les lems*/
WINDOW *fen_col;							/* Fenetre de sélection de la colonne */

int running = 1;


void ncurses_initialiser() {
	initscr();								/* Demarre le mode ncurses */
	cbreak();								/* Pour les saisies clavier (desac. mise en buffer) */
	noecho();								/* Desactive l'affichage des caracteres saisis */
	keypad(stdscr, TRUE);					/* Active les touches specifiques */
	refresh();								/* Met a jour l'affichage */
	curs_set(FALSE);						/* Masque le curseur */
	mousemask(BUTTON1_CLICKED, NULL);		/* Active le clic gauche de la souris*/
}

void ncurses_stopper() {
	endwin();
}

void ncurses_couleurs() {
    /* Verification du support de la couleur */
    if(has_colors() == FALSE) {
        ncurses_stopper();
        fprintf(stderr, "Thé terminal doesn't handle colors.\n");
        exit(EXIT_FAILURE);
    }

    /* Activation des couleurs */
    start_color();
    use_default_colors();
    /* Definition de la palette */
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_RED);
    init_pair(2, COLOR_WHITE, COLOR_BLUE);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_WHITE);
    //attron(COLOR_PAIR(0)); 
    
    
}


void handler(int signum) {

	switch(signum){

		case SIGINT:
			//printf("Interruption \n");
			//running = 4;
			//TODO Envoyer une demande d'interruption au serveur
		break;
	}
  
}




WINDOW *creer_fenetre_box_sim() {
/*Creation de la fenetre de contour de la fenetre de simulation */

	WINDOW *fen_box_sim;
	
	fen_box_sim = newwin(NB_LIGNES_SIM + 2, NB_COL_SIM + 2, 0, 0);
	box(fen_box_sim, 0, 0);
	mvwprintw(fen_box_sim, 0, (NB_COL_SIM + 2) / 2 - 5, "SIMULATION");	
	wrefresh(fen_box_sim);
	
	return fen_box_sim;
}

WINDOW *creer_fenetre_sim() {
/* Creation de la fenetre de simulation dans la fenetre de contour */
/* La simulation est affichee dans cette fenetre */

	WINDOW *fen_sim;
	
	fen_sim = newwin(NB_LIGNES_SIM, NB_COL_SIM, 1, 1);
	
	return fen_sim;
}

WINDOW *creer_fenetre_box_msg() {
/* Creation de la fenetre de contour de la fenetre de messages */

	WINDOW *fen_box_msg;
	
	fen_box_msg = newwin(NB_LIGNES_MSG + 2, NB_COL_MSG + 2, 0, NB_COL_SIM + 2);
	box(fen_box_msg, 0, 0);
	mvwprintw(fen_box_msg, 0, (NB_COL_MSG + 2) / 2 - 4, "MESSAGES");
	wrefresh(fen_box_msg);
	
	return fen_box_msg;
}

WINDOW *creer_fenetre_msg() {
/* Creation de la fenetre de messages dans la fenetre de contour */
/* Les messages indicatifs des evenements de la simulation et de l'interface */
/* utilisateur sont affiches dans cete fenetre */

	WINDOW *fen_msg;
	
	fen_msg = newwin(NB_LIGNES_MSG, NB_COL_MSG, 1, NB_COL_SIM + 3);
	scrollok(fen_msg, TRUE);
	
	return fen_msg;
}


void afficherGrille(unsigned char** grille){
	int offset = 0;
	use_default_colors();
	for(int i = 0; i < HAUTEUR; i++){
		offset = 0;
		for(int j = 0; j < LONGUEUR; j++){

			
			use_default_colors();
			
			//hoix des couleurs
			switch(grille[i][j]){
				case 0:
					attron(COLOR_PAIR(3)); 
					use_default_colors();
				break;
				case 1:
					attron(COLOR_PAIR(1));
				break;
				case 2: 
					attron(COLOR_PAIR(2));
			}
			//attron(COLOR_PAIR(grille[i][j])); 
			mvaddch(i*2+1,j*6+1 + offset,' ');
			mvaddch(i*2+1,j*6+2 + offset,' ');
			mvaddch(i*2+1,j*6+3 + offset,' ');
			mvaddch(i*2+1,j*6+4 + offset,' ');
			mvaddch(i*2+1,j*6+5 + offset,' ');
			mvaddch(i*2+1,j*6+6 + offset,' ');

			mvaddch(i*2+2,j*6+1 + offset,' ');
			mvaddch(i*2+2,j*6+2 + offset,' ');
			mvaddch(i*2+2,j*6+3 + offset,' ');
			mvaddch(i*2+2,j*6+4 + offset,' ');
			mvaddch(i*2+2,j*6+5 + offset,' ');
			mvaddch(i*2+2,j*6+6 + offset,' ');
		}
	}
	use_default_colors();
	refresh();

}

WINDOW *creer_fenetre_box_col() {
/*Creation de la fenetre de contour de la fenetre de simulation */

	WINDOW *fen_box_col;
	
	fen_box_col = newwin(NB_LIGNES_COL + 2, NB_COL_COL + 2, NB_LIGNES_SIM + 4, 0);
	box(fen_box_col, 0, 0);
	mvwprintw(fen_box_col, 0, (NB_COL_COL + 2) / 2 - 5, "Choose a column");	
	wrefresh(fen_box_col);
	mvwprintw(fen_box_col, 2, 3 , "1     2     3     4     5     6     7");	
	wrefresh(fen_box_col);
	
	return fen_box_col;
}

WINDOW *creer_fenetre_col() {
/* Creation de la fenetre de simulation dans la fenetre de contour */
/* La simulation est affichee dans cette fenetre */

	WINDOW *fen_col;
	
	fen_col = newwin(NB_LIGNES_COL, NB_COL_COL,NB_LIGNES_SIM+2, 1);
	

	return fen_col;
}


int testerVictoire(unsigned char** gridTable,int rowNum,int colNum) {
	//  For checking whether any win or lose condition is reached. Returns 1 if win or lose is reached. else returns 0
	//  gridTable[][] is the game matrix(can be any number of rows and columns between 4 and 40)
	//  colNum is the column number where the last token was placed
	//  rowNum is the row number where the last token was placed
	//  maxRow is the number of rows in my grid
	//  maxCol is the number of columns in my grid
	int maxCol = LONGUEUR;
	int maxRow = HAUTEUR;
	unsigned char player = gridTable[rowNum][colNum]; //player ID
	int count=0;

	// Horizontal check
	for (int i=0;i<maxCol;i++){
	    if (gridTable[rowNum][i]==player){
	        count++;
	    }else{
	        count=0;
	    }

	    if (count>=4){
	        return 1;
	    }
	}
	//Vertical check
	for (int i=0;i<maxRow;i++){
	    if (gridTable[i][colNum]==player){
	        count++;
	    }else{
	        count=0;
	    }

	    if (count>=4){
	        return 1;
	    }
	} 
	count=0;
	// 4 in a row diagonally
	for(int i=colNum+1,j=rowNum+1;i<maxRow && j<maxCol;i++,j++) 
	{ 
	    if(gridTable[j][i]!=player)
	    {
	        count=1;
	        break;        
	    }
	    count++;
	}
	// 4 in a row diagonally
	for(int i=colNum-1,j=rowNum-1;i>=0 && j>=0;i--,j--) 
	{ 
	    if(gridTable[j][i]!=player)
	    {
	        count=1;
	        break;        
	    }
	    count++;
	}
	// 4 in a row diagonally
	for(int i=colNum+1,j=rowNum-1;i<maxRow && j>=0;i++,j--) 
	{ 
	    if(gridTable[j][i]!=player)
	    {
	        count=1;
	        break;        
	    }
	    count++;
	}

	for(int i=colNum-1,j=rowNum+1;i>=0 && j<maxCol;i--,j++) 
	{ // 4 in a row diagonally
	    if(gridTable[j][i]!=player)
	    {
	        count=1;
	        break;        
	    }
	    count++;
	}


	// Diagonale Haut gauche jusqu'a bas droite
	for( int rowStart = 0; rowStart < maxRow - 4; rowStart++){
	    count = 0;
	    int row, col;
	    for( row = rowStart, col = 0; row < maxRow && col < maxCol; row++, col++ ){
	        if(gridTable[row][col] == player){
	            count++;
	            if(count >= 4) return 1;
	        }
	        else {
	            count = 0;
	        }
	    }
	}

	// Haut droit vers bas gauche
	for(int colStart = 0; colStart < maxCol - 4; colStart++){
	    count = 0;
	    int row, col;
	    for( row = 0, col = colStart; row < maxRow && col < maxCol; row++, col++ ){
	        if(gridTable[row][col] == player){
	            count++;
	            if(count >= 4) return 1;
	        }
	        else {
	            count = 0;
	        }
	    }
	}



	if(count>=4)
	    return 1;

	return 0;
}

int matchNul(unsigned char** grille){
	for(int i = 0; i < LONGUEUR; i++){
		if(grille[0][i] == 0){
			return 0;
		}
	}

	return 1;
}

int colNonPleine(unsigned char*** grille, int col){
	return ((*grille)[0][col] == 0);
}



//Depot d'une piece dans la grille
//Retourne -1 si le pion n'a pas pu etre joué, 0 si le joueur n'a pas gagné, 1 si il vient de gagner
int ajouterPiece(unsigned char*** grille, unsigned char ligne, unsigned char joueur){
	if(ligne >= LONGUEUR || ligne < 0){
		//Ligne impossible
		return -1;
	}
	for(int i = HAUTEUR -1; i >= 0; i--){
		//printf("%d %d\n", i,ligne);
		
		if((*grille)[i][ligne] == 0){
			(*grille)[i][ligne] = joueur;
			return testerVictoire(*grille, i, ligne);
			
		}
		
	}
	return -1;
	
}

int main(int argc, char *argv[]){

	unsigned char type,idPartie,idJoueur,tmp;
	idPartie = idJoueur = 0;
	unsigned char** grille;
	int ch, res, repeat;
	int sockfd;
  	struct sockaddr_in adresseServeur;
  	unsigned char connexion_master[sizeof(unsigned char)];
  	unsigned char bufferMsg[sizeof(unsigned char) * 500];
  	socklen_t adresseSlaveLen = sizeof(struct sockaddr_in);
  	MEVENT event;

	if(argc != 2) {
        fprintf(stderr, "Usage: %s portServeur \n", argv[0]);
        fprintf(stderr, "\tOu:\n");
        fprintf(stderr, "\tport : port UDP du serveur\n");
        ncurses_stopper();
        exit(EXIT_FAILURE);
    }

    /*****************************************************
	Broadcast UDP
    ******************************************************/
    struct sockaddr_in s;



    memset(&s, '\0', sizeof(struct sockaddr_in));
    s.sin_family = AF_INET;
    s.sin_port = (in_port_t)htons(atoi(argv[1]));
    s.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    type = 1;
	memcpy(&connexion_master,&type,sizeof(type));

	
    // Creation de la socket 
	if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("Socket création failed ");
		ncurses_stopper();
		exit(EXIT_FAILURE);
	}
	int broadcastEnable=1;
	setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
	/*
	// Creation de l'adresse du serveur 
	memset(&adresseServeur, 0, sizeof(struct sockaddr_in));
	adresseServeur.sin_family = AF_INET;
	adresseServeur.sin_port = htons(atoi(argv[2]));
	if(inet_pton(AF_INET, argv[1], &adresseServeur.sin_addr) != 1) {
		perror("Adress conversion failure ");
		exit(EXIT_FAILURE);
	}
	// connexion_master_udp.port = atoi(argv[3]); 
	type = 1;
	memcpy(&connexion_master,&type,sizeof(type));
	*/

	if(sendto(sockfd, connexion_master, sizeof(connexion_master), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) ==-1 ){
		perror("Erreur lors de l'envoi de l'envoi de la demande de connexion ");
		ncurses_stopper();
    	exit(EXIT_FAILURE);
	}


	if(recvfrom(sockfd, bufferMsg, sizeof(bufferMsg), 0, (struct sockaddr*)&adresseServeur, &adresseSlaveLen) == -1) {
		perror("Erreur lors de la reception de la reception du message ");
		ncurses_stopper();
		exit(EXIT_FAILURE);
	}

	type = 9;
	memcpy(&connexion_master,&type,sizeof(type));

	if(sendto(sockfd, connexion_master, sizeof(connexion_master), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) ==-1 ){
		perror("Erreur lors de l'envoi de l'envoi de l'accuse de reception ");
		ncurses_stopper();
    	exit(EXIT_FAILURE);
	}



	memcpy(&type,&bufferMsg,sizeof(unsigned char));
	//printf("Type : %u\n",type);

	switch(type){
		case 2: 
			//Connexion reussi
			memcpy(&idPartie,&bufferMsg[sizeof(unsigned char)],sizeof(unsigned char));
			memcpy(&idJoueur,&bufferMsg[sizeof(unsigned char) * 2],sizeof(unsigned char));

			grille = (unsigned char**) malloc(sizeof(unsigned char*) * HAUTEUR);
			for(int i = 0; i < HAUTEUR; i++){
				grille[i] = (unsigned char*) malloc(sizeof(unsigned char) * LONGUEUR);
				for(int j = 0; j < LONGUEUR;j++){
					grille[i][j] = 1;
				}
			}


		break;
		case 3:
			running = 7;
		break;
		//TODO gerer les connexions refusees

	}
	
	struct sigaction action;
	action.sa_handler = handler;
	sigemptyset(&action.sa_mask);
	action.sa_flags = 0;

	if(sigaction(SIGINT, &action, NULL) == -1) {
		perror("Erreur lors du positionnement ");
		ncurses_stopper();
		exit(EXIT_FAILURE);
	}


	//Lancement des visuels
	WINDOW *fen_box_sim, *fen_box_msg, *fen_box_col;



	ncurses_initialiser();
	ncurses_couleurs();
	
	fen_box_sim = creer_fenetre_box_sim();
	fen_sim = creer_fenetre_sim();
	fen_box_msg = creer_fenetre_box_msg();
	fen_msg = creer_fenetre_msg();
	fen_box_col = creer_fenetre_box_col();
	fen_col = creer_fenetre_col();
	
	

	mvprintw(LINES - 1, 0, "Tapez F2 pendant votre tour pour quitter");
	wrefresh(stdscr);


	/**
	Running : 
			2 = Victoire
			3 = victoire de l'adversaire
			4 = Interruption
			5 = Interruption de l'adversaire
			6 = Match NUL
			7 = refus de connexion
	**/
	while(running == 1 && (res = recvfrom(sockfd, bufferMsg, sizeof(bufferMsg), 0, (struct sockaddr*)&adresseServeur, &adresseSlaveLen))){
		if( res == -1){
			perror("Erreur lors de la reception du message ");
			ncurses_stopper();
			exit(EXIT_FAILURE);
		}

		type = 9;
		memcpy(&connexion_master,&type,sizeof(type));

		if(sendto(sockfd, connexion_master, sizeof(connexion_master), 0, (struct sockaddr *)&s, sizeof(struct sockaddr_in)) ==-1 ){
			perror("Erreur lors de l'envoi de l'envoi de l'accuse de reception ");
			ncurses_stopper();
	    	exit(EXIT_FAILURE);
		}


		
		memcpy(&type,&bufferMsg,sizeof(unsigned char));
		//printf("Type : %u\n",type);

		//On attends l'état de la partie, on joue, puis on envoi à nouveau

		switch(type){
			case 4:
				grille = (unsigned char**) malloc(sizeof(unsigned char *)* HAUTEUR);
				
				for(int i = 0; i< HAUTEUR; i++){
					grille[i] = (unsigned char *) malloc(sizeof(unsigned char) * LONGUEUR);
					for(int j = 0; j < LONGUEUR; j++){
						memcpy(&grille[i][j],&bufferMsg[sizeof(unsigned char) + ((i * LONGUEUR  + j ) * sizeof(unsigned char))],sizeof(unsigned char));
					}
					
				}

				afficherGrille(grille);

				repeat = 1;

				while(repeat){

					wprintw(fen_msg, "A vous de jouer\n");
					wrefresh(fen_msg);
					ch = getch();
					//while ((ch = getchar()) != '\n' && ch != EOF) { }
					
					wrefresh(fen_msg);
					switch(ch) {
						case KEY_MOUSE :
							if (getmouse(&event) == OK) {
								wprintw(fen_msg, "Clic a la position %d %d de l'ecran\n", event.x, event.y);
								wrefresh(fen_msg);
								if (event.y >= 17 && event.y <= 19 && event.x >= 1 && event.x <= 41) {
									
									wprintw(fen_msg, "Tentative d'ajout à la colonne %d\n",(event.x) /6);
									wrefresh(fen_msg);
									if(colNonPleine(&grille,(event.x) /6)){
										//La colonne n'est pas pleine, on joue la piece
										//Piece ajoutée mais pas de victoire
										res = ajouterPiece(&grille, (event.x) /6,idJoueur);
										//wprintw(fen_msg, "Ajout OK\n");
										if( res == 0){

											//Pas de gagnant apres ce coup
											//On test si il y a un match nul

											if(matchNul(grille)){
												//Si match nul on envoit un message au serveur
												running = 6;
												repeat = 0;

												afficherGrille(grille);
												memset(&bufferMsg, 0, sizeof(bufferMsg));
												//type
												tmp = 8;
												memcpy(&bufferMsg,&tmp,sizeof(tmp));

												//idPartie
												tmp = idPartie;
												memcpy(&bufferMsg[sizeof(unsigned char)],&tmp,sizeof(tmp));

												//idJoueur
												tmp = idJoueur;
												memcpy(&bufferMsg[sizeof(unsigned char) *2],&tmp,sizeof(tmp));

												for(int i = 0; i< HAUTEUR; i++){
													for(int j = 0; j < LONGUEUR; j++){
														memcpy(&bufferMsg[sizeof(unsigned char)*3 + ((i * LONGUEUR  + j ) * sizeof(unsigned char))],&grille[i][j],sizeof(unsigned char));
													}
													
												}

												if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) ==-1 ){
													perror("Erreur lors de l'envoi de l'etat du match nul ");
													ncurses_stopper();
											    	exit(EXIT_FAILURE);
												}


											}else{
												//Sinon on continu la partie
												afficherGrille(grille);
												repeat = 0;
												//Envoi du coup joué

												memset(&bufferMsg, 0, sizeof(bufferMsg));
												//type
												tmp = 5;
												memcpy(&bufferMsg,&tmp,sizeof(tmp));

												//idPartie
												tmp = idPartie;
												memcpy(&bufferMsg[sizeof(unsigned char)],&tmp,sizeof(tmp));

												//idJoueur
												tmp = idJoueur;
												memcpy(&bufferMsg[sizeof(unsigned char) *2],&tmp,sizeof(tmp));



												//On copie le contenu de la grille ligne par ligne
												for(int i = 0; i< HAUTEUR; i++){
													for(int j = 0; j < LONGUEUR; j++){
														memcpy(&bufferMsg[sizeof(unsigned char)*3 + ((i * LONGUEUR  + j ) * sizeof(unsigned char))],&grille[i][j],sizeof(unsigned char));
													}
													
												}

												if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) ==-1 ){
													perror("Erreur lors de l'envoi de l'etat de la partie ");
													ncurses_stopper();
											    	exit(EXIT_FAILURE);
												}
											}




											



										}else{
											//Victoire du joueur
											afficherGrille(grille);

											//Envoi de la victoire au serveur
											memset(&bufferMsg, 0, sizeof(bufferMsg));
											//type
											tmp = 7;
											memcpy(&bufferMsg,&tmp,sizeof(tmp));

											//idPartie
											tmp = idPartie;
											memcpy(&bufferMsg[sizeof(unsigned char)],&tmp,sizeof(tmp));

											//idJoueur
											tmp = idJoueur;
											memcpy(&bufferMsg[sizeof(unsigned char) *2],&tmp,sizeof(tmp));

											for(int i = 0; i< HAUTEUR; i++){
												for(int j = 0; j < LONGUEUR; j++){
													memcpy(&bufferMsg[sizeof(unsigned char)*3 + ((i * LONGUEUR  + j ) * sizeof(unsigned char))],&grille[i][j],sizeof(unsigned char));
												}
												
											}

											if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) ==-1 ){
												perror("Erreur lors de l'envoi de l'etat de la victoire ");
												ncurses_stopper();
										    	exit(EXIT_FAILURE);
											}


											running = 2;
											repeat = 0;
										}

									}else{
										wprintw(fen_msg, "La colonne %d est pleine, veuillez en sélectionner une autre!\n",(event.x-3) /6);
										wrefresh(fen_msg);
									}

									
								}else{
									wprintw(fen_msg, "Veuillez cliquer sur un numéro de colonne!\n");
									wrefresh(fen_msg);
								}
							}

						break;

						//Interruption de la partie
						case 266 :
							//wprintw(fen_msg, "Vous avez quitté la partie\n");
							wrefresh(fen_msg);
							running = 4;
							repeat = 0;

							//Envoi d'un message au serveur
							//Type 3 == refus
							memset(&bufferMsg, 0, sizeof(bufferMsg));
							//type
							tmp = 6;
							memcpy(&bufferMsg,&tmp,sizeof(tmp));

							//idPartie
							tmp = idPartie;
							memcpy(&bufferMsg[sizeof(unsigned char)],&tmp,sizeof(tmp));

							//idJoueur
							tmp = idJoueur;
							memcpy(&bufferMsg[sizeof(unsigned char) *2],&tmp,sizeof(tmp));

							if(sendto(sockfd, bufferMsg, sizeof(bufferMsg), 0, (struct sockaddr *)&adresseServeur, sizeof(struct sockaddr_in)) ==-1 ){
								perror("Erreur lors de l'envoi de l'etat de l'interruption ");
								ncurses_stopper();
						    	exit(EXIT_FAILURE);
							}

							

						break;
					}
				}

				break;

				//Interruption du programme par l'adversaire
				case 6:
					running = 5;
					repeat = 0;
				break;


				case 7:
				//Victoire de l'adversaire

					grille = (unsigned char**) malloc(sizeof(unsigned char *)* HAUTEUR);
				
					for(int i = 0; i< HAUTEUR; i++){
						grille[i] = (unsigned char *) malloc(sizeof(unsigned char) * LONGUEUR);
						for(int j = 0; j < LONGUEUR; j++){
							memcpy(&grille[i][j],&bufferMsg[sizeof(unsigned char)*3 + ((i * LONGUEUR  + j ) * sizeof(unsigned char))],sizeof(unsigned char));
						}
						
					}

					afficherGrille(grille);



					running = 3;
					repeat = 0;


				break;

				case 8:
				//Match Nul

					grille = (unsigned char**) malloc(sizeof(unsigned char *)* HAUTEUR);
				
					for(int i = 0; i< HAUTEUR; i++){
						grille[i] = (unsigned char *) malloc(sizeof(unsigned char) * LONGUEUR);
						for(int j = 0; j < LONGUEUR; j++){
							memcpy(&grille[i][j],&bufferMsg[sizeof(unsigned char)*3 + ((i * LONGUEUR  + j ) * sizeof(unsigned char))],sizeof(unsigned char));
						}
						
					}

					afficherGrille(grille);



					running = 6;
					repeat = 0;


				break;
		}


		
	}

	/**
	Running : 
			2 = Victoire
			3 = victoire de l'adversaire
			4 = Interruption
			5 = Interruption de l'adversaire
			6  = match nul
	**/
	//Selon le contenu de running, on determine la cause de la fin de la partie
	switch(running){

		case 2:
			wprintw(fen_msg, "Vous avez gagnez!!\n");
			wrefresh(fen_msg);
		break;

		case 3:
			wprintw(fen_msg, "Vous avez perdu!!\n");
			wrefresh(fen_msg);
		break;




		//Interruption du joueur
		case 4: 
			

			wprintw(fen_msg, "Vous avez quitté la partie\n");
			wrefresh(fen_msg);



		break;

		case 5: 
			wprintw(fen_msg, "L'adversaire a quitté la partie\n");
			wrefresh(fen_msg);

		break;

		case 6: 
			wprintw(fen_msg, "Match nul\n");
			wrefresh(fen_msg);

		break;

		case 7: 
			wprintw(fen_msg, "Connexion refusée\n");
			wrefresh(fen_msg);

		break;
	}

	wprintw(fen_msg,"Appuyez sur F2 pour quitter ! \n");
  	wrefresh(fen_msg);
	while((ch = getch()) != 266) {
		wprintw(fen_msg,"F2 on vous a dit ! \n");
		wrefresh(fen_msg);
	}



	delwin(fen_box_sim);
	delwin(fen_sim);
	delwin(fen_box_msg);
	delwin(fen_msg);
	delwin(fen_box_col);
	delwin(fen_col);
	ncurses_stopper();

	return 1;
}

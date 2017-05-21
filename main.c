/* -------------------------------------------------------------------------------------------

                                main.c

        Module central.

---------------------------------------------------------------------------------------------- */


// ----- includes standards ----------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// ----- includes fichiers projet ----------

#include "webReq.h"
#include "tree.h"


// ----- defines ----------

typedef int bool;
#define true 1
#define false 0

// l'appel système pour effacer l'écran est différent sous Windows et Linux ...
#define CLEARSCR system ( "clear" )
#if defined MSDOS || defined WIN32
	#define CLEARSCR system ( "cls" )
#endif



// ========== fonctions ======================================================================


void asciiArtTitre (void) {

	// affiche le titre de l'interface en Ascii Art (c'est pas beau ça ?) !
	printf ("		         _________ _       _________ _______  _______  _\n");
	printf ("		|\\     /|\\__   __/| \\    /\\\\__   __/(  ____ \\(  ___  )( \\    |\\     /|\n");
	printf ("		| )   ( |   ) (   |  \\  / /   ) (   | (    \\/| (   ) || (    | )   ( |\n");
	printf ("		| | _ | |   | |   |  (_/ /    | |   | (_____ | |   | || |    | |   | |\n");
	printf ("		| |( )| |   | |   |   _ (     | |   (_____  )| |   | || |    ( (   ) )\n");
	printf ("		| || || |   | |   |  ( \\ \\    | |         ) || |   | || |     \\ \\_/ /\n");
	printf ("		| () () |___) (___|  /  \\ \\___) (___/\\____) || (___) || (____/\\\\   /\n");
	printf ("		(_______)\\_______/|_/    \\/\\_______/\\_______)(_______)(_______/ \\_/\n");
	}



void setParam ( char * param ) {

	char choix;

	CLEARSCR;

	asciiArtTitre();

	choix = getchar();
	}

 

void usage ( char * programme ) {

	fprintf (stderr, "Usage : %s [-s <départ>] -e [<fin>] [-g] [-h]\n", programme);
	fprintf (stderr, "Avec : \n");
	fprintf (stderr, "	-s : pour préciser le sujet de départ\n");
	fprintf (stderr, "	-d : pour préciser le sujet de fin\n");
	fprintf (stderr, "	-g : utilisation de la résolution Google\n");
	fprintf (stderr, "	-h : affichage de ce message d'aide\n");
	}




// ========== interface ======================================================================

// ==> La boucle principale du jeu !

void interface ( char * start, char * end, bool google) {

	bool useGoogle = google;
	char choix;
	char * depart = malloc (1); * depart = 0;
	char * cible = malloc (1); * cible = 0;

	// récupération des éventuelles options start & end
	if (start != NULL) {
		depart = realloc (depart, 50);
		strcpy (depart, start);
		}
	if (end != NULL) {
        cible = (char *) realloc (cible, strlen (end + 1));
        strcpy (cible, end);
        }

	// boucle
	do {

		// effacement de l'écran
		CLEARSCR;

		// affichage du titre
		printf ("\n\n");
		asciiArtTitre();
		printf ("\n\n");
	
		// affichage des options
		printf ("		s) Sujet de départ :                        %s\n", depart);
		printf ("		c) Sujet cible :                            %s\n", cible);
		printf ("\n		g) Utilisation de la résolution Google :    ");
		if (useGoogle) printf ("oui\n"); else printf ("non\n");
		printf ("\n\n");
		printf ("		q) Quitter\n");
	
		// saisie du choix 
		printf ("\n\n		Votre choix ? ");
		choix = getchar (); 
		printf ("\n");

		// positionnement d'un paramètre
		if (choix == 's') setParam ( depart );
		if (choix == 'c') setParam ( cible );
		}

	while (choix != 'q');

	CLEARSCR;
	}




// ========== main ===========================================================================

int main ( int argc, char *argv[]) {


	// ----- variables ----------

	// pour le traitement des options
	int option;
	bool erreurs = false;
	extern char * optarg; 
  	extern int optind; 
  	char * programme = argv[0];

	// pour la résolution du jeu
	char * start = NULL;
	char * end = NULL;
	bool useGoogle = false;	

	// traitement des options (format -<option> [<argument>]
	while ((option = getopt(argc , argv, "s:e:g")) != -1) {
    	switch (option) {
    		case 's':
     			start = optarg;
      			break;
    		case 'e':
				end = optarg;
				break;
    		case 'g':
				useGoogle = true;
      			break;
    		case '?':
      			erreurs = true;
      			break;
    		}
		}

	// si il y a eu une erreur ou demande de l'aide on affiche l'usage
  	if (erreurs) usage(programme);
	else interface (start, end, useGoogle);
	}

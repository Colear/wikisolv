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
#include "game.h"


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


// ----- asciiArtTitre ----------

// Affiche un magnifique titre !

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


// ----- clearInputBuffer ----------

// Cette fonction permet de vider le buffer d'entrée clavier. C'est nécessaire car un appel à getchar ()
// stocke le caractère choisi mais aussi un retour chariot. Du coup le prochain appel à getchar ()
// utilise ce retour chariot, sans attente d'entrée utilisateur.
//
// Attention : n'apppeler que si il y a eu un appel getchar ou scanf préalable !

void clearInputBuffer (void) {

	char c;

	do 
    	{
        c = getchar();
    	} while (c != '\n' && c != EOF);
	}



// ----- setParamBool ----------

// Positionne un paramètre de type booléen (0 ou 1)

void setParamBool ( const char * description, int  * param ) {

	char choix;
 
    CLEARSCR;

    printf ("\n\n");
    asciiArtTitre();
    printf ("\n\n");

    // récupération de la valeur
    printf ("       %s : ", description);
	choix = getchar (); 

	if (choix == 'o' || choix == 'y' || choix == 'O' || choix == 'Y') * param = 1;
	else * param = 0;

    // nettoyage du buffer d'entrée
    clearInputBuffer ();
    }



// ----- setParamChaine ----------

// Positionne un paramètre de type chaîne

void setParamChaine ( const char * description, char * param ) {

	char buffer[512];
	int res;

	CLEARSCR;

	printf ("\n\n");
	asciiArtTitre();
	printf ("\n\n");

	// récupération de la valeur
	printf ("		%s : ", description);
	res = scanf ("%511[0-9a-zA-Z ]", buffer);
	
	// recopie de la saisie (si conforme) dans le paramètre
	if (res == 1) {
		param = realloc (param, strlen (buffer + 1));
		strcpy (param, buffer);
		}

	// nettoyage du buffer d'entrée
	clearInputBuffer ();
	}


 
// ----- setParamNum ----------

// Positionne un paramètre de type numérique (entier)

void setParamNum ( const char * description, int  * param ) {

    int res;
	char buffer[512];
 
    CLEARSCR;

    printf ("\n\n");
    asciiArtTitre();
    printf ("\n\n");

    // récupération de la valeur
    printf ("       %s : ", description);
    res = scanf ("%d", buffer);

	printf ("%s\n", buffer);

    // nettoyage du buffer d'entrée
    clearInputBuffer ();
	getchar();
    }



// ----- usage ----------

// Affiche l'aide en cas d'erreur dans les arguments de lancement du programme

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
		if (strlen (depart) && strlen (cible))
			printf("\n		l) Lancement de la résolution\n");
		printf ("\n\n");
		printf ("		q) Quitter\n");
	
		// saisie du choix 
		printf ("\n\n		Votre choix ? ");
		choix = getchar (); 
		clearInputBuffer ();
		printf ("\n");

		// positionnement d'un paramètre
		if (choix == 's') setParamChaine ( "Sujet de départ", depart );
		if (choix == 'c') setParamChaine ( "Sujet cible", cible );
		if (choix == 'g') setParamBool ("Utilisation de la résolution google (o/n)", &useGoogle);
		if (choix == 'l') {
			CLEARSCR;
			lanceResolution (depart, cible, useGoogle);
			printf ("\n\n	Une touche pour continuer ... ");
			getchar ();
			clearInputBuffer ();
			}
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

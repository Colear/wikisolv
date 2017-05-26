/* -------------------------------------------------------------------------------------------
  
   				game.c

	Ce module gère le déroulement du jeu et de la résolution

---------------------------------------------------------------------------------------------- */


// ----- includes standards ----------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// ----- includes fichiers projet ----------

#include "game.h"
#include "webReq.h"



// ----- defines ----------

typedef int bool;
#define true 1
#define false 0

// l'appel système pour effacer l'écran est différent sous Windows et Linux ...
#define CLEARSCR system ( "clear" )
#if defined MSDOS || defined WIN32
    #define CLEARSCR system ( "cls" )
#endif



// ===== fonctions ==========


// ----- cibleInLiens ----------

// Permet de vérifier sila cible n'est pas dans la liste des liens en cours

bool cibleInLiens (char * cible, Liens * liens) {

	while (liens != NULL) {
		if (! strcmp (cible, liens->lien)) return true;
		liens = liens->suivant;	
		}
	return false;
	}



// ----- lanceResolution ----------

// Fonction principale pour la résolution.

int lanceResolution (char * depart, char * cible, int useGoogle) {

	Liens * liens;
	char * examen = malloc (strlen (depart) + 1);
	char * path = malloc (strlen (depart) + 1);
	int bonds = 1;

	strcpy (examen, depart);
	strcpy (path, depart);

	// while (1) {
	
		liens = getLiensWiki (examen);

		if (liens == NULL) return -1;
	
		if (cibleInLiens (cible, liens)) printf ("trouvé !!!");
		else printf ("pas trouvé !"); 	
	getchar();
	return 0;
	}


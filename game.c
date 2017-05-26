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

	Liens * arbre = creeListeWiki ();
	Liens * scan; 
	Liens * end;

	char * examen = malloc (strlen (depart) + 1);
	int bonds = 1;

	arbre->lien = malloc (strlen (depart) + 1);
	strcpy (arbre->lien, depart);
	arbre->suivant = NULL;
	arbre->precedent = NULL;

	end = arbre;

	while (bonds < 100) {

		// infos
		printf ("exament du sujet %s\n", arbre->lien);

		// pointeur sur la fin de liste
		while (end->suivant != NULL) end = end->suivant;

		// on récupére la liste des liens fils du lien courant
		scan = getLiensWiki (arbre->lien);
			
		// on vérifie qu'on ait pas résolu le problème
		if (cibleInLiens (cible, scan)) {
			printf ("Cible %s trouvée comme fils de %s\n", cible, arbre->lien);
			return bonds;
			}
	
		// on les ajoute en fin de liste 
		end->suivant = scan;
		scan->precedent = end;

		// passage au noeud suivant
		arbre = arbre->suivant;
		
		bonds++;
		}

	return 0;
	}


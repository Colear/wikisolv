/* -------------------------------------------------------------------------------------------
  
   				tree.c

   	Ce module gère les liste et les arbres du projet.

---------------------------------------------------------------------------------------------- */


// ----- includes standards ----------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// ----- includes fichiers projet ----------

#include "tree.h"



// ===== fonctions ==========


// ----- ajouteDansListe ----------

// Ajoute un nouveau lien dans la liste

int ajouteDansListe (Liens * liste, char * nvLien, long pertinence) {

	Liens * nvElement;


	// deux cas possible : 
	//   - la liste a été crée mais jamais utilisé, dans ce cas il faut juste remplir le lien
	//   - la liste comprends au moins un élément renseigné, il faut insérer le nouveau
	//
	// remarque : vu que l'ordre des liens importe peu on insère au début de la liste fournie, 
	//            c'est plus facile.

	if (liste->lien == NULL) {

		// premier cas on copie juste le lien et la pertinence
		liste->lien = malloc(strlen(nvLien) + 1);
		memcpy(liste->lien, nvLien, strlen(nvLien) + 1);
		liste->pertinence = pertinence;
		}

	else {
		
		// second cas, on crée le nouvel élément
		nvElement = malloc(sizeof(Liens));
		
		// on y copie lien et pertinence
		nvElement->lien = malloc(strlen(nvLien) + 1);
        memcpy(nvElement->lien, nvLien, strlen(nvLien) + 1);
        nvElement->pertinence = pertinence;
	
		// on insère le nouvel élément dans la liste, au niveau du pointeur liste
		nvElement->suivant = liste->suivant;
		nvElement->precedent = liste;
		liste->suivant = nvElement;
		}
	}


// ----- creeListe ----------

// Crée une liste vide destinée à contenir l'ensemble des liens wiki d'une page

Liens * creeListeWiki () {

    Liens * liste = malloc(sizeof(Liens));

    // initialisation 
    liste->lien = NULL;
	liste->pertinence = 0;
    liste->suivant = NULL;
	liste->precedent = NULL;

	// on retourne le premier élément - vide - de la liste
	return liste;
	}


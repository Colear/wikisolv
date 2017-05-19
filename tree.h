/* -------------------------------------------------------------------------------------------
  
   				tree.h

   	Ce module gère les liste et les arbres du projet.

---------------------------------------------------------------------------------------------- */

#ifndef TREE_H_   // pour ne pas faire cette inclusion plusieurs fois
#define TREE_H_



// ===== structures ==========

// pour la liste chainée contenant les liens

typedef struct Liens Liens;
struct Liens
{
        char* lien;			// la chaine contenant le lien
		long pertinence;	// pour stocker une info numérique permettant de mettre un poids au lien
        Liens* suivant;		// élement suivant dans la liste
		Liens* precedent;	// élément précédent dans la liste
};



// ===== fonctions ==========


// ----- ajouteDansListe ----------

// Ajoute un nouveau lien dans la liste

int ajouteDansListe (Liens * liens, char * nvLien, long pertinence);



// ----- creeListe ----------

// Crée une liste vide destinée à contenir l'ensemble des liens wiki d'une page

Liens * creeListeWiki ();


#endif // TREE_H_

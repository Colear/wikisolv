/* -------------------------------------------------------------------------------------------
  
   				webReq.h

   	Ce module contient toutes les fonctions passant les requêtes CURL

---------------------------------------------------------------------------------------------- */

#ifndef REQWEB_H_   // pour ne pas faire cette inclusion plusieurs fois
#define REQWEB_H_

// on a besoin des fonctions de listes et d'arbres
#include "tree.h"



// ===== structures ==========



// ===== fonctions ==========


// ----- getLiens Wiki ----------

// Retourne une liste de tous les liens wiki d'une page Wikipedia
// Les liens fournis sont juste les sujets, et décodés (par exemple
// "état" au lieu de "%C3%A9tat".

Liens * getLiensWiki (char* sujet);


#endif // REQWEB_H_

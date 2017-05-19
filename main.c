/* -------------------------------------------------------------------------------------------

                                main.c

        Module central.

---------------------------------------------------------------------------------------------- */


// ----- includes standards ----------

#include <stdio.h>



// ----- includes fichiers projet ----------

#include "webReq.h"
#include "tree.h"



// ========== fonctions ======================================================================






// ========== main ===========================================================================

int main ( int argc, char *argv[]) {

	printf ("début du main\n");
	
	printf ("test des fonctions de liste\n");

	Liens * liste = creeListeWiki();
	ajouteDansListe (liste, "truc", 1);
	ajouteDansListe (liste, "bidule", 0);
	ajouteDansListe (liste, "machin", 100);
	
	while (liste != NULL) {
		printf ("liens : %s, pondération = %d\n", liste->lien, liste->pertinence);
		liste = liste->suivant;
	}

	printf ("fin du main\n");
	}

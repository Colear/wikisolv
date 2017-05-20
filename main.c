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

	char sujet[] = "Nogent le rotrou";

	printf ("début du main\n");
	
	Liens * liste;

	liste = getLiensWiki (sujet);

	if (liste->lien != NULL) {	
		while (liste != NULL) {
			printf ("liens : %s, pondération = %d\n", liste->lien, liste->pertinence);
			liste = liste->suivant;
			}
		}

	printf ("test de la requête Google\n");
	printf ("nombre de résultats pour Ours et Choucroute = %d\n", getResGoogle ("Ours", "Choucroute")); 
	
	printf ("fin du main\n");
	}

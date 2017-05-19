#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>



// structure pour contenir les données reçues

struct Buffer
{
   char* buffer;
   size_t size;
};


// structure pour la liste chainée contenant les liens

typedef struct Liens Liens;
struct Liens
{
	char* lien;
	Liens* suivant;
};



/* ----- maCallback ----------------------------------------------------

   Cette fonction place la page web dans la structure Buffer.
   Elle est appelée automatiquement par le curl_easy_perform
   si on a pris soin de positionner l'option CURL_WRITE_FONCTION
   via curl_easy_setopt.

   Important :
     Super important à comprendre : pour une requête CURL cette
     fonction peut être appelée plusieurs fois !!!
     Libcurl va découper en petits bout le retour, en fonction
     de son max de traitement à la fois.
     ==> a chaque appel on ajoute les nouvelles données à celles
         déjà reçues et sotckées dans la structure !

   Arguments :
     - ptr => pointeur sur les données retournées par la requête
     - size et nmemb => donne la taille des données retournées. Pas
         bien compris pourquoi le besoin de deux valeurs, mais la
         taille de la page s'obtient en faisant size * nmemb
     - data => pointeur sur la structure à remplir avec les données

   Retour : il faut retourner la taille des données

   Attention : les données reçues dans ptr ne sont pas terminées par
               un zéro !

------------------------------------------------------------------------ */


static size_t maCallback(void* ptr, size_t size, size_t nmemb, void* data)
{

  // taille réelle des données
  size_t realsize = size * nmemb;

  // structure cible pour les données
  struct Buffer* mem = (struct Buffer*) data;

  // on redimmensionne l'espace mémoire de la structure cible à la taille
  // des données (précédentes + nouvelles) + 1 pour mettre un zéro terminal
  mem->buffer = (char*)realloc(mem->buffer, mem->size + realsize + 1);

  // si l'allocation mémoire s'est bien passée ...
  if ( mem->buffer )
  {
      // on copie les données de la requête dans la structure à la fin
      // des données déjà existantes
      memcpy(&(mem->buffer[mem->size]), ptr, realsize );

      // on ajoute la taille des nouvelles données
      mem->size += realsize;

      // on ajoute le zéro terminal
      mem->buffer[ mem->size ] = 0;
  }

  // retour du nombre de caractères reçus
  return realsize;
}



inline int ishex(int x)
{
    return  (x >= '0' && x <= '9')  ||
        (x >= 'a' && x <= 'f')  ||
        (x >= 'A' && x <= 'F');
}

int decode(const char *s, char *dec)
{
    char *o;
    const char *end = s + strlen(s);
    int c;

    for (o = dec; s <= end; o++) {
        c = *s++;
        if (c == '+') c = ' ';
        else if (c == '%' && (  !ishex(*s++)    ||
                    !ishex(*s++)    ||
                    !sscanf(s - 2, "%2x", &c)))
            return -1;

        if (dec) *o = c;
    }

    return o - dec;
}


/* ----- getNbResult --------------------------------------------------- 

	Retourne le nombre de résultats Google du lien passé en param.

-----------------------------------------------------------------------  */

long getNbResult (char* link) {

	char * sujet;
	CURL *curl;
  	CURLcode res;
  	struct Buffer req;
	char * URL;
	char * debut;
	char * fin;
	char temp [512];
	char result [512];
	int retour; char * reste;

	sujet = link; 

	// si c'est bien un lien wiki on vire le /wiki/
	if (strstr (sujet, "/wiki/") != NULL) sujet += 6;

    // création de l'URL
	URL = (char *) malloc (32 + strlen (sujet) + 1);
	strcpy (URL, "https://www.google.fr/search?q=");
	strcpy (URL + 31, sujet);

	// initialisation du buffer de réception
  	req.buffer = NULL;
  	req.size = 0;

  	curl = curl_easy_init();
  	if(curl) {

    	// requête Google
    	curl_easy_setopt(curl, CURLOPT_URL, URL);

    	// ajout de l'option pour la callback
    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, maCallback);
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&req);
	}

    // exécution de la requête
    res = curl_easy_perform(curl);

    // contrôle des erreurs ...
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
      return 1;
      }

    // ... et si tout va bien on récupère les données
    else {
		// traiter les données récupérées
		debut = strstr (req.buffer, "id=\"resultStats\"");

		if (debut == NULL) 
			printf ("pas de résultats Google !");
		else {
			debut += 17;
			fin = strstr (debut, "</div>");

			if (fin != NULL) {
				strncpy (temp, debut, fin - debut);
				temp [ fin - debut] = 0;

				int i = 0; int j = 0;
				// on se positionne au début du nombre
				while (! isdigit(temp [i])) i++ ;
				// tant qu'on ne récupère pas le "r" de résultat on est sur le nombre
				while ( temp[i] != 'r') {
					// si on est sur un caractère & on incrément jusqu'au ;
					if (temp[i] == '&') {
						while (temp[i++] != ';');
					}
					result[j++] = temp[i++];
				} 
				result[j] = 0;
				retour = strtol (result, &reste, 10);
			}
		} 
    }

    // toujours nettoyer !
    curl_easy_cleanup(curl);
	
	return retour;
}

 

/* ----- getLinks2 -----------------------------------------------------
 *
 * --------------------------------------------------------------------- */

Liens * memoriseLiensWiki (char* page) {

	char * pageOk = strstr ( page , "<h1 id=\"firstHeading\" class=\"firstHeading\"");
    char * pageFin = strstr ( page , "<div class=\"printfooter\">");
    char * lienFin;
    char buffer[512];
	int found;
	
	// structures pour liste chainée
	Liens * liens = malloc(sizeof(Liens));
	Liens * firstLien = liens;
	Liens * scanLiens;

	// initialisation du premier élément de la liste
	liens->lien = NULL;
	liens->suivant = NULL;

	// on met un 0 terminal au niveau de pageFin
    *pageFin = 0;

    // on boucle tant qu'on trouve des occurences de lien
    while ( (pageOk = strstr (pageOk,  "<a href=\"/wiki/")) != NULL ) {

                // on avance un peu pour supprimer la balise et le href
                pageOk = strstr (pageOk, "/wiki/");

                // la fin du lien est marquée par une guillement double
                lienFin = strchr (pageOk, '"');

                // on copie le lien dans le buffer
				// on prends le nb de car entre pageOk et lienFin, moins 1 (pour ne pas avoir le ")
                memcpy(buffer, pageOk, lienFin - pageOk);
                buffer[lienFin - pageOk] = 0;
                // printf ("%s\n", buffer);

		// on vérifie que le lien n'existe pas déja dans la liste
		found = 0;
		scanLiens = firstLien;
 		do {
			if (scanLiens->lien != NULL && ! strcmp(scanLiens->lien, buffer)) {
				found = 1;
				break;
			}
			scanLiens = scanLiens->suivant;
		} while (scanLiens != NULL);

		// si le lien n'existe pas ET si le lien ne contient pas de ':' (catégorie, ...)
		// on crée une nouveau membre dans la liste
		if (! found && strchr (buffer, ':') == NULL) {

			// si premier lien ...
			if (liens->lien == NULL) {
				liens->lien = malloc(strlen(buffer) + 1);
				memcpy(liens->lien, buffer, strlen(buffer) + 1);
			}

			// sinon ...
			else {
				Liens * nouveau = malloc(sizeof(Liens));
				liens->suivant = nouveau;
				liens = nouveau;
				liens->lien = malloc(strlen(buffer) + 1);
                memcpy(liens->lien, buffer, strlen(buffer) + 1);
				liens->suivant = NULL;
			}
		}

		// on se déplace après la balise
		pageOk = lienFin + 1;
	}

	return firstLien;
}



Liens * getLiensWiki (char* URL) { 

	
  CURL *curl;
  CURLcode res;

  // variable de type Buffer pour réception de la page
  struct Buffer page;
  page.buffer = NULL;
  page.size = 0;

  // récupération des liens
  Liens * liens;


  curl = curl_easy_init();
  if(curl) {

	printf ("URL = %s\n", URL);
    // URL de test
    curl_easy_setopt(curl, CURLOPT_URL, URL);

    // ajout de l'option pour la callback
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, maCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&page);

    // exécution de la requête
    res = curl_easy_perform(curl);

    // contrôle des erreurs ...
    if(res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));
      return NULL;
      }

    // ... et si tout va bien on récupère les données
    else {

      // getLinks (page.buffer);
      liens = memoriseLiensWiki (page.buffer);
    }

    // toujours nettoyer !
    curl_easy_cleanup(curl);
	}

	return liens;
}


 
/* ----- main ----------------------------------------------------------

   Exemple simple qui interroge un de mes serveurs Joomla interne.

   La CURLOPT_URL peut être remplacée par n'importe quoi.

------------------------------------------------------------------------ */

int main(void)
{
  	// récupération des liens
  	Liens * liens;
	char buffer [512];

	liens = getLiensWiki ("https://fr.wikipedia.org/wiki/Nogent-le-Rotrou");
	
	// affichage du premier lien
	// printf ("%s\n", liens->lien );

    while (liens->suivant != NULL) {
		decode(liens->lien, buffer);
		printf ("lien brut : %s - lien : %s - nombre de résultat Google : %d\n", liens->lien, buffer, getNbResult(buffer));
		liens = liens->suivant;
	}

  	return 0;
}

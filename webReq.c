/* -------------------------------------------------------------------------------------------
  
   				webReq.c

   	Ce module contient toutes les fonctions passant les requêtes CURL

---------------------------------------------------------------------------------------------- */


// ----- includes standards ----------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <curl/curl.h>



// ----- includes fichiers projet ----------

#include "webReq.h"



// ===== structures ==========

// réceptions des données CURL

struct Buffer
{
   char* buffer;
   size_t size;
};



// ========== fonctions internes =============================================================

// ----- curlCallback ----------

// CURL fonctionne avec un buffer (16k par défaut). Il va appeler cette fonction autant
// de fois que nécessaire pour afficher la page complète (2 fois pour une page supérieure
// à 16 Ko et inférieure à 32, ...).
// La callback doit donc non seulement stocker les données d'un appel, mais aussi les
// concatener avec les appels précédents. 

static size_t curlCallback(void* ptr, size_t size, size_t nmemb, void* data)
{

	// taille réelle des données (pas compris le pourquoi d'avoir ces deux valeurs...)
	size_t realsize = size * nmemb;

	// via l'option CURLOPT_WRITEDATA lors du lancement de la requête on a passé un pointeur
	// sur une structure que la callback nous renvoie à chaque fois. IL faut juste la retyper. 
	struct Buffer* mem = (struct Buffer*) data;

  	// on redimmensionne l'espace mémoire de la structure cible à la taille
  	// des données (précédentes + nouvelles) + 1 pour mettre un zéro terminal
  	mem->buffer = (char*)realloc(mem->buffer, mem->size + realsize + 1);

  	// si l'allocation mémoire s'est bien passée ...
  	if ( mem->buffer ) {
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



// ----- remplaceCar ----------

// Remplace toutes les occurences d'un caractère dans une chaine.
// Retourne le nombre d'occurence trouvées / remplacées.
// Attention : cette fonction ne marche qu'avec des vraies variables chaines !!!
//             lancer remplaceChar ("ma chaine", 'a', 'o') retournera une
//             erreur de segmentation car les chaines littérales sont en read only !

int remplaceChar (char * chaine, char carOld, char carNew) {

	int nbOcc =0;
	char * positionAct;

    positionAct = strchr (chaine, carOld);
	 
    while (positionAct){
        * positionAct = carNew;
		positionAct++;
        positionAct = strchr ( positionAct, carOld);
		nbOcc++;
    	} 

    return nbOcc;
	}



// ========== fonctions publiques ============================================================


// ----- getLiensWiki ----------

// Retourne une liste de tous les liens wiki d'une page Wikipedia
// Les liens fournis sont juste les sujets, et décodés (par exemple
// "état" au lieu de "%C3%A9tat".

Liens * getLiensWiki (char* sujet) {

  	CURL *curl;
  	CURLcode codeRet;
	char * URL;
  	struct Buffer page;
	char * debut; char * fin; char * finLien;
	char buffer[512]; 
	char * decode;
	int found;
  	Liens * liens; Liens * firstLien;

	// initialisation
  	page.buffer = NULL;
  	page.size = 0;
	firstLien = creeListeWiki();

	// création de l'URL
	// attention : dans la chaine sujet il faut remplacer les blancs par des "-" !
	remplaceChar (sujet, ' ', '_');
    URL = (char *) malloc (strlen ("https://fr.wikipedia.org/wiki/") + strlen (sujet) + 1);
	strcpy (URL, "https://fr.wikipedia.org/wiki/");
	strcpy (URL + strlen("https://fr.wikipedia.org/wiki/"), sujet);

	// initialisation de CURL
  	curl = curl_easy_init();
  	if(curl) {

    	curl_easy_setopt(curl, CURLOPT_URL, URL);
    	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
    	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&page);

    	// exécution de la requête
    	codeRet = curl_easy_perform(curl);

    	// contrôle des erreurs ...
    	if (codeRet != CURLE_OK) {
      		fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(codeRet));
      		return NULL;
      		}

    	// ... et si tout va bien on récupère les données
    	else {

			// si la taille est nulle, il y a eu redirection (=> le sujet proposé n'est pas un sujet Wiki !)
			if (! page.size) return NULL;
			
			// on encadre la zone où les liens sont significatifs 	
			debut = strstr ( page.buffer , "<h1 id=\"firstHeading\" class=\"firstHeading\"");
    		fin   = strstr ( page.buffer , "<div class=\"printfooter\">");

			// on met un zero terminal au niveau de page fin (pour limiter les fonctions
			// de chaine à la zone debut -> fin
			*fin = 0;

			// on boucle tant qu'on trouve des occurences de lien
    		while ( (debut = strstr (debut,  "<a href=\"/wiki/")) != NULL ) {

                // on avance un peu pour supprimer la balise, le href et le "/wiki/"
                debut += strlen ("<a href=\"/wiki/");

                // la fin du lien est marquée par une guillement double
                finLien = strchr (debut, '"');

                // on copie le lien dans le buffer (nb de car entre debut et finLien, moins 1  pour ne pas avoir le ")
                memcpy(buffer, debut, finLien - debut);
                buffer[finLien - debut] = 0;

				// on transforme l'UTF8
				decode = curl_easy_unescape(curl, buffer, 0, NULL);
				
				// on vérifie que le lien n'existe pas déja dans la liste
        		found = 0;
        		liens = firstLien;
        		do {
            		if (liens->lien != NULL && ! strcmp(liens->lien, decode)) {
                		found = 1;
                		break;
            			}
            		liens = liens->suivant;
        			} while (liens != NULL);

     			// si le lien n'existe pas ET si le lien ne contient pas de ':' (catégorie, ...)
        		// on crée une nouveau membre dans la liste
        		if (! found && strchr (buffer, ':') == NULL) {
					ajouteDansListe (firstLien, decode, 0);
					}

				} // while ... lien trouvé
    		} // else ... on récupère les données

    	// toujours nettoyer !
    	curl_easy_cleanup(curl);

    	} // if ... curl

    return firstLien;
	}



// ----- getResGoogle ----------

// Retourne le nombre de résultats d'une recherche combinée entre un 
// lien et le sujet de départ.

long getResGoogle (char* lien, char* depart) {

    CURL *curl;
    CURLcode codeRet;
    struct Buffer page;
    char * URL; char * mkURL;
    char * debut; char * fin; char * reste;
    char temp [512]; char result [512];
	int i; int j;
    int nbResults = -1; 							// -1 si le curl_easy_init echoue

    // création de l'URL
	URL = (char *) malloc (strlen ("https://www.google.fr/search?q=") + strlen (depart) + 1 + strlen (lien) + 1);
	mkURL = URL;
    strcpy (mkURL, "https://www.google.fr/search?q=");
	mkURL += strlen ("https://www.google.fr/search?q=");
    strcpy (mkURL, depart);
	mkURL += strlen (depart);
	strcpy (mkURL, "+");
	mkURL++;
	strcpy (mkURL, lien);
	mkURL += strlen (lien);
	mkURL[0] = 0;

    // initialisation du buffer de réception
	page.buffer = NULL;
    page.size = 0;

	// initialisation de CURL
    curl = curl_easy_init();
    if(curl) {
        curl_easy_setopt(curl, CURLOPT_URL, URL);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&page);

    	// exécution de la requête
    	codeRet = curl_easy_perform(curl);

    	// contrôle des erreurs ...
    	if(codeRet != CURLE_OK) {
    		fprintf (stderr, "Erreur dans la requête Google : %s\n", curl_easy_strerror(codeRet));
      		return 1;
      		}


    	// ... et si tout va bien on récupère les données
    	else {

			// on recherche l'id resultStats
		 	debut = strstr (page.buffer, "id=\"resultStats\"");

			// si pas trouvé sortie avec un message d'erreur
        	if (debut == NULL) {
				fprintf (stderr, "Erreur sur les résultats Google !");
				nbResults = -1;
				}

			// sinon il faut maintenant isoler juste les chiffres
        	else {
            	debut += 17;
            	fin = strstr (debut, "</div>");

				// copie dans un buffer de la zone contenant l'info
            	strncpy (temp, debut, fin - debut);
            	temp [ fin - debut] = 0;

				// on se positionne au début du nombre
            	i = 0; j = 0;
            	while (! isdigit(temp [i])) i++ ;

            	// tant qu'on ne récupère pas le "r" de résultat on est sur le nombre
            	while ( temp[i] != 'r') {

					// si on est sur un caractère & on incrément jusqu'au ;
                	if (temp[i] == '&') {
						while (temp[i++] != ';');
                    	}

					// on stocke le chiffre
                    result[j++] = temp[i++];
                	}
			
				// ca y est on a le resultat sous forme de chaine, reste à le convertir en entier
            	result[j] = 0;
            	nbResults = strtol (result, &reste, 10);
            	}

    		} // else ... récuperation des données

    	// toujours nettoyer !
    	curl_easy_cleanup(curl);

		} // if ... curl 

    return nbResults;
	}

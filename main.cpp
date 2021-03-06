#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <list>
#include <algorithm>
#include <cmath>

#include "Constantes.h"

SDL_Window *fenetre;
SDL_Renderer *renderer;
SDL_Texture  *textureChemin,
             *textureCaseFond,
             *textureChateau,

             *textureTourClassiqueBase,
             *textureTourPoisonBase,
             *textureTourSniperBase,

             *textureTourClassiqueCanon,
             *textureTourSniperCanon,

             *textureEnnemiClassique,
             *textureEnnemiRapideSimple,
             *textureEnnemiTank,
             *textureEnnemiVolantSimple,

             *textureTirClassique,
             *textureTirSniper,

             *textureExplosion[TEMPS_ANIM_TIR],

             *textureEnnemiVolant[NB_IMAGE_ENNEMI_VOLANT],
             *textureEnnemiRapide[NB_IMAGE_ENNEMI_RAPIDE],
             *textureCase[NB_CASE_FOND],
             *textureTourFond[NB_TOUR_FOND],

             *textureBpTourSniper,
             *textureBpTourClassique,
             *textureBpTourPoison,
             *textureBpEffacer,
             *textureBpAnnuler,
             *textureCadence,
             *textureDegatTir,
             *texturePorteeTour,
             *textureVitesseTir,


             *textureAccueil,
             *textureLogo;
TTF_Font *font;

#include "Case.h"
#include "TourClassique.h"
#include "TourSniper.h"
#include "TourPoison.h"
#include "TourAerien.h"
#include "Chemin.h"
#include "Chateau.h"
#include "EnnemiTank.h"
#include "EnnemiRapide.h"
#include "EnnemiClassique.h"
#include "EnnemiVolant.h"
#include "TirClassique.h"
#include "TirSniper.h"

#include "Variables.h"
#include "VariablesSDL.h"
#include "Utilitaires.h"
#include "Infos.h"
#include "Menus.h"

vector<int> vague;                  // Tableau permettant d'ajouter le type d'ennemi et de l'afficher pendant la vague
Case*** listeCases;
vector<Ennemi*> listeEnnemis;
vector<Tir*> listeTirs;

int nbParties;                      // Nombre de parties du joueur
string pseudoHighscore ="";                  // Variable contenant le pseudo entré par le joueur
int highscore;

string pseudo="";
int nbVagues=80;                    // Nombre de vague totale dans chaque partie
int depart=0;                       // Variable d'état pour commencer à afficher la vague d'ennemis
int occurences=10;                  // Nombre d'ennemis dans la première vague
int argent=200;                         // Argent du joueur
int numLevel=1;                     // Numero du level
int continuer=1;                    // Variable d'état pour quitter la boucle principale et fermer la fenêtre de jeu
int affichageArgent=1;

int TAILLE_X_PLATEAU = 20;
int TAILLE_Y_PLATEAU = 15;
int nbEnnemisSupplementaires =4;    // Nb d'ennemis supplémentaires à chaque vague
int compteurImage=0;                // Compteur d'images permettant d'effectuer des actions à un temps donné dans une boucle while
int rafraichissement=30;

int xChateau=0,
    yChateau=0;

int xVague=-1,                      // Variables permettant de connaitre la position de la case de départ
    yVague=-1;


void initListeCase()  // Initialisation des cases vides
{
    listeCases = new Case**[TAILLE_Y_PLATEAU];
    for(int y=0; y<TAILLE_Y_PLATEAU; y++)
    {
        listeCases[y]=new Case*[TAILLE_X_PLATEAU];
    }
    for(int y=0; y<TAILLE_Y_PLATEAU; y++)
    {
        for(int x=0; x<TAILLE_X_PLATEAU; x++)
        {
            listeCases[y][x]=NULL;
        }
    }
}

void quitListeCase()                // Fonction de suppression de la listeCases à la fin de la partie
{
    for(int y=0; y<TAILLE_Y_PLATEAU; y++)
    {
        for(int x=0; x<TAILLE_X_PLATEAU; x++)
        {
            delete listeCases[y][x];
        }
    }
    for(int y=0; y<TAILLE_Y_PLATEAU; y++)
    {
        delete[] listeCases[y];
    }
    delete[] listeCases;
}

void remplissageVague()                 // Fonction remplissant le tableau vague par le nombre d'ennemis voulu
{
    for (int i=0; i<(occurences); i++){
        vague.push_back(rand()%4);      // Tirage aléatoire du type d'ennemi (nombre entre 0 et 2)
    }
    depart=1;                           // Affichage de la vague à la prochaine condition
}


void initLevel(int numLevel) // Remplissage des cases en fonction des chiffres du fichier level.txt
{
    quitListeCase();

    // On ouvre le fichier level.txt
    string nomLvl= CHEMIN_LEVELS + "level" + to_string(numLevel) + ".txt";
    ifstream level(nomLvl.c_str(), ios::in);
    string ligne;
    int y=0;

    // On récupère la taille du plateau
    getline(level, ligne);
    TAILLE_X_PLATEAU=stoi(ligne);
    getline(level, ligne);
    TAILLE_Y_PLATEAU=stoi(ligne);
    initListeCase();

    // La taille de la fenêtre est adaptée à la taille du plateau
    SDL_SetWindowSize(fenetre,TAILLE_X_PLATEAU*TAILLE_CASE + MARGE_GAUCHE, TAILLE_Y_PLATEAU*TAILLE_CASE + MARGE_HAUT);

    // Chaque case est crée en fonction de son type
    while(getline(level, ligne))
    {
        for(int x=0; x<ligne.length(); x++)
        {
            if(ligne.at(x)=='1')
            {
                listeCases[y][x]=new Case(x, y, 0 );
            }
            else if(ligne.at(x)=='2')
            {
                listeCases[y][x]=new Chemin(x, y, 0);

            }
            else if(ligne.at(x)=='7')
            {
                listeCases[y][x]=new Chemin(x, y, 1);

            }
            else if(ligne.at(x)=='3')
            {
                listeCases[y][x]=new Chateau(x, y, 100);
                xChateau=x;
                yChateau=y;
            }
            else if(ligne.at(x)=='4')
            {
                listeCases[y][x]=new TourClassique(x, y, 100);
            }
            else if(ligne.at(x)=='5')
            {
                listeCases[y][x]=new TourSniper(x, y, 100);
            }
            else if(ligne.at(x)=='6')
            {
                listeCases[y][x]=new TourPoison(x, y, 100);
            }
        }
        y++;
    }
}

void jeu(int numlevel)  // Fonction de gestion et d'affichage de la partie
{
    initLevel(numlevel);
    SDL_Event events;
    bool terminer = false;

    int xSouris, ySouris, xCase, yCase, xCaseTour=-1, yCaseTour=-1; // Variables contenant la position x,y de la souris, ainsi que de la case cliquée
    remplissageVague();

    int itterations=0;      // Variable permettant d'arreter l'affichage des vagues au bout du nombre de vagues voulu
    int creation=0;
    int numeroEnnemi=0;

    while(!terminer && continuer==1)
    {
            if(compteurImage%10==0 && depart==1 && compteurImage>150)               // Affichage des vagues en fct de la valeur du tableau vague
            {
                if (vague[numeroEnnemi]==0){
                    listeEnnemis.push_back(new EnnemiRapide(listeCases[1][1]->getXcentre()-TAILLE_ENNEMI/2,
                                                            listeCases[1][1]->getYcentre()-TAILLE_ENNEMI/2,
                                                            itterations,
                                                            xChateau,
                                                            yChateau));
                }
                else if (vague[numeroEnnemi]==1){
                    listeEnnemis.push_back(new EnnemiClassique(listeCases[1][1]->getXcentre()-TAILLE_ENNEMI/2,
                                                               listeCases[1][1]->getYcentre()-TAILLE_ENNEMI/2,
                                                               itterations,
                                                               xChateau,
                                                               yChateau));

                }
                else if (vague[numeroEnnemi]==2){
                    listeEnnemis.push_back(new EnnemiTank(listeCases[1][1]->getXcentre()-TAILLE_ENNEMI/2,
                                                          listeCases[1][1]->getYcentre()-TAILLE_ENNEMI/2,
                                                          itterations,
                                                          xChateau,
                                                          yChateau));

                }else if (vague[numeroEnnemi]==3){
                    int nbAlea = rand()%15;
                    for(int i=0;i<nbAlea;i++){
                       int nbAlea2 = rand()%15;
                       listeEnnemis.push_back(new EnnemiVolant(listeCases[nbAlea2][1]->getXcentre()-TAILLE_ENNEMI/2,
                                                          listeCases[nbAlea2][1]->getYcentre()-TAILLE_ENNEMI/2,
                                                          itterations,
                                                          xChateau,
                                                          yChateau));
                    }


                }
                numeroEnnemi++;
                creation=1;             // Variable de création du premier ennemi pour éviter de remplir à nouveau le tableau vague lorsque aucun ennemi n'est affiché

            }

        if (numeroEnnemi==occurences){      // Si tous les ennemis sont affichés, on met à 0 la variable de départ
            numeroEnnemi=0;
            depart=0;
        }

         if (listeEnnemis.size()==0 && creation==1 && itterations<nbVagues-1){      // S'il n'y a plus d'ennemi dans la partie, on déchenche la vague suivante
         creation=0;
         vague.clear();
         occurences+=nbEnnemisSupplementaires*2*numLevel;             // La vague suivante contient 4 ennemis en plus
         remplissageVague();
         itterations++;

         }

        while(SDL_PollEvent(&events))       // Récupération des évènements
        {
            switch(events.type)
            {

            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
                continue;
                break;

            case SDL_QUIT :
                continuer=0;
                break;

            case SDL_KEYUP:
                SDL_GetMouseState(&xSouris, &ySouris);         // Récupération de la position de la souris
                xCase = (xSouris-MARGE_GAUCHE)/ TAILLE_CASE;   // On définit la case sur laquelle se trouve la souris
                yCase = (ySouris-MARGE_HAUT)/ TAILLE_CASE;

                // Création manuelle des 3 types d'ennemis pour le débogage
                if(events.key.keysym.scancode==SDL_SCANCODE_Q)
                {
                    listeEnnemis.push_back(new EnnemiClassique(listeCases[yCase][xCase]->getXcentre()-TAILLE_ENNEMI/2,
                                           listeCases[yCase][xCase]->getYcentre()-TAILLE_ENNEMI/2,
                                           itterations,
                                           xChateau,
                                           yChateau));
                }
                else if(events.key.keysym.scancode==SDL_SCANCODE_W)
                {
                    listeEnnemis.push_back(new EnnemiRapide(listeCases[yCase][xCase]->getXcentre()-TAILLE_ENNEMI/2,
                                                            listeCases[yCase][xCase]->getYcentre()-TAILLE_ENNEMI/2,
                                                            itterations,
                                                            xChateau,
                                                            yChateau));
                }
                else if(events.key.keysym.scancode==SDL_SCANCODE_E)
                {
                    listeEnnemis.push_back(new EnnemiTank(listeCases[yCase][xCase]->getXcentre()-TAILLE_ENNEMI/2,
                                                          listeCases[yCase][xCase]->getYcentre()-TAILLE_ENNEMI/2,
                                                          itterations,
                                                          xChateau,
                                                          yChateau));
                }
                else if(events.key.keysym.scancode==SDL_SCANCODE_R)
                {
                    listeEnnemis.push_back(new EnnemiVolant(listeCases[yCase][xCase]->getXcentre()-TAILLE_ENNEMI/2,
                                                            listeCases[yCase][xCase]->getYcentre()-TAILLE_ENNEMI/2,
                                                            itterations,
                                                            xChateau,
                                                            yChateau));
                }
                else if(events.key.keysym.scancode==SDL_SCANCODE_1)
                {
                    listeCases[yCase][xCase]->amelioration(AMELIORATION_CADENCE);
                }
                else if(events.key.keysym.scancode==SDL_SCANCODE_2)
                {
                    listeCases[yCase][xCase]->amelioration(AMELIORATION_DEGAT_TIR);
                }
                else if(events.key.keysym.scancode==SDL_SCANCODE_3)
                {
                    listeCases[yCase][xCase]->amelioration(AMELIORATION_PORTEE_TOUR);
                }
                else if(events.key.keysym.scancode==SDL_SCANCODE_4)
                {
                    listeCases[yCase][xCase]->amelioration(AMELIORATION_VITESSE_TIR);
                }

                // Suppression manuelle de tous les ennemis
                else if(events.key.keysym.scancode==SDL_SCANCODE_RETURN)
                {
                    for(int i=0; i<listeEnnemis.size(); i++)
                    {
                        delete listeEnnemis[i];
                    }
                    listeEnnemis.clear();
                }
                // Touche Echap permettant de quitter la partie
                else if(events.key.keysym.scancode==SDL_SCANCODE_ESCAPE)
                {
                    terminer=true;
                }
                // Création manuelle des cases / tours pour le débogage
                else if(SDL_SCANCODE_A<=events.key.keysym.scancode && events.key.keysym.scancode<=SDL_SCANCODE_Z){
                    if(listeCases[yCase][xCase]!=NULL)
                    {
                        delete listeCases[yCase][xCase];
                        listeCases[yCase][xCase]=NULL;
                    }
                    if(events.key.keysym.scancode==SDL_SCANCODE_Z)
                    {
                        listeCases[yCase][xCase]=new Case(xCase, yCase, 0);
                    }
                    else if(events.key.keysym.scancode==SDL_SCANCODE_X)
                    {
                        listeCases[yCase][xCase]=new Chemin(xCase, yCase,0);
                    }
                    else if(events.key.keysym.scancode==SDL_SCANCODE_C)
                    {
                        listeCases[yCase][xCase]=new Chateau(xCase, yCase, 100);
                    }
                    else if(events.key.keysym.scancode==SDL_SCANCODE_A)
                    {
                        listeCases[yCase][xCase]=new TourClassique(xCase, yCase,-1);
                    }
                    else if(events.key.keysym.scancode==SDL_SCANCODE_S)
                    {
                        listeCases[yCase][xCase]=new TourSniper(xCase, yCase, -1);
                    }
                    else if(events.key.keysym.scancode==SDL_SCANCODE_D)
                    {
                        listeCases[yCase][xCase]=new TourPoison(xCase, yCase, -1);
                    }
                    else if(events.key.keysym.scancode==SDL_SCANCODE_F)
                    {
                        listeCases[yCase][xCase]=new TourAerien(xCase, yCase, -1);
                    }
                }
                break;

            case SDL_MOUSEBUTTONUP:

                // Récupération de la position de la souris
                int x = events.button.x;
                int y = events.button.y;

                if(x>MARGE_GAUCHE && y>MARGE_HAUT)
                {
                    xCase=(x-MARGE_GAUCHE)/ TAILLE_CASE;
                    yCase=(y-MARGE_HAUT)/ TAILLE_CASE;
                    if(listeCases[yCase][xCase]!=NULL)                                      // Si la case contient qqch
                    {
                        if(listeCases[yCase][xCase]->getType()=="Case"
                                || listeCases[yCase][xCase]->getType()=="TourClassique"
                                || listeCases[yCase][xCase]->getType()=="TourSniper"
                                || listeCases[yCase][xCase]->getType()=="TourPoison")
                        {
                            // On enregistre la position de la case cliquée comme une tour
                            xCaseTour = xCase;
                            yCaseTour = yCase;
                        }
                    }
                }
                else if (x>24 && x<124 && y>21 && y<87)         // On quitte la partie avec le bp retour
                {
                    terminer=true;
                }
                else if(xCaseTour!=-1 && listeCases[yCaseTour][xCaseTour]->getType()=="Case")
                {
                    if (x>303 && x<377 && y>10 && y<86                                                  // S'il on clique sur le bp Tour Classique
                        && argent>=ARGENT_TOUR*TourClassique::multiplicateurCout)
                    {
                        //TOUR CLASSIQUE
                        if(listeCases[yCaseTour][xCaseTour]!=NULL)
                        {
                            delete listeCases[yCaseTour][xCaseTour];                                    // On supprime le contenu du tableau pour la case cliquée
                        }
                        listeCases[yCaseTour][xCaseTour]=new TourClassique(xCaseTour, yCaseTour,-1);    // On crée la tour voulue
                        argent-= ARGENT_TOUR*TourClassique::multiplicateurCout;
                        xCaseTour = -1;
                        yCaseTour = -1;
                    }
                    else if (x>503 && x<577 && y>10 && y<86                                             // S'il on clique sur le bp Tour Sniper
                             && argent>=ARGENT_TOUR*TourSniper::multiplicateurCout)
                    {
                        //TOUR SNIPER
                        if(listeCases[yCaseTour][xCaseTour]!=NULL)
                        {
                            delete listeCases[yCaseTour][xCaseTour];
                        }
                        listeCases[yCaseTour][xCaseTour]=new TourSniper(xCaseTour, yCaseTour, -1);
                        argent-= ARGENT_TOUR*TourSniper::multiplicateurCout;
                        xCaseTour = -1;
                        yCaseTour = -1;
                    }
                    else if (x>703 && x<777 && y>10 && y<86                                             // S'il on clique sur le bp Tour Poison
                             && argent>=ARGENT_TOUR*TourPoison::multiplicateurCout)
                    {
                        //TOUR POISON
                        if(listeCases[yCaseTour][xCaseTour]!=NULL)
                        {
                            delete listeCases[yCaseTour][xCaseTour];
                        }
                        listeCases[yCaseTour][xCaseTour]=new TourPoison(xCaseTour, yCaseTour, -1);
                        argent-= ARGENT_TOUR*TourPoison::multiplicateurCout;
                        xCaseTour = -1;
                        yCaseTour = -1;
                    }
                    else if (x>200 && x<280 && y>10 && y<90)                                            // Bp annuler pour pouvoir sélectionner une autre tour
                    {
                        //ANNULER CASE
                        xCaseTour = -1;
                        yCaseTour = -1;
                    }
                }


                // Si la case est l'une des 3 tours, on peut effectuer des actions sur ces tours
                else if(xCaseTour!=-1 && (listeCases[yCaseTour][xCaseTour]->getType()=="TourClassique"
                                          || listeCases[yCaseTour][xCaseTour]->getType()=="TourSniper"
                                          || listeCases[yCaseTour][xCaseTour]->getType()=="TourPoison"))
                {

                    if (x>500 && x<580 && y>10 && y<90)
                    {
                        //ANNULER TOUR
                        xCaseTour = -1;
                        yCaseTour = -1;
                        affichageArgent=1;
                    }
                    else if (x>400 && x<480 && y>10 && y<90)
                    {
                        //EFFACER TOUR
                        delete listeCases[yCaseTour][xCaseTour];
                        listeCases[yCaseTour][xCaseTour]=new Case(xCaseTour, yCaseTour, 0);
                        xCaseTour = -1;
                        yCaseTour = -1;
                        affichageArgent=1;
                    }
                    ///// AMELIORATIONS /////
                    else if(x>600 && x<680 && y>10 && y<90)
                    {
                        listeCases[yCaseTour][xCaseTour]->amelioration(AMELIORATION_CADENCE);
                        xCaseTour = -1;
                        yCaseTour = -1;
                        affichageArgent=1;
                    }
                    else if(x>700 && x<780 && y>10 && y<90)
                    {
                        listeCases[yCaseTour][xCaseTour]->amelioration(AMELIORATION_DEGAT_TIR);
                        xCaseTour = -1;
                        yCaseTour = -1;
                        affichageArgent=1;
                    }
                    else if(x>800 && x<880 && y>10 && y<90)
                    {
                        listeCases[yCaseTour][xCaseTour]->amelioration(AMELIORATION_PORTEE_TOUR);
                        xCaseTour = -1;
                        yCaseTour = -1;
                        affichageArgent=1;
                    }
                    else if(x>900 && x<980 && y>10 && y<90)
                    {
                        listeCases[yCaseTour][xCaseTour]->amelioration(AMELIORATION_VITESSE_TIR);
                        xCaseTour = -1;
                        yCaseTour = -1;
                        affichageArgent=1;
                    }
                }
            }
        }

        // ACTIONS // Placement des tours en direction des ennemis et tirs pour les détruire
        for(int y=0 ; y<TAILLE_Y_PLATEAU ; y++)
        {
            for(int x=0 ; x<TAILLE_X_PLATEAU ; x++)
            {
                if(listeCases[y][x]!=NULL)
                {
                    listeCases[y][x]->action();
                }
            }
        }

        for(int i=0; i<listeEnnemis.size(); i++)
        {
            listeEnnemis[i]->action();
        }
        for(int i=0; i<listeTirs.size(); i++)
        {
            listeTirs[i]->action();
        }

        // AFFICHAGE //

        SDL_SetRenderDrawColor(renderer, 0,127,127,255);    // Couleur de fond du plateau
        SDL_RenderClear(renderer);

        // Création de la zone contenant les boutons
        SDL_SetRenderDrawColor(renderer, 0, 127, 147, 255);
        SDL_Rect rect = {150,5,860,90};
        SDL_RenderDrawRect(renderer, &rect);
        SDL_RenderFillRect(renderer, &rect);

        // On trace le rectangle
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer,150,5,1010,5);
        SDL_RenderDrawLine(renderer,150,95,1010,95);
        SDL_RenderDrawLine(renderer,150,5,150,95);
        SDL_RenderDrawLine(renderer,1010,5,1010,95);


        // Affichage des cases (tours, chemin, chateau ...)
        for(int y=0 ; y<TAILLE_Y_PLATEAU ; y++)
        {
            for(int x=0 ; x<TAILLE_X_PLATEAU ; x++)
            {
                if(listeCases[y][x]!=NULL)
                {
                    listeCases[y][x]->affiche();
                }
            }
        }

        // Affichage des ennemis et des tirs
        for(int i=0; i<listeEnnemis.size(); i++)
        {
            listeEnnemis[i]->affiche();
        }
        for(int i=0; i<listeTirs.size(); i++)
        {
            listeTirs[i]->affiche();
        }

        if (affichageArgent==1){
        EcrireArgent(); }// Ecriture et affichage de l'argent

        SDL_GetMouseState(&xSouris, &ySouris);

        // Affichage de la portée par un cercle lorsque le pointeur est sur une tour
        if (xSouris>MARGE_GAUCHE && ySouris > MARGE_HAUT)
        {
            xCase = (xSouris-MARGE_GAUCHE)/ TAILLE_CASE;
            yCase = (ySouris-MARGE_HAUT)/ TAILLE_CASE;

            if(listeCases[yCase][xCase]!=NULL)
            {
                listeCases[yCase][xCase]->affichePortee();
            }
        }

        // Si l'on sélectionne une case, elle est encadrée en rouge
        if (xCaseTour!=-1)
        {
            SDL_SetRenderDrawColor(renderer, 255,0,0,100);
            {
                SDL_Point points[5] = {{MARGE_GAUCHE+TAILLE_CASE*xCaseTour, MARGE_HAUT+TAILLE_CASE*yCaseTour},
                    {MARGE_GAUCHE+TAILLE_CASE*(xCaseTour+1), MARGE_HAUT+TAILLE_CASE*yCaseTour},
                    {MARGE_GAUCHE+TAILLE_CASE*(xCaseTour+1), MARGE_HAUT+TAILLE_CASE*(yCaseTour+1)},
                    {MARGE_GAUCHE+TAILLE_CASE*xCaseTour, MARGE_HAUT+TAILLE_CASE*(yCaseTour+1)},
                    {MARGE_GAUCHE+TAILLE_CASE*xCaseTour, MARGE_HAUT+TAILLE_CASE*yCaseTour}
                };
                SDL_RenderDrawLines(renderer, points, 5);
            }
            {
                SDL_Point points[5] = {{MARGE_GAUCHE+TAILLE_CASE*xCaseTour+1, MARGE_HAUT+TAILLE_CASE*yCaseTour+1},
                    {MARGE_GAUCHE+TAILLE_CASE*(xCaseTour+1)-1, MARGE_HAUT+TAILLE_CASE*yCaseTour+1},
                    {MARGE_GAUCHE+TAILLE_CASE*(xCaseTour+1)-1, MARGE_HAUT+TAILLE_CASE*(yCaseTour+1)-1},
                    {MARGE_GAUCHE+TAILLE_CASE*xCaseTour+1, MARGE_HAUT+TAILLE_CASE*(yCaseTour+1)-1},
                    {MARGE_GAUCHE+TAILLE_CASE*xCaseTour+1, MARGE_HAUT+TAILLE_CASE*yCaseTour+1}
                };
                SDL_RenderDrawLines(renderer, points, 5);
            }

            // Affichage du prix de chaque tour + texture des tours
            if(listeCases[yCaseTour][xCaseTour]->getType()=="Case")
            {
                affichageArgent=1;
                affichageTexture(textureBpAnnuler,200,10);
                affichageTexture(textureBpTourClassique,300,10);
                Ecrire("CollegiateInsideFLF",20,to_string(ARGENT_TOUR*TourClassique::multiplicateurCout)+"$",0,0,0,380,40);
                affichageTexture(textureBpTourSniper,500,10);
                Ecrire("CollegiateInsideFLF",20,to_string(ARGENT_TOUR*TourSniper::multiplicateurCout)+"$",0,0,0,580,40);
                affichageTexture(textureBpTourPoison,700,10);
                Ecrire("CollegiateInsideFLF",20,to_string(ARGENT_TOUR*TourPoison::multiplicateurCout)+"$",0,0,0,780,40);
            }
            else
            {
                affichageArgent=0;

                // Affichage du nom de la tour
                if (listeCases[yCaseTour][xCaseTour]->getType()=="TourClassique"){
                    Ecrire("CollegiateInsideFLF",25,"Tour Classique :",0,0,0,180,15);
                }

                else if (listeCases[yCaseTour][xCaseTour]->getType()=="TourSniper"){
                    Ecrire("CollegiateInsideFLF",25,"Tour Sniper :",0,0,0,180,15);
                }

                else if (listeCases[yCaseTour][xCaseTour]->getType()=="TourPoison"){
                    Ecrire("CollegiateInsideFLF",25,"Tour Poison :",0,0,0,180,15);
                }

                affichageTexture(textureBpEffacer,400,10);
                affichageTexture(textureBpAnnuler,500,10);
                affichageTexture(textureCadence,600,10);
                affichageTexture(textureDegatTir,700,10);
                affichageTexture(texturePorteeTour,800,10);
                affichageTexture(textureVitesseTir,900,10);
            }
        }
        else
        {
            affichageTexture(textureLogo,250,12); // Affichage du logo Tower Defense
        }

        // Affichage du bp retour
        Ecrire("WingdingReview",40,"ñ",255,255,255,50,50);
        Ecrire("CollegiateInsideFLF",25,"Retour",255,255,255,30,25);

        if (listeCases[yChateau][xChateau]->action()==1){                               // Si le chateau n'a plus de vie
            compteurImage=0;

            while (compteurImage<150){                                      // Affichage du fond Game Over pendant 5s
                SDL_SetRenderDrawColor(renderer, 0,127,127,255);
                SDL_RenderClear(renderer);
                Ecrire("CollegiateBlackFLF",42,"GAME OVER",255,255,255,440,300);
                SDL_RenderPresent(renderer);
                SDL_Delay(30);
                compteurImage++;
            }
            terminer=true;      // On quitte la partie
        }
        // Rafraichissement de l'écran toutes les 30 ms
        SDL_RenderPresent(renderer);
        SDL_Delay(rafraichissement);
        compteurImage++;


    }
    // Suppression des listes ennemis et tirs
    for(int i=listeEnnemis.size()-1; i>=0; i--)
    {
        delete listeEnnemis[i];
    }
    for(int i=listeTirs.size()-1; i>=0; i--)
    {
        delete listeTirs[i];
    }
}

int initSDL()
{

    // Initialisation de la SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        cout << "Erreur lors de l'initialisation de la SDL : " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    if(TTF_Init() == -1)
    {
        cout << "Erreur d'initialisation de TTF_Init : "<< TTF_GetError()<<endl;
        return -1;
    }

    // Création de la fenêtre
    fenetre = SDL_CreateWindow("Tower Defense SDL 2.0",
                               SDL_WINDOWPOS_CENTERED,
                               SDL_WINDOWPOS_CENTERED,
                               TAILLE_X_PLATEAU*TAILLE_CASE + MARGE_GAUCHE,
                               TAILLE_Y_PLATEAU*TAILLE_CASE + MARGE_HAUT,
                               SDL_WINDOW_SHOWN);
    if(fenetre == 0)
    {
        cout << "Erreur lors de la creation de la fenetre : " << SDL_GetError() << endl;
        SDL_Quit();
        return -1;
    }

    //Création du renderer
    renderer = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_ACCELERATED);
    if(renderer==NULL)
    {
        cout << "Erreur lors de la creation du renderer : " << SDL_GetError() << std::endl;
        SDL_Quit();
        return -1;

    }
    SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" );


    //Surface pour charger toutes les textures
    SDL_Surface* surface;

    chargeTexture(textureCadence, "bpAmeliorationCadence.png");
    chargeTexture(textureDegatTir, "bpAmeliorationDegatTir.png");
    chargeTexture(texturePorteeTour, "bpAmeliorationPorteeTour.png");
    chargeTexture(textureVitesseTir, "bpAmeliorationVitesseTir.png");
    chargeTexture(textureBpTourClassique, "bpTourClassique.png");
    chargeTexture(textureBpTourSniper, "bpTourSniper.png");
    chargeTexture(textureBpTourPoison, "bpTourPoison.png");
    chargeTexture(textureBpEffacer, "bpEffacer.png");
    chargeTexture(textureBpAnnuler, "bpAnnuler.png");
    chargeTexture(textureLogo, "logo.png");
    chargeTexture(textureAccueil, "towerDefense.png");
    chargeTexture(textureCaseFond, "caseFond.png");
    chargeTexture(textureChemin, "chemin.png");
    chargeTexture(textureChateau, "chateau.png");
    chargeTexture(textureTourClassiqueBase, "tourClassiqueBase.png");
    chargeTexture(textureTourSniperBase, "tourSniperBase.png");
    chargeTexture(textureTourPoisonBase, "tourPoisonBase.png");
    chargeTexture(textureTourClassiqueCanon, "tourClassiqueCanon.png");
    chargeTexture(textureTourSniperCanon, "tourSniperCanon.png");
    chargeTexture(textureEnnemiClassique, "ennemiClassique.png");
    chargeTexture(textureEnnemiRapideSimple, "ennemiRapide.png");
    chargeTexture(textureEnnemiTank, "ennemiTank.png");
    chargeTexture(textureEnnemiVolantSimple, "ennemiVolant.png");
    chargeTexture(textureTirClassique, "tirClassique.png");
    chargeTexture(textureTirSniper, "tirSniper.png");

    for(int i=0;i<NB_CASE_FOND;i++){
        chargeTexture(textureCase[i], "case"+to_string(i)+".png");
    }
    for(int i=0;i<NB_TOUR_FOND;i++){
        chargeTexture(textureTourFond[i], "tourFond"+to_string(i)+".png");
    }
    for(int i=0;i<NB_IMAGE_ENNEMI_VOLANT;i++){
        chargeTexture(textureEnnemiVolant[i], "ennemiVolant"+to_string(i)+".png");
    }
    for(int i=0;i<NB_IMAGE_ENNEMI_RAPIDE;i++){
        chargeTexture(textureEnnemiRapide[i], "ennemiRapide"+to_string(i)+".png");
    }
    for(int i=0;i<TEMPS_ANIM_TIR;i++){
        chargeTexture(textureExplosion[i], "explosion"+to_string(i)+".png");
    }

    return 1;
}

int main(int argc, char **argv) // Boucle principale avec appel de chaque fonction jusqu'à la fermeture du jeu
{
    initSDL();
    initListeCase();
    debut();
    quitListeCase();
    infos();
    SDL_DestroyWindow(fenetre);
    TTF_Quit();
    SDL_Quit();
    return 0;
}

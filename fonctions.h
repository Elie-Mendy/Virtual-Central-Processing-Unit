


/*______________________________________________________________________________

                    DEFINITION FONCTIONS - OUTILS DU MAIN 
______________________________________________________________________________*/

/*  fonction: usage()
    objectif: impression de messages d'erreur (sur flux stderr)
    parametres: une string (le messages à renvoyer)*/
void usage(str message);


/*  fonction: stepper()
    objectif: permettre a l'utilisateur d'utiliser la touche 'enter
              pour passer une instruction */
void stepper() ;


/*  fonction: chargerBootstrap()   
    objectif: 
      - li le fichier "bootstrap"
      - le charge en memoire
*/
void chargerBootstrap();


/*  fonction: afficherMemoire()   
    objectif: 
      - afficher la memoire 
*/
void afficherMemoire();


/*  fonction: afficherMemoire()   
    objectif: 
      - afficher le programme
*/
void afficherProgramme();

/*  fonction: initialiserRegistres()
    objectif: attribuer une valeur initiale aux différents registres */
void initialiserRegistres();


/*  fonction: lireProgramme()
    objectif: 
      - prend le nom d'un programme à lire 
      - stoque les instructions du programme dans le vecteur 'programme'
    parametres: 
      - une string (le nom du programme à lire)*/
int lireProgramme(str programme);


/*  fonction: chargerProgramme()
    objectif: 
      - ecrit le programme dans la mémoire 
      - sans passer par le bootstrap manuel
    parametres: 
      - un entier (la taille  du programme )*/
void chargerProgramme(int taille);


/*  fonction: executer()
    objectif: 
      - prend en compte le microcode entré en parametre 
      - appel la fonction associé (voir page 224)
    parametres: 
      - un entier (le code associé a une fonction)*/
void executer(int code);

/*______________________________________________________________________________

                        DEFINITION FONCTIONS - MICROCODES 
______________________________________________________________________________*/




/*  fonction: transfert()
    objectif: 
      - prend en compte le microcode entré en parametre 
      - effectur le transfert mémoire associé (voir page 227)
    parametres: 
      - un entier (le microcode)*/
void transfert(int code);


/*  fonction: prepaCalcul()
    objectif: 
      - prend en compte le microcode entré en parametre 
      - préparer le calcul associé (voir page 227)
        (sauvegarde le microcode du calcul dans UAL)
    parametres: 
      - un entier (le calcul demandé)*/
void prepaCalcul(int code);


/*  fonction: hexaToInt()
    objectif: 
      - traduit un hexadecimal en int
    parametres: 
      - un hexa (l'operande a traduire)
    retour: 
      - un entier */
int hexaToInt(Hexa h[TAILLE_ADRESSE]);


/*  fonction: intTostr()
    objectif: 
      - traduit un int en str
    parametres: 
      - un entier (l'operande a traduire)*/
void intToStr(Hexa * registre , int code );

/*  fonction: intToHexa()
    objectif: 
      - traduit un hexadecimal en int
    parametres: 
      - un entier (l'operande a traduire)
    retour: 
      - un hexadecimal */
void intToHexa(Hexa * registre , int code );

/*  fonction: calcul()
    objectif: 
      - traduit les deux operandes en entiers 
      - effectue le calcul associé (voir page 227)
        (sauvegarde le resultat du calcul dans A)
    parametres: 
      - un entier (le calcul demandé)*/
void calcul();


/*  fonction: lireMemoire()
    objectif: 
      - decoder l'adresse contenu dans RS
      - reporter le contenu de la mémoire dans RM
    parametres: 
      - un hexa (l'adresse a decoder)*/
void lireMemoire();


/*  fonction: ecrire()
    objectif: 
      - decoder l'adresse contenu dans RS
      - reporter le contenu de la mémoire dans RM
    parametres: 
      - un hexa (l'adresse a decoder)*/
void ecrire();

/*  fonction: saisir()
    objectif: 
      - saisir une donnée dans IN*/
void saisir();


/*  fonction: saisir()
    objectif: 
      - supprime le saut de ligne en fin de saisie user 
    retour
      - 0 si il est impossible de lire la saisie
      - 1 si la saisie est reussi et lu*/ 
int lire(char * mot, int taille);




/*______________________________________________________________________________

                        DEFINITION FONCTIONS - CYCLE 
______________________________________________________________________________*/


/*  fonction: phase1()
    objectif: 
      - rechercher une instruction
      - la stoquer dans OP*/
void phase1();

/*  fonction: phase3()
    objectif: 
      - increpenter PC*/
void phase3();

/*  fonction: incrementerPC()
    objectif: 
      - increpenter PC*/
void incrementerPC();
/*______________________________________________________________________________

                        DEFINITION FONCTIONS - OPERATIONS 
______________________________________________________________________________*/


// _____________ ARITHMETIQUE ________________ 

//
// additions 
//

/*  fonction: addValeur()   20
    description: 
      ADD# -->  A ← A+V
*/
void addValeur();

/*  fonction: addValeur()   60
    description: 
      ADDα -->  A ← A + (α)
*/
void addValeurP();

/*  fonction: addValeur()   E0
    description: 
      ADD*α -->  A ← A + *(α)
*/
void addValeurPP();

//
// soustractions 
//

/*  fonction: subValeur()   21
    description: 
      SUB# -->  A ← A-V
*/
void subValeur();

/*  fonction: subValeurP()   61
    description: 
      SUBα -->  A ← A - (α)
*/
void subValeurP();

/*  fonction: subValeurPP()   E1
    description: 
      SUB*α -->  A ← A - *(α)
*/
void subValeurPP();



// LOGIQUES

/*  fonction: nand()   22
    description: 
      NAND# -->  A ← ¬[A&V]
*/
void nand();

/*  fonction: nandP()   62
    description: 
      NANDα -->  A ← ¬[A&(α)]
*/
void nandP();

/*  fonction: nandPP()   E2
    description: 
      NAND*α -->  A ← ¬[A& ∗(α)]
*/
void nandPP();


// TRANSFERTS


/*  fonction: load()   00
    description: 
      LOAD# -->  A ← V
*/
void load();

/*  fonction: loadP()   40
    description: 
      LOADα -->  A ← (α)
*/
void loadP();

/*  fonction: loadPP()   C0
    description: 
      LOAD*α -->  A ← ∗(α)
*/
void loadPP();


/*  fonction: storeP()   48
    description: 
      STOREα -->  (α) ← A
*/
void storeP();

/*  fonction: storePP()   C8
    description: 
      LOAD*α -->  A ← ∗(α)
*/
void storePP();


// ENTREES / SORTIES 

/*  fonction: inP()   49
    description: 
      INα -->  (α) ← Entrée
*/
void inP();

/*  fonction: inPP()   C9
    description: 
      IN*α -->  ∗(α) ← Entrée
*/
void inPP();

/*  fonction: outP()   41
    description: 
      OUTα -->  Sortie ← (α)
*/
void outP();

/*  fonction: outPP()   C1
    description: 
      OUT*α -->  Sortie ← ∗(α)
*/
void outPP();

// BRANCHEMENT INCONDITIONNEL (saut de la phase 3)

/*  fonction: jump()   10
    description: 
      JUMP α -->  PC ← α
*/
void jump();

// BRANCHEMENT CONDITIONNEL (saut de la phase 3)

/*  fonction: brn()   11
    description: 
      BRN α -->  si A < 0 alors PC ← α
*/
void brn();


/*  fonction: brz()   12
    description: 
      BRN α -->  si A < 0 alors PC ← α
*/
void brz();






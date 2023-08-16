/*
  NOM: MENDY
  Prenom : Elie
  n°etudiant : 19004664

  EXERCICE cx17.8
*/


/********************************************************
# Nom ......... : ordinateur en papier
# Rôle ........ : emuler le fonctionnement d'un ordinateur
                  repris au chapitre 10 du courd d'architecture des ordinateurs
# Auteur ...... : MENDY Elie
# Version ..... : V0.1 du 08/06/2021
# Compilation : gcc -Wall -c ordinateur_en_papier.c -o op                                        
# Usage : Pour exécuter : ./op <nom du programme a executer>
********************************************************/

/*______________________________________________________________________________

                    INCLUDES - TYPEDEFS - DEFINES 
______________________________________________________________________________*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

//#define DEBUG                                // debug général
//#define DEBUG_n2                             // debug plus pointu
//#define STEPPER                              // possibilitée d'executer chaques instruction via la touche 'enter'
#define AUTO                                 // chargement automatique du programme en mémoire
//#define EXO                                  // contexte de l'exercice 10.6 
#define TAILLE_MEMOIRE 256                   // nombre d'adresses disponible en memoire
#define TAILLE_ADRESSE 3                     // taille de la chaine representative d'une adresse 
#define TAILLE_BOOTSTRAP 32                  // combre d'instruction que contient le bootstrap
#define TAILLE_MAX_PROGRAMME 200             // combre de lignes maximum que peut contenir le programme - mode automatique
#define FIRST_INSTRUCTION "28"                 // adresse de la première instruction (en hexadécimal) - mode automatique
#define FIRST_INSTRUCTION_EXO_10_6 "50"                 // adresse de la première instruction (en hexadécimal) - mode automatique

typedef char Hexa ;                          // definition du type Hexadecimal 
typedef char * str ;                         // definition du type str 
typedef unsigned idx;                        // definition du type str 
// Allocation d'espace pour:
// - la memoire
Hexa * memoire[TAILLE_MEMOIRE];       // --> la barette memoire (ressource de l'ordinateur)

// -les registres
Hexa RS[TAILLE_ADRESSE];                     // --> registre de Selection
Hexa RM[TAILLE_ADRESSE];                     // --> registre de Mot

Hexa IN[TAILLE_ADRESSE];                     // --> peripherique Entrée
Hexa OUT[TAILLE_ADRESSE];                    // --> peripherique Sortie (affichage)

Hexa PC[TAILLE_ADRESSE];                     // --> registre Program Counter

Hexa OP[TAILLE_ADRESSE];                     // --> registre de Registre Instruction (code opératoire)
Hexa AD[TAILLE_ADRESSE];                     // --> registre de Registre Instruction (adresse de la donnée à traiter)

Hexa A[TAILLE_ADRESSE];                      // --> registre Accumulateur

// - l'unité de calcul        
int UAL;                    // --> registre d'indication du calcul a effectuer


// - le programme à charger en memoire 
Hexa * prog[TAILLE_MAX_PROGRAMME] = { NULL };

// import des headers des fonctions relative 
// au trois phases du cycle d'execution d'une instruction
#include "fonctions.h"                                 


/*______________________________________________________________________________

                    EMULATION DE L'ORDINATEUR EN PAPIER (main) 
______________________________________________________________________________*/
#ifdef AUTO
  int main(int k, char  *argv[]) {
    // test du nombre d'arguments
    if (k < 2) usage("veuillez indiquer le nom du programme a lire");

    printf("\n\n____________________________________________________________________\n");
    printf(" Emulateur de l'Ordinateur En Papier \n");
    printf("____________________________________________________________________\n\n"); 

    // initialisation de la mémoire 
    initialiserRegistres();
    chargerBootstrap();
    //afficherMemoire(55);

    // chargement du programme
    int taille = lireProgramme(argv[1]);
    //afficherProgramme();
    chargerProgramme(taille);
    //afficherMemoire(250);
    

    
    #ifdef EXO
      Hexa fin[TAILLE_ADRESSE] = "6E";
    #else
      Hexa fin[TAILLE_ADRESSE] = "00";
    #endif 


    do{
      #ifdef DEBUG 
        printf("____________________________________________________________________\n"); 
      #endif 
      phase1();         // recherche d'instruction
      executer(hexaToInt(OP));  
      #ifdef DEBUG 
        printf("____________________________________________________________________\n");
        printf("Etat des Registres:\n");
        printf("- PC=%s \t AD=%s \t OP=%s / %i\n" , PC, AD, OP , hexaToInt(OP));
        printf("- RS=%s \t RM=%s \t  A=%s\n" , RS, RM , A);
        printf("- UAL=%i \t IN=%s \t OUT=%s\n" , UAL, IN , OUT);
        printf("____________________________________________________________________\n\n\n");  
      #endif 
      #ifdef STEPPER
      stepper();
      #endif
    } while (strcasecmp(PC , fin));  // fin du programme quand on jump sur 00
    return 0;
  }
#else
  int main(int k, char  *argv[]) {
    initialiserRegistres();
    chargerBootstrap();
    do{
      phase1();         // recherche d'instruction
      executer(hexaToInt(OP));  

    } while (strcasecmp(PC , "00"));  // fin du programme quand on jump sur 00 
    return 0;
  }
#endif



/*______________________________________________________________________________

                    DEFINITION FONCTIONS - OUTILS DU MAIN 
______________________________________________________________________________*/

/*  fonction: usage()
    objectif: impression de messages d'erreur (sur flux stderr)
    parametres: une string (le messages à renvoyer)*/
void usage(str message) { fprintf(stderr, "Usage : %s\n", message), exit(1) ;}


/*  fonction: stepper()
    objectif: permettre a l'utilisateur d'utiliser la touche 'enter
              pour passer une instruction */
void stepper() {
  printf("('Enter' pour passer à l'instruction suivante)");
  char c;
  while((c =getchar()) == '\0');
}



/*  fonction: afficherMemoire()   
    objectif: 
      - afficher la memoire 
*/
void afficherMemoire(int adresse) {
  #ifdef DEBUG 
    printf("- affichage de la memoire \n");  
  #endif 
  for (int i = 0; i < adresse && i < TAILLE_MEMOIRE ; i++) {
    printf("adresse %i\t--> b16: %x:\t%s\n", i, i, memoire[i]);
  }
}


/*  fonction: afficherProgramme()   
    objectif: 
      - afficher le programme
*/
void afficherProgramme() {
  #ifdef DEBUG 
    printf("- affichage du programme \n");  
  #endif 
  for (int i = 0; prog[i] ; i++) {
    printf("instruction : %s\n",  prog[i]);
  }
}


/*  fonction: initialiserRegistres()
    objectif: attribuer une valeur initiale aux différents registres */
void initialiserRegistres(){
  #ifdef DEBUG 
    printf("- initialisation des registres  \n"); 
  #endif 
 
  // registre memoire
  strcpy(RS,"00");
  strcpy(RM,"15");

  // Entrées / sorties
  strcpy(IN,"00");
  strcpy(OUT,"00");

  // Program Counter
  #ifdef EXO
    strcpy(PC,FIRST_INSTRUCTION_EXO_10_6);
  #else
    strcpy(PC,FIRST_INSTRUCTION);
  #endif
  // Registre Instruction
  strcpy(OP,"00");
  strcpy(AD,"00");

  // unité de calcul
  strcpy(A,"10");
  UAL = 0;
}

/*  fonction: lireProgramme()
    objectif: 
      - prend le nom d'un programme à lire 
      - stoque les instructions du programme dans le vecteur 'programme'
    parametres: 
      - une string (le nom du programme à lire)*/
int lireProgramme(str programme) {
  // ouverture du flux
  FILE * fichier = fopen(programme, "r");
  if (! fichier) usage(" stoplist illisible");

  int i = 0;
  char lu = '\0';
  while (i < TAILLE_MAX_PROGRAMME && lu != EOF){
    char sas[TAILLE_ADRESSE];              // sas de reception du mot
    lu = fscanf(fichier, "%s ", sas);
    if (lu != EOF){
      prog[i++] = strdup(sas);
      }
    }
  fclose(fichier);
  return i -1;
}


/*  fonction: chargerBootstrap()   
    objectif: 
      - li le fichier "bootstrap"
      - le charge en memoire
*/
void chargerBootstrap(){  
  #ifdef DEBUG 
    printf("- chargement du bootstrap \n");  
  #endif 
  // ouverture du flux
  FILE * fichier = fopen("bootstrap", "r");
  if (! fichier) usage("bootstrap illisible");

  idx i = 0;
  char lu = '\0';
  while (i < TAILLE_MEMOIRE && lu != EOF){
    char sas[TAILLE_ADRESSE];               // sas de reception du mot
    lu = fscanf(fichier, "%s ", sas);
    if (lu != EOF){
      memoire[i++] = strdup(sas);
      }
    }

  // fermeture du flux
  fclose(fichier);
};


/*  fonction: chargerProgramme()
    objectif: 
      - ecrit le programme dans la mémoire 
      - sans passer par le bootstrap manuel
    parametres: 
      - un entier (l'adresse de la première instruction du programme')*/
void chargerProgramme(int t) {
  #ifdef DEBUG 
    printf("- chargement du programme\n"); 
  #endif 
  //int adresse = hexaToInt(RS) + 40;
  #ifdef EXO
    int adresse = hexaToInt(FIRST_INSTRUCTION_EXO_10_6);
  #else
    int adresse = hexaToInt(FIRST_INSTRUCTION);
  #endif
  for (int i = 0; i < t ; i ++){
    memoire[adresse + i] = strdup(prog[i]);
  }
  // ecriture de l'emplacement de la première instruction
  #ifdef EXO
    memoire[32] = strdup(FIRST_INSTRUCTION_EXO_10_6);
  #else
    memoire[32] = strdup(FIRST_INSTRUCTION);
  #endif
  // ecriture de la taille de la mémoire
  Hexa taille[TAILLE_ADRESSE];
  intToStr(taille, t);
  strcpy(memoire[34], taille); 
}



/*  fonction: executer()
    objectif: 
      - prend en compte le microcode entré en parametre 
      - appel la fonction associé (voir page 224)
    parametres: 
      - un entier (le code associé a une fonction)*/
void executer(int code) {
  #ifdef DEBUG 
    printf("==> phase 2 : exec code -> %i \n", hexaToInt(OP)); 
  #endif 
  switch (code)
  {
    //
    // ARITHMETIQUE
    //
    // ADD #  20
    case 32:  
      addValeur();
      break;

    // ADD α  60
    case 96:
      addValeurP();
      break;

    // ADD *α E0
    case 224:
      addValeurPP();
      break;
    
      // SUB #  21
    case 33:  
      subValeur();
      break;

    // SUB α  61
    case 97:
      subValeurP();
      break;

    // SUB *α E1
    case 225:
      subValeurPP();
      break;
    

    //
    // LOGIQUE
    //
    // NAND #  22
    case 34:  
      nand();
      break;

    // NAND α  62
    case 98:
      nandP();
      break;

    // NAND *α E2
    case 226:
      nandPP();
      break;


    //
    //  TRANSFERTS
    //
    // LOAD #  00
    case 0:  
      load();
      break;

    // LOAD α  40
    case 64:
      loadP();
      break;

    // LOAD *α C0
    case 192:
      loadPP();
      break;

    // STORE α  48
    case 72:
      storeP();
      break;

    // STORE *α C8
    case 200:
      storePP();
      break;

    //
    // ENTREE / SORTIES
    //
    // IN α  49
    case 73:
      inP();
      break;

    // IN *α C9
    case 201:
      inPP();
      break;

    // OUT α 41
    case 65:
      outP();
      break;

    // OUT *α C1
    case 193:
      outPP();
      break;

    // 
    // BRANCHEMENT INCONDITIONNEL
    //
    // JUMP α 10
    case 16:
      jump();
      break;

    // 
    // BRANCHEMENT CONDITIONNEL
    //
    // BRN α 11
    case 17:
      brn();
      break;
    
    // BRN α 12
    case 18:
      brz();
      break;

    default:
      exit(1);
      break;
  }
}



/*______________________________________________________________________________

                        DEFINITION FONCTIONS - MICROCODES 
______________________________________________________________________________*/



/*  fonction: transfert()
    objectif: 
      - prend en compte le microcode entré en parametre 
      - effectur le transfert mémoire associé (voir page 227)
    parametres: 
      - un entier (le microcode)*/
void transfert(int code){ 
  #ifdef DEBUG_n2 
    printf("- transfert -> %i \n", code);   
  #endif                
  
  switch (code)
  {
  case 1:
    /* (RS) ←(PC) */
    strcpy(RS,PC);
    break;
  case 2:
    /* (PC) ←(RM) ; ne pas faire la phase III. */
    strcpy(PC,RM);
    break;
  case 3:
    /* (A) ←(RM) */
    strcpy(A,RM);
    break;
  case 4:
    /* (RM) ←(A) */
    strcpy(RM,A);
    break;
  case 5:
    /* (OP) ←(RM) */
    strcpy(OP,RM);
    break;
  case 6:
    /* (AD) ←(RM) */
    strcpy(AD,RM);
    break;
  case 7:
    /* (RS) ←(AD) */
    strcpy(RS,AD);
    break;
  case 8:
    /* (RM) ←(Entrée) */
    strcpy(RM,IN);
    break;
  case 9:
    /* (Sortie) ←(RM) */
    strcpy(OUT,RM);
    break;
  
  default:
    usage("Erreur - fonction transfert() : le microcode ne correspond pas a un transfert ou n'existe pas.");
    break;
  }
}


/*  fonction: prepaCalcul()
    objectif: 
      - prend en compte le microcode entré en parametre 
      - préparer le calcul associé (voir page 227)
        (sauvegarde le microcode du calcul dans UAL)
    parametres: 
      - un entier (le calcul demandé)*/
void prepaCalcul(int code){  
  #ifdef DEBUG 
    printf("- preparation calcul:  %i \n", code); 
  #endif 
               
  UAL = code;
}

/*  fonction: hexaToInt()
    objectif: 
      - traduit un hexadecimal en int
    parametres: 
      - un hexa (l'operande a traduire)
    retour: 
      - un entier */
int hexaToInt(Hexa h[TAILLE_ADRESSE] ){ 
  #ifdef DEBUG_n2 
    int i = strtol(h, NULL, 16);
    printf("   - convertion hexa: %s  --> ToInt : %i\n", h, i);
  #endif 
  
  return strtol(h, NULL, 16);   
}

/*  fonction: intToHexa()
    objectif: 
      - traduit un int en hexadecimal
    parametres: 
      - un entier (l'operande a traduire)*/
void intToHexa(Hexa * registre , int code ){ 
  Hexa value[TAILLE_ADRESSE];
  sprintf(value, "%x", code);
  
  // gestion des valeurs < 10
  if (strlen(value) < 2) {
    value[1] = value[0];
    value[0] = '0';
  }

  // attribution de la nouvelle valeur au registre
  strcpy(registre, value);
  #ifdef DEBUG_n2 
    printf("   - convertion int: %i  --> ToHexa : %s\n", code, registre); 
  #endif 
  
}

/*  fonction: intTostr()
    objectif: 
      - traduit un int en str
    parametres: 
      - un entier (l'operande a traduire)*/
void intToStr(Hexa * registre , int code ){ 
  sprintf(registre, "%d", code);
  #ifdef DEBUG_n2 
    printf("   - convertion int: %i  --> ToStr : %s\n", code, registre);
  #endif
}



/*  fonction: calcul()
    objectif: 
      - traduit les deux operandes en entiers 
      - effectue le calcul associé (voir page 227)
        (sauvegarde le resultat du calcul dans A)
    parametres: 
      - un entier (le calcul demandé)*/
void calcul(){   
  // recuperation des operandes 
  int a = strtol(A, NULL, 10);   
  int rm = strtol(RM, NULL, 10); 
  
  // calcul       
  switch (UAL) {
    /* addition */
    case 10:
      a = a + rm;
      #ifdef DEBUG 
        printf("-calcul operandes %i + %i \n",a, rm);
      #endif
      
      break;

    /* soustraction */
    case 11:
      a = a - rm;
      #ifdef DEBUG 
        printf("-calcul operandes %i - %i \n",a, rm);
      #endif
      break;
    
    /* logique -> NAND */
    case 17:
      if (!(a > 0 && rm > 0)){
        a = 1;
        #ifdef DEBUG 
          printf("-calcul operandes ~(%i ET %i) --> T \n",a, rm);
        #endif
      } else {
        a = 0;
      }
      break;
    default:
      usage("Erreur- fonction calcul() : le microcode ne correspond pas a un opération ou n'existe pas.");
      break;
  }

  // sauvegarde du resultat dans l'accumulateur
  intToStr(A, a);
}


/*  fonction: lireMemoire()
    objectif: 
      - decoder l'adresse contenu dans RS
      - reporter le contenu de la mémoire dans RM
    parametres: 
      - un hexa (l'adresse a decoder)*/
void lireMemoire(){ 
  strcpy(RM, memoire[hexaToInt(RS)]);
  #ifdef DEBUG_n2 
    printf("- lecture de la memoire memoire[%s]=%s\n",RS,RM);
  #endif
  
};


/*  fonction: ecrire()
    objectif: 
      - decoder l'adresse contenu dans RS
      - reporter le contenu de la mémoire dans RM
    parametres: 
      - un hexa (l'adresse a decoder)*/
void ecrire(){   
  int idx = hexaToInt(RS);
  #ifdef DEBUG_n2 
    printf("- ecriture valeur: %s dans la case mémoire %i\n",RM, idx);
  #endif
  
  memoire[idx] = strdup(RM);
};

/*  fonction: saisir()
    objectif: 
      - saisir une donnée dans IN*/
void saisir(){  
  #ifdef DEBUG_n2 
    printf("- saisie d'une valeur \n"); 
  #endif
  printf("\n\tVeuillez saisir une donnée  : ");
  lire(IN, TAILLE_ADRESSE);
};


/*  fonction: lire()
    objectif: 
      - supprime le saut de ligne en fin de saisie user 
    retour
      - 0 si il est impossible de lire la saisie
      - 1 si la saisie est reussi et lu*/ 
int lire(char * mot, int taille){
  if (fgets(mot, taille, stdin)){
    char * monPointeur = strchr(mot,'\n');
    if (monPointeur)
      *monPointeur = '\0';

    while(getchar() != '\n' && getchar() != EOF);
    return 1;
  }
  while(getchar() != '\n' && getchar() != EOF);
  return 0; // si impossible de lire la saisie
}




/*______________________________________________________________________________

                        DEFINITION FONCTIONS - CYCLE 
______________________________________________________________________________*/


/*  fonction: phase1()
    objectif: 
      - rechercher une instruction
      - la stoquer dans OP*/
void phase1(){   
  #ifdef DEBUG
    printf("==> phase 1 \n");
  #endif
  transfert(1);     // 1 
  lireMemoire();    // 13
  transfert(5);     // 5
  incrementerPC();  // 15
};

/*  fonction: phase3()
    objectif: 
      - increpenter PC*/
void phase3(){ 
  #ifdef DEBUG 
    printf("\n==> phase 3 \n");
  #endif  
  incrementerPC();  // 15
};

/*  fonction: incrementerPC()
    objectif: 
      - increpenter PC*/
void incrementerPC(){ 
  #ifdef DEBUG_n2
    printf("    - incrémentation de PC\n");
  #endif  
  int pc = hexaToInt(PC);
  pc ++;
  
  intToHexa(PC , pc);
};

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
void addValeur(){  
  #ifdef DEBUG 
    printf("\n\t[ ADD # ]\n");
  #endif 
  // phase 2
  prepaCalcul(10);   // 10
  transfert(1);      // 1
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};

/*  fonction: addValeur()   60
    description: 
      ADDα -->  A ← A + (α)
*/
void addValeurP(){   
  #ifdef DEBUG 
    printf("\n\t[ ADD α ]\n");
  #endif 
  // phase 2
  prepaCalcul(10);  // 10
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};

/*  fonction: addValeur()   E0
    description: 
      ADD*α -->  A ← A + *(α)
*/
void addValeurPP(){ 
  #ifdef DEBUG 
    printf("\n\t[ ADD * α ]\n");
  #endif   
  // phase 2
  prepaCalcul(10);   // 10
  transfert(1);      // 1
  lireMemoire();    // 13
  transfert(6);      // 6
  transfert(7);     // 7
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();    // 13
  calcul();          // 12
  // phase 3
  phase3();
};

//
// soustractions 
//

/*  fonction: subValeur()   21
    description: 
      SUB# -->  A ← A-V
*/
void subValeur(){ 
  #ifdef DEBUG 
    printf("\n\t[ SUB # ]\n");
  #endif  
  // phase 2
  prepaCalcul(11);   // 11
  transfert(1);      // 1
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};

/*  fonction: subValeurP()   61
    description: 
      SUBα -->  A ← A - (α)
*/
void subValeurP(){ 
  #ifdef DEBUG 
    printf("\n\t[ SUB α ]\n");
  #endif  
  // phase 2
  prepaCalcul(11);   // 11
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};

/*  fonction: subValeurPP()   E1
    description: 
      SUB*α -->  A ← A - *(α)
*/
void subValeurPP(){ 
  #ifdef DEBUG 
    printf("\n\t[ SUB * α ]\n");
  #endif  
  // phase 2
  prepaCalcul(11);   // 11
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};



// LOGIQUES

/*  fonction: nand()   22
    description: 
      NAND# -->  A ← ¬[A&V]
*/
void nand(){ 
  #ifdef DEBUG 
    printf("\n\t[ NAND # ]\n"); 
  #endif   
  // phase 2
  prepaCalcul(17);   // 17
  transfert(1);      // 1
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};

/*  fonction: nandP()   62
    description: 
      NANDα -->  A ← ¬[A&(α)]
*/
void nandP(){
  #ifdef DEBUG 
    printf("\n\t[ NAND α ]\n"); 
  #endif 
  // phase 2
  prepaCalcul(17);   // 17
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};

/*  fonction: nandPP()   E2
    description: 
      NAND*α -->  A ← ¬[A& ∗(α)]
*/
void nandPP(){ 
  #ifdef DEBUG 
    printf("\n\t[ NAND * α ]\n"); 
  #endif 
  // phase 2
  prepaCalcul(17);   // 17
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  calcul();          // 12
  // phase 3
  phase3();
};


// TRANSFERTS


/*  fonction: load()   00
    description: 
      LOAD# -->  A ← V
*/
void load(){ 
  #ifdef DEBUG 
    printf("\n\t[ LOAD # ]\n"); 
  #endif  
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(3);      // 3
  // phase 3
  phase3();
};

/*  fonction: loadP()   40
    description: 
      LOADα -->  A ← (α)
*/
void loadP(){ 
  #ifdef DEBUG 
    printf("\n\t[ LOAD α ]\n"); 
  #endif  
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(3);      // 3
  // phase 3
  phase3();
};

/*  fonction: loadPP()   C0
    description: 
      LOAD*α -->  A ← ∗(α)
*/
void loadPP(){  
  #ifdef DEBUG 
    printf("\n\t[ LOAD * α ]\n"); 
  #endif 
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(3);      // 3
  // phase 3
  phase3();
};


/*  fonction: storeP()   48
    description: 
      STOREα -->  (α) ← A
*/
void storeP(){   
  #ifdef DEBUG 
    printf("\n\t[ STORE α ]\n"); 
  #endif 
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(4);      // 4
  ecrire();          // 14
  // phase 3
  phase3();
};

/*  fonction: storePP()   C8
    description: 
      LOAD*α -->  A ← ∗(α)
*/
void storePP(){   
  #ifdef DEBUG 
    printf("\n\t[ STORE * α ]\n"); 
  #endif 
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  transfert(4);      // 4
  ecrire();          // 14
  // phase 3
  phase3();
};


// ENTREES / SORTIES 

/*  fonction: inP()   49
    description: 
      INα -->  (α) ← Entrée
*/
void inP(){  
  #ifdef DEBUG 
    printf("\n\t[ IN α ]\n"); 
  #endif  
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  saisir();          // 16
  transfert(8);      // 8
  ecrire();          // 14
  // phase 3
  phase3();
};

/*  fonction: inPP()   C9
    description: 
      IN*α -->  ∗(α) ← Entrée
*/
void inPP(){   
  #ifdef DEBUG 
    printf("\n\t[ IN * α ]\n"); 
  #endif 
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  saisir();          // 16
  transfert(8);      // 8
  ecrire();          // 14
  // phase 3
  phase3();
};

/*  fonction: outP()   41
    description: 
      OUTα -->  Sortie ← (α)
*/
void outP(){ 
  #ifdef DEBUG 
    printf("\n\t[ OUT α ]\n"); 
  #endif 
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(9);      // 9
  printf("\n\n\t\t Affichage OUT:  %s\n", OUT);
  // phase 3
  phase3();
};

/*  fonction: outPP()   C1
    description: 
      OUT*α -->  Sortie ← ∗(α)
*/
void outPP(){   
  #ifdef DEBUG 
    printf("\n\t[ OUT * α ]\n"); 
  #endif 
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(6);      // 6
  transfert(7);      // 7
  lireMemoire();     // 13
  transfert(9);      // 9
  printf("--> Affichage OUT:  %s\n", OUT);
  // phase 3
  phase3();
};

// BRANCHEMENT INCONDITIONNEL (saut de la phase 3)

/*  fonction: jump()   10
    description: 
      JUMP α -->  PC ← α
*/
void jump(){
  #ifdef DEBUG 
    printf("\n\t[ JUMP α ]\n"); 
  #endif    
  // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(2);      // 2
};

// BRANCHEMENT CONDITIONNEL (saut de la phase 3)

/*  fonction: brn()   11
    description: 
      BRN α -->  si A < 0 alors PC ← α
*/
void brn(){   
  #ifdef DEBUG 
    printf("\n\t[ si A < 0 : BRN α ]\n"); 
  #endif 
  // condition
  if (hexaToInt(A) < 0) {
    // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(2);      // 2
  } else {
    phase3();
  }
};


/*  fonction: brz()   12
    description: 
      BRN α -->  si A < 0 alors PC ← α
*/
void brz(){   
  #ifdef DEBUG 
    printf("\n\t[ si A = 0 : BRZ α ]\n"); 
  #endif 
  // condition
  if (hexaToInt(A) == 0) {
    // phase 2
  transfert(1);      // 1
  lireMemoire();     // 13
  transfert(2);      // 2
  } else {
    phase3();
  }
};


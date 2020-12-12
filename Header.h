#include <iostream>
#include <string.h>

using namespace std;

int const nmax = 300;
int const ntmax = 100;
int const infini = INT_MAX;

typedef struct T_instance{
	int n;								// Nombre de client
	int nt;								// Nombre de vehicules/tournees
	double d[nmax+1][nmax+1];			// Distance entre les clients
	double D[nmax + 1];					// Demande de chaque clients
	double Capa[ntmax + 1];				// Capacite du camion de chaque type
	double Cv[ntmax + 1];				// Cout variable
	double Cf[ntmax + 1];				// Cout fixe
	int ntc[ntmax];						// Nbr de camion pour chaque type
}T_instance;

typedef struct T_tournee {
	int nc;								// Num de client
	int list_clt[nmax + 2];			// Liste des clients
	double Vol;							// Volume 
	int ntc;							// Num de camion
	double cout;						// Cout 
}T_tournee;

typedef struct T_solution {
	int nt;								// 
	T_tournee tours[];					//
}T_solution;


void LireInstance(string file, T_instance& inst);
void afficherTournee(T_instance inst, T_tournee& tournee);

//Solution initiale
void plusProcheVoisin(T_instance inst, T_tournee& tournee);
void plusProcheVoisinRand(T_instance inst, T_tournee& tournee, int v);
void heuristiqueChoisi(T_instance inst, T_tournee& tournee, int nbVoisin, int voisinChoisi);

// Recherche locale
void deuxOPT(T_instance inst, T_tournee& tournee, int iter);
void deplacerSommet(T_instance inst, T_tournee& tournee);
void genererVoisinSwap(T_instance inst, T_tournee& tournee, T_tournee& voisin);
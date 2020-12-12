#include <iostream>
#include <string.h>
#include <fstream>
#include <cstdlib>
#include<tuple>
#include <algorithm> 
#include <time.h>
#include "Header.h"

using namespace std;

void LireInstance(string file, T_instance& inst) {
	ifstream flux(file);
	int i, j, tmp;

	if (flux) {

		flux >> inst.n;
		flux >> inst.nt;
		for (i = 1; i <= inst.nt; i++) {
			flux >> inst.ntc[i];
			flux >> inst.Capa[i];
			flux >> inst.Cf[i];
			flux >> inst.Cv[i];
		}
		flux >> tmp;
		for (i = 0; i <= inst.n ; i++) {
			for (j = 0; j <= inst.n ; j++) {
				flux >> inst.d[i][j];
			}
		}
		for (i = 1; i <= inst.n; i++) {
			flux >> tmp;
			flux >> inst.D[i];
		}
	}
}

void plusProcheVoisin(T_instance inst, T_tournee& tournee) {
	int			i, j, tmp, pos, imin, clt, k;
	double		dmin;
	tournee.nc = inst.n;
	tournee.cout = 0;
	k = 0;
	// remplir la liste par tous les clients
	for (j = 0; j <= tournee.nc ; j++) {
		tournee.list_clt[j] = j;
	}

	// ordoner la liste selon le plus proche voisin
	i = 0;
	clt = tournee.list_clt[i];
	while (i < tournee.nc)
	{
		imin = -1;
		dmin = infini; 
		for (j = i + 1; j <= tournee.nc; j++) {
			int t = tournee.list_clt[j];
			if (inst.d[clt][t] < dmin) {
				dmin = inst.d[clt][t];
				imin = t;
			}
		}
		
		pos = -1;
		k = i + 1;
		while (pos == -1 && k <= tournee.nc) {
			if (tournee.list_clt[k] == imin) {
				pos = k;
			}
			k++;
		}
		// updater le cout:
		tournee.cout += inst.d[clt][imin];
		
		// permuter l'elem de dont l'indice est i+1 avec le minimum
		tmp = tournee.list_clt[pos];
		tournee.list_clt[pos] = tournee.list_clt[i + 1];
		tournee.list_clt[i + 1] = tmp;
		i = i + 1;
		clt = tournee.list_clt[i];
	}
	tournee.cout += inst.d[tournee.list_clt[i-1]][tournee.list_clt[i]];
	tournee.cout += inst.d[tournee.list_clt[i]][0];
	tournee.list_clt[i + 1] = 0;
}

void plusProcheVoisinRand(T_instance inst, T_tournee& tournee, int v) {
	int			i, j, k, t, pos, tmp, imin, dmin, current;
	int*		mins = (int*)malloc(sizeof(int) * v);
	bool		found;
	bool		tri;
	tournee.nc = inst.n;
	tournee.cout = 0;

	// remplir la liste par tous les clients
	for (t = 0; t <= tournee.nc; t++) {
		tournee.list_clt[t] = t;
	}

	i = 0;
	while (i < tournee.nc) {
		current = tournee.list_clt[i];
		if (tournee.nc - i < v) {
			v--;
		}
		//remplir mins par les v cases a cote de i
		for (t = 0; t < v; t++) {
			mins[t] = tournee.list_clt[i + t + 1];
		}

		// trier mins
		tri = false;
		k = v;
		while (!tri) {
			tri = true;
			for (t = 0; t < k - 1; t++) {
				if (inst.d[current][mins[t]] > inst.d[current][mins[t + 1]]) {
					tmp = mins[t];
					mins[t] = mins[t + 1];
					mins[t + 1] = tmp;
					tri = false;
				}
			}
			k--;
		}
		
		// determiner les vrais v plus proche voisins de i
		for (t = i + v + 1; t <= tournee.nc; t++) {
			int courant = tournee.list_clt[t];
			pos = -1;
			j = 0;
			found = false;
			while (!found && j < v) {
				if (inst.d[current][courant] < inst.d[current][mins[j]]) {
					pos = j;
					found = true;
				}
				j++;
			}
			// si on a trouver une distance inf stritement a une dans le mins 
			if (pos != -1) {
				for (k = v - 1; k > pos; k--) {
					mins[k] = mins[k - 1];
				}
				mins[pos] = courant;
			}
		}
		
		// choisir au hasard 1 de ces v proche voisins
		int random = rand() % v;
		int voisin = mins[random];
		// trouver la position de random (le voisin choisit) dans list_client
		k = i + 1;
		pos = -1;
		while (pos == -1 && k <= tournee.nc)
		{
			if (tournee.list_clt[k] == voisin) {
				pos = k;
			}
			k++;
		}

		// updater le cout:
		tournee.cout += inst.d[current][voisin];

		// positionner le voisin choisit à cote de i
		tournee.list_clt[pos] = tournee.list_clt[i + 1];
		tournee.list_clt[i + 1] = voisin;
		i++;
	}
	tournee.cout += inst.d[tournee.list_clt[i - 1]][tournee.list_clt[i]];
	tournee.cout += inst.d[tournee.list_clt[i]][0];
	tournee.list_clt[i + 1] = 0;
}

void heuristiqueChoisi(T_instance inst, T_tournee& tournee, int nbVoisin, int voisinChoisi) {
	int			i, j, k, t, pos, tmp, imin, dmin, current;
	bool		found;
	bool		tri;
	tournee.nc = inst.n;
	tournee.cout = 0;

	//On va choisir un voisin parmis les nbVoisin plus proche:
	int* mins = (int*)malloc(sizeof(int) * nbVoisin);

	// remplir la liste par tous les clients
	for (t = 0; t <= tournee.nc; t++) {
		tournee.list_clt[t] = t;
	}

	i = 0;
	while (i < tournee.nc) {
		current = tournee.list_clt[i];
		if (tournee.nc - i < nbVoisin) {
			nbVoisin--;
		}
		//remplir mins par les v cases a cote de i
		for (t = 0; t < nbVoisin; t++) {
			mins[t] = tournee.list_clt[i + t + 1];
		}

		// trier mins
		tri = false;
		k = nbVoisin;
		while (!tri) {
			tri = true;
			for (t = 0; t < k - 1; t++) {
				if (inst.d[current][mins[t]] > inst.d[current][mins[t + 1]]) {
					tmp = mins[t];
					mins[t] = mins[t + 1];
					mins[t + 1] = tmp;
					tri = false;
				}
			}
			k--;
		}

		// determiner les vrais v plus proche voisins de i
		for (t = i + nbVoisin + 1; t <= tournee.nc; t++) {
			int courant = tournee.list_clt[t];
			pos = -1;
			j = 0;
			found = false;
			while (!found && j < nbVoisin) {
				if (inst.d[current][courant] < inst.d[current][mins[j]]) {
					pos = j;
					found = true;
				}
				j++;
			}
			// si on a trouver une distance inf stritement a une dans le mins 
			if (pos != -1) {
				for (k = nbVoisin - 1; k > pos; k--) {
					mins[k] = mins[k - 1];
				}
				mins[pos] = courant;
			}
		}

		// choisir le jéme plus proche voisin (j = voisinChoisi - 1)
		int voisin = mins[voisinChoisi-1];

		// trouver la position du voisin choisi dans list_client
		k = i + 1;
		pos = -1;
		while (pos == -1 && k <= tournee.nc)
		{
			if (tournee.list_clt[k] == voisin) {
				pos = k;
			}
			k++;
		}

		// updater le cout:
		tournee.cout += inst.d[current][voisin];

		// positionner le voisin choisit à cote de i
		tournee.list_clt[pos] = tournee.list_clt[i + 1];
		tournee.list_clt[i + 1] = voisin;
		i++;
	}
	tournee.cout += inst.d[tournee.list_clt[i - 1]][tournee.list_clt[i]];
	tournee.cout += inst.d[tournee.list_clt[i]][0];
	tournee.list_clt[i + 1] = 0;
}

void afficherTournee(T_instance inst, T_tournee& tournee)
{
	cout << "Tournee: \n";
	for (int i = 0; i <= tournee.nc + 1 ; i++)
	{
		cout << tournee.list_clt[i] << "  ";
	}
	cout << endl << "Le cout est:\t" << fixed << tournee.cout << endl;
}



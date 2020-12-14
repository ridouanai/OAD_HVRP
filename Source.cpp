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
	int voisin = mins[voisinChoisi - 1];

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
	for (int i = 0; i <= tournee.nc + 1; i++)
	{
		cout << tournee.list_clt[i] << "  ";
	}
	cout << endl << "Le cout est:\t" << fixed << tournee.cout << endl;
}

void deuxOPT(T_instance inst, T_tournee& tournee, int iter) {

	int n = inst.n;
	int j, i = 0;
	int stop;
	int compteur = 0;
	double diff, tmp;

	while ((i != n - 2) && (compteur < iter) )
	{
		stop = 0;
		j = i + 2;
		while ((j != n) && (stop == 0))
		{
			diff = -inst.d[tournee.list_clt[i]][tournee.list_clt[i + 1]] - inst.d[tournee.list_clt[j]][tournee.list_clt[j + 1]] + inst.d[tournee.list_clt[i + 1]][tournee.list_clt[j + 1]] + inst.d[tournee.list_clt[i]][tournee.list_clt[j]];
			if (diff < 0)
			{
				int k, t;
				for (k = i + 1, t = j; k < t; k++, t--) {
					tmp = tournee.list_clt[k];
					tournee.list_clt[k] = tournee.list_clt[t];
					tournee.list_clt[t] = tmp;
				}
				cout << endl << " " << tournee.list_clt[i + 1] << " => " << tournee.list_clt[j] << endl;
				stop = 1;
				compteur++;
				i = 0;
				tournee.cout += diff;
			}
			j++;
		}
		i++;

	}
}

void deplacerSommet(T_instance inst, T_tournee& tournee) {
	double in1, out1, in2, out2;
	int ipos = 0, jpos = 0;
	int n = inst.n;
	for (int i = 1; i < n - 2; i++) {
		in1 = inst.d[tournee.list_clt[i - 1]][tournee.list_clt[i]] + inst.d[tournee.list_clt[i]][tournee.list_clt[i + 1]];
		out1 = inst.d[tournee.list_clt[i - 1]][tournee.list_clt[i + 1]];
		
		double diffOpt = 0;
		
		for (int j = 1; j < n; j++) 
		{
			if (j != i)
			{
				in2 = inst.d[tournee.list_clt[j]][tournee.list_clt[i]] + inst.d[tournee.list_clt[i]][tournee.list_clt[j + 1]];
				out2 = inst.d[tournee.list_clt[j]][tournee.list_clt[j + 1]];

				double diff = -in1 + out1 + in2 - out2;

				if (diff < diffOpt)
				{
					diffOpt = diff;
					ipos = i;
					jpos = j;
				}
			}
		}
		int k;
		if (jpos > ipos)
		{
			int temp = tournee.list_clt[ipos];
			for (k = ipos; k < jpos; k++)
			{
				tournee.list_clt[k] = tournee.list_clt[k + 1];

			}
			tournee.list_clt[jpos] = temp;
		}
		else {
			int temp = tournee.list_clt[ipos];
			for (k = ipos; k > jpos + 1; k--)
			{
				tournee.list_clt[k] = tournee.list_clt[k - 1];

			}
			tournee.list_clt[jpos + 1] = temp;
		}
		tournee.cout += diffOpt;
	}
}

void genererVoisinSwap(T_instance inst, T_tournee& tournee, T_tournee& voisin)
{
	int n = inst.n;
	voisin = tournee;

	int t1 = rand() % n + 1;
	int t2 = rand() % n + 1;

	while (voisin.list_clt[t1] == voisin.list_clt[t2])
	{
		t2 = rand() % n + 1;
	}

	// mettre à jour le cout 
	double remove1, remove2, add1, add2;
	remove1 = -inst.d[voisin.list_clt[t1 - 1]][voisin.list_clt[t1]] - inst.d[voisin.list_clt[t1]][voisin.list_clt[t1 + 1]];
	remove2 = -inst.d[voisin.list_clt[t2 - 1]][voisin.list_clt[t2]] - inst.d[voisin.list_clt[t2]][voisin.list_clt[t2 + 1]];
	add1 = inst.d[voisin.list_clt[t2 - 1]][voisin.list_clt[t1]] + inst.d[voisin.list_clt[t1]][voisin.list_clt[t2 + 1]];
	add2 = inst.d[voisin.list_clt[t1 - 1]][voisin.list_clt[t2]] + inst.d[voisin.list_clt[t2]][voisin.list_clt[t1 + 1]];
	voisin.cout += remove1 + add1 + remove2 + add2;

	// Faire la permutation
	int tmp = voisin.list_clt[t1];
	voisin.list_clt[t1] = voisin.list_clt[t2];
	voisin.list_clt[t2] = tmp;
	

}

void testerInsertion(T_instance& instance, T_listLabel& labels, T_label& Q, int& res)
{
	res = 1;
	int sameDispo = 1;
	for (int i = 0; i < labels.size; i++)
	{
		for (int j = 1; j <= instance.nt; j++)
		{
			if (labels.list->types[j] != Q.types[j])
			{
				sameDispo = 0;
			}
		}
		if (sameDispo == 1)
		{
			if (labels.list[i].cout > Q.cout)
			{
				// supprimer le label labels.list[i] et ajouter Q donc res reste à 1
			}
			else
			{
				res = 0;
			}
		}
	}
}

void split(T_instance& inst, T_tournee& tournee, T_solution& solution)
{
	int n = inst.n;
	int nc = inst.nt;
	int i, j, l, c, res;
	int stop;
	T_listLabel LL[nmax];
	double mark[nmax], Pere[nmax];
	double cout, load, distance;

	//initialiser les labels du sommet 1: contient un seul label 
	for (i = 1; i <= nc; i++)
	{
		LL[0].list[0].types[i] = inst.ntc[i];
	}
	LL[0].list[0].cout = 0;
	LL[0].list[0].position = 0;
	LL[0].list[0].pere = NULL;
	LL[0].size = 1;


	//initialiser la taille des listes pour les autres sommets
	for (i = 1; i <= n; i++) 
	{
		LL[i].size = 0;
		//mark[i] = infini;
	}

	//la construction du graphe
	for (i = 0; i <= n - 1; i++)
	{	
		j = i + 1;
		stop = 0;
		load = 0;
		while ((j <= n) && (stop == 0))
		{
			//LL[j].size = 0;
			for (c = 1; c <= nc; c++) //Pour chaque type de camion
			{ 
				if (inst.Capa[c] >= inst.D[tournee.list_clt[j]] + load) // capa suffisante
				{
					for (l = 0; l <= LL[i].size; l++) //Pour chaque label
					{
						T_label label = LL[i].list[l];
						T_label Q = label;

						if (label.types[c] > 0)
						{
							//std::cout << endl << "i: " << i << " j: " << j << "size est: " << LL[j].size << endl;
							Q.types[c] = label.types[c] - 1;
							Q.position = l;
							if (j == i + 1) //le premier clt dans la tournee
							{
								distance = inst.d[0][tournee.list_clt[j]] + inst.d[tournee.list_clt[j]][0];
								cout = inst.Cf[c] + inst.Cv[c] * distance;
								load = inst.D[tournee.list_clt[j]];
								Q.cout = cout;
								Q.pere = i;
							}
							else
							{
								distance = -inst.d[tournee.list_clt[j - 1]][0] + inst.d[tournee.list_clt[j - 1]][tournee.list_clt[j]] + inst.d[tournee.list_clt[j]][0];
								cout = label.cout + distance * inst.Cv[c];
								load = label.charge + inst.D[tournee.list_clt[j]];
								Q.cout = cout;
								Q.pere = label.pere;
							}
							testerInsertion(inst, LL[j], Q, res);
							if (res == 1)
							{
								LL[j].list[LL[j].size] = Q;
								LL[j].size++;
							}
						}
					}
				}
				else
				{
					stop = 1;
				}
			}
			j++;
		}
	}
	// Construire les tournées
}

void TransformToTour(T_instance& inst, T_solution& solution, T_tournee& tournee) {
	
	tournee.cout = 0;
	tournee.list_clt[0] = 0;
	tournee.nc = inst.n + 2;
	int j, t = 1;

	for (int i = 0; i < solution.nt; i++) {
		j = 1;
		while (solution.tours[i].list_clt[j] != 0)
		{
			tournee.list_clt[t] = solution.tours[i].list_clt[j];
			tournee.cout = tournee.cout + inst.d[tournee.list_clt[t - 1]][tournee.list_clt[t]];
			t++;
			j++;
		}
	}
	// Ajouter le depot vers la fin de la tournee
	tournee.list_clt[t] = 0;
	tournee.cout += inst.d[tournee.list_clt[t - 1]][0];
}

void GraspxELS(T_instance& inst, T_solution& solution, int iterMax)
{
	int n = inst.n;
	double choix1, choix2;
	T_tournee tournee;
	T_tournee voisin1, voisin2;
	// Transformer la solution en tour geant
	TransformToTour(inst, solution, tournee);

	for (int i = 0; i < iterMax; i++)
	{
		// Generer les deux voisins de tournee
		genererVoisinSwap(inst, tournee, voisin1);
		genererVoisinSwap(inst, tournee, voisin2);

		// Faire un choix aléatoire de la methode de la recherche locale pour les deux voisins
		choix1 = (double)rand();
		choix2 = (double)rand();
		if (choix1 < 0.5)
		{
			deuxOPT(inst, voisin1, iterMax);
		}
		else {
			deplacerSommet(inst, voisin1);
		}
		if (choix2 < 0.5)
		{
			deuxOPT(inst, voisin2, iterMax);
		}
		else {
			deplacerSommet(inst, voisin2);
		}

		// Prendre le voisin qui a le cout minimal 
		if (voisin1.cout < voisin2.cout) {
			tournee = voisin1;
		}
		else {
			tournee = voisin2;
		}
	}

	//Construire les tournees de dernier tour geant choisi
	split(inst, tournee, solution);
}
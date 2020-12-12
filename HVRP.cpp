#include <iostream>
#include <algorithm>
#include "Header.h"

using namespace std;

int main()
{
    T_instance* ptr_inst = new T_instance;
    T_instance mon_instance = *ptr_inst;
    T_tournee tourneeG;
    LireInstance("instance.txt", mon_instance);

    plusProcheVoisin(mon_instance, tourneeG);
    //plusProcheVoisinRand(mon_instance, tourneeG, 5);
    //heuristiqueChoisi(mon_instance, tourneeG, 5, 4);
    afficherTournee(mon_instance, tourneeG),
    
    delete(ptr_inst);
}

// utiliser des pointeurs pour la capacite, les couts le nombre de vehicule.. est faire une allocation dynamiq dans le main

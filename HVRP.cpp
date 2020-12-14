#include <iostream>
#include <algorithm>
#include "Header.h"

using namespace std;

int main()
{
    T_instance* ptr_inst = new T_instance;
    T_instance mon_instance = *ptr_inst;
    T_tournee tourneeG;
    T_solution solution;
    LireInstance("instance.txt", mon_instance);
    
    plusProcheVoisin(mon_instance, tourneeG);
    //plusProcheVoisinRand(mon_instance, tourneeG, 5);
    //heuristiqueChoisi(mon_instance, tourneeG, 5, 4);
    afficherTournee(mon_instance, tourneeG);


    //deuxOPT(mon_instance, tourneeG, 50);
    deplacerSommet(mon_instance, tourneeG);
    afficherTournee(mon_instance, tourneeG);

    //split(mon_instance, tourneeG, solution);
    //GraspxELS(mon_instance, solution, 500);

    delete(ptr_inst);
}

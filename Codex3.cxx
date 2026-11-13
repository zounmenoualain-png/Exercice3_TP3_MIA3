
//  Exercice 3 - Gestion d'un personnel universitaire
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

// ------------- Classe mere abstraite : Employe -------------
class Employe {
protected:
    string matricule, nom, prenom;
    double salaireBase;
public:
    Employe(string m, string n, string p, double s)
        : matricule(m), nom(n), prenom(p), salaireBase(s) {}

    virtual ~Employe() {}                           // destructeur virtuel

    virtual double calculerSalaire() const = 0;     // methode virtuelle pure

    virtual void afficher() const {                 // methode virtuelle
        cout << "Matricule       : " << matricule << endl;
        cout << "Nom et prenom   : " << nom << " " << prenom << endl;
        cout << "Salaire de base : " << salaireBase << " FCFA" << endl;
    }
};

// ------------------- Classe derivee : Enseignant -------------------
class Enseignant : public Employe {
    double indemniteEnseignement, indemniteRecherche;
public:
    Enseignant(string m, string n, string p, double s, double ie, double ir)
        : Employe(m, n, p, s), indemniteEnseignement(ie), indemniteRecherche(ir) {}

    double calculerSalaire() const override {
        return salaireBase + indemniteEnseignement + indemniteRecherche;
    }
    void afficher() const override {
        cout << "--- Enseignant ---" << endl;
        Employe::afficher();
        cout << "Indemnite d'enseignement : " << indemniteEnseignement << " FCFA" << endl;
        cout << "Indemnite de recherche   : " << indemniteRecherche << " FCFA" << endl;
        cout << "Salaire total   : " << calculerSalaire() << " FCFA" << endl;
    }
};

// --------------- Classe derivee : AgentAdministratif ---------------
class AgentAdministratif : public Employe {
    double primeRendement, primeResponsabilite;
public:
    AgentAdministratif(string m, string n, string p, double s, double pr, double presp)
        : Employe(m, n, p, s), primeRendement(pr), primeResponsabilite(presp) {}

    double calculerSalaire() const override {
        return salaireBase + primeRendement + primeResponsabilite;
    }
    void afficher() const override {
        cout << "--- Agent administratif ---" << endl;
        Employe::afficher();
        cout << "Prime de rendement       : " << primeRendement << " FCFA" << endl;
        cout << "Prime de responsabilite  : " << primeResponsabilite << " FCFA" << endl;
        cout << "Salaire total   : " << calculerSalaire() << " FCFA" << endl;
    }
};

// ------------------- Classe derivee : Technicien -------------------
class Technicien : public Employe {
    double primeTechnicite, primeRisque;
public:
    Technicien(string m, string n, string p, double s, double pt, double pr)
        : Employe(m, n, p, s), primeTechnicite(pt), primeRisque(pr) {}

    double calculerSalaire() const override {
        return salaireBase + primeTechnicite + primeRisque;
    }
    void afficher() const override {
        cout << "--- Technicien ---" << endl;
        Employe::afficher();
        cout << "Prime de technicite      : " << primeTechnicite << " FCFA" << endl;
        cout << "Prime de risque          : " << primeRisque << " FCFA" << endl;
        cout << "Salaire total   : " << calculerSalaire() << " FCFA" << endl;
    }
};

// ------------------------- Programme principal -------------------------
int main() {
    cout << fixed << setprecision(0);               // affichage des montants sans notation scientifique

    // Employe e("M0", "X", "Y", 1000);   // ERREUR : Employe est abstraite

    // Collection contenant plusieurs categories d'employes
    vector<Employe*> personnel;
    personnel.push_back(new Enseignant("ENS001", "Adjovi", "Koffi", 450000, 120000, 80000));
    personnel.push_back(new Enseignant("ENS002", "Dossou", "Mariam", 500000, 150000, 100000));
    personnel.push_back(new AgentAdministratif("ADM001", "Hounkpatin", "Sylvie", 250000, 40000, 30000));
    personnel.push_back(new Technicien("TEC001", "Gbaguidi", "Rodrigue", 220000, 35000, 25000));

    double masseSalariale = 0;
    for (Employe* e : personnel) {
        e->afficher();                              // liaison dynamique
        masseSalariale += e->calculerSalaire();     // liaison dynamique
        cout << "------------------------------------" << endl;
    }
    cout << "Masse salariale : " << masseSalariale << " FCFA" << endl;

    for (Employe* e : personnel) delete e;          // liberation de la memoire
    return 0;
}

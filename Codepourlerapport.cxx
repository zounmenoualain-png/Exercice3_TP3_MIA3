
//  TP 3 - Gestion d'une residence universitaire
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
using namespace std;

// Etats possibles d'une demande d'hebergement
enum Statut { EN_ATTENTE, ACCEPTEE, REFUSEE, ANNULEE, TERMINEE };

string texteStatut(Statut s) {
    string noms[] = {"EN_ATTENTE", "ACCEPTEE", "REFUSEE", "ANNULEE", "TERMINEE"};
    return noms[s];
}

// ---------------------- Classe Etudiant ----------------------
class Etudiant {
    string matricule, nom, prenom;
public:
    Etudiant(string m, string n, string p) : matricule(m), nom(n), prenom(p) {}
    string getMatricule() const { return matricule; }
    string getNomComplet() const { return nom + " " + prenom; }
};

// ---------------------- Classe Chambre -----------------------
class Chambre {
    int numero, capacite, occupants;
public:
    Chambre(int n, int c) : numero(n), capacite(c), occupants(0) {}
    int getNumero() const { return numero; }
    int getCapacite() const { return capacite; }
    int getOccupants() const { return occupants; }
    int placesLibres() const { return capacite - occupants; }
    void occuper() { occupants++; }
    void liberer() { occupants--; }
};

// ---------------------- Classe Batiment ----------------------
class Batiment {
    string nom;
    vector<Chambre> chambres;
public:
    Batiment(string n) : nom(n) {}
    string getNom() const { return nom; }
    const vector<Chambre>& getChambres() const { return chambres; }

    void ajouterChambre(int numero, int capacite) {
        chambres.push_back(Chambre(numero, capacite));
    }
    Chambre* trouverChambre(int numero) {
        for (Chambre& c : chambres)
            if (c.getNumero() == numero) return &c;
        return nullptr;
    }
    int capacite() const {
        int total = 0;
        for (const Chambre& c : chambres) total += c.getCapacite();
        return total;
    }
    int occupants() const {
        int total = 0;
        for (const Chambre& c : chambres) total += c.getOccupants();
        return total;
    }
};

// ------------------ Classe DemandeHebergement ----------------
class DemandeHebergement {
    int id;
    string matricule;
    Statut statut;
public:
    DemandeHebergement(int i, string m) : id(i), matricule(m), statut(EN_ATTENTE) {}
    int getId() const { return id; }
    string getMatricule() const { return matricule; }
    Statut getStatut() const { return statut; }
    void setStatut(Statut s) { statut = s; }
};

// -------------------- Classe Attribution ---------------------
class Attribution {
    string matricule, batiment;
    int chambre;
    string debut, fin;
    bool active;
public:
    Attribution(string m, string b, int c, string d)
        : matricule(m), batiment(b), chambre(c), debut(d), fin(""), active(true) {}
    string getMatricule() const { return matricule; }
    string getBatiment() const { return batiment; }
    int getChambre() const { return chambre; }
    bool estActive() const { return active; }
    void cloturer(string date) { fin = date; active = false; }
    void afficher() const {
        cout << "  " << matricule << " | " << batiment << " ch." << chambre
             << " | du " << debut << (active ? " -> en cours" : " au " + fin) << endl;
    }
};

// --------------------- Classe Residence ----------------------
class Residence {
    string nom;
    vector<Batiment> batiments;
    vector<Etudiant> etudiants;
    vector<DemandeHebergement> demandes;
    vector<Attribution> attributions;
    int prochainId;

    // ----- methodes de recherche -----
    bool etudiantExiste(string matricule) const {
        for (const Etudiant& e : etudiants)
            if (e.getMatricule() == matricule) return true;
        return false;
    }
    DemandeHebergement* trouverDemande(int id) {
        for (DemandeHebergement& d : demandes)
            if (d.getId() == id) return &d;
        return nullptr;
    }
    Batiment* trouverBatiment(string nomBat) {
        for (Batiment& b : batiments)
            if (b.getNom() == nomBat) return &b;
        return nullptr;
    }
    Attribution* attributionActive(string matricule) {
        for (Attribution& a : attributions)
            if (a.estActive() && a.getMatricule() == matricule) return &a;
        return nullptr;
    }
    // Refuser ou annuler : seule une demande EN_ATTENTE peut changer d'etat
    void traiterDemande(int id, Statut nouveau) {
        DemandeHebergement* d = trouverDemande(id);
        if (d == nullptr || d->getStatut() != EN_ATTENTE) {
            cout << "  Erreur : demande #" << id << " introuvable ou deja traitee" << endl;
            return;
        }
        d->setStatut(nouveau);
        cout << "  Demande #" << id << " : " << texteStatut(nouveau) << endl;
    }

public:
    Residence(string n) : nom(n), prochainId(1) {}
    void ajouterBatiment(const Batiment& b) { batiments.push_back(b); }
    void ajouterEtudiant(const Etudiant& e) { etudiants.push_back(e); }

    // 1) Enregistrer une demande d'hebergement
    void enregistrerDemande(string matricule) {
        if (!etudiantExiste(matricule)) {
            cout << "  Erreur : etudiant " << matricule << " inconnu" << endl;
            return;
        }
        if (attributionActive(matricule) != nullptr) {
            cout << "  Erreur : " << matricule << " a deja une chambre" << endl;
            return;
        }
        for (const DemandeHebergement& d : demandes)
            if (d.getMatricule() == matricule && d.getStatut() == EN_ATTENTE) {
                cout << "  Erreur : " << matricule << " a deja une demande en attente" << endl;
                return;
            }
        demandes.push_back(DemandeHebergement(prochainId, matricule));
        cout << "  Demande #" << prochainId << " enregistree pour " << matricule << endl;
        prochainId++;
    }

    // 2) Attribuer une chambre (toutes les verifications avant de modifier)
    void attribuerChambre(int idDemande, string nomBat, int numChambre, string date) {
        DemandeHebergement* d = trouverDemande(idDemande);
        if (d == nullptr || d->getStatut() != EN_ATTENTE) {
            cout << "  Erreur : demande #" << idDemande << " introuvable ou deja traitee" << endl;
            return;
        }
        if (attributionActive(d->getMatricule()) != nullptr) {
            cout << "  Erreur : l'etudiant a deja une chambre" << endl;
            return;
        }
        Batiment* b = trouverBatiment(nomBat);
        Chambre* c = (b == nullptr) ? nullptr : b->trouverChambre(numChambre);
        if (c == nullptr) {
            cout << "  Erreur : chambre introuvable" << endl;
            return;
        }
        if (c->placesLibres() == 0) {
            cout << "  Erreur : la chambre " << nomBat << " " << numChambre << " est pleine" << endl;
            return;
        }
        c->occuper();
        attributions.push_back(Attribution(d->getMatricule(), nomBat, numChambre, date));
        d->setStatut(ACCEPTEE);
        cout << "  Demande #" << idDemande << " acceptee : " << nomBat
             << ", chambre " << numChambre << endl;
    }

    void refuserDemande(int id) { traiterDemande(id, REFUSEE); }
    void annulerDemande(int id) { traiterDemande(id, ANNULEE); }

    // 4) Enregistrer un depart
    void enregistrerDepart(string matricule, string date) {
        Attribution* a = attributionActive(matricule);
        if (a == nullptr) {
            cout << "  Erreur : " << matricule << " n'est pas loge" << endl;
            return;
        }
        trouverBatiment(a->getBatiment())->trouverChambre(a->getChambre())->liberer();
        a->cloturer(date);
        for (DemandeHebergement& d : demandes)
            if (d.getMatricule() == matricule && d.getStatut() == ACCEPTEE)
                d.setStatut(TERMINEE);
        cout << "  Depart de " << matricule << " enregistre (" << date << ")" << endl;
    }

    // 3) Verifier les places disponibles
    void afficherPlaces() const {
        int total = 0;
        for (const Batiment& b : batiments) {
            int libres = b.capacite() - b.occupants();
            cout << "  " << b.getNom() << " : " << libres << " place(s) libre(s) sur "
                 << b.capacite() << endl;
            total += libres;
        }
        cout << "  Total : " << total << " place(s) disponible(s)" << endl;
    }

    // 5) Consulter les chambres occupees et libres
    void afficherChambres() const {
        cout << " Chambres occupees :" << endl;
        for (const Batiment& b : batiments)
            for (const Chambre& c : b.getChambres())
                if (c.getOccupants() > 0)
                    cout << "  " << b.getNom() << " ch." << c.getNumero() << " : "
                         << c.getOccupants() << "/" << c.getCapacite() << endl;
        cout << " Chambres libres :" << endl;
        for (const Batiment& b : batiments)
            for (const Chambre& c : b.getChambres())
                if (c.getOccupants() == 0)
                    cout << "  " << b.getNom() << " ch." << c.getNumero()
                         << " (" << c.getCapacite() << " place(s))" << endl;
    }

    // 6) Consulter l'historique des occupations
    void afficherHistorique() const {
        for (const Attribution& a : attributions) a.afficher();
    }

    // Extension : taux d'occupation et statistiques
    void afficherStatistiques() const {
        int capacite = 0, occupants = 0;
        cout << fixed << setprecision(1);
        for (const Batiment& b : batiments) {
            capacite += b.capacite();
            occupants += b.occupants();
            cout << "  " << b.getNom() << " : " << b.occupants() << "/" << b.capacite()
                 << " -> " << 100.0 * b.occupants() / b.capacite() << " %" << endl;
        }
        double taux = (capacite == 0) ? 0 : 100.0 * occupants / capacite;
        cout << "  Residence : " << occupants << "/" << capacite
             << " -> taux d'occupation = " << taux << " %" << endl;
        cout << "  Demandes :";
        for (int s = EN_ATTENTE; s <= TERMINEE; s++) {
            int n = 0;
            for (const DemandeHebergement& d : demandes)
                if (d.getStatut() == s) n++;
            cout << " " << texteStatut((Statut)s) << "=" << n;
        }
        cout << endl;
    }
};

// ------------------------- Programme -------------------------
int main() {
    Residence res("Residence du Campus");

    Batiment blocA("Bloc A"), blocB("Bloc B");
    blocA.ajouterChambre(101, 2);
    blocA.ajouterChambre(102, 1);
    blocB.ajouterChambre(201, 2);
    blocB.ajouterChambre(202, 1);
    res.ajouterBatiment(blocA);
    res.ajouterBatiment(blocB);

    res.ajouterEtudiant(Etudiant("E001", "Adjovi", "Koffi"));
    res.ajouterEtudiant(Etudiant("E002", "Dossou", "Mariam"));
    res.ajouterEtudiant(Etudiant("E003", "Hounkpatin", "Sylvie"));
    res.ajouterEtudiant(Etudiant("E004", "Agbodjan", "Serge"));
    res.ajouterEtudiant(Etudiant("E005", "Gbaguidi", "Rodrigue"));
    res.ajouterEtudiant(Etudiant("E006", "Tossou", "Estelle"));

    cout << "=== 1. Demandes d'hebergement ===" << endl;
    res.enregistrerDemande("E001");
    res.enregistrerDemande("E002");
    res.enregistrerDemande("E003");
    res.enregistrerDemande("E004");
    res.enregistrerDemande("E005");
    res.enregistrerDemande("E006");
    res.enregistrerDemande("E001");   // doublon : refuse

    cout << "\n=== 2. Places disponibles ===" << endl;
    res.afficherPlaces();

    cout << "\n=== 3. Attribution des chambres ===" << endl;
    res.attribuerChambre(1, "Bloc A", 101, "2026-09-15");
    res.attribuerChambre(2, "Bloc A", 101, "2026-09-15");
    res.attribuerChambre(3, "Bloc A", 102, "2026-09-16");
    res.attribuerChambre(4, "Bloc B", 201, "2026-09-16");
    res.attribuerChambre(5, "Bloc A", 101, "2026-09-17");   // chambre pleine
    res.attribuerChambre(1, "Bloc B", 202, "2026-09-17");   // deja acceptee
    res.refuserDemande(6);

    cout << "\n=== 4. Chambres occupees et libres ===" << endl;
    res.afficherChambres();

    cout << "\n=== 5. Departs ===" << endl;
    res.enregistrerDepart("E002", "2026-12-20");
    res.enregistrerDepart("E006", "2026-12-20");   // jamais loge
    res.attribuerChambre(5, "Bloc A", 101, "2026-12-21");
    res.afficherPlaces();

    cout << "\n=== 6. Historique des occupations ===" << endl;
    res.afficherHistorique();

    cout << "\n=== 7. Statistiques ===" << endl;
    res.afficherStatistiques();

    return 0;
}

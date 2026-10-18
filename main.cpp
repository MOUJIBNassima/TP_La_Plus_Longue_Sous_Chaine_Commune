#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <unordered_map>
#include <iomanip>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace chrono;

struct Resultat {
    string sousChaine;
    long long comparaisons = 0;
};

// Génération de mots aléatoires
vector<string> genererMots(int nombre, mt19937& gen) {
    const string alphabet = "abcdefghijklmnopqrstuvwxyz";
    uniform_int_distribution<int> longueur(8, 16);
    uniform_int_distribution<int> lettre(0, 25);

    vector<string> mots;
    mots.reserve(nombre);

    for (int i = 0; i < nombre; ++i) {
        string mot;
        int taille = longueur(gen);

        for (int j = 0; j < taille; ++j) {
            mot += alphabet[lettre(gen)];
        }

        mots.push_back(mot);
    }

    return mots;
}

// Recherche de la plus longue sous-chaîne commune
// à au moins deux mots différents.
Resultat rechercher(const vector<string>& mots) {
    Resultat resultat;

    if (mots.size() < 2)
        return resultat;

    size_t longueurMax = 0;

    for (const string& mot : mots) {
        if (mot.size() > longueurMax)
            longueurMax = mot.size();
    }

    // On teste les longueurs de la plus grande à la plus petite.
    for (size_t longueur = longueurMax; longueur > 0; --longueur) {

        // Pour chaque candidate, mémoriser le premier mot où elle apparaît.
        unordered_map<string, size_t> premierMot;

        for (size_t i = 0; i < mots.size(); ++i) {

            if (mots[i].size() < longueur)
                continue;

            for (size_t debut = 0;
                 debut + longueur <= mots[i].size();
                 ++debut) {

                string candidate = mots[i].substr(debut, longueur);

                auto it = premierMot.find(candidate);

                if (it == premierMot.end()) {
                    // Première apparition de cette candidate.
                    premierMot.emplace(candidate, i);
                }
                else if (it->second != i) {
                    // Candidate rencontrée dans un autre mot.
                    // Vérification explicite avec string::find().
                    ++resultat.comparaisons;

                    if (mots[it->second].find(candidate)
                        != string::npos) {

                        resultat.sousChaine = candidate;
                        return resultat;
                    }
                }
            }
        }
    }

    return resultat;
}

void creerFichierMots(const string& nomFichier,
                      const vector<string>& mots) {
    ofstream fichier(nomFichier);

    if (!fichier.is_open()) {
        cerr << "Erreur : impossible de creer "
             << nomFichier << endl;
        return;
    }

    for (const string& mot : mots) {
        fichier << mot << '\n';
    }

    fichier.close();

    cout << "Fichier " << nomFichier
         << " cree avec " << mots.size()
         << " mots." << endl;
}

vector<string> lireFichierMots(const string& nomFichier) {
    vector<string> mots;
    ifstream fichier(nomFichier);
    string mot;

    if (!fichier.is_open()) {
        cerr << "Erreur : impossible de lire "
             << nomFichier << endl;
        return mots;
    }

    while (fichier >> mot) {
        mots.push_back(mot);
    }

    fichier.close();
    return mots;
}

int main() {
    mt19937 gen(
        static_cast<unsigned int>(
            high_resolution_clock::now()
                .time_since_epoch().count()
        )
    );

    vector<int> tailles = {
        10, 100, 1000, 10000, 50000, 100000
    };

    // Generer les mots aleatoires
    vector<string> motsGeneres =
    genererMots(100000, gen);

    // Enregistrer les mots dans mots.txt
    creerFichierMots("mots.txt", motsGeneres);

    // Relire les mots depuis le fichier
    vector<string> tousLesMots =
    lireFichierMots("mots.txt");

if (tousLesMots.size() != 100000) {
    cerr << "Erreur lors du chargement des mots."
         << endl;
    return 1;
}

    cout << "PLUS LONGUE SOUS-CHAINE COMMUNE\n";
    cout << "Commune a au moins deux mots\n\n";

    cout << left
         << setw(15) << "Nb de mots"
         << setw(25) << "Sous-chaine"
         << setw(15) << "Longueur"
         << setw(20) << "Comparaisons"
         << "Temps (ms)\n";

    cout << string(90, '-') << '\n';

    for (int taille : tailles) {

        vector<string> mots(
            tousLesMots.begin(),
            tousLesMots.begin() + taille
        );

        auto debut = high_resolution_clock::now();

        Resultat r = rechercher(mots);

        auto fin = high_resolution_clock::now();

        double temps = duration<double, milli>(
            fin - debut
        ).count();

        cout << left
             << setw(15) << taille
             << setw(25)
             << (r.sousChaine.empty()
                     ? "(aucune)" : r.sousChaine)
             << setw(15) << r.sousChaine.size()
             << setw(20) << r.comparaisons
             << fixed << setprecision(3)
             << temps << '\n';
    }

    return 0;
}

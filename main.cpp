#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>

using namespace std;
//        CLASSE DESTINATARIO
class destinatario {
private:
    string name;
    string iban;
    string Numtransaction;  // Numéro unico

public:
    // Costruttore per una persona fisica
    destinatario(string n, string i) : name(n), iban(i) {
        Numtransaction = destinatario::genererNumeroTransaction();
    }

    // Costruttore per un pagamento senza IBAN (magasin, etc.)
    destinatario(string n) : name(n), iban("N/A") {
        Numtransaction = destinatario::genererNumeroTransaction();
    }

    // funzioni di accesso
    string getName() const { return name; }
    string getIban() const { return iban; }
    string getNumtransaction() const { return Numtransaction; }

    // Générare un numéro unico di transazione
    static string genererNumeroTransaction() {
         // Initialisation pour éviter d’avoir toujours le même nombre
         return "TX-" + to_string(rand() % 1000000);
    }

    // Convertire un destinatario in format texte
    string toString() const {
        return name + "," + iban + "," + Numtransaction;
    }

    // Creare un destinatario da una linea di testo
    static destinatario fromString(const string& line) {
        stringstream ss(line);
        string name, iban, Numtransaction;
        getline(ss, name, ',');
        getline(ss, iban, ',');
        getline(ss, Numtransaction, ',');
        return destinatario(name, iban);  // Un numéro de transaction sera généré automatiquement
    }
};

class transaction {
private:
    double montant;
    string type;  // "entrée" ou "sortie"
    string date;
    destinatario dest;
public:
    // Costruttore
    transaction(double m, string t, string d, destinatario desti)
        : montant(m), type(t), date(d),dest(desti){}

    // funzione di accesso
    double getMontant() const { return montant; }
    string getType() const { return type; }
    string getDate() const { return date; }
    destinatario getDestinatario() const { return dest; }

    // Convertire une transazione en format texte
    string toString() const {
        return date + "," + type + "," + to_string(montant) + "," + dest.toString();
    }

    // Creare une transazione da una linea di testo
    static transaction fromString(const string& line) {
        stringstream ss(line);
        string type, date, montantStr, destname, iban, Numtransaction;
        getline(ss, type, ',');
        getline(ss, date, ',');
        getline(ss, montantStr, ',');
        getline(ss, destname, ',');
        getline(ss, iban, ',');
        return transaction(stod(montantStr), type, date, destinatario(destname, iban));
    }
};

class comptebancaire {
private:
    double solde;
    string numcompte;
    string iban;
    vector<transaction> transactions;

public:
    // Costructtore
    comptebancaire() : solde(0.0) {
        numcompte = generernumcompte();
        iban = genererIban();
    }

    // Générare un numéro di conto unico
    static string generernumcompte() {
        return "ACC-" + to_string(rand() % 1000000);
    }

    // Générare un IBAN fictif
    static string genererIban() {
        return "IT73J0589653695500000012321-" + to_string(rand() % 10000);
    }

    // funzione di accesso
    double getSolde() const { return solde; }
    string getNumcompte() const { return numcompte; }
    string getIban() const { return iban; }

    // Aggiunto di una transazione
    void addTransaction(double montant, string type, string date, string destname, string destiban = "N/A") {
        if (type == "uscita" && montant > solde) {
            cout << "credito insufficiente !" << endl;
            return;
        }
        destinatario dest(destname, destiban);
        transaction t(montant, type, date,dest);
        transactions.push_back(t);

        // aggiornare il soldo
        if (type == "entrata") {
            solde += montant;
        } else {
            solde -= montant;
        }
    }

    // monstrare il conto e le sue transazione
    void affichercompte() const {
        cout << "Numéro di conto: " << numcompte << endl;
        cout << "IBAN: " << iban << endl;
        cout << "Solde actuel: " << solde << " EUR" << endl;
        cout << "Historico di transazione:" << endl;
        for (const auto& t : transactions) {
            cout << t.getDate() << " - " << t.getType() << " : " << t.getMontant() << " EUR - "
                 << (t.getType() == "entrée" ? "Provenance" : "Destinatario") << " : " << t.getDestinatario().getName();

            if (t.getDestinatario().getIban() != "N/A") {
                cout << " (IBAN: " << t.getDestinatario().getIban() << ")";
            }

            cout << " - Transaction ID: " << t.getDestinatario().getNumtransaction() << endl;
        }
    }

    // Save transactions in un file
    void savefile(const string& filename) const {
        ofstream file(filename);
        if (!file) {
            cout << "Error di apertura di un file!" << endl;
            return;
        }

        file << numcompte << "," << iban << "," << solde << endl;
        for (const auto& t : transactions) {
            file << t.toString() << endl;
        }
        file.close();
    }

    // load transazione da un file
    void loadfile(const string& filename) {
        ifstream file(filename);
        if (!file) {
            cout << "Error: nessun file trovato, initiallizzazzione." << endl;
            return;
        }

        transactions.clear();
        string ligne;
        getline(file, ligne);
        stringstream ss(ligne);
        string soldeStr;
        getline(ss, numcompte, ',');
        getline(ss, iban, ',');
        getline(ss, soldeStr, ',');
        solde = stod(soldeStr);

        while (getline(file, ligne)) {
            transaction t = transaction::fromString(ligne);
            transactions.push_back(t);
        }
        file.close();
    }
};

int main() {
    srand(time(0)); // per avere delle sequenze diverse ad ogni iterazione
    comptebancaire compte;
    compte.addTransaction(500, "entrata", "2025-03-25", "stipendio");
    compte.addTransaction(300, "uscita", "2025-03-30", "Andrea Fabrizio", "IT13K1523654780000001236547");
    compte.affichercompte();
    compte.savefile("transaction.txt");

    return 0;
}
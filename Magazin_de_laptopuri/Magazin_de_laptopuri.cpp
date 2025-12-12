#include <iostream>
#include <vector>
#include <string> // Pentru siruri de caractere
#include <fstream> // Pentru lucrul cu fisiere
#include <memory> // Pentru pointeri inteligenti
#include <sstream> // Pentru a prelucra linii de text
using namespace std;


class Produs {
protected:
    static int contor; // Rolul lui este de a da ID-uri unice
    int id;
    string nume;
    string brand;
    double pret;
public:
    Produs(const string& nume, const string& brand, double pret) : nume(nume), brand(brand), pret(pret) {
        id = ++contor;
    }
    // Metode abstracte - obliga clasele copil sa implementeze aceste metode
    virtual void afiseaza() const = 0;
    virtual string serializeaza() const = 0;
    virtual string getTip() const = 0;

    virtual ~Produs() = default;

    // Avem o functie getter pentru pret, brand, id – folosite in filtre
    double getPret() const { return pret; }
    string getBrand() const { return brand; }
    int getId() const { return id; }

    // Am suprascris operatorul << pentru afisarea directa a obiectelor
    friend ostream& operator<<(ostream& os, const Produs& p) {
        os << "[" << p.getTip() << " " << p.id << "] " << p.nume << " - " << p.brand << ", " << p.pret << " lei";
        return os;
    }
};
int Produs::contor = 0; // Definitie in afara clasei

class Laptop : public Produs {
    int ram;
    string procesor;
    int spatiuStocare;
public:
    Laptop(const string& nume, const string& brand, double pret, int ram, const string& procesor, int spatiuStocare)
        : Produs(nume, brand, pret), ram(ram), procesor(procesor), spatiuStocare(spatiuStocare) {
    }

    // Afiseaza informatiile despre laptop
    void afiseaza() const override {
        cout << *this << " | RAM: " << ram << "GB, Procesor: " << procesor << ", Stocare: " << spatiuStocare << "GB\n";
    }

    // Converteste laptopul in text (pentru salvarea in fisier)
    string serializeaza() const override {
        return "Laptop," + nume + "," + brand + "," + to_string(pret) + "," + to_string(ram) + "," + procesor + "," + to_string(spatiuStocare);
    }

    // Returneaza „Laptop”
    string getTip() const override { return "Laptop"; }
};

class Magazin {
    vector<shared_ptr<Produs>> produse;
public:
    // Adauga un produs în vector
    void adauga(shared_ptr<Produs> p) {
        produse.push_back(p);
    }

    // Afiseaza toate produsele
    void afiseazaToate() const {
        for (const auto& p : produse)
            p->afiseaza();
    }
    // Deschide fisierul laptopuri.txt, citeste fiecare linie, o parseaza si creeaza un Laptop, daca linia incepe cu „Laptop”
    void incarcaDinFisier(const string& numeFisier) {
        ifstream fin(numeFisier);
        string linie;
        while (getline(fin, linie)) {
            istringstream ss(linie);
            string tip, nume, brand, pretStr, ramStr, proc, spatiuStr;
            getline(ss, tip, ',');
            getline(ss, nume, ',');
            getline(ss, brand, ',');
            getline(ss, pretStr, ',');
            getline(ss, ramStr, ',');
            getline(ss, proc, ',');
            getline(ss, spatiuStr, ',');

            double pret = stod(pretStr);
            int ram = stoi(ramStr);
            int spatiu = stoi(spatiuStr);

            if (tip == "Laptop") {
                adauga(make_shared<Laptop>(nume, brand, pret, ram, proc, spatiu));
            }
        }
        fin.close();
    }

    // Folosim template pentru o functie care cauta produse dupa criteriu generic
    template <typename T>
    vector<shared_ptr<Produs>> cauta(T criteriu) const {
        vector<shared_ptr<Produs>> rezultate;
        for (const auto& p : produse) {
            if (criteriu(*p))
                rezultate.push_back(p);
        }
        return rezultate;
    }

    // Salveaza lista de produse in fisierul rezultate.txt
    void salveazaInFisier(const vector<shared_ptr<Produs>>& produse, const string& numeFisier) {
        ofstream fout(numeFisier);
        for (const auto& p : produse) {
            fout << p->serializeaza() << endl;
        }
        fout.close();
    }
};


int main() {
    // Creeaza un obiect "Magazin" unde incarca laptopuri din fisierul laptopuri.txt
    Magazin m;
    m.incarcaDinFisier("laptopuri.txt");

    // Afiseaza toate laptopurile
    cout << "Laptopuri disponibile:\n";
    m.afiseazaToate();

    // Cauta toate laptopurile cu pretul sub 4000 lei
    cout << "\nLaptopuri sub 4000 lei:\n";
    auto ieftine = m.cauta([](const Produs& p) { return p.getPret() < 4000; });
    for (const auto& p : ieftine)
        p->afiseaza();

    // Salveaza aceste laptopuri intr-un fisier nou (rezultate.txt)
    m.salveazaInFisier(ieftine, "rezultate.txt");
    cout << "\nLaptopurile ieftine au fost salvate in 'rezultate.txt'.\n";

    // Afiseaza laptopuri populare in functie de brand
    cout << "\nTop 3 laptopuri preferate de clienti:\n";
    for (const auto& p : m.cauta([](const Produs& p) {
        return p.getBrand() == "Apple" || p.getBrand() == "Asus" || p.getBrand() == "Lenovo";
    }))
        p->afiseaza();

    return 0;
}

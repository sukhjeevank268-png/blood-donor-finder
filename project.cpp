// Blood Donor Finder - C++ console project
// Compile: g++ main.cpp -o main
// Run:     ./main   

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
using namespace std;

// ---------- helper functions ----------
string toUpper(string s) {
    transform(s.begin(), s.end(), s.begin(), ::toupper);
    return s;
}

bool isValidBloodGroup(const string &bg) {
    static const vector<string> groups = {"A+", "A-", "B+", "B-", "AB+", "AB-", "O+", "O-"};
    return find(groups.begin(), groups.end(), toUpper(bg)) != groups.end();
}

bool isValidPhone(const string &p) {
    if (p.size() != 10) return false;
    for (char c : p)
        if (!isdigit(static_cast<unsigned char>(c))) return false;
    return true;
}

// ---------- Donor class ----------
class Donor {
private:
    int id;
    string name;
    int age;
    string bloodGroup;
    string city;
    string phone;

public:
    Donor() : id(0), age(0) {}
    Donor(int id, string name, int age, string bg, string city, string phone)
        : id(id), name(name), age(age), bloodGroup(toUpper(bg)), city(city), phone(phone) {}

    int getId() const { return id; }
    string getName() const { return name; }
    int getAge() const { return age; }
    string getBloodGroup() const { return bloodGroup; }
    string getCity() const { return city; }
    string getPhone() const { return phone; }

    void setName(const string &n) { name = n; }
    void setAge(int a) { age = a; }
    void setBloodGroup(const string &bg) { bloodGroup = toUpper(bg); }
    void setCity(const string &c) { city = c; }
    void setPhone(const string &p) { phone = p; }

    void display() const {
        cout << "ID: " << id
             << " | Name: " << name
             << " | Age: " << age
             << " | Blood Group: " << bloodGroup
             << " | City: " << city
             << " | Phone: " << phone << endl;
    }

    // one line in file: id|name|age|bg|city|phone
    string toFileLine() const {
        return to_string(id) + "|" + name + "|" + to_string(age) + "|" +
               bloodGroup + "|" + city + "|" + phone;
    }

    static bool fromFileLine(const string &line, Donor &d) {
        stringstream ss(line);
        string idS, nameS, ageS, bgS, cityS, phoneS;
        if (!getline(ss, idS, '|')) return false;
        if (!getline(ss, nameS, '|')) return false;
        if (!getline(ss, ageS, '|')) return false;
        if (!getline(ss, bgS, '|')) return false;
        if (!getline(ss, cityS, '|')) return false;
        if (!getline(ss, phoneS, '|')) return false;
        try {
            d = Donor(stoi(idS), nameS, stoi(ageS), bgS, cityS, phoneS);
        } catch (...) {
            return false;
        }
        return true;
    }
};

// ---------- DonorManager class ----------
class DonorManager {
private:
    vector<Donor> donors;
    int nextId;
    const string fileName = "donors.txt";

    int findIndexById(int id) const {
        for (size_t i = 0; i < donors.size(); i++)
            if (donors[i].getId() == id) return (int)i;
        return -1;
    }

public:
    DonorManager() : nextId(1) { loadFromFile(); }

    void loadFromFile() {
        ifstream in(fileName);
        if (!in) return; // file not created yet
        string line;
        while (getline(in, line)) {
            Donor d;
            if (!line.empty() && Donor::fromFileLine(line, d)) {
                donors.push_back(d);
                nextId = max(nextId, d.getId() + 1);
            }
        }
    }

    void saveToFile() const {
        ofstream out(fileName);
        for (const Donor &d : donors) out << d.toFileLine() << "\n";
    }

    void addDonor() {
        string name, bg, city, phone;
        int age;

        cout << "Enter name: ";
        getline(cin, name);

        cout << "Enter age (18-65): ";
        cin >> age;
        cin.ignore(10000, '\n');
        if (cin.fail() || age < 18 || age > 65) {
            cin.clear();
            cout << "Invalid age. Donor must be between 18 and 65.\n";
            return;
        }

        cout << "Enter blood group (A+, A-, B+, B-, AB+, AB-, O+, O-): ";
        getline(cin, bg);
        if (!isValidBloodGroup(bg)) {
            cout << "Invalid blood group.\n";
            return;
        }

        cout << "Enter city: ";
        getline(cin, city);

        cout << "Enter 10-digit phone number: ";
        getline(cin, phone);
        if (!isValidPhone(phone)) {
            cout << "Invalid phone number.\n";
            return;
        }

        donors.push_back(Donor(nextId++, name, age, bg, city, phone));
        saveToFile();
        cout << "Donor added successfully!\n";
    }

    void displayAll() const {
        if (donors.empty()) {
            cout << "No donors found.\n";
            return;
        }
        for (const Donor &d : donors) d.display();
    }

    void searchDonors() const {
        string bg, city;
        cout << "Enter blood group to search: ";
        getline(cin, bg);
        cout << "Enter city (leave empty for all cities): ";
        getline(cin, city);

        bool found = false;
        for (const Donor &d : donors) {
            bool bgMatch = (d.getBloodGroup() == toUpper(bg));
            bool cityMatch = city.empty() || (toUpper(d.getCity()) == toUpper(city));
            if (bgMatch && cityMatch) {
                d.display();
                found = true;
            }
        }
        if (!found) cout << "No matching donors found.\n";
    }

    void updateDonor() {
        int id;
        cout << "Enter donor ID to update: ";
        cin >> id;
        cin.ignore(10000, '\n');

        int idx = findIndexById(id);
        if (idx == -1) {
            cout << "Donor not found.\n";
            return;
        }

        string input;
        cout << "Leave a field empty to keep the old value.\n";

        cout << "New name (" << donors[idx].getName() << "): ";
        getline(cin, input);
        if (!input.empty()) donors[idx].setName(input);

        cout << "New city (" << donors[idx].getCity() << "): ";
        getline(cin, input);
        if (!input.empty()) donors[idx].setCity(input);

        cout << "New phone (" << donors[idx].getPhone() << "): ";
        getline(cin, input);
        if (!input.empty()) {
            if (isValidPhone(input)) donors[idx].setPhone(input);
            else cout << "Invalid phone, old number kept.\n";
        }

        saveToFile();
        cout << "Donor updated.\n";
    }

    void deleteDonor() {
        int id;
        cout << "Enter donor ID to delete: ";
        cin >> id;
        cin.ignore(10000, '\n');

        int idx = findIndexById(id);
        if (idx == -1) {
            cout << "Donor not found.\n";
            return;
        }
        donors.erase(donors.begin() + idx);
        saveToFile();
        cout << "Donor deleted.\n";
    }
};

// ---------- main ----------
int main() {
    DonorManager manager;
    int choice;

    do {
        cout << "\n===== BLOOD DONOR FINDER =====\n"
             << "1. Add donor\n"
             << "2. Display all donors\n"
             << "3. Search donor (blood group / city)\n"
             << "4. Update donor\n"
             << "5. Delete donor\n"
             << "6. Exit\n"
             << "Enter choice: ";
        cin >> choice;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Please enter a number.\n";
            continue;
        }
        cin.ignore(10000, '\n');

        switch (choice) {
            case 1: manager.addDonor(); break;
            case 2: manager.displayAll(); break;
            case 3: manager.searchDonors(); break;
            case 4: manager.updateDonor(); break;
            case 5: manager.deleteDonor(); break;
            case 6: cout << "Goodbye!\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 6);

    return 0;
}
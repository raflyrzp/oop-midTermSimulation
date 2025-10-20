#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip> // quoted
#include <stdexcept>

using namespace std;

struct Contact
{
    int id{};
    string name;
    string phone;
};

void saveContacts(const string &path, const vector<Contact> &contacts)
{
    ofstream ofs(path);
    if (!ofs)
    {
        throw runtime_error("Gagal membuka file untuk menulis: " + path);
    }

    // Format per baris: <id> <"name"> <"phone">
    // name dan phone diserialisasi dengan quoted agar aman dari spasi, kutip, dll.
    for (const auto &c : contacts)
    {
        ofs << c.id << ' ' << quoted(c.name) << ' ' << quoted(c.phone) << '\n';
    }
}

vector<Contact> loadContacts(const string &path)
{
    ifstream ifs(path);
    if (!ifs)
    {
        throw runtime_error("Gagal membuka file untuk membaca: " + path);
    }

    vector<Contact> result;
    Contact c;

    // Membaca sesuai urutan: id, "name", "phone"
    // Loop berhenti saat tidak bisa membaca format berikutnya (EOF atau format tidak cocok).
    while (true)
    {
        if (!(ifs >> c.id))
            break; // baca id
        if (!(ifs >> quoted(c.name)))
            break; // baca name
        if (!(ifs >> quoted(c.phone)))
            break; // baca phone
        result.push_back(c);
    }

    return result;
}

int main()
{
    try
    {
        vector<Contact> contactsToSave = {
            {1, "Alice Johnson", "+62-812-3456-7890"},
            {2, "Budi \"Bi\" Santoso", "0812 0000 1111"},
            {3, "Chandra, S.Kom", "(021) 555-1234"}};

        const string filePath = "contacts.db";

        // Serialize (simpan ke file)
        saveContacts(filePath, contactsToSave);
        cout << "Berhasil menyimpan " << contactsToSave.size() << " kontak ke " << filePath << "\n";

        // Deserialize (muat dari file)
        auto loaded = loadContacts(filePath);
        cout << "Berhasil memuat " << loaded.size() << " kontak dari " << filePath << ":\n";

        for (const auto &c : loaded)
        {
            cout << "- id=" << c.id
                 << ", name=" << c.name
                 << ", phone=" << c.phone << '\n';
        }
    }
    catch (const exception &ex)
    {
        cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
    return 0;
}
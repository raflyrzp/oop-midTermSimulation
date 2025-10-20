#include <iostream>
#include "Store.h"
#include "Utils.h"
#include "Menus.h"
#include "BuyerSession.h"
#include "AdminSession.h"
#include "Buyer.h"

using namespace std;

int main()
{
    Store store;
    Utils::ensureDataDir();
    cout << "=== Terminal E-Commerce Simulation ===\n";

    while (true)
    {
        cout << "\n--- Menu Utama ---\n";
        cout << "1. Login\n";
        cout << "2. Register\n";
        cout << "3. Store & Bank Analytics (Admin)\n";
        cout << "4. Exit\n";
        cout << "Pilih (1-4): ";
        string line;
        getline(cin, line);

        int choice = -1;
        try
        {
            choice = stoi(line);
        }
        catch (...)
        {
            choice = -1;
        }

        switch (choice)
        {
        case 1:
        {
            string u = Utils::promptLine("Username");
            string p = Utils::promptLine("Password");
            Buyer loggedIn;
            if (!Buyer::login(store, u, p, loggedIn))
            {
                cout << "Login gagal.\n";
                break;
            }
            cout << "Login berhasil. Selamat datang, " << loggedIn.fullName << "!\n";
            BuyerSession::run(store, loggedIn.username);
            break;
        }
        case 2:
        {
            string u = Utils::promptLine("Username");
            string p = Utils::promptLine("Password");
            string n = Utils::promptLine("Nama Lengkap");
            if (Buyer::registerBuyer(store, store.bank(), u, p, n))
            {
                cout << "Registrasi berhasil. Akun bank otomatis dibuat.\n";
            }
            else
            {
                cout << "Registrasi gagal. Username sudah digunakan?\n";
            }
            break;
        }
        case 3:
        {
            AdminSession::run(store);
            break;
        }
        case 4:
        {
            cout << "Sampai jumpa!\n";
            store.save();
            return 0;
        }
        default:
            cout << "Pilihan tidak dikenal. Silakan pilih 1-4.\n";
            break;
        }
    }
}
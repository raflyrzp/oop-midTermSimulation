#pragma once
#include <string>
#include <vector>
#include <iostream>
#include "Store.h"
#include "Buyer.h"
#include "Seller.h"
#include "BankSession.h"
#include "SellerSession.h"
#include "Render.h"
#include "Menus.h"
#include "Utils.h"

using namespace std;

class BuyerSession
{
public:
    static void run(Store &store, const string &username)
    {
        vector<Buyer::CartEntry> cart;
        string line;

        while (true)
        {
            cout << "\n--- Buyer Menu (" << username << ") ---\n";
            cout << "1. Cek Akun Toko\n";
            cout << "2. Upgrade ke Seller\n";
            cout << "3. Menu Bank\n";
            cout << "4. Lihat & Tambah Barang ke Keranjang\n";
            cout << "5. Lihat Keranjang\n";
            cout << "6. Checkout\n";
            cout << "7. Kelola Pesanan (riwayat)\n";
            cout << "8. Cek pengeluaran k hari terakhir\n";
            cout << "9. Masuk Seller Mode\n";
            cout << "10. Logout\n";
            cout << "Pilih (1-10): ";
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
            case 10:
            {
                cout << "Logout.\n";
                return;
            }
            case 1:
            {
                showAccount(store, username);
                break;
            }
            case 2:
            {
                upgradeToSeller(store, username);
                break;
            }
            case 3:
            {
                BankSession::run(store, username);
                break;
            }
            case 4:
            {
                browseAndAddToCart(store, cart);
                break;
            }
            case 5:
            {
                viewCart(store, cart);
                break;
            }
            case 6:
            {
                checkout(store, username, cart);
                break;
            }
            case 7:
            {
                manageOrders(store, username);
                break;
            }
            case 8:
            {
                viewSpending(store, username);
                break;
            }
            case 9:
            {
                Buyer fresh;
                if (!Buyer::getBuyer(store, username, fresh) || !fresh.isSeller)
                {
                    cout << "Anda belum menjadi seller. Silakan upgrade dulu.\n";
                    Render::pausePressEnter();
                    break;
                }
                SellerSession::run(store, username);
                break;
            }
            default:
            {
                cout << "Pilihan tidak valid. Silakan pilih 1-10.\n";
                Render::pausePressEnter();
                break;
            }
            }
        }
    }

private:
    static void showAccount(Store &store, const string &username)
    {
        Buyer fresh;
        if (Buyer::getBuyer(store, username, fresh))
        {
            cout << "Username: " << fresh.username << "\n";
            cout << "Nama: " << fresh.fullName << "\n";
            cout << "Seller?: " << (fresh.isSeller ? "Ya" : "Tidak") << "\n";
        }
        else
        {
            cout << "Akun tidak ditemukan.\n";
        }
        auto acc = store.bank().ensureAccount(username);
        cout << "Akun Bank ID: " << acc.accountId << " | Saldo: " << acc.balance << "\n";
        Render::pausePressEnter();
    }

    static void upgradeToSeller(Store &store, const string &username)
    {
        Buyer fresh;
        if (Buyer::getBuyer(store, username, fresh) && fresh.isSeller)
        {
            cout << "Anda sudah menjadi seller.\n";
        }
        else
        {
            string shop = Utils::promptLine("Nama Toko");
            if (Seller::upgradeToSeller(store, username, shop))
                cout << "Berhasil upgrade ke Seller.\n";
            else
                cout << "Gagal upgrade.\n";
        }
        Render::pausePressEnter();
    }

    static void browseAndAddToCart(Store &store, vector<Buyer::CartEntry> &cart)
    {
        auto list = Buyer::browseItems(store);
        Render::printItems(list);
        string line;
        cout << "\nTambahkan ke keranjang? (y/n): ";
        getline(cin, line);
        if (line == "y" || line == "Y")
        {
            string id = Utils::promptLine("Masukkan ID Item");
            int qty = Utils::promptInt("Kuantitas", 1, 1000000);
            cart.push_back({id, qty});
            cout << "Item ditambahkan ke keranjang.\n";
        }
        Render::pausePressEnter();
    }

    static void viewCart(Store &store, vector<Buyer::CartEntry> &cart)
    {
        if (cart.empty())
            cout << "(Keranjang kosong)\n";
        else
        {
            double total = 0.0;
            for (size_t i = 0; i < cart.size(); ++i)
            {
                Item it;
                if (!Buyer::getItem(store, cart[i].itemId, it))
                    continue;
                double sub = it.price * cart[i].quantity;
                total += sub;
                cout << i + 1 << ". [" << it.id << "] " << it.name
                     << " x" << cart[i].quantity << " @ " << it.price
                     << " = " << sub << "\n";
            }
            cout << "Total estimasi: " << total << "\n";
            string line;
            cout << "Hapus item dari keranjang? (y/n): ";
            getline(cin, line);
            if (line == "y" || line == "Y")
            {
                int idx = Utils::promptInt("Nomor item (1..N)", 1, (int)cart.size());
                cart.erase(cart.begin() + (idx - 1));
                cout << "Dihapus.\n";
            }
        }
        Render::pausePressEnter();
    }

    static void checkout(Store &store, const string &username, vector<Buyer::CartEntry> &cart)
    {
        string msg;
        if (Buyer::checkout(store, store.bank(), username, cart, msg))
        {
            cout << msg << "\n";
            cart.clear();
        }
        else
        {
            cout << "Checkout gagal: " << msg << "\n";
        }
        Render::pausePressEnter();
    }

    static void manageOrders(Store &store, const string &username)
    {
        string line;
        cout << "Filter status? (paid/complete/cancelled/ALL): ";
        getline(cin, line);
        string filter = line;
        auto ords = Buyer::getBuyerOrders(store, username, filter);
        Render::printOrders(ords);
        cout << "\nBatalkan pesanan berstatus 'paid'? (y/n): ";
        getline(cin, line);
        if (line == "y" || line == "Y")
        {
            string oid = Utils::promptLine("Masukkan Order ID");
            if (Buyer::cancelOrderByBuyer(store, store.bank(), username, oid))
                cout << "Pesanan dibatalkan & dana direfund.\n";
            else
                cout << "Gagal membatalkan.\n";
        }
        Render::pausePressEnter();
    }

    static void viewSpending(Store &store, const string &username)
    {
        int k = Utils::promptInt("Masukkan k hari terakhir", 1, 3650);
        double spent = Buyer::spendingLastKDays(store.bank(), username, k);
        cout << "Pengeluaran " << k << " hari terakhir: " << spent << "\n";
        Render::pausePressEnter();
    }
};
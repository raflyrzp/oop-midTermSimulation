#pragma once
#include <string>
#include <iostream>
#include "Store.h"
#include "Menus.h"
#include "Render.h"
#include "Utils.h"
#include "Seller.h"

using namespace std;

class SellerSession
{
public:
    static void run(Store &store, const string &sellerUsername)
    {
        string line;
        while (true)
        {
            cout << "\n--- Seller Menu (" << sellerUsername << ") ---\n";
            cout << "1. Kelola Inventaris\n";
            cout << "2. Lihat Analitik Seller\n";
            cout << "3. Lihat & Kelola Pesanan Masuk (paid -> complete)\n";
            cout << "4. Kembali ke Buyer Menu\n";
            cout << "Pilih (1-4): ";
            getline(cin, line);

            int sm = -1;
            try
            {
                sm = stoi(line);
            }
            catch (...)
            {
                sm = -1;
            }

            switch (sm)
            {
            case 1: // Kelola Inventaris
                inventoryMenu(store, sellerUsername);
                break;

            case 2: // Analitik Seller
                analyticsMenu(store, sellerUsername);
                break;

            case 3: // Pesanan masuk
                incomingOrdersMenu(store, sellerUsername);
                break;

            case 4: // Kembali
                return;

            default:
                cout << "Pilihan tidak valid. Silakan pilih 1-4.\n";
                Render::pausePressEnter();
                break;
            }
        }
    }

private:
    static void inventoryMenu(Store &store, const string &sellerUsername)
    {
        string line;
        while (true)
        {
            cout << "\n--- Seller Inventory Menu ---\n";
            cout << "1. Lihat Inventaris\n";
            cout << "2. Tambah Item\n";
            cout << "3. Update Item (nama/deskripsi)\n";
            cout << "4. Set Harga\n";
            cout << "5. Replenish Stok\n";
            cout << "6. Discard Stok\n";
            cout << "7. Toggle Visibility\n";
            cout << "8. Kembali\n";
            cout << "Pilih (1-8): ";
            getline(cin, line);

            int inv = -1;
            try
            {
                inv = stoi(line);
            }
            catch (...)
            {
                inv = -1;
            }

            switch (inv)
            {
            case 1:
            {
                auto invList = Seller::getSellerInventory(store, sellerUsername);
                Render::printItems(invList);
                Render::pausePressEnter();
                break;
            }
            case 2:
            {
                string name = Utils::promptLine("Nama item");
                string desc = Utils::promptLine("Deskripsi");
                double price = Utils::promptDouble("Harga", 0.0);
                int stock = Utils::promptInt("Stok awal", 0, 1000000000);
                if (Seller::addItem(store, sellerUsername, name, desc, price, stock))
                    cout << "Item ditambahkan.\n";
                else
                    cout << "Gagal menambahkan item.\n";
                Render::pausePressEnter();
                break;
            }
            case 3:
            {
                string id = Utils::promptLine("ID Item");
                string name = Utils::promptLine("Nama baru");
                string desc = Utils::promptLine("Deskripsi baru");
                if (Seller::updateItem(store, sellerUsername, id, name, desc))
                    cout << "Item diupdate.\n";
                else
                    cout << "Gagal update.\n";
                Render::pausePressEnter();
                break;
            }
            case 4:
            {
                string id = Utils::promptLine("ID Item");
                double price = Utils::promptDouble("Harga baru", 0.0);
                if (Seller::setPrice(store, sellerUsername, id, price))
                    cout << "Harga diset.\n";
                else
                    cout << "Gagal set harga.\n";
                Render::pausePressEnter();
                break;
            }
            case 5:
            {
                string id = Utils::promptLine("ID Item");
                int add = Utils::promptInt("Tambah stok", 1, 1000000000);
                if (Seller::replenishStock(store, sellerUsername, id, add))
                    cout << "Stok ditambah.\n";
                else
                    cout << "Gagal tambah stok.\n";
                Render::pausePressEnter();
                break;
            }
            case 6:
            {
                string id = Utils::promptLine("ID Item");
                int rem = Utils::promptInt("Kurangi stok", 1, 1000000000);
                if (Seller::discardStock(store, sellerUsername, id, rem))
                    cout << "Stok dikurangi.\n";
                else
                    cout << "Gagal kurangi stok.\n";
                Render::pausePressEnter();
                break;
            }
            case 7:
            {
                string id = Utils::promptLine("ID Item");
                if (Seller::toggleVisibility(store, sellerUsername, id))
                    cout << "Visibility ditoggle.\n";
                else
                    cout << "Gagal toggle.\n";
                Render::pausePressEnter();
                break;
            }
            case 8:
                return;

            default:
                cout << "Pilihan tidak valid. Silakan pilih 1-8.\n";
                Render::pausePressEnter();
                break;
            }
        }
    }

    static void analyticsMenu(Store &store, const string &sellerUsername)
    {
        string month = Utils::promptLine("Bulan (YYYY-MM), kosongkan untuk bulan ini");
        if (month.empty())
            month = Utils::toMonth(Utils::nowString());
        auto topItems = Seller::topItemsForSellerByMonth(store, sellerUsername, month, 5);
        auto loyal = Seller::loyalCustomersForSellerByMonth(store, sellerUsername, month, 5);

        cout << "Top Items bulan " << month << ":\n";
        for (auto &p : topItems)
            cout << " - " << p.first << " | Qty: " << p.second << "\n";

        cout << "Pelanggan Loyal bulan " << month << " (berdasarkan jumlah order):\n";
        for (auto &p : loyal)
            cout << " - " << p.first << " | Orders: " << p.second << "\n";

        Render::pausePressEnter();
    }

    static void incomingOrdersMenu(Store &store, const string &sellerUsername)
    {
        auto incoming = Seller::getSellerIncomingPaid(store, sellerUsername);
        Render::printOrders(incoming);

        string line;
        cout << "\nSet order ke 'complete'? (y/n): ";
        getline(cin, line);
        if (line == "y" || line == "Y")
        {
            string oid = Utils::promptLine("Order ID");
            if (Seller::completeOrder(store, sellerUsername, oid))
                cout << "Order ditandai complete.\n";
            else
                cout << "Gagal update order.\n";
        }
        Render::pausePressEnter();
    }
};
#pragma once
#include <string>
#include <iostream>
#include "Store.h"
#include "Render.h"
#include "Utils.h"

using namespace std;

class AdminSession
{
public:
    static void run(Store &store)
    {
        string line;
        while (true)
        {
            cout << "\n--- Admin: Store & Bank Analytics ---\n";
            cout << "1. Store: Transaksi ... hari terakhir\n";
            cout << "2. Store: Transaksi 'paid' belum 'complete'\n";
            cout << "3. Store: m Item Terpopuler\n";
            cout << "4. Store: Buyer & Seller paling aktif (hari ini)\n";
            cout << "5. Bank: Transaksi seminggu terakhir\n";
            cout << "6. Bank: List semua nasabah\n";
            cout << "7. Bank: List akun dormant (>=1 bulan)\n";
            cout << "8. Bank: Top-n pengguna dgn transaksi terbanyak (hari ini)\n";
            cout << "9. Kembali\n";
            cout << "Pilih: ";
            getline(cin, line);
            int adm = 0;
            try
            {
                adm = stoi(line);
            }
            catch (...)
            {
                adm = 0;
            }
            if (adm == 9)
                break;

            if (adm == 1)
            {
                int k = Utils::promptInt("Masukkan beberapa hari terakhir", 1, 3650);
                auto list = store.transactionsLastKDays(k);
                Render::printOrders(list);
                Render::pausePressEnter();
            }
            else if (adm == 2)
            {
                auto list = store.paidNotCompleted();
                Render::printOrders(list);
                Render::pausePressEnter();
            }
            else if (adm == 3)
            {
                int m = Utils::promptInt("Jumlah top item (m)", 1, 1000);
                auto tops = store.topMItems(m);
                if (tops.empty())
                    cout << "(Tidak ada data)\n";
                else
                    for (auto &p : tops)
                        cout << " - " << p.first << " | Qty: " << p.second << "\n";
                Render::pausePressEnter();
            }
            else if (adm == 4)
            {
                auto pair = store.mostActiveBuyersSellersToday(5);
                auto &buyersTop = pair.first;
                auto &sellersTop = pair.second;
                cout << "Buyer paling aktif (hari ini):\n";
                if (buyersTop.empty())
                    cout << "(Tidak ada)\n";
                for (const auto &p : buyersTop)
                    cout << " - " << p.first << " | Orders: " << p.second << "\n";
                cout << "Seller paling aktif (complete, hari ini):\n";
                if (sellersTop.empty())
                    cout << "(Tidak ada)\n";
                for (const auto &p : sellersTop)
                    cout << " - " << p.first << " | Completed: " << p.second << "\n";
                Render::pausePressEnter();
            }
            else if (adm == 5)
            {
                auto list = store.bank().listTransactionsLastWeek();
                if (list.empty())
                    cout << "(Tidak ada transaksi minggu ini)\n";
                for (const auto &t : list)
                {
                    cout << "#" << t.id << " | " << t.timestamp << " | " << t.username
                         << " | " << t.type << " | " << t.amount << " | lawan: " << t.counterparty
                         << " | " << t.note << "\n";
                }
                Render::pausePressEnter();
            }
            else if (adm == 6)
            {
                auto users = store.bank().listAllCustomers();
                if (users.empty())
                    cout << "(Tidak ada nasabah)\n";
                for (const auto &c : users)
                {
                    cout << "User: " << c.username << " | Acc: " << c.accountId << " | Saldo: " << c.balance << "\n";
                }
                Render::pausePressEnter();
            }
            else if (adm == 7)
            {
                auto dorm = store.bank().listDormantAccountsOneMonth();
                if (dorm.empty())
                    cout << "(Tidak ada akun dormant)\n";
                for (const auto &c : dorm)
                {
                    cout << "Dormant: " << c.username << " | Acc: " << c.accountId << " | Saldo: " << c.balance << "\n";
                }
                Render::pausePressEnter();
            }
            else if (adm == 8)
            {
                int n = Utils::promptInt("Top n", 1, 1000);
                auto top = store.bank().topNUsersByTxToday(n);
                if (top.empty())
                    cout << "(Tidak ada aktivitas)\n";
                for (auto &p : top)
                {
                    cout << "User: " << p.first << " | Tx count: " << p.second << "\n";
                }
                Render::pausePressEnter();
            }
        };
    }
};

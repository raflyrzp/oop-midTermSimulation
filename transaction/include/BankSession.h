#pragma once
#include <string>
#include <iostream>
#include <map>
#include <algorithm>
#include "Store.h"
#include "Menus.h"
#include "Utils.h"
#include "Render.h"

using namespace std;

class BankSession
{
public:
    static void run(Store &store, const string &username)
    {
        string line;
        while (true)
        {
            cout << "\n--- Bank Menu ---\n";
            cout << "1. Cek Saldo\n";
            cout << "2. Deposit (Top-up)\n";
            cout << "3. Withdraw (Tarik)\n";
            cout << "4. Lihat Cash Flow (harian/bulanan)\n";
            cout << "5. Kembali\n";
            cout << "Pilih: ";
            getline(cin, line);

            int bk;
            try
            {
                bk = stoi(line);
            }
            catch (...)
            {
                bk = -1;
            }

            switch (bk)
            {
            case 1:
            {
                auto acc = store.bank().ensureAccount(username);
                cout << "Saldo: " << acc.balance << "\n";
                Render::pausePressEnter();
                break;
            }
            case 2:
            {
                double amt = Utils::promptDouble("Jumlah deposit", 0.01);
                if (store.bank().deposit(username, amt, "User deposit"))
                    cout << "Deposit berhasil.\n";
                else
                    cout << "Deposit gagal.\n";
                Render::pausePressEnter();
                break;
            }
            case 3:
            {
                double amt = Utils::promptDouble("Jumlah withdraw", 0.01);
                if (store.bank().withdraw(username, amt, "User withdraw"))
                    cout << "Withdraw berhasil.\n";
                else
                    cout << "Withdraw gagal. Saldo cukup?\n";
                Render::pausePressEnter();
                break;
            }
            case 4:
            {
                cout << "1. Ringkasan harian (7 hari terakhir)\n";
                cout << "2. Ringkasan bulanan (bulan ini)\n";
                cout << "Pilih: ";
                getline(cin, line);
                int sub = 0;
                try
                {
                    sub = stoi(line);
                }
                catch (...)
                {
                    sub = 0;
                }

                auto tx = store.bank().getUserTransactions(username);

                switch (sub)
                {
                case 1:
                {
                    map<string, double> dailyIn, dailyOut;
                    for (const auto &t : tx)
                    {
                        if (!Utils::isWithinLastDays(t.timestamp, 7))
                            continue;
                        auto day = Utils::toDate(t.timestamp);
                        if (t.type == "DEPOSIT" || t.type == "TRANSFER_IN")
                            dailyIn[day] += t.amount;
                        else
                            dailyOut[day] += t.amount;
                    }
                    cout << "Cash Flow Harian (7 hari):\n";
                    for (const auto &kv : dailyIn)
                    {
                        double out = dailyOut.count(kv.first) ? dailyOut.at(kv.first) : 0.0;
                        cout << kv.first << " | Masuk: " << kv.second << " | Keluar: " << out << "\n";
                    }
                    break;
                }
                case 2:
                {
                    auto month = Utils::toMonth(Utils::nowString());
                    double in = 0.0, out = 0.0;
                    for (const auto &t : tx)
                    {
                        if (!Utils::isWithinMonth(t.timestamp, month))
                            continue;
                        if (t.type == "DEPOSIT" || t.type == "TRANSFER_IN")
                            in += t.amount;
                        else
                            out += t.amount;
                    }
                    cout << "Cash Flow Bulan " << month << " | Masuk: " << in << " | Keluar: " << out << "\n";
                    break;
                }
                default:
                    cout << "Pilihan tidak valid (1/2).\n";
                    break;
                }

                cout << "\nTransaksi Terbaru (maks 10):\n";
                int start = max(0, (int)tx.size() - 10);
                for (int i = start; i < (int)tx.size(); ++i)
                {
                    const auto &t = tx[i];
                    cout << "#" << t.id << " | " << t.timestamp << " | " << t.type
                         << " | " << t.amount << " | lawan: " << t.counterparty
                         << " | " << t.note << "\n";
                }
                Render::pausePressEnter();
                break;
            }
            case 5:
                return;

            default:
                cout << "Pilihan tidak valid. Silakan pilih 1-5.\n";
                Render::pausePressEnter();
                break;
            }
        }
    }
};
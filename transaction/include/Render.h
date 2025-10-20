#pragma once
#include <vector>
#include <iostream>
#include <limits>
#include "Item.h"
#include "Order.h"

using namespace std;

namespace Render
{

    inline void pausePressEnter()
    {
        cout << "Tekan ENTER untuk lanjut...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    inline void printItems(const vector<Item> &items)
    {
        if (items.empty())
        {
            cout << "(Tidak ada item)\n";
            return;
        }
        for (const auto &it : items)
        {
            cout << "ID: " << it.id
                 << " | Nama: " << it.name
                 << " | Harga: " << it.price
                 << " | Stok: " << it.stock
                 << " | Seller: " << it.seller
                 << " | Visible: " << (it.visible ? "Yes" : "No")
                 << "\n  Deskripsi: " << it.description << "\n";
        }
    }

    inline void printOrders(const vector<Order> &orders)
    {
        if (orders.empty())
        {
            cout << "(Tidak ada pesanan)\n";
            return;
        }
        for (const auto &o : orders)
        {
            cout << "Order #" << o.id << " | Buyer: " << o.buyer << " | Seller: " << o.seller
                 << " | Tanggal: " << o.timestamp << " | Status: " << o.status
                 << " | Total: " << o.totalAmount << "\n";
            for (const auto &it : o.items)
            {
                cout << "  - [" << it.itemId << "] " << it.nameAtPurchase
                     << " x" << it.quantity << " @ " << it.unitPrice << "\n";
            }
        }
    }

}
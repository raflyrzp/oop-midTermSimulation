#pragma once
#include <string>
#include <vector>

class Store;
class Bank;
struct Item;
struct Order;

using namespace std;

struct Buyer
{
    string username;
    string password;
    string fullName;
    bool isSeller = false;

    Buyer() = default;
    Buyer(string u, string p, string n, bool s = false)
        : username(move(u)), password(move(p)), fullName(move(n)), isSeller(s) {}

    struct CartEntry
    {
        string itemId;
        int quantity;
    };

    static bool registerBuyer(Store &store, Bank &bank,
                              const string &username,
                              const string &password,
                              const string &fullName);

    static bool login(Store &store, const string &username,
                      const string &password, Buyer &outBuyer);

    static bool getBuyer(Store &store, const string &username, Buyer &outBuyer);

    static vector<Item> browseItems(Store &store);
    static bool getItem(Store &store, const string &itemId, Item &out);

    static bool checkout(Store &store, Bank &bank,
                         const string &buyerUsername,
                         const vector<CartEntry> &cart,
                         string &message);

    static vector<Order> getBuyerOrders(Store &store,
                                        const string &buyerUsername,
                                        const string &statusFilter);

    static bool cancelOrderByBuyer(Store &store, Bank &bank,
                                   const string &buyerUsername,
                                   const string &orderId);

    static double spendingLastKDays(Bank &bank,
                                    const string &buyerUsername,
                                    int kDays);
};
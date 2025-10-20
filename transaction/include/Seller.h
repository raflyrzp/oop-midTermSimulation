#pragma once
#include <string>
#include <vector>

using namespace std;

class Store;
struct Item;
struct Order;

struct SellerProfile
{
    string username;
    string shopName;

    SellerProfile() = default;
    SellerProfile(string u, string s) : username(move(u)), shopName(move(s)) {}
};

class Seller
{
public:
    static bool upgradeToSeller(Store &store, const string &username, const string &shopName);

    static vector<Item> getSellerInventory(Store &store, const string &sellerUsername);
    static bool addItem(Store &store, const string &sellerUsername,
                        const string &name, const string &desc,
                        double price, int stock);
    static bool updateItem(Store &store, const string &sellerUsername,
                           const string &itemId,
                           const string &newName,
                           const string &newDesc);
    static bool setPrice(Store &store, const string &sellerUsername,
                         const string &itemId, double newPrice);
    static bool replenishStock(Store &store, const string &sellerUsername,
                               const string &itemId, int added);
    static bool discardStock(Store &store, const string &sellerUsername,
                             const string &itemId, int removed);
    static bool toggleVisibility(Store &store, const string &sellerUsername,
                                 const string &itemId);

    static vector<Order> getSellerIncomingPaid(Store &store, const string &sellerUsername);
    static bool completeOrder(Store &store, const string &sellerUsername, const string &orderId);

    static vector<pair<string, int>>
    topItemsForSellerByMonth(Store &store, const string &sellerUsername,
                             const string &monthYYYYMM, int topN);

    static vector<pair<string, int>>
    loyalCustomersForSellerByMonth(Store &store, const string &sellerUsername,
                                   const string &monthYYYYMM, int topN);
};
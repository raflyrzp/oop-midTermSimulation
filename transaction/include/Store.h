#pragma once
#include "Buyer.h"
#include "Seller.h"
#include "Item.h"
#include "Order.h"
#include "Bank.h"
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Store
{
public:
    Store();

    void load();
    void save();

    vector<Order> transactionsLastKDays(int kDays) const;
    vector<Order> paidNotCompleted() const;
    vector<pair<string, int>> topMItems(int m) const;
    pair<vector<pair<string, int>>, vector<pair<string, int>>>
    mostActiveBuyersSellersToday(int topN = 5) const;

    Bank &bank();

private:
    unordered_map<string, Buyer> buyers;
    unordered_map<string, SellerProfile> sellers;
    unordered_map<string, Item> items;
    unordered_map<string, Order> orders;

    int nextItemId = 1;
    int nextOrderId = 1;

    Bank bankSystem;

    string generateItemId();
    string generateOrderId();

    void loadMeta();
    void saveMeta() const;
    void computeNextIdsIfNeeded();
    void saveBuyers() const;
    void loadBuyers();
    void saveSellers() const;
    void loadSellers();
    void saveItems() const;
    void loadItems();
    void saveOrders() const;
    void loadOrders();

    friend struct Buyer;
    friend class Seller;
};
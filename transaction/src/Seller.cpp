#include "Seller.h"
#include "Store.h"
#include "Item.h"
#include "Order.h"
#include "Buyer.h"
#include "Utils.h"
#include <algorithm>
#include <unordered_map>

using namespace std;

bool Seller::upgradeToSeller(Store &store, const string &username, const string &shopName)
{
    auto it = store.buyers.find(username);
    if (it == store.buyers.end())
        return false;
    if (it->second.isSeller)
        return true;
    it->second.isSeller = true;
    store.sellers[username] = SellerProfile(username, shopName);
    store.save();
    return true;
}

vector<Item> Seller::getSellerInventory(Store &store, const string &sellerUsername)
{
    vector<Item> v;
    for (const auto &kv : store.items)
        if (kv.second.seller == sellerUsername)
            v.push_back(kv.second);
    sort(v.begin(), v.end(), [](const Item &a, const Item &b)
         { return Utils::toInt(a.id, 0) < Utils::toInt(b.id, 0); });
    return v;
}

bool Seller::addItem(Store &store, const string &sellerUsername,
                     const string &name, const string &desc,
                     double price, int stock)
{
    auto itBuyer = store.buyers.find(sellerUsername);
    if (itBuyer == store.buyers.end() || !itBuyer->second.isSeller)
        return false;
    if (price < 0 || stock < 0)
        return false;
    Item it(store.generateItemId(), sellerUsername, name, desc, price, stock, true);
    store.items[it.id] = it;
    store.save();
    return true;
}

bool Seller::updateItem(Store &store, const string &sellerUsername, const string &itemId, const string &newName, const string &newDesc)
{
    auto it = store.items.find(itemId);
    if (it == store.items.end())
        return false;
    if (it->second.seller != sellerUsername)
        return false;
    it->second.name = newName;
    it->second.description = newDesc;
    store.save();
    return true;
}

bool Seller::setPrice(Store &store, const string &sellerUsername, const string &itemId, double newPrice)
{
    auto it = store.items.find(itemId);
    if (it == store.items.end())
        return false;
    if (it->second.seller != sellerUsername)
        return false;
    if (newPrice < 0)
        return false;
    it->second.price = newPrice;
    store.save();
    return true;
}

bool Seller::replenishStock(Store &store, const string &sellerUsername, const string &itemId, int added)
{
    auto it = store.items.find(itemId);
    if (it == store.items.end())
        return false;
    if (it->second.seller != sellerUsername)
        return false;
    if (added <= 0)
        return false;
    it->second.stock += added;
    store.save();
    return true;
}

bool Seller::discardStock(Store &store, const string &sellerUsername, const string &itemId, int removed)
{
    auto it = store.items.find(itemId);
    if (it == store.items.end())
        return false;
    if (it->second.seller != sellerUsername)
        return false;
    if (removed <= 0)
        return false;
    it->second.stock = max(0, it->second.stock - removed);
    store.save();
    return true;
}

bool Seller::toggleVisibility(Store &store, const string &sellerUsername, const string &itemId)
{
    auto it = store.items.find(itemId);
    if (it == store.items.end())
        return false;
    if (it->second.seller != sellerUsername)
        return false;
    it->second.visible = !it->second.visible;
    store.save();
    return true;
}

vector<Order> Seller::getSellerIncomingPaid(Store &store, const string &sellerUsername)
{
    vector<Order> v;
    for (const auto &kv : store.orders)
    {
        const auto &o = kv.second;
        if (o.seller == sellerUsername && o.status == "paid")
            v.push_back(o);
    }
    sort(v.begin(), v.end(), [](const Order &a, const Order &b)
         { return a.timestamp < b.timestamp; });
    return v;
}

bool Seller::completeOrder(Store &store, const string &sellerUsername, const string &orderId)
{
    auto it = store.orders.find(orderId);
    if (it == store.orders.end())
        return false;
    if (it->second.seller != sellerUsername)
        return false;
    if (it->second.status != "paid")
        return false;
    it->second.status = "complete";
    store.save();
    return true;
}

vector<pair<string, int>>
Seller::topItemsForSellerByMonth(Store &store, const string &sellerUsername, const string &monthYYYYMM, int topN)
{
    unordered_map<string, int> qtyByItem;
    unordered_map<string, string> nameByItem;
    for (const auto &kv : store.orders)
    {
        const auto &o = kv.second;
        if (o.seller != sellerUsername)
            continue;
        if (!(o.status == "paid" || o.status == "complete"))
            continue;
        if (!Utils::isWithinMonth(o.timestamp, monthYYYYMM))
            continue;
        for (const auto &oi : o.items)
        {
            qtyByItem[oi.itemId] += oi.quantity;
            if (!nameByItem.count(oi.itemId))
                nameByItem[oi.itemId] = oi.nameAtPurchase;
        }
    }
    vector<pair<string, int>> vec;
    for (const auto &kv2 : qtyByItem)
    {
        string display = nameByItem[kv2.first] + " (ID " + kv2.first + ")";
        vec.emplace_back(display, kv2.second);
    }
    sort(vec.begin(), vec.end(), [](auto &a, auto &b)
         {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first; });
    if ((int)vec.size() > topN)
        vec.resize(topN);
    return vec;
}

vector<pair<string, int>>
Seller::loyalCustomersForSellerByMonth(Store &store, const string &sellerUsername, const string &monthYYYYMM, int topN)
{
    unordered_map<string, int> cntByBuyer;
    for (const auto &kv : store.orders)
    {
        const auto &o = kv.second;
        if (o.seller != sellerUsername)
            continue;
        if (!(o.status == "paid" || o.status == "complete"))
            continue;
        if (!Utils::isWithinMonth(o.timestamp, monthYYYYMM))
            continue;
        cntByBuyer[o.buyer] += 1;
    }
    vector<pair<string, int>> vec(cntByBuyer.begin(), cntByBuyer.end());
    sort(vec.begin(), vec.end(), [](auto &a, auto &b)
         {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first; });
    if ((int)vec.size() > topN)
        vec.resize(topN);
    return vec;
}
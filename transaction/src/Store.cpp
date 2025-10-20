#include "Store.h"
#include "Utils.h"
#include <fstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

Store::Store() { load(); }

void Store::loadMeta()
{
    ifstream in("data/store_meta.txt");
    if (!in)
        return;
    string line;
    while (getline(in, line))
    {
        auto p = Utils::split(line, '|');
        if (p.size() >= 2)
        {
            if (p[0] == "nextItemId")
                nextItemId = Utils::toInt(p[1], nextItemId);
            if (p[0] == "nextOrderId")
                nextOrderId = Utils::toInt(p[1], nextOrderId);
        }
    }
}

void Store::saveMeta() const
{
    ofstream out("data/store_meta.txt", ios::trunc);
    out << "nextItemId|" << nextItemId << "\n";
    out << "nextOrderId|" << nextOrderId << "\n";
}

void Store::loadBuyers()
{
    buyers.clear();
    ifstream in("data/buyers.txt");
    string line;
    while (getline(in, line))
    {
        auto p = Utils::split(line, '|');
        if (p.size() < 4)
            continue;
        Buyer b;
        b.username = p[0];
        b.password = p[1];
        b.fullName = Utils::unescape(p[2]);
        b.isSeller = (p[3] == "1");
        buyers[b.username] = b;
    }
}

void Store::saveBuyers() const
{
    ofstream out("data/buyers.txt", ios::trunc);
    out << "username|password|fullName|isSeller\n";
    for (const auto &kv : buyers)
    {
        const auto &b = kv.second;
        out << b.username << "|" << b.password << "|" << Utils::escape(b.fullName) << "|" << (b.isSeller ? "1" : "0") << "\n";
    }
}

void Store::loadSellers()
{
    sellers.clear();
    ifstream in("data/sellers.txt");
    string line;
    while (getline(in, line))
    {
        auto p = Utils::split(line, '|');
        if (p.size() < 2)
            continue;
        SellerProfile s;
        s.username = p[0];
        s.shopName = Utils::unescape(p[1]);
        sellers[s.username] = s;
    }
}

void Store::saveSellers() const
{
    ofstream out("data/sellers.txt", ios::trunc);
    out << "username|shopName\n";
    for (const auto &kv : sellers)
    {
        out << kv.second.username << "|" << Utils::escape(kv.second.shopName) << "\n";
    }
}

void Store::loadItems()
{
    items.clear();
    ifstream in("data/items.txt");
    string line;
    while (getline(in, line))
    {
        auto p = Utils::split(line, '|');
        if (p.size() < 7)
            continue;
        Item it;
        it.id = p[0];
        it.seller = p[1];
        it.name = Utils::unescape(p[2]);
        it.description = Utils::unescape(p[3]);
        it.price = Utils::toDouble(p[4], 0.0);
        it.stock = Utils::toInt(p[5], 0);
        it.visible = (p[6] == "1");
        items[it.id] = it;
    }
}

void Store::saveItems() const
{
    ofstream out("data/items.txt", ios::trunc);
    out << "id|seller|name|description|price|stock|visible\n";
    for (const auto &kv : items)
    {
        const auto &it = kv.second;
        out << it.id << "|" << it.seller << "|" << Utils::escape(it.name) << "|"
            << Utils::escape(it.description) << "|" << it.price << "|" << it.stock << "|"
            << (it.visible ? "1" : "0") << "\n";
    }
}

void Store::loadOrders()
{
    orders.clear();
    unordered_map<string, Order> tmp;
    {
        ifstream in("data/orders.txt");
        string line;
        while (getline(in, line))
        {
            auto p = Utils::split(line, '|');
            if (p.size() < 7)
                continue;
            Order o;
            o.id = p[0];
            o.buyer = p[1];
            o.seller = p[2];
            o.timestamp = p[3];
            o.status = p[4];
            o.totalAmount = Utils::toDouble(p[5], 0.0);
            tmp[o.id] = o;
        }
    }
    {
        ifstream in("data/order_items.txt");
        string line;
        while (getline(in, line))
        {
            auto p = Utils::split(line, '|');
            if (p.size() < 5)
                continue;
            OrderItem oi;
            string orderId = p[0];
            oi.itemId = p[1];
            oi.nameAtPurchase = Utils::unescape(p[2]);
            oi.unitPrice = Utils::toDouble(p[3], 0.0);
            oi.quantity = Utils::toInt(p[4], 0);
            tmp[orderId].items.push_back(oi);
        }
    }
    orders = move(tmp);
}

void Store::saveOrders() const
{
    ofstream outO("data/orders.txt", ios::trunc);
    outO << "id|buyer|seller|timestamp|status|totalAmount|reserved\n";
    for (const auto &kv : orders)
    {
        const auto &o = kv.second;
        outO << o.id << "|" << o.buyer << "|" << o.seller << "|" << o.timestamp << "|"
             << o.status << "|" << o.totalAmount << "|-\n";
    }
    ofstream outI("data/order_items.txt", ios::trunc);
    outI << "orderId|itemId|nameAtPurchase|unitPrice|quantity\n";
    for (const auto &kv : orders)
    {
        const auto &o = kv.second;
        for (const auto &it : o.items)
        {
            outI << o.id << "|" << it.itemId << "|" << Utils::escape(it.nameAtPurchase) << "|"
                 << it.unitPrice << "|" << it.quantity << "\n";
        }
    }
}

void Store::computeNextIdsIfNeeded()
{
    int maxItem = 0;
    for (const auto &kv : items)
        maxItem = max(maxItem, Utils::toInt(kv.first, 0));
    int maxOrder = 0;
    for (const auto &kv : orders)
        maxOrder = max(maxOrder, Utils::toInt(kv.first, 0));
    if (nextItemId <= maxItem)
        nextItemId = maxItem + 1;
    if (nextOrderId <= maxOrder)
        nextOrderId = maxOrder + 1;
}

void Store::load()
{
    Utils::ensureDataDir();
    loadMeta();
    loadBuyers();
    loadSellers();
    loadItems();
    loadOrders();
    bankSystem.load();
    computeNextIdsIfNeeded();
}

void Store::save()
{
    Utils::ensureDataDir();
    saveMeta();
    saveBuyers();
    saveSellers();
    saveItems();
    saveOrders();
    bankSystem.save();
}

vector<Order> Store::transactionsLastKDays(int kDays) const
{
    vector<Order> v;
    for (const auto &kv : orders)
        if (Utils::isWithinLastDays(kv.second.timestamp, kDays))
            v.push_back(kv.second);
    sort(v.begin(), v.end(), [](const Order &a, const Order &b)
         { return a.timestamp < b.timestamp; });
    return v;
}

vector<Order> Store::paidNotCompleted() const
{
    vector<Order> v;
    for (const auto &kv : orders)
        if (kv.second.status == "paid")
            v.push_back(kv.second);
    sort(v.begin(), v.end(), [](const Order &a, const Order &b)
         { return a.timestamp < b.timestamp; });
    return v;
}

vector<pair<string, int>> Store::topMItems(int m) const
{
    unordered_map<string, int> qty;
    unordered_map<string, string> nameMap;
    for (const auto &kv : orders)
    {
        const auto &o = kv.second;
        if (!(o.status == "paid" || o.status == "complete"))
            continue;
        for (const auto &oi : o.items)
        {
            qty[oi.itemId] += oi.quantity;
            if (!nameMap.count(oi.itemId))
                nameMap[oi.itemId] = oi.nameAtPurchase;
        }
    }
    vector<pair<string, int>> vec;
    for (const auto &kv2 : qty)
    {
        string display = nameMap[kv2.first] + " (ID " + kv2.first + ")";
        vec.emplace_back(display, kv2.second);
    }
    sort(vec.begin(), vec.end(), [](auto &a, auto &b)
         {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first; });
    if ((int)vec.size() > m)
        vec.resize(m);
    return vec;
}

pair<vector<pair<string, int>>, vector<pair<string, int>>>
Store::mostActiveBuyersSellersToday(int topN) const
{
    string today = Utils::toDate(Utils::nowString());
    unordered_map<string, int> buyerCount;
    unordered_map<string, int> sellerCount;
    for (const auto &kv : orders)
    {
        const auto &o = kv.second;
        if (Utils::isSameDay(o.timestamp, today))
        {
            buyerCount[o.buyer]++;
            if (o.status == "complete")
                sellerCount[o.seller]++;
        }
    }
    vector<pair<string, int>> buyersVec(buyerCount.begin(), buyerCount.end());
    vector<pair<string, int>> sellersVec(sellerCount.begin(), sellerCount.end());
    auto sortDesc = [](auto &a, auto &b)
    {
        if (a.second != b.second)
            return a.second > b.second;
        return a.first < b.first;
    };
    sort(buyersVec.begin(), buyersVec.end(), sortDesc);
    sort(sellersVec.begin(), sellersVec.end(), sortDesc);
    if ((int)buyersVec.size() > topN)
        buyersVec.resize(topN);
    if ((int)sellersVec.size() > topN)
        sellersVec.resize(topN);
    return {buyersVec, sellersVec};
}

Bank &Store::bank() { return bankSystem; }

string Store::generateItemId() { return to_string(nextItemId++); }
string Store::generateOrderId() { return to_string(nextOrderId++); }
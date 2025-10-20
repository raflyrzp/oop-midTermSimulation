#include "Buyer.h"
#include "Store.h"
#include "Item.h"
#include "Order.h"
#include "Bank.h"
#include "Utils.h"
#include <unordered_map>
#include <algorithm>

using namespace std;

bool Buyer::registerBuyer(Store &store, Bank &bank,
                          const string &username,
                          const string &password,
                          const string &fullName)
{
    if (store.buyers.find(username) != store.buyers.end())
        return false;
    store.buyers[username] = Buyer(username, password, fullName, false);
    bank.ensureAccount(username);
    store.save();
    return true;
}

bool Buyer::login(Store &store, const string &username,
                  const string &password, Buyer &outBuyer)
{
    auto it = store.buyers.find(username);
    if (it == store.buyers.end())
        return false;
    if (it->second.password != password)
        return false;
    outBuyer = it->second;
    return true;
}

bool Buyer::getBuyer(Store &store, const string &username, Buyer &outBuyer)
{
    auto it = store.buyers.find(username);
    if (it == store.buyers.end())
        return false;
    outBuyer = it->second;
    return true;
}

vector<Item> Buyer::browseItems(Store &store)
{
    vector<Item> v;
    v.reserve(store.items.size());
    for (const auto &kv : store.items)
    {
        const auto &it = kv.second;
        if (it.visible && it.stock > 0)
            v.push_back(it);
    }
    sort(v.begin(), v.end(), [](const Item &a, const Item &b)
         {
        if (a.name != b.name) return a.name < b.name;
        return Utils::toInt(a.id, 0) < Utils::toInt(b.id, 0); });
    return v;
}

bool Buyer::getItem(Store &store, const string &itemId, Item &out)
{
    auto it = store.items.find(itemId);
    if (it == store.items.end())
        return false;
    out = it->second;
    return true;
}

bool Buyer::checkout(Store &store, Bank &bank,
                     const string &buyerUsername,
                     const vector<CartEntry> &cart,
                     string &message)
{
    if (cart.empty())
    {
        message = "Keranjang kosong.";
        return false;
    }

    unordered_map<string, vector<CartEntry>> bySeller;
    for (const auto &ce : cart)
    {
        auto it = store.items.find(ce.itemId);
        if (it == store.items.end() || !it->second.visible)
        {
            message = "Item tidak valid/tersembunyi.";
            return false;
        }
        if (ce.quantity <= 0)
        {
            message = "Kuantitas tidak valid.";
            return false;
        }
        if (it->second.stock < ce.quantity)
        {
            message = "Stok tidak cukup untuk item ID " + ce.itemId + " (" + it->second.name + ")";
            return false;
        }
        bySeller[it->second.seller].push_back(ce);
    }

    double grandTotal = 0.0;
    struct SellerBill
    {
        string seller;
        double total;
        vector<OrderItem> orderItems;
    };
    vector<SellerBill> bills;

    for (auto &kv : bySeller)
    {
        SellerBill bill{kv.first, 0.0, {}};
        for (const auto &ce : kv.second)
        {
            const auto &it = store.items.at(ce.itemId);
            OrderItem oi{it.id, it.name, it.price, ce.quantity};
            bill.total += it.price * ce.quantity;
            bill.orderItems.push_back(oi);
        }
        grandTotal += bill.total;
        bills.push_back(move(bill));
    }

    BankCustomer bc;
    if (!bank.getCustomer(buyerUsername, bc))
        bank.ensureAccount(buyerUsername);
    bool okGet = bank.getCustomer(buyerUsername, bc);
    if (!okGet || bc.balance < grandTotal)
    {
        message = "Saldo bank tidak mencukupi. Total: " + to_string(grandTotal);
        return false;
    }

    for (const auto &ce : cart)
    {
        store.items[ce.itemId].stock -= ce.quantity;
    }

    string ts = Utils::nowString();
    for (const auto &bill : bills)
    {
        if (!bank.transfer(buyerUsername, bill.seller, bill.total, "Payment for order"))
        {
            message = "Transfer ke penjual gagal.";
            for (const auto &ce : cart)
                store.items[ce.itemId].stock += ce.quantity;
            return false;
        }
        Order o;
        o.id = store.generateOrderId();
        o.buyer = buyerUsername;
        o.seller = bill.seller;
        o.timestamp = ts;
        o.status = "paid";
        o.items = bill.orderItems;
        o.totalAmount = bill.total;
        store.orders[o.id] = o;
    }

    store.save();
    message = "Checkout berhasil. Total pembayaran: " + to_string(grandTotal);
    return true;
}

vector<Order> Buyer::getBuyerOrders(Store &store,
                                    const string &buyerUsername,
                                    const string &statusFilter)
{
    vector<Order> v;
    bool useFilter = !statusFilter.empty() && statusFilter != "ALL";
    for (const auto &kv : store.orders)
    {
        const auto &o = kv.second;
        if (o.buyer == buyerUsername)
        {
            if (!useFilter || o.status == statusFilter)
                v.push_back(o);
        }
    }
    sort(v.begin(), v.end(), [](const Order &a, const Order &b)
         {
        if (a.timestamp != b.timestamp) return a.timestamp > b.timestamp;
        return Utils::toInt(a.id,0) > Utils::toInt(b.id,0); });
    return v;
}

bool Buyer::cancelOrderByBuyer(Store &store, Bank &bank,
                               const string &buyerUsername,
                               const string &orderId)
{
    auto it = store.orders.find(orderId);
    if (it == store.orders.end())
        return false;
    Order &o = it->second;
    if (o.buyer != buyerUsername)
        return false;
    if (o.status != "paid")
        return false;

    if (!bank.transfer(o.seller, o.buyer, o.totalAmount, "Refund for order #" + o.id))
        return false;

    for (const auto &oi : o.items)
    {
        auto itItem = store.items.find(oi.itemId);
        if (itItem != store.items.end())
            itItem->second.stock += oi.quantity;
    }
    o.status = "cancelled";
    store.save();
    return true;
}

double Buyer::spendingLastKDays(Bank &bank, const string &buyerUsername, int kDays)
{
    double sum = 0.0;
    for (const auto &t : bank.getUserTransactions(buyerUsername))
    {
        if (t.type == "TRANSFER_OUT" && Utils::isWithinLastDays(t.timestamp, kDays))
            sum += t.amount;
    }
    return sum;
}
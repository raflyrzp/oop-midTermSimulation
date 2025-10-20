#pragma once
#include <string>
#include <vector>

using namespace std;

struct OrderItem
{
    string itemId;
    string nameAtPurchase;
    double unitPrice = 0.0;
    int quantity = 0;
};

struct Order
{
    string id;
    string buyer;
    string seller;
    string timestamp;
    string status;
    vector<OrderItem> items;
    double totalAmount = 0.0;
};
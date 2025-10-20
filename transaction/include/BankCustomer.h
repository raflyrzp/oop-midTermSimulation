#pragma once
#include <string>

using namespace std;
struct BankCustomer
{
    string accountId;
    string username;
    double balance = 0.0;

    BankCustomer() = default;
    BankCustomer(string accId, string user, double bal)
        : accountId(move(accId)), username(move(user)), balance(bal) {}
};
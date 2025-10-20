#pragma once
#include "BankCustomer.h"
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct BankTransaction
{
    string id;
    string timestamp;
    string username;
    string type;
    double amount = 0.0;
    string counterparty;
    string note;
};

class Bank
{
public:
    Bank();

    void load();
    void save();

    BankCustomer ensureAccount(const string &username);
    bool getCustomer(const string &username, BankCustomer &out) const;
    vector<BankCustomer> listAllCustomers() const;

    bool deposit(const string &username, double amount, const string &note = "Deposit");
    bool withdraw(const string &username, double amount, const string &note = "Withdraw");
    bool transfer(const string &fromUser, const string &toUser, double amount, const string &note = "Transfer");

    vector<BankTransaction> getUserTransactions(const string &username) const;
    vector<BankTransaction> getAllTransactions() const;

    vector<BankTransaction> listTransactionsLastWeek() const;
    vector<BankCustomer> listDormantAccountsOneMonth() const;
    vector<pair<string, int>> topNUsersByTxToday(int n) const;

    void computeNextIdsIfNeeded();

private:
    unordered_map<string, BankCustomer> customersByUser;
    vector<BankTransaction> transactions;
    int nextAccountId = 1;
    int nextTxnId = 1;

    string generateAccountId();
    string generateTxnId();
    void recordTransaction(const BankTransaction &t);

    void loadMeta();
    void saveMeta() const;
};
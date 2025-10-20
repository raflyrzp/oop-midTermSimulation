#include "Bank.h"
#include "Utils.h"
#include <fstream>
#include <algorithm>
#include <unordered_map>

using namespace std;

Bank::Bank() { load(); }

void Bank::loadMeta()
{
    ifstream in("data/bank_meta.txt");
    if (!in)
        return;
    string line;
    while (getline(in, line))
    {
        auto parts = Utils::split(line, '|');
        if (parts.size() >= 2)
        {
            if (parts[0] == "nextAccountId")
                nextAccountId = Utils::toInt(parts[1], nextAccountId);
            if (parts[0] == "nextTxnId")
                nextTxnId = Utils::toInt(parts[1], nextTxnId);
        }
    }
}

void Bank::saveMeta() const
{
    ofstream out("data/bank_meta.txt", ios::trunc);
    out << "nextAccountId|" << nextAccountId << "\n";
    out << "nextTxnId|" << nextTxnId << "\n";
}

void Bank::load()
{
    Utils::ensureDataDir();
    loadMeta();
    customersByUser.clear();
    transactions.clear();

    {
        ifstream in("data/bank_customers.txt");
        string line;
        while (getline(in, line))
        {
            auto p = Utils::split(line, '|');
            if (p.size() < 4)
                continue;
            BankCustomer c;
            c.accountId = p[0];
            c.username = p[1];
            c.balance = Utils::toDouble(p[3], 0.0);
            customersByUser[c.username] = c;
        }
    }
    {
        ifstream in("data/bank_transactions.txt");
        string line;
        while (getline(in, line))
        {
            auto p = Utils::split(line, '|');
            if (p.size() < 7)
                continue;
            BankTransaction t;
            t.id = p[0];
            t.timestamp = p[1];
            t.username = p[2];
            t.type = p[3];
            t.amount = Utils::toDouble(p[4], 0.0);
            t.counterparty = p[5];
            t.note = Utils::unescape(p[6]);
            transactions.push_back(t);
        }
    }
    computeNextIdsIfNeeded();
}

void Bank::save()
{
    Utils::ensureDataDir();
    {
        ofstream out("data/bank_customers.txt", ios::trunc);
        out << "accountId|username|fullNameIgnored|balance\n";
        for (const auto &kv : customersByUser)
        {
            const auto &c = kv.second;
            out << c.accountId << "|" << c.username << "|-|" << c.balance << "\n";
        }
    }
    {
        ofstream out("data/bank_transactions.txt", ios::trunc);
        out << "id|timestamp|username|type|amount|counterparty|note\n";
        for (const auto &t : transactions)
        {
            out << t.id << "|" << t.timestamp << "|" << t.username << "|" << t.type
                << "|" << t.amount << "|" << t.counterparty << "|" << Utils::escape(t.note) << "\n";
        }
    }
    saveMeta();
}

BankCustomer Bank::ensureAccount(const string &username)
{
    auto it = customersByUser.find(username);
    if (it != customersByUser.end())
        return it->second;
    BankCustomer c(generateAccountId(), username, 0.0);
    customersByUser[username] = c;
    save();
    return c;
}

bool Bank::getCustomer(const string &username, BankCustomer &out) const
{
    auto it = customersByUser.find(username);
    if (it == customersByUser.end())
        return false;
    out = it->second;
    return true;
}

vector<BankCustomer> Bank::listAllCustomers() const
{
    vector<BankCustomer> v;
    v.reserve(customersByUser.size());
    for (const auto &kv : customersByUser)
        v.push_back(kv.second);
    sort(v.begin(), v.end(), [](const BankCustomer &a, const BankCustomer &b)
         { return a.username < b.username; });
    return v;
}

bool Bank::deposit(const string &username, double amount, const string &note)
{
    if (amount <= 0)
        return false;
    ensureAccount(username);
    customersByUser[username].balance += amount;
    BankTransaction t{generateTxnId(), Utils::nowString(), username, "DEPOSIT", amount, "-", note};
    recordTransaction(t);
    save();
    return true;
}

bool Bank::withdraw(const string &username, double amount, const string &note)
{
    if (amount <= 0)
        return false;
    auto it = customersByUser.find(username);
    if (it == customersByUser.end())
        return false;
    if (it->second.balance < amount)
        return false;
    it->second.balance -= amount;
    BankTransaction t{generateTxnId(), Utils::nowString(), username, "WITHDRAW", amount, "-", note};
    recordTransaction(t);
    save();
    return true;
}

bool Bank::transfer(const string &fromUser, const string &toUser, double amount, const string &note)
{
    if (amount <= 0)
        return false;
    auto itFrom = customersByUser.find(fromUser);
    if (itFrom == customersByUser.end())
        return false;
    ensureAccount(toUser);
    auto &from = itFrom->second;
    auto &to = customersByUser.at(toUser);
    if (from.balance < amount)
        return false;

    from.balance -= amount;
    to.balance += amount;

    string ts = Utils::nowString();
    BankTransaction outTx{generateTxnId(), ts, fromUser, "TRANSFER_OUT", amount, toUser, note};
    BankTransaction inTx{generateTxnId(), ts, toUser, "TRANSFER_IN", amount, fromUser, note};
    recordTransaction(outTx);
    recordTransaction(inTx);

    save();
    return true;
}

vector<BankTransaction> Bank::getUserTransactions(const string &username) const
{
    vector<BankTransaction> v;
    for (const auto &t : transactions)
        if (t.username == username)
            v.push_back(t);
    sort(v.begin(), v.end(), [](const BankTransaction &a, const BankTransaction &b)
         { return a.timestamp < b.timestamp; });
    return v;
}

vector<BankTransaction> Bank::getAllTransactions() const
{
    return transactions;
}

vector<BankTransaction> Bank::listTransactionsLastWeek() const
{
    vector<BankTransaction> v;
    for (const auto &t : transactions)
        if (Utils::isWithinLastWeeks(t.timestamp, 1))
            v.push_back(t);
    sort(v.begin(), v.end(), [](const BankTransaction &a, const BankTransaction &b)
         {
        if (a.timestamp != b.timestamp) return a.timestamp < b.timestamp;
        return a.id < b.id; });
    return v;
}

vector<BankCustomer> Bank::listDormantAccountsOneMonth() const
{
    unordered_map<string, string> lastTs;
    for (const auto &t : transactions)
    {
        auto it = lastTs.find(t.username);
        if (it == lastTs.end() || it->second < t.timestamp)
            lastTs[t.username] = t.timestamp;
    }
    vector<BankCustomer> dormant;
    for (const auto &kv : customersByUser)
    {
        const auto &user = kv.first;
        auto it = lastTs.find(user);
        bool isDormant = false;
        if (it == lastTs.end())
            isDormant = true;
        else
            isDormant = !Utils::isWithinLastDays(it->second, 30);
        if (isDormant)
            dormant.push_back(kv.second);
    }
    sort(dormant.begin(), dormant.end(), [](const BankCustomer &a, const BankCustomer &b)
         { return a.username < b.username; });
    return dormant;
}

vector<pair<string, int>> Bank::topNUsersByTxToday(int n) const
{
    string today = Utils::toDate(Utils::nowString());
    unordered_map<string, int> cnt;
    for (const auto &t : transactions)
        if (Utils::isSameDay(t.timestamp, today))
            cnt[t.username]++;
    vector<pair<string, int>> vec(cnt.begin(), cnt.end());
    sort(vec.begin(), vec.end(), [](auto &a, auto &b)
         {
        if (a.second != b.second) return a.second > b.second;
        return a.first < b.first; });
    if ((int)vec.size() > n)
        vec.resize(n);
    return vec;
}

void Bank::computeNextIdsIfNeeded()
{
    int maxAcc = 0;
    for (const auto &kv : customersByUser)
        maxAcc = max(maxAcc, Utils::toInt(kv.second.accountId, 0));
    int maxTxn = 0;
    for (const auto &t : transactions)
        maxTxn = max(maxTxn, Utils::toInt(t.id, 0));
    if (nextAccountId <= maxAcc)
        nextAccountId = maxAcc + 1;
    if (nextTxnId <= maxTxn)
        nextTxnId = maxTxn + 1;
}

string Bank::generateAccountId() { return to_string(nextAccountId++); }
string Bank::generateTxnId() { return to_string(nextTxnId++); }

void Bank::recordTransaction(const BankTransaction &t)
{
    transactions.push_back(t);
}
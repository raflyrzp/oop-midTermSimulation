#pragma once
#include <string>
#include <vector>
#include <chrono>

using namespace std;

namespace Utils
{

    void ensureDataDir();

    string trim(const string &s);
    string escape(const string &s);
    string unescape(const string &s);
    vector<string> split(const string &s, char delim);
    string join(const vector<string> &parts, char delim);

    string nowString();
    tm toTm(const string &ts);
    string toDate(const string &ts);
    string toMonth(const string &ts);
    bool isSameDay(const string &ts, const string &dayYYYYMMDD);
    bool isWithinLastDays(const string &ts, int days);
    bool isWithinLastWeeks(const string &ts, int weeks);
    bool isWithinMonth(const string &ts, const string &monthYYYYMM);

    int toInt(const string &s, int fallback = 0);
    double toDouble(const string &s, double fallback = 0.0);

    bool fileExists(const string &path);

    int promptInt(const string &label, int minVal, int maxVal);
    double promptDouble(const string &label, double minVal);
    string promptLine(const string &label);

}
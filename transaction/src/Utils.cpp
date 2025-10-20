#include "Utils.h"
#include <filesystem>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cctype>
#include <fstream>

using namespace std;

namespace Utils
{
    void ensureDataDir()
    {
        filesystem::create_directories("data");
    }

    string trim(const string &s)
    {
        size_t start = 0;
        while (start < s.size() && isspace(static_cast<unsigned char>(s[start])))
            start++;
        size_t end = s.size();
        while (end > start && isspace(static_cast<unsigned char>(s[end - 1])))
            end--;
        return s.substr(start, end - start);
    }

    string escape(const string &s)
    {
        string out;
        out.reserve(s.size());
        for (char c : s)
        {
            if (c == '|' || c == '\n' || c == '\r')
                out.push_back('/');
            else
                out.push_back(c);
        }
        return out;
    }

    string unescape(const string &s)
    {
        return s;
    }

    vector<string> split(const string &s, char delim)
    {
        vector<string> parts;
        stringstream ss(s);
        string item;
        while (getline(ss, item, delim))
            parts.push_back(item);
        return parts;
    }

    string join(const vector<string> &parts, char delim)
    {
        ostringstream oss;
        for (size_t i = 0; i < parts.size(); ++i)
        {
            if (i)
                oss << delim;
            oss << parts[i];
        }
        return oss.str();
    }

    string nowString()
    {
        using namespace chrono;
        auto now = system_clock::now();
        time_t t = system_clock::to_time_t(now);
        tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        ostringstream oss;
        oss << put_time(&tm, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }

    tm toTm(const string &ts)
    {
        tm tm{};
        istringstream iss(ts);
        iss >> get_time(&tm, "%Y-%m-%d %H:%M:%S");
        if (iss.fail())
        {
            istringstream iss2(ts);
            iss2 >> get_time(&tm, "%Y-%m-%d");
        }
        return tm;
    }

    string toDate(const string &ts)
    {
        tm tm = toTm(ts);
        ostringstream oss;
        oss << put_time(&tm, "%Y-%m-%d");
        return oss.str();
    }

    string toMonth(const string &ts)
    {
        tm tm = toTm(ts);
        ostringstream oss;
        oss << put_time(&tm, "%Y-%m");
        return oss.str();
    }

    bool isSameDay(const string &ts, const string &dayYYYYMMDD)
    {
        return toDate(ts) == dayYYYYMMDD;
    }

    static chrono::system_clock::time_point tmToTimePoint(tm tm)
    {
        time_t tt = mktime(&tm);
        return chrono::system_clock::from_time_t(tt);
    }

    bool isWithinLastDays(const string &ts, int days)
    {
        using namespace chrono;
        auto now = system_clock::now();
        auto start = now - hours(24 * days);
        auto t = tmToTimePoint(toTm(ts));
        return t >= start && t <= now;
    }

    bool isWithinLastWeeks(const string &ts, int weeks)
    {
        return isWithinLastDays(ts, weeks * 7);
    }

    bool isWithinMonth(const string &ts, const string &monthYYYYMM)
    {
        return toMonth(ts) == monthYYYYMM;
    }

    int toInt(const string &s, int fallback)
    {
        try
        {
            return stoi(s);
        }
        catch (...)
        {
            return fallback;
        }
    }

    double toDouble(const string &s, double fallback)
    {
        try
        {
            return stod(s);
        }
        catch (...)
        {
            return fallback;
        }
    }

    bool fileExists(const string &path)
    {
        return filesystem::exists(path);
    }

    int promptInt(const string &label, int minVal, int maxVal)
    {
        while (true)
        {
            cout << label << " [" << minVal << " - " << maxVal << "]: ";
            string s;
            getline(cin, s);
            s = trim(s);
            try
            {
                int v = stoi(s);
                if (v >= minVal && v <= maxVal)
                    return v;
            }
            catch (...)
            {
            }
            cout << "Input tidak valid. Coba lagi.\n";
        }
    }

    double promptDouble(const string &label, double minVal)
    {
        while (true)
        {
            cout << label << " (>= " << minVal << "): ";
            string s;
            getline(cin, s);
            s = trim(s);
            try
            {
                double v = stod(s);
                if (v >= minVal)
                    return v;
            }
            catch (...)
            {
            }
            cout << "Input tidak valid. Coba lagi.\n";
        }
    }

    string promptLine(const string &label)
    {
        cout << label << ": ";
        string s;
        getline(cin, s);
        return trim(s);
    }

}
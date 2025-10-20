#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

// Boost.Serialization
#include <boost/serialization/access.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>

using namespace std;

class User
{
private:
    friend class boost::serialization::access;

    int id;
    string name;
    string phone;
    int loginState;
    vector<User> contact;

    template <class Archive>
    void serialize(Archive &ar, const unsigned int /*version*/)
    {
        ar & id;
        ar & name;
        ar & phone;
        ar & loginState;
        ar & contact;
    }

    bool checkExistingContact(const string &phoneNumber) const
    {
        for (const auto &c : contact)
        {
            if (c.phone == phoneNumber)
            {
                return true;
            }
        }
        return false;
    }

    bool addContact(const User &user)
    {
        if (checkExistingContact(user.phone))
        {
            return false;
        }
        contact.push_back(user);
        return true;
    }

public:
    User() : id(0), name(""), phone(""), loginState(0) {}
    User(int inputId, string inputName, string inputPhone) : id(inputId), name(std::move(inputName)), phone(std::move(inputPhone)), loginState(0) {}
    User(string inputName, string inputPhone) : id(0), name(std::move(inputName)), phone(std::move(inputPhone)), loginState(0) {}

    // getter
    string getName() const { return name; }
    string getPhone() const { return phone; }
    int getLoginState() const { return loginState; }
    vector<User> getContact() const { return contact; }

    // setter
    void setName(string inputName) { name = std::move(inputName); }
    void setPhone(string inputPhone) { phone = std::move(inputPhone); }
    void setLoginState(int state) { loginState = state; }
    void setContact(vector<User> inputContact) { contact = std::move(inputContact); }

    // Helpers
    void addContactPrompt()
    {
        string nm, ph;
        while (true)
        {
            cout << "Enter contact name: ";
            cin >> nm;
            cout << "Enter contact phone: ";
            cin >> ph;

            if (checkExistingContact(ph))
            {
                cout << "Contact with this phone already exists. Try again? (y/n): ";
                char choice;
                cin >> choice;
                if (choice == 'y' || choice == 'Y')
                {
                    continue;
                }
                else
                {
                    break;
                }
            }
            addContact(User(nm, ph));
            cout << "Contact added successfully!" << endl;

            cout << "Add another? (y/n): ";
            char choice;
            cin >> choice;
            if (!(choice == 'y' || choice == 'Y'))
                break;
        }
    }

    void printContact() const
    {
        cout << "Contact List:" << endl;
        if (contact.empty())
        {
            cout << "Contact not found." << endl;
            return;
        }
        int i = 1;
        for (const auto &c : contact)
        {
            cout << i++ << ". " << c.name << " - " << c.phone << endl;
        }
    }

    bool checkUserExist(const string &phoneNumber) const
    {
        for (const auto &c : contact)
        {
            if (c.getPhone() == phoneNumber)
            {
                return true;
            }
        }
        return false;
    }

    User seekContact(const string &phoneNumber) const
    {
        for (const auto &c : contact)
        {
            if (c.getPhone() == phoneNumber)
            {
                return c;
            }
        }
        return User();
    }

    void updateContact(int contactId, const User &newUser)
    {
        for (auto &c : contact)
        {
            if (c.id == contactId)
            {
                c = newUser;
                cout << "Contact updated successfully!" << endl;
                return;
            }
        }
        cout << "Contact not found." << endl;
    }

    // Use Boost.Serialization to save/load
    void saveToFile(const string &filename) const
    {
        ofstream ofs(filename);
        if (!ofs)
        {
            cerr << "Failed to open file for serialization: " << filename << endl;
            return;
        }
        boost::archive::text_oarchive oa(ofs);
        oa << *this;
        cout << "Object serialized to " << filename << endl;
    }

    static User loadFromFile(const string &filename)
    {
        ifstream ifs(filename);
        if (!ifs)
        {
            cerr << "Error: Failed to open file for reading: " << filename << endl;
            return User();
        }
        boost::archive::text_iarchive ia(ifs);
        User obj;
        ia >> obj;
        cout << "Object deserialized from " << filename << endl;
        return obj;
    }
};

#endif // USER_H
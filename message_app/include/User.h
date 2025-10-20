#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

class User
{
private:
    int id;
    string name;
    string phone;
    int loginState;
    vector<User> contact;

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
    int getId() const { return id; }
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

    void deleteContact(int contactId)
    {
        for (auto it = contact.begin(); it != contact.end();)
        {
            if (it->id == contactId)
            {
                it = contact.erase(it);
                cout << "Contact deleted successfully!" << endl;
                return;
            }
            else
            {
                it++;
            }
        }
        cout << "Contact not found." << endl;
    }

    // ========== SAVE TO FILE ==========
    void saveToFile(const string &filename) const
    {
        ofstream file(filename);
        if (!file)
        {
            cerr << "Error opening file for writing!\n";
            return;
        }

        // Save main user data
        file << id << "|" << name << "|" << phone << "|" << loginState << "\n";

        // Save number of contacts
        file << contact.size() << "\n";

        // Save each contact
        for (const auto &c : contact)
        {
            file << c.id << "|" << c.name << "|" << c.phone << "\n";
        }
    }

    // ========== LOAD FROM FILE ==========
    static User loadFromFile(const string &filename)
    {
        ifstream file(filename);
        if (!file)
        {
            cerr << "Error opening file for reading!\n";
            return User();
        }

        User u;
        string line;

        // ----- Load main user -----
        if (!getline(file, line))
        {
            cerr << "File is empty or corrupted!\n";
            return User();
        }

        size_t p1 = line.find('|');
        size_t p2 = line.find('|', p1 + 1);
        size_t p3 = line.find('|', p2 + 1);

        u.id = stoi(line.substr(0, p1));
        u.name = line.substr(p1 + 1, p2 - p1 - 1);
        u.phone = line.substr(p2 + 1, p3 - p2 - 1);
        u.loginState = stoi(line.substr(p3 + 1));

        // ----- Load contacts -----
        int contactCount = 0;
        file >> contactCount;
        file.ignore();

        u.contact.clear();
        for (int i = 0; i < contactCount; i++)
        {
            getline(file, line);

            size_t a = line.find('|');
            size_t b = line.find('|', a + 1);

            User c;
            c.id = stoi(line.substr(0, a));
            c.name = line.substr(a + 1, b - a - 1);
            c.phone = line.substr(b + 1);

            u.contact.push_back(c);
        }

        return u;
    }
};

#endif // USER_H
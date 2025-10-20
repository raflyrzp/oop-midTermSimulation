#include <iostream>
#include "User.h"
#include <vector>
#include <fstream>

using namespace std;

enum PrimaryPrompt
{
    REGISTER,
    LOGIN,
    EXIT,
    MAIN_PROMPT
};
enum SubLoginPrompt
{
    USER_VERIFICATION,
    LOGIN_MENU
};
enum FeaturePrompt
{
    WHOAMI,
    LIST_CONTACT,
    ADD_CONTACT,
    SEEK_CONTACT,
    DELETE_CONTACT,
    LOGOUT,
    LOGIN_MENU_PROMPT
};

const string SAVE_FILE = "users.txt";

// Simpan semua user ke file
void saveAllUsers(const vector<User> &users)
{
    ofstream file(SAVE_FILE);
    if (!file)
    {
        cerr << "Failed to save data!" << endl;
        return;
    }
    file << users.size() << "\n";
    for (const auto &user : users)
    {
        user.saveToFile("temp_user.txt");

        ifstream temp("temp_user.txt");
        string line;
        while (getline(temp, line))
        {
            file << line << "\n";
        }
        file << "END_USER\n";
    }
    cout << "All users saved successfully!" << endl;
}

// Load semua user dari file
vector<User> loadAllUsers()
{
    vector<User> users;
    ifstream file(SAVE_FILE);
    if (!file)
        return users;

    int totalUsers;
    file >> totalUsers;
    file.ignore();

    for (int i = 0; i < totalUsers; i++)
    {
        ofstream temp("temp_user.txt");
        string line;
        while (getline(file, line) && line != "END_USER")
        {
            temp << line << "\n";
        }
        temp.close();

        users.push_back(User::loadFromFile("temp_user.txt"));
    }
    cout << "Loaded " << users.size() << " users from file." << endl;
    return users;
}

int main()
{
    cout << "Welcome to the Message App" << endl;

    PrimaryPrompt prompt = MAIN_PROMPT;
    SubLoginPrompt subLoginPrompt = USER_VERIFICATION;
    FeaturePrompt featurePrompt = WHOAMI;

    vector<User> users = loadAllUsers(); // load dari file
    User currentUser;

    while (true)
    {
        switch (prompt)
        {
        case MAIN_PROMPT:
        {
            cout << "\n--- Main Menu ---" << endl;
            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter choice: ";
            int mainChoice;
            cin >> mainChoice;
            prompt = static_cast<PrimaryPrompt>(mainChoice - 1);
            break;
        }
        case REGISTER:
        {
            string name, phone;
            cout << "Enter your name: ";
            cin >> name;
            cout << "Enter your phone: ";
            cin >> phone;
            users.emplace_back(users.size() + 1, name, phone);
            cout << "Registration successful!" << endl;
            prompt = MAIN_PROMPT;
            break;
        }
        case LOGIN:
            switch (subLoginPrompt)
            {
            case USER_VERIFICATION:
            {
                string name, phone;
                cout << "Enter name: ";
                cin >> name;
                cout << "Enter phone: ";
                cin >> phone;

                bool userExist = false;
                for (auto &u : users)
                {
                    if (u.getName() == name && u.getPhone() == phone)
                    {
                        currentUser = u;
                        currentUser.setLoginState(1);
                        subLoginPrompt = LOGIN_MENU;
                        userExist = true;
                        break;
                    }
                }
                if (!userExist)
                {
                    cout << "User not found!" << endl;
                    prompt = MAIN_PROMPT;
                }
                break;
            }
            case LOGIN_MENU:
                cout << "\n--- Login Menu ---" << endl;
                cout << "1. Who Am I" << endl;
                cout << "2. List Contacts" << endl;
                cout << "3. Add Contact" << endl;
                cout << "4. Seek Contact" << endl;
                cout << "5. Delete Contact" << endl;
                cout << "6. Logout" << endl;
                cout << "Enter choice: ";
                int loginChoice;
                cin >> loginChoice;
                featurePrompt = static_cast<FeaturePrompt>(loginChoice - 1);

                if (featurePrompt == WHOAMI)
                {
                    cout << "Name: " << currentUser.getName() << ", Phone: " << currentUser.getPhone() << endl;
                }
                else if (featurePrompt == LIST_CONTACT)
                {
                    currentUser.printContact();
                }
                else if (featurePrompt == ADD_CONTACT)
                {
                    currentUser.addContactPrompt();
                }
                else if (featurePrompt == SEEK_CONTACT)
                {
                    string phone;
                    cout << "Enter phone: ";
                    cin >> phone;
                    User found = currentUser.seekContact(phone);
                    if (found.getPhone() != "")
                    {
                        cout << "Found: " << found.getName() << " - " << found.getPhone() << endl;
                    }
                    else
                    {
                        cout << "No contact with that number." << endl;
                    }
                }
                else if (featurePrompt == DELETE_CONTACT)
                {
                    string phone;
                    cout << "Enter phone of contact to delete: ";
                    cin >> phone;
                    User toDelete = currentUser.seekContact(phone);
                    if (toDelete.getPhone() != "")
                    {
                        currentUser.deleteContact(toDelete.getId());
                    }
                    else
                    {
                        cout << "No contact with that number." << endl;
                    }
                }
                else if (featurePrompt == LOGOUT)
                {
                    currentUser.setLoginState(0);
                    subLoginPrompt = USER_VERIFICATION;
                    prompt = MAIN_PROMPT;
                }
                break;
            }
            break;
        case EXIT:
            saveAllUsers(users);
            cout << "Goodbye!" << endl;
            return 0;
        }
    }
}

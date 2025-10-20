#include <iostream>
#include "User.h"

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
    LOGOUT,
    LOGIN_MENU_PROMPT
};

void editPrompt(User &newUser, int id)
{
    cout << "Do you want to edit the contact with id " << id << "? (y/n): ";
    char choice;
    cin >> choice;
    if (choice == 'n' || choice == 'N')
    {
        return;
    }
    cout << endl;

    string name, phone;
    cout << "Enter new name: ";
    cin >> name;
    cout << "Enter new phone: ";
    cin >> phone;
    newUser.updateContact(id, User(name, phone));

    cout << "Contact updated successfully!" << endl;
    return;
}

int main()
{
    cout << "Welcome to the Message App" << endl;
    PrimaryPrompt prompt = MAIN_PROMPT;
    SubLoginPrompt subLoginPrompt = USER_VERIFICATION;
    FeaturePrompt featurePrompt = WHOAMI;

    const string serialname = "user_data.bin";
    User currentUser;
    vector<User> users;

    while (true)
    {
        switch (prompt)
        {
        case MAIN_PROMPT:
            cout << "Main Menu:" << endl;
            cout << "1. Register" << endl;
            cout << "2. Login" << endl;
            cout << "3. Exit" << endl;
            cout << "Enter your choice: ";
            int mainChoice;
            cin >> mainChoice;
            prompt = static_cast<PrimaryPrompt>(mainChoice - 1);
            break;
        case REGISTER:
        {
            string name, phone;
            cout << "Enter your name: ";
            cin >> name;
            cout << "Enter your phone number: ";
            cin >> phone;
            User newUser(name, phone);
            users.push_back(newUser);
            cout << "Registration successful! welcome " << name << endl;
            prompt = MAIN_PROMPT;
            break;
        }
        case LOGIN:
            switch (subLoginPrompt)
            {
            case USER_VERIFICATION:
            {
                string name, phone;
                cout << "Enter your name: ";
                cin >> name;
                cout << "Enter your phone number: ";
                cin >> phone;
                bool userExist = false;
                for (auto &u : users)
                {
                    if (u.getName() == name && u.getPhone() == phone)
                    {
                        currentUser = u;
                        userExist = true;
                        break;
                    }
                }

                if (userExist)
                {
                    cout << "Login successful! welcome " << currentUser.getName() << endl;
                    currentUser.setLoginState(1);
                    subLoginPrompt = LOGIN_MENU;
                    prompt = LOGIN;
                }
                else
                {
                    cout << "User not found. Please register first." << endl;
                    prompt = MAIN_PROMPT;
                    subLoginPrompt = USER_VERIFICATION;
                }
                break;
            }
            case LOGIN_MENU:
                cout << "Login Menu:" << endl;
                cout << "1. Who Am I" << endl;
                cout << "2. List Contacts" << endl;
                cout << "3. Add Contact" << endl;
                cout << "4. Seek Contact" << endl;
                cout << "5. Logout" << endl;
                cout << "Enter your choice: ";
                int loginChoice;
                cin >> loginChoice;
                featurePrompt = static_cast<FeaturePrompt>(loginChoice - 1);
                switch (featurePrompt)
                {
                case WHOAMI:
                    cout << "User Name: " << currentUser.getName() << endl;
                    cout << "User Phone: " << currentUser.getPhone() << endl;
                    break;
                case LIST_CONTACT:
                    currentUser.printContact();
                    break;
                case ADD_CONTACT:
                    currentUser.addContactPrompt();
                    break;
                case SEEK_CONTACT:
                {
                    string phone;
                    cout << "Enter contact phone to seek: ";
                    cin >> phone;
                    bool found = false;
                    for (auto &c : currentUser.getContact())
                    {
                        if (c.getPhone() == phone)
                        {
                            cout << "Contact found: " << c.getName() << " - " << c.getPhone() << endl;
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        cout << "Contact not found." << endl;
                    }
                    break;
                }
                case LOGOUT:
                    currentUser.setLoginState(0);
                    cout << "Logged out successfully." << endl;
                    prompt = MAIN_PROMPT;
                    subLoginPrompt = USER_VERIFICATION;
                    break;
                default:
                    cout << "Invalid choice." << endl;
                    break;
                }
                break;
            }
            break;
        case EXIT:
            cout << "Goodbye!" << endl;
            return 0;
        default:
            cout << "Invalid choice." << endl;
            break;
        }

        if (prompt == MAIN_PROMPT)
        {
            if (currentUser.getLoginState() == 1)
            {
                cout << "Do you want to logout? (y/n): ";
                char choice;
                cin >> choice;
                if (choice == 'y' || choice == 'Y')
                {
                    currentUser.setLoginState(0);
                    cout << "Logged out successfully." << endl;
                    prompt = MAIN_PROMPT;
                    subLoginPrompt = USER_VERIFICATION;
                }
            }
        }
    }
    return 0;
}
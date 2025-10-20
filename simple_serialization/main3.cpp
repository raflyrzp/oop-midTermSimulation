#include <iostream>
#include <fstream>
using namespace std;

struct Contact
{
    int id;
    char name[50];
    char phone[20];
};

void saveContact(const Contact &contact, const string &filename)
{
    ofstream file(filename, ios::binary | ios::app);
    if (!file)
    {
        cerr << "Error opening file for writing!" << endl;
        return;
    }
    file.write(reinterpret_cast<const char *>(&contact), sizeof(Contact));
    file.close();
}

void loadContacts(const string &filename)
{
    ifstream file(filename, ios::binary);
    if (!file)
    {
        cerr << "Error opening file for reading!" << endl;
        return;
    }

    Contact contact;
    while (file.read(reinterpret_cast<char *>(&contact), sizeof(Contact)))
    {
        cout << "ID: " << contact.id
             << ", Name: " << contact.name
             << ", Phone: " << contact.phone << endl;
    }
    file.close();
}

int main()
{
    Contact c1 = {1, "John Doe", "08123456789"};
    Contact c2 = {2, "Jane Smith", "08987654321"};

    saveContact(c1, "contacts.txt");
    saveContact(c2, "contacts.txt");

    cout << "Contacts Saved!\n\nLoaded Contacts:\n";
    loadContacts("contacts.txt");

    return 0;
}

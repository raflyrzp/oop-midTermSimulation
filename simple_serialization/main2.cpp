// #include "Store.h"
#include <fstream>

void Store::saveData()
{
    ofstream userFile("users.txt");
    for (int i = 0; i < allUsers.size(); i++)
    {
        userFile << allUsers[i]->get_username() << " " << allUsers[i]->get_password() << " " << allUsers[i]->getType() << endl;
    }
    userFile.close();
}

void Store::loadData()
{
    ifstream userFile("users.txt");
    string user, pass;
    char type;
    while (userFile >> user >> pass >> type)
    {
        if (type == 'S')
            allUsers.push_back(new Seller(user, pass));
        else
            allUsers.push_back(new Buyer(user, pass));
    }
    userFile.close();
}
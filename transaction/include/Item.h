#pragma once
#include <string>

using namespace std;

struct Item
{
  string id;
  string seller;
  string name;
  string description;
  double price = 0.0;
  int stock = 0;
  bool visible = true;

  Item() = default;
  Item(string id_, string seller_, string name_, string desc_, double price_, int stock_, bool vis)
      : id(move(id_)), seller(move(seller_)), name(move(name_)),
        description(move(desc_)), price(price_), stock(stock_), visible(vis) {}
};
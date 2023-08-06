#include <iostream>

#include "lib/regexp.h"

using namespace std;

int main(int, char **) {

  //   assert(argc == 3);

  //   auto regex = string{argv[1]};
  //   auto word = string{argv[2]};
  {
    auto match = create_matcher("a*b");

    if (match("")) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    if (match("b")) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    if (match("ab")) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    if (match("aaaaaaaab")) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }
  }

  {
    auto match = create_matcher("a?");

    if (match("")) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    if (match("b")) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }

    if (match("a")) {
      cout << "true" << endl;
    } else {
      cout << "false" << endl;
    }
  }
}
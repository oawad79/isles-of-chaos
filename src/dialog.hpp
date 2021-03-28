#ifndef SKYVAULT_DIALOG_HPP
#define SKYVAULT_DIALOG_HPP

#include <map>
#include <string>
#include <vector>

struct Choice {
  std::string message;
  std::string nextId;
};

struct DialogBranch {
  std::string message{""};
  std::string nextId{""};
  std::vector<Choice> choices{};
  int cursor{0};
};

struct DialogTree {
  std::map<std::string, DialogBranch> branches;
  std::string currentId{"start"};
};

#endif // SKYVAULT_DIALOG_HPP

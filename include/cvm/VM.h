#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "cvm/Bytecode.h"

namespace cvm {

struct Value {
  std::variant<double, bool> data;
};

std::string valueToString(const Value& value);
bool isTruthy(const Value& value);

class VM {
 public:
  Value execute(const Chunk& chunk);

 private:
  Value pop();
  const Value& peek() const;
  void push(Value value);
  double requireNumber(const Value& value, const std::string& context) const;

  std::vector<Value> stack_;
  std::unordered_map<std::string, Value> globals_;
};

}  // namespace cvm

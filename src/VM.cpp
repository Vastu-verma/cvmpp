#include "cvm/VM.h"

#include <cmath>
#include <iostream>
#include <stdexcept>

namespace cvm {

std::string valueToString(const Value& value) {
  if (std::holds_alternative<double>(value.data)) {
    double number = std::get<double>(value.data);
    if (std::floor(number) == number)
      return std::to_string(static_cast<long long>(number));
    return std::to_string(number);
  }
  return std::get<bool>(value.data) ? "true" : "false";
}

bool isTruthy(const Value& value) {
  if (std::holds_alternative<bool>(value.data))
    return std::get<bool>(value.data);
  return std::get<double>(value.data) != 0.0;
}

Value VM::execute(const Chunk& chunk) {
  stack_.clear();
  std::size_t ip = 0;
  Value lastValue{{0.0}};

  while (ip < chunk.code.size()) {
    const Instruction& instruction = chunk.code[ip];
    switch (instruction.opcode) {
      case OpCode::Constant: {
        const Constant& constant =
            chunk.constants.at(static_cast<std::size_t>(instruction.operand));
        if (std::holds_alternative<double>(constant))
          push({std::get<double>(constant)});
        else if (std::holds_alternative<bool>(constant))
          push({std::get<bool>(constant)});
        else
          throw std::runtime_error("VM error: string constant is not executable.");
        ++ip;
        break;
      }
      case OpCode::LoadGlobal: {
        const std::string& name = std::get<std::string>(
            chunk.constants.at(static_cast<std::size_t>(instruction.operand)));
        auto it = globals_.find(name);
        if (it == globals_.end())
          throw std::runtime_error("VM error: undefined variable '" + name + "'");
        push(it->second);
        ++ip;
        break;
      }
      case OpCode::StoreGlobal: {
        const std::string& name = std::get<std::string>(
            chunk.constants.at(static_cast<std::size_t>(instruction.operand)));
        globals_[name] = peek();
        ++ip;
        break;
      }
      case OpCode::Add: {
        double right = requireNumber(pop(), "addition");
        double left  = requireNumber(pop(), "addition");
        push({left + right}); ++ip; break;
      }
      case OpCode::Subtract: {
        double right = requireNumber(pop(), "subtraction");
        double left  = requireNumber(pop(), "subtraction");
        push({left - right}); ++ip; break;
      }
      case OpCode::Multiply: {
        double right = requireNumber(pop(), "multiplication");
        double left  = requireNumber(pop(), "multiplication");
        push({left * right}); ++ip; break;
      }
      case OpCode::Divide: {
        double right = requireNumber(pop(), "division");
        double left  = requireNumber(pop(), "division");
        if (right == 0.0) throw std::runtime_error("VM error: division by zero.");
        push({left / right}); ++ip; break;
      }
      case OpCode::Negate: {
        double value = requireNumber(pop(), "negation");
        push({-value}); ++ip; break;
      }
      case OpCode::Not:
        push({!isTruthy(pop())}); ++ip; break;
      case OpCode::Equal: {
        Value right = pop(); Value left = pop();
        push({left.data == right.data}); ++ip; break;
      }
      case OpCode::NotEqual: {
        Value right = pop(); Value left = pop();
        push({left.data != right.data}); ++ip; break;
      }
      case OpCode::Greater: {
        double right = requireNumber(pop(), "comparison");
        double left  = requireNumber(pop(), "comparison");
        push({left > right}); ++ip; break;
      }
      case OpCode::GreaterEqual: {
        double right = requireNumber(pop(), "comparison");
        double left  = requireNumber(pop(), "comparison");
        push({left >= right}); ++ip; break;
      }
      case OpCode::Less: {
        double right = requireNumber(pop(), "comparison");
        double left  = requireNumber(pop(), "comparison");
        push({left < right}); ++ip; break;
      }
      case OpCode::LessEqual: {
        double right = requireNumber(pop(), "comparison");
        double left  = requireNumber(pop(), "comparison");
        push({left <= right}); ++ip; break;
      }
      case OpCode::Jump:
        ip = static_cast<std::size_t>(instruction.operand); break;
      case OpCode::JumpIfFalse:
        if (!isTruthy(peek()))
          ip = static_cast<std::size_t>(instruction.operand);
        else
          ++ip;
        break;
      case OpCode::Print:
        lastValue = pop();
        std::cout << valueToString(lastValue) << "\n";
        ++ip; break;
      case OpCode::ReadInput: {
        std::string line;
        std::getline(std::cin, line);
        try {
          push({std::stod(line)});
        } catch (...) {
          throw std::runtime_error("VM error: input must be a number.");
        }
        ++ip; break;
      }
      case OpCode::Pop:
        lastValue = pop(); ++ip; break;
      case OpCode::Halt:
        if (!stack_.empty()) return stack_.back();
        return lastValue;
    }
  }

  if (!stack_.empty()) return stack_.back();
  return lastValue;
}

Value VM::pop() {
  if (stack_.empty()) throw std::runtime_error("VM error: stack underflow.");
  Value value = stack_.back();
  stack_.pop_back();
  return value;
}

const Value& VM::peek() const {
  if (stack_.empty()) throw std::runtime_error("VM error: empty stack.");
  return stack_.back();
}

void VM::push(Value value) { stack_.push_back(std::move(value)); }

double VM::requireNumber(const Value& value, const std::string& context) const {
  if (!std::holds_alternative<double>(value.data))
    throw std::runtime_error("VM error: expected number during " + context);
  return std::get<double>(value.data);
}

}  // namespace cvm

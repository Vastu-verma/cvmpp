#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "cvm/Compiler.h"
#include "cvm/Lexer.h"
#include "cvm/Parser.h"
#include "cvm/VM.h"

namespace {

struct Options {
  bool showAst = false;
  bool showBytecode = false;
  std::string filePath;
};

Options parseOptions(int argc, char* argv[]) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--ast") {
      options.showAst = true;
    } else if (arg == "--bytecode") {
      options.showBytecode = true;
    } else if (!arg.empty() && arg[0] == '-') {
      throw std::runtime_error("Unknown option: " + arg);
    } else {
      options.filePath = arg;
    }
  }
  return options;
}

std::string readFile(const std::string& path) {
  std::ifstream in(path);
  if (!in) {
    throw std::runtime_error("Could not open file: " + path);
  }
  std::ostringstream buffer;
  buffer << in.rdbuf();
  return buffer.str();
}

void runSource(const std::string& source, const Options& options, cvm::VM& vm,
               bool printFinalResult) {
  cvm::Lexer lexer(source);
  std::vector<cvm::Token> tokens = lexer.scanTokens();

  cvm::Parser parser(tokens);
  std::vector<cvm::StmtPtr> program = parser.parse();

  if (options.showAst) {
    std::cout << "== AST ==\n" << cvm::debugProgram(program) << "\n";
  }

  cvm::Compiler compiler;
  cvm::Chunk chunk = compiler.compile(program);

  if (options.showBytecode) {
    std::cout << cvm::disassemble(chunk);
  }

  cvm::Value result = vm.execute(chunk);
  if (printFinalResult) {
    std::cout << "Result: " << cvm::valueToString(result) << "\n";
  }
}

bool isLikelyComplete(const std::string& source) {
  int braceBalance = 0;
  int parenBalance = 0;
  for (char c : source) {
    if (c == '{') ++braceBalance;
    if (c == '}') --braceBalance;
    if (c == '(') ++parenBalance;
    if (c == ')') --parenBalance;
  }

  if (braceBalance != 0 || parenBalance != 0) {
    return false;
  }

  for (std::size_t i = source.size(); i > 0; --i) {
    char c = source[i - 1];
    if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
      continue;
    }
    return c == ';' || c == '}';
  }

  return false;
}

void startRepl(const Options& options) {
  cvm::VM vm;
  std::string line;
  std::string buffer;

  std::cout << "CVM++ REPL\n";
  std::cout << "Type 'exit' to quit.\n";

  while (true) {
    std::cout << (buffer.empty() ? "cvm> " : "...> ");
    if (!std::getline(std::cin, line)) {
      std::cout << "\n";
      break;
    }
    if (buffer.empty() && line == "exit") {
      break;
    }

    buffer += line + "\n";
    if (!isLikelyComplete(buffer)) {
      continue;
    }

    try {
      runSource(buffer, options, vm, false);
    } catch (const std::exception& ex) {
      std::cerr << ex.what() << "\n";
    }
    buffer.clear();
  }
}

}  // namespace

int main(int argc, char* argv[]) {
  try {
    Options options = parseOptions(argc, argv);
    if (options.filePath.empty()) {
      startRepl(options);
      return 0;
    }

    cvm::VM vm;
    runSource(readFile(options.filePath), options, vm, true);
    return 0;
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << "\n";
    return 1;
  }
}

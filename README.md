# CVM++

CVM++ is a small educational language toolchain written in C++:

- `Lexer`: converts source text into tokens
- `Parser`: builds an Abstract Syntax Tree (AST)
- `Compiler`: lowers the AST into custom bytecode
- `VM`: executes bytecode on a stack-based virtual machine

## Language Features

- Numeric literals: `10`, `3.14`
- Boolean literals: `true`, `false`
- Variables: `let x = 5;`, `x = x + 1;`
- Arithmetic: `+`, `-`, `*`, `/`
- Line comments: `// comment`
- Comparison: `<`, `<=`, `>`, `>=`
- Equality: `==`, `!=`
- Unary operators: `-`, `!`
- Statements: `print`, expression statements
- Control flow: `if / else`, `while`
- Blocks: `{ ... }`

## Project Structure

- `include/cvm/`: headers
- `src/`: implementation
- `examples/`: sample scripts

## Build

```bash
cd /Users/vastu/Downloads/vastu.cvmm
make
```

## Run

Run a script file:

```bash
./cvmpp examples/basic.cvm
```

Start the REPL:

```bash
./cvmpp
```

## Debug Modes

Print AST:

```bash
./cvmpp --ast examples/control_flow.cvm
```

Print bytecode:

```bash
./cvmpp --bytecode examples/control_flow.cvm
```

Print both:

```bash
./cvmpp --ast --bytecode examples/control_flow.cvm
```

## Example

```c
let x = 5;
let sum = 0;

while (x > 0) {
  sum = sum + x;
  x = x - 1;
}

print sum;
```

Output:

```text
15
Result: 15
```


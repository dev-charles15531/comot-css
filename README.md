
# **Comot-CSS**

A fast, efficient, and flexible CSS tokenizer that is **fail-safe** and **W3C compliant**. Designed for maximum performance, **Comot-CSS** parses CSS input while handling edge cases gracefully. It is an ideal choice for embedded systems, web tools, or any application needing a reliable and robust CSS tokenizer.

---

## **Features**

* **Efficient CSS Tokenization**: A high-performance tokenizer that parses CSS input with minimal overhead.
* **Fail-Safe Design**: **Comot-CSS** is built to handle malformed or unexpected CSS gracefully without crashing, making it suitable for production environments where stability is key.
* **W3C Compliant**: Fully compliant with the W3C CSS specification, ensuring that it accurately tokenizes valid CSS as well as handles invalid or malformed CSS according to the specification.
* **Error Handling**: Comprehensive error tracking and reporting mechanisms that log parsing issues with detailed information on line and column numbers.
* **Extensibility**: Designed with modularity in mind, allowing for easy extensions and modifications.
* **Memory Efficiency**: Optimized for handling large CSS files with minimal memory usage, leveraging **arena allocation** for fast memory management.
* **Fuzz and Unit Testing**: Includes both unit and fuzz tests to ensure stability and correctness across a variety of edge cases.
* **Cross-Platform**: Compatible with all platforms that support C, including Linux, macOS, and Windows.

---

## **Getting Started**

### **Prerequisites**

Before you begin, ensure you have the following dependencies:

* **CMake (v3.15 or higher)** for building the project
* **Git** to clone the repository
* **A C Compiler** (GCC, Clang, etc.)

This project uses `arena-allocator` as a submodule for efficient memory allocation, which will be fetched automatically via CMake.

---

### **Installation**

To install and build **Comot-CSS**, follow these steps:

1. Clone the repository:

   ```bash
   git clone https://github.com/dev-charles15531/comot-css.git
   cd comot-css
   ```

2. Create a build directory:

   ```bash
   mkdir build
   cd build
   ```

3. Run CMake to configure the project:

   ```bash
   cmake ..
   ```

4. Build the project:

   ```bash
   cmake --build .
   ```

5. (Optional) To build tests:

   ```bash
   cmake --build . --target tests
   ```

---

## **Project Structure**

The **Comot-CSS** project is organized as follows:

```
comot-css/
├── CMakeLists.txt              # Root CMake configuration
├── LICENSE                     # Project license
├── README.md                   # Project documentation
├── examples/                   # Example projects using the tokenizer
│   ├── main.c                  # Main example code
│   └── CMakeLists.txt          # CMake configuration for examples
├── include/                    # Public headers
│   ├── comot-css/              # All the public header files
│   │   ├── diag.h
│   │   ├── error.h
│   │   ├── tokenizer.h
│   │   └── tokens.h
├── src/                        # Core tokenizer implementation
│   ├── CMakeLists.txt          # CMake configuration for source files
│   ├── tokenizer/              # Tokenizer-related files
│   └── utils/                  # Utility functions (error handling, etc.)
├── tests/                      # Unit and fuzz tests
│   ├── unit/                   # Unit tests for the tokenizer
│   ├── fuzz/                   # Fuzz testing for edge cases
├── run_fuzz_test.sh            # Script to run fuzz tests
└── run_unit_test.sh            # Script to run unit tests
```

* **`include/`** contains public header files for the tokenizer and utility functions.
* **`src/`** includes the core implementation of the tokenizer and utility code.
* **`tests/`** contains unit and fuzz tests to ensure correctness and robustness.

---

## **Core Concepts**

### **Tokenizer**

The core of **Comot-CSS** is the **Tokenizer**—a finite state machine (FSM) designed to tokenize CSS input in compliance with the W3C specification. The tokenizer ensures that the CSS is broken down into tokens that represent valid CSS syntax (e.g., selectors, properties, values) and can also handle errors gracefully, logging them without causing a crash.

### **W3C Compliance & Fail-Safe Design**

**Comot-CSS** ensures full **W3C compliance**, adhering strictly to the CSS tokenization rules as defined in the specification. In addition, it has been designed to be **fail-safe**—even if the input is invalid or malformed, the tokenizer handles such cases without crashing. Instead, it will gracefully log the error and continue processing, providing robust handling of edge cases and malformed CSS.

---

### **Token Types**

The **Comot-CSS** tokenizer supports all the token types defined by the **W3C CSS specification**. Below is the complete list of supported token types:

* **`TOKEN_IDENT`**: Identifiers, e.g., `body`, `#main`.
* **`TOKEN_FUNCTION`**: Function names, e.g., `rgb()`, `url()`.
* **`TOKEN_AT_KEYWORD`**: At-rule keywords, e.g., `@media`, `@import`.
* **`TOKEN_HASH`**: Hashes (used for IDs), e.g., `#element`.
* **`TOKEN_STRING`**: String literals, e.g., `"Hello World"`, `'Hello'`.
* **`TOKEN_BAD_STRING`**: Malformed string literals, e.g., `"Hello World`.
* **`TOKEN_URL`**: URL values, e.g., `url('image.png')`.
* **`TOKEN_BAD_URL`**: Malformed URL values, e.g., `url( image.png )`.
* **`TOKEN_DELIM`**: Delimiters, e.g., `;`, `:`, `,`, `()`, `[]`.
* **`TOKEN_NUMBER`**: Numeric values, e.g., `1`, `1px`, `100%`.
* **`TOKEN_PERCENTAGE`**: Percentage values, e.g., `50%`, `100%`.
* **`TOKEN_DIMENSION`**: Dimension values (numbers with units), e.g., `10px`, `2em`.
* **`TOKEN_WHITESPACE`**: Whitespace, e.g., spaces, tabs, newlines.
* **`TOKEN_COMMENT`**: CSS comments, e.g., `/* This is a comment */`.
* **`TOKEN_COLON`**: Colon used in property definitions, e.g., `color: red;`.
* **`TOKEN_SEMICOLON`**: Semicolon used in separating declarations, e.g., `color: red;`.
* **`TOKEN_COMMA`**: Comma used in value lists, e.g., `red, blue, green`.
* **`TOKEN_LEFT_PAREN`**: Left parenthesis `(`.
* **`TOKEN_RIGHT_PAREN`**: Right parenthesis `)`.
* **`TOKEN_LEFT_SQUARE`**: Left square bracket `[`.
* **`TOKEN_RIGHT_SQUARE`**: Right square bracket `]`.
* **`TOKEN_LEFT_CURLY`**: Left curly brace `{`.
* **`TOKEN_RIGHT_CURLY`**: Right curly brace `}`.
* **`TOKEN_CDO`**: Character sequence `<!--` (start of comment).
* **`TOKEN_CDC`**: Character sequence `-->` (end of comment).
* **`TOKEN_EOF`**: End of file, indicating the end of the input.
* **`TOKEN_ERROR`**: Error token, generated when the tokenizer encounters an invalid or unrecognized character sequence.

---

## **Testing**

This project includes both unit tests and fuzz tests to ensure the correctness of the tokenizer.

### **Running Unit Tests**

To run the unit tests, simply execute the following script:

```bash
./run_unit_test.sh
```
This script will automatically build the project (if necessary) and execute the unit tests.

### **Running Fuzz Tests**

To run the fuzz tests, execute the following script:

```bash
./run_fuzz_test.sh
```
This script will automatically build the project (if necessary) and run the fuzz tests, ensuring that edge cases are handled correctly.

---

## **Contributing**

We welcome contributions to **Comot-CSS**. To get started, please fork the repository and create a pull request. Ensure your changes are covered by tests, and adhere to the following guidelines:

1. **Code Quality**: Follow C coding conventions and style guidelines.
2. **Documentation**: Update the README or comments when introducing new features or significant changes.
3. **Testing**: Ensure that your code is covered by unit or fuzz tests.

---

## **License**

This project is licensed under the **MIT License** - see the [LICENSE](LICENSE) file for details.

---

## **Dependency**

* **arena-allocator**: A fast and efficient memory allocator for C that is used to manage memory in this project.
  [GitHub Repository](https://github.com/dev-charles15531/arena-allocator)

---

### **Contact**

Feel free to open an issue or reach out if you have any questions or suggestions!
[dev-charles15531](https://github.com/dev-charles15531)

---

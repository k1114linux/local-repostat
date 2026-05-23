# local-repostat — Repository Language Statistics Analyzer
**local-repostat** is a lightweight repository analysis tool written in C.
It scans a project directory and generates a colorful language breakdown similar to GitHub Linguist statistics directly in your terminal.

It automatically detects programming languages by file extension, respects `.gitignore` and `.gitattributes`, skips vendored/generated files, and displays repository size, lines of code, and per-language usage statistics.

---

## Features
* Recursively scans repositories and source trees.
* Detects hundreds of programming and markup languages.
* Respects `.gitignore` exclusion rules.
* Supports `.gitattributes` Linguist overrides:
  * `linguist-language`
  * `linguist-generated`
  * `linguist-vendored`
  * `linguist-documentation`
  * `linguist-detectable=false`
* Automatically skips:
  * binary files
  * vendored dependencies
  * ignored paths
* Displays:
  * total repository size
  * lines of code
  * file count
  * language distribution
* Renders a colored terminal language bar similar to GitHub.
* Minimal dependencies — only standard POSIX/Linux libraries.

---

## Requirements
* **Linux / POSIX-compatible system**
* **GCC** or any C99-compatible compiler

---

## Build
```bash
gcc -O2 -o repostat repostat.c
```
Or with `make`:
```bash
make
```

---

## Usage
```bash
./repostat <directory>
```
If no directory is specified, the current directory is scanned.

---

## How It Works
1. **Directory Walker:** Recursively traverses the repository tree.
2. **Ignore Engine:** Parses and applies `.gitignore` rules.
3. **Attribute Parser:** Reads `.gitattributes` Linguist attributes.
4. **Language Detector:** Maps file extensions to known languages.
5. **Statistics Engine:** Counts:
   * bytes
   * lines
   * files
6. **Renderer:** Produces a colorful terminal summary and language graph.

---

## Language Detection
Languages are identified primarily through:
* file extensions
* special filenames:
  * `Makefile`
  * `Dockerfile`
  * `CMakeLists.txt`

Unknown file types are grouped under:
```text
Other
```

---

## Ignored & Skipped Files
The following are automatically skipped:
* Binary files
* `.gitignore` matches
* Vendored dependencies:
  * `node_modules`
  * `vendor`
  * `third_party`
  * `external`
  * `.git`
  * and similar directories

Files marked in `.gitattributes` as generated or non-detectable are also excluded.

---

## Supported Linguist Attributes
Example `.gitattributes`:
```gitattributes
*.generated linguist-generated
vendor/** linguist-vendored
docs/** linguist-documentation
special.ext linguist-language=Rust
```

---

## Output
repostat displays:
* Total repository size
* Total lines of code
* Total source files
* Number of detected languages
* Number of skipped files
* Per-language statistics:
  * percentage
  * lines
  * size
* ANSI-colored language distribution bar

---

## Design Goals
* Fast startup
* Minimal memory usage
* No external dependencies
* GitHub-like repository statistics in a single binary
* Portable C implementation

---

## License
Apache-2.0 license.

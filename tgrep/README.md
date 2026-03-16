# tgrep

A tuned `grep` built in C using PCRE2 for full Perl-compatible regex support.

## Requirements

```sh
sudo apt install libpcre2-dev
```

## Build

| Command | Description |
|---|---|
| `make` / `make release` | Optimized release build (`-O2`), binary: `tgrep` |
| `make debug` | Debug build (`-g3 -O0`), binary: `tgrep_debug` |
| `make re` | Full rebuild (release) |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binaries |

## Install / Uninstall

```sh
sudo make install      # installs to /usr/local/bin/tgrep
sudo make uninstall
```

## Usage

```
tgrep [OPTIONS] PATTERN [FILE...]
tgrep [OPTIONS] -e PATTERN [-e PATTERN...] [FILE...]
```

## Options

### Match
| Flag | Description |
|---|---|
| `-e PATTERN` / `--regexp` | Add a search pattern (multiple allowed — OR logic) |
| `-i` / `--ignore-case` | Case insensitive |
| `-S` / `--smart-case` | Insensitive when pattern is all lowercase |
| `-w` / `--word-regexp` | Whole word match only |
| `-x` / `--line-regexp` | Whole line match only |
| `-v` / `--invert-match` | Show non-matching lines |

### Output
| Flag | Description |
|---|---|
| `-n` / `--line-number` | Show line numbers |
| `-o` / `--only-matching` | Print only the matched part (one per line) |
| `-c` / `--count` | Count matches per file |
| `-l` / `--files-with-matches` | List files with at least one match |
| `-L` / `--files-without-match` | List files with no matches |
| `-H` / `--with-filename` | Always show filename prefix |
| `--no-filename` | Never show filename prefix |
| `-Z` / `--null` | NUL-separated output (for `xargs -0`) |
| `--color[=WHEN]` | Color: `auto` (default), `always`, `never` |
| `--json` | Output matches as JSON objects (one per line) |
| `--stats` | Print search statistics to stderr after finishing |

### Context
| Flag | Description |
|---|---|
| `-A N` / `--after-context N` | N lines after each match |
| `-B N` / `--before-context N` | N lines before each match |
| `-C N` / `--context N` | N lines before and after each match |

### Files
| Flag | Description |
|---|---|
| `-r` / `--recursive` | Recursively search directories |
| `-m N` / `--max-count N` | Stop after N matches per file |
| `--include GLOB` | Only search files matching glob (e.g. `"*.c"`) |
| `--exclude-dir DIR` | Skip directories matching name (e.g. `.git`) |
| `-s` / `--no-messages` | Suppress file error messages |

## Examples

```sh
# basic search
tgrep "main" src/main.c

# PCRE2 — match function calls
tgrep "\w+\(.*\)" src/main.c

# case insensitive with line numbers
tgrep -in "error" *.log

# smart case (auto case-insensitive when all lowercase)
tgrep -S "parse" src/parse_args.c

# multiple patterns (OR)
tgrep -e "malloc" -e "free" src/matcher_create.c

# only the matched text
tgrep -o "[0-9]+" src/search_file.c

# context lines with separator
tgrep -C3 "handle_match" src/search_file.c

# recursive search, C files only, skip .git
tgrep -r --include "*.c" --exclude-dir ".git" "TODO" .

# JSON output
tgrep --json "error" src/main.c

# count per file with stats
tgrep -rc "int" src/ --stats

# list files with matches, NUL-separated (for xargs)
tgrep -rl -Z "TODO" . | xargs -0 ls -l
```

## Exit codes

| Code | Meaning |
|---|---|
| `0` | At least one match found |
| `1` | No matches found |
| `2` | Error (bad pattern, unreadable file, etc.) |

## How it works

1. Patterns are compiled with PCRE2 (`-DPCRE2_CODE_UNIT_WIDTH=8`), optionally wrapped with `\b...\b` (-w) or `^...$` (-x).
2. Files are read line-by-line with `getline()`. Binary files (detected by null bytes) are skipped.
3. Each line is matched against all patterns; match intervals are sorted and merged for clean highlighting.
4. Before-context is managed with a circular ring buffer; after-context with a countdown counter. A `--` separator is printed between non-adjacent output groups.
5. Output supports plain text with ANSI color, `-o` (only matching), and `--json` modes.

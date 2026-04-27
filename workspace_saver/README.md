# workspace_saver

Saves a named workspace as a list of directories under `~/.config/workspace_saver/`.

## Build

| Command | Description |
|---|---|
| `make` / `make release` | Optimized release build (`-O2`), binary: `workspace_saver` |
| `make debug` | Debug build (`-g3 -O0`), binary: `workspace_saver_debug` |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binaries |
| `make re` | Full rebuild (release) |

## Install / Uninstall

```sh
sudo make install
sudo make uninstall
```

## Usage

```sh
./workspace_saver [--ui] <workspace_name> [directory ...]
```

- If no directory is provided, the current directory is saved.
- Workspace names accept letters, numbers, `_` and `-`.
- `--ui` opens a terminal UI (ncurses) to manage folders before saving.

UI controls:

- `a`: add folder path
- `d`: remove selected folder
- `↑/↓`: change selection
- `s` or `Enter`: save
- `q` or `Esc`: quit without saving

Saved file location:

```txt
~/.config/workspace_saver/<workspace_name>.workspace
```

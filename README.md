# softwares

A collection of C utilities built with safety, no memory leaks, and POSIX compliance in mind.

## Projects

| Project | Description | Docs |
|---|---|---|
| [port_scanner](./port_scanner) | Lists open TCP/UDP ports with their PID and process name | [README](./port_scanner/README.md) |
| [tgrep](./tgrep) | Tuned grep with PCRE2 regex, color, context, JSON output and more | [README](./tgrep/README.md) |
| [workspace_saver](./workspace_saver) | Saves named workspaces to `~/.config/workspace_saver/` with optional terminal UI folder manager | [README](./workspace_saver/README.md) |
| [btop](./btop) | Resource monitor (CPU, MEM, NET, DISK) — btop++ v1.3.2 submodule | [README](./btop/README.md) |

## Build all

```sh
make          # build all projects
make clean    # remove all object files
make fclean   # remove all object files and binaries
make re       # full rebuild
```

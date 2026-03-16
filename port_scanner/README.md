# port_scanner

Lists all open TCP and UDP ports on the system, along with the PID and process name associated with each socket.

## Build

| Command | Description |
|---|---|
| `make` / `make release` | Optimized release build (`-O2`), binary: `port_scanner` |
| `make debug` | Debug build (`-g3 -O0`), binary: `port_scanner_debug` |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and binaries |
| `make re` | Full rebuild (release) |

## Install / Uninstall

Installs the release binary system-wide to `/usr/local/bin`:

```sh
sudo make install
```

```sh
sudo make uninstall
```

## Run

```sh
./port_scanner
```

Filter by a specific port with `-p` or `--port`:

```sh
./port_scanner -p 22
./port_scanner --port 8080
```

> **Note:** Ports owned by other users (e.g. root services) will show `?` for PID and process name.
> Run with `sudo` to resolve all entries:

```sh
sudo ./port_scanner -p 22
```

## Example output

```
Proto  Local Address    Port   State         PID     Process
-----  -------------    ----   -----         ---     -------
TCP    0.0.0.0          22     LISTEN        ?       <?>
TCP    10.95.4.11       48486  ESTABLISHED   709445  <claude>
UDP    0.0.0.0          5353   -             ?       <?>
```

## How it works

1. Parses `/proc/net/tcp` and `/proc/net/udp` to collect open sockets (local address, port, state, inode).
2. Walks every `/proc/<PID>/fd/` directory and resolves `socket:[inode]` symlinks to build an inode→PID map.
3. Reads `/proc/<PID>/comm` to get the process name for each resolved PID.
4. Prints a formatted table to stdout.

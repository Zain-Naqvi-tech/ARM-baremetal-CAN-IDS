import sys
import threading
import serial
from rich.live import Live
from rich.table import Table
from rich.console import Console
from rich import box

try:
    import msvcrt                      # Windows: read single keystrokes, no Enter, no echo
except ImportError:
    msvcrt = None

# ----------------------------------------------------------------------
PORT = "COM5"
BAUD = 115200

KNOWN = {
    0x100: ("Engine RPM",    "rpm"),
    0x200: ("Throttle",      "%"),
    0x300: ("Vehicle Speed", "km/h"),
    0x400: ("Coolant Temp",  "C"),
}

STATUS_STYLE = {
    "OK":         "green",
    "OVER_RANGE": "bold red",
    "TOO_FAST":   "bold yellow",
    "MISSING":    "bold magenta",
    "UNKNOWN_ID": "bold red",
    "SPIKE":      "bold blue",
}

# Single-char commands the firmware's UART-RX switch understands
COMMANDS = {
    "f": "TOO_FAST (throttle)",
    "o": "OVER_RANGE (RPM)",
    "u": "UNKNOWN_ID (0x500)",
    "s": "SPIKE (coolant)",
}

state = {}                # CAN ID -> latest record (owned by the reader thread)
last_command = "-"        # shown in the footer (set by the keyboard thread)


def parse_line(line):
    parts = line.strip().split(",")
    if len(parts) < 5 or parts[0] != "RX":
        return None
    try:
        can_id  = int(parts[1], 16)
        dlc     = int(parts[2])
        status  = parts[-2]
        ts      = int(parts[-1])
        payload = parts[3:-2]          # variable length == dlc bytes
        return can_id, dlc, payload, status, ts
    except (ValueError, IndexError):
        return None


def decode_value(payload):
    # value = (byte0 << 8) | byte1, matching the firmware's packing
    if len(payload) >= 2:
        try:
            hi = int(payload[0], 16)
            lo = int(payload[1], 16)
            return (hi << 8) | lo
        except ValueError:
            return None
    return None


def update_state(parsed):
    can_id, dlc, payload, status, ts = parsed
    name, unit = KNOWN.get(can_id, ("UNKNOWN", ""))
    if can_id in KNOWN:
        val = decode_value(payload)
        value_str = f"{val} {unit}" if val is not None else "-"
    else:
        value_str = "-"
    raw = " ".join(payload) if payload else "(none)"
    prev_count = state.get(can_id, {}).get("count", 0)
    state[can_id] = {
        "name": name, "value": value_str, "raw": raw,
        "status": status, "ts": ts, "count": prev_count + 1,
    }


def build_table():
    table = Table(
        title="CAN Bus IDS  -  Live Monitor",
        box=box.HEAVY_HEAD,
        header_style="bold cyan",
        caption=(
            "[f] too-fast    [o] over-range    [u] unknown-id    [s] spike    [q] quit"
            f"        last sent: [bold]{last_command}[/bold]"
        ),
        expand=False,
    )
    table.add_column("CAN ID")
    table.add_column("Signal")
    table.add_column("Value", justify="right")
    table.add_column("Raw bytes (hex)")
    table.add_column("Status")
    table.add_column("Last seen (ms)", justify="right")
    table.add_column("Frames", justify="right")
    for can_id in sorted(state):
        rec = state[can_id]
        style = STATUS_STYLE.get(rec["status"], "white")
        table.add_row(
            f"0x{can_id:03X}", rec["name"], rec["value"], rec["raw"],
            f"[{style}]{rec['status']}[/{style}]", str(rec["ts"]), str(rec["count"]),
        )
    return table


def reader_loop(ser, live):
    """Background thread: blocking reads give complete lines; update + redraw."""
    while True:
        try:
            raw = ser.readline().decode("ascii", errors="ignore")
        except serial.SerialException:
            break
        if not raw:
            continue                   # 1s timeout tick, no data
        parsed = parse_line(raw)
        if parsed:
            update_state(parsed)
            live.update(build_table())


def main():
    global last_command
    console = Console()

    if msvcrt is None:
        console.print("[red]This interactive build needs Windows (msvcrt).[/red]")
        sys.exit(1)

    try:
        ser = serial.Serial(PORT, BAUD, timeout=1)
    except serial.SerialException as e:
        console.print(f"[red]Could not open {PORT}: {e}[/red]")
        console.print("[yellow]Is RealTerm still holding the port? Close it and retry.[/yellow]")
        sys.exit(1)

    console.print(f"[green]Listening on {PORT} @ {BAUD} baud.  Press f / o / u / s to inject, q to quit.[/green]")

    with Live(build_table(), console=console, refresh_per_second=10, screen=False) as live:
        # Reader runs in the background; the main thread owns the keyboard.
        reader = threading.Thread(target=reader_loop, args=(ser, live), daemon=True)
        reader.start()
        try:
            while True:
                key = msvcrt.getch().decode("ascii", errors="ignore").lower()
                if key in COMMANDS:
                    ser.write(key.encode("ascii"))     # one byte -> firmware UART RX
                    last_command = f"{key}  ({COMMANDS[key]})"
                    live.update(build_table())          # instant footer feedback
                elif key in ("q", "\x03"):              # q or Ctrl-C
                    break
        finally:
            ser.close()


if __name__ == "__main__":
    main()
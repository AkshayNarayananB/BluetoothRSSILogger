import asyncio
import os
import argparse
from bleak import BleakScanner
from datetime import datetime
from prettytable import PrettyTable
from colorama import Fore, Style, init
import csv

LOG_FILE = "bluetooth_log.txt"
EXPORT_FILE = "bluetooth_devices.csv"
DEFAULT_DURATION = 10
DEFAULT_INTERVAL = 20
RSSI_THRESHOLD = -70

init(autoreset=True) #initialization for colours on terminal

def color_rssi(rssi):
    if rssi is None:
        return "N/A"
    elif rssi < -80:
        return Fore.RED + str(rssi) + Style.RESET_ALL
    elif rssi < -60:
        return Fore.YELLOW + str(rssi) + Style.RESET_ALL
    else:
        return Fore.GREEN + str(rssi) + Style.RESET_ALL

def create_table(devices):
    table = PrettyTable()
    table.field_names = ["Time", "Address", "Name", "RSSI (dBm)"]
    table.align = "l"  # Align all columns left
    table.align["RSSI (dBm)"] = "r"  # RSSI aligned right

    now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

    for device in devices:
        name = device.name if device.name else "Unknown"
        rssi_raw = device.rssi if device.rssi is not None else "N/A"
        if device.rssi is None or device.rssi < RSSI_THRESHOLD:
            continue
        rssi_colored = color_rssi(rssi_raw)
        table.add_row([now, device.address, name, rssi_colored])

    return table

async def scan_and_log(scan_duration, scan_interval):
    print(f"Scanning for Bluetooth devices for {scan_duration} seconds every {scan_interval} seconds... (Press Ctrl+C to stop)\n")
    scanner = BleakScanner()
    
    if not os.path.exists(EXPORT_FILE):
        with open(EXPORT_FILE, mode='w', newline='') as file:
            writer = csv.writer(file)
            writer.writerow(["Time", "Address", "Name", "RSSI (dBm)"])

    while True:
        devices = await scanner.discover(timeout=scan_duration)

        table = create_table(devices)
        with open(LOG_FILE, "a") as f:
            for device in devices:
                name = device.name if device.name else "Unknown"
                rssi_raw = device.rssi if device.rssi is not None else "N/A"
                log_entry = f"{datetime.now()}, Address: {device.address}, Name: {name}, RSSI: {rssi_raw} dBm\n"
                f.write(log_entry)

                with open(EXPORT_FILE, mode='a', newline='') as export_file:
                    writer = csv.writer(export_file)
                    writer.writerow([datetime.now(), device.address, name, rssi_raw])

        print("\033c", end="") #clear screen
        print(table)

        await asyncio.sleep(scan_interval)

def parse_args():
    """Parse command-line arguments."""
    parser = argparse.ArgumentParser(description="Scan for Bluetooth devices and log information.")
    parser.add_argument(
        '--duration', 
        type=int, 
        default=DEFAULT_DURATION, 
        help=f"Duration of the scan in seconds (default {DEFAULT_DURATION}s)"
    )
    parser.add_argument(
        '--interval', 
        type=int, 
        default=DEFAULT_INTERVAL, 
        help=f"Interval between scans in seconds (default {DEFAULT_INTERVAL}s)"
    )
    return parser.parse_args()

if __name__ == "__main__":
    args = parse_args()
    
    try:
        asyncio.run(scan_and_log(args.duration, args.interval))
    except KeyboardInterrupt:
        print("\nScan stopped by user.")

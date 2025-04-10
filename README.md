# Bluetooth RSSI Logger

This is a simple script that has been designed to scan and log bluetooth devices in the vicinity. This acts as a beginner project in a domain which I hope to explore more deep soon.

## Packages to Install
```
pip install bleak, prettytable, colorama, argparse
```

## How does the script work
The script uses the BleakScanner to asynchronously discover Bluetooth devices within a specified duration (default 10 seconds) and interval (default 20 seconds). It supports command-line arguments to customize these values. The script logs the device information (name, address, RSSI) to a text file and CSV file for later analysis. The RSSI values are color-coded based on strength using colorama. The results are displayed in a dynamically updated table via PrettyTable, where the columns are aligned, and the screen is cleared after each scan. The data is logged to files, ensuring persistence for later review. The scan process repeats indefinitely until manually stopped.

## CMD Arguments Structure
```
python bluetooth_scanner_colored_logger.py --duration 15 --interval 30
```

## Screen Shot of the script's result

![Screenshot 2025-04-10 213718](https://github.com/user-attachments/assets/b24113e2-4acb-47f4-bf4b-c4df7872a02e)

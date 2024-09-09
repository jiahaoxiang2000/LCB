import os
import re
import csv

# Define the base directory containing the platform folders
base_dir = "/home/xjh/code/embed/LCB/src/outputs/l475vg"

# Define the output file
output_file = "l475vg.csv"

# Function to extract data from a file
def extract_data(file_path):
    data = {
        "variant": "",
        "impl": "",
        "config": "",
        "enc": "",
        "flash": ""
    }
    with open(file_path, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if "variant =" in line:
                data["variant"] = line.split('=')[1].strip()
            elif "impl =" in line:
                data["impl"] = line.split('=')[1].strip()
            elif "config =" in line:
                data["config"] = line.split('=')[1].strip()
            elif "enc=" in line:
                data["enc"] = line.split('=')[1].strip()

    size_file = file_path.replace("timing", "size")
    size_file = size_file.replace(".txt", "-out.txt")
    with open(size_file, 'r') as file:
        lines = file.readlines()
        for line in lines:
            if ".text" in line:
                match = re.search(r'\.text\s+(\d+)', line)
                if match:
                    data["flash"] = match.group(1)
    return data

# Function to traverse platform folders and collect data
def collect_data(base_dir):
    aggregated_data = []
    for root, dirs, files in os.walk(base_dir):
        for file in files:
            if "timing" in root and file.endswith(".txt"):
                file_path = os.path.join(root, file)
                data = extract_data(file_path)
                aggregated_data.append(data)
    return aggregated_data

# Function to write aggregated data to a CSV file
def write_to_csv(data, output_file):
    with open(output_file, 'w', newline='') as csvfile:
        fieldnames = ["variant", "impl", "config", "enc", "flash"]
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for row in data:
            writer.writerow(row)

# Main script
if __name__ == "__main__":
    aggregated_data = collect_data(base_dir)
    write_to_csv(aggregated_data, output_file)
    print(f"Data aggregated and written to {output_file}")
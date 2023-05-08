import sys
import csv

if len(sys.argv) < 2:
    print("Usage: python script.py file.log")
    exit(1)

input_file = sys.argv[1]
output_file = input_file.replace('.log', '_result.csv')

data = {}

with open(input_file, 'r') as f:
    for line in f:
        if len(line.strip()) == 0: # Ignora las líneas vacías
            continue
        fields = line.strip().split(';')
        if len(fields) < 3:
            continue
        nodes = fields[0]
        processes = fields[1]
        time = int(fields[-1])
        key = (nodes, processes)
        if key not in data or time > data[key]:
            data[key] = time

with open(output_file, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['Número de Nodos', 'Número de Procesos', 'Tiempo de Procesamiento'])
    for (nodes, processes), time in data.items():
        corrected_time = time - 400000 * int(processes)
        writer.writerow([nodes, processes, corrected_time])

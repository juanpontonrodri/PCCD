import csv
import sys

if len(sys.argv) < 2:
    print("Usage: python script.py file.log")
    exit(1)

input_file = sys.argv[1]
output_file = input_file.replace('.log', '_medias.csv')

data = {}

with open(input_file, 'r') as f:
    for line in f:
        if len(line.strip()) == 0: # Ignora las líneas vacías
            continue
        fields = line.strip().split(';')
        if len(fields) < 2:
            continue
        nodes = fields[0]
        time = fields[-1]
        if nodes not in data:
            data[nodes] = [float(time)]
        else:
            data[nodes].append(float(time))

with open(output_file, 'w', newline='') as f:
    writer = csv.writer(f)
    writer.writerow(['Número de Nodos', 'Tiempo Medio de Atención'])
    for nodes, times in data.items():
        writer.writerow([nodes, int(sum(times) / len(times))])

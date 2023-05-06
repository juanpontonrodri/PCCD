import csv
import collections
import matplotlib.pyplot as plt

# Crear un diccionario para almacenar los tiempos de procesamiento para cada número de procesos
procesos_tiempo = collections.defaultdict(list)

# Abrir el archivo "registrosimple.log" y leer los datos
with open('registrosimple.log', 'r') as archivo:
    lector_csv = csv.reader(archivo, delimiter=';')
    for fila in lector_csv:
        num_procesos = int(fila[1])
        tiempo_procesamiento = float(fila[6])
        procesos_tiempo[num_procesos].append(tiempo_procesamiento)

# Calcular el tiempo medio de procesamiento para cada número de procesos
procesos_tiempo_medio = {}
for num_procesos, tiempos in procesos_tiempo.items():
    procesos_tiempo_medio[num_procesos] = sum(tiempos) / len(tiempos)

# Crear una lista de números de procesos y tiempos medios de procesamiento
procesos = sorted(procesos_tiempo_medio.keys())
tiempos_medios = [procesos_tiempo_medio[num_procesos] for num_procesos in procesos]

# Graficar los tiempos medios de procesamiento en función del número de procesos
plt.plot(procesos, tiempos_medios, 'o-')
plt.xlabel('Número de procesos')
plt.ylabel('Tiempo medio de procesamiento (segundos)')
plt.title('Tiempos medios de procesamiento por número de procesos')
plt.savefig('grafica.png')  # Guardar la gráfica como una imagen
plt.show()

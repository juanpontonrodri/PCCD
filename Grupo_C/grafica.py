import csv
import plotly.graph_objects as go

# Crear un diccionario para almacenar los tiempos de procesamiento para cada número de procesos
procesos_tiempo = {}

# Abrir el archivo "registrosimple.log" y leer los datos
with open('registrosimple.log', 'r') as archivo:
    lector_csv = csv.reader(archivo, delimiter=';')
    for fila in lector_csv:
        num_procesos = int(fila[1])
        tiempo_procesamiento = float(fila[6])
        if num_procesos in procesos_tiempo:
            procesos_tiempo[num_procesos].append(tiempo_procesamiento)
        else:
            procesos_tiempo[num_procesos] = [tiempo_procesamiento]

# Calcular el tiempo medio de procesamiento para cada número de procesos
procesos = sorted(procesos_tiempo.keys())
tiempos_medios = [sum(procesos_tiempo[num_procesos]) / len(procesos_tiempo[num_procesos]) for num_procesos in procesos]

# Crear el gráfico de línea
fig = go.Figure(data=go.Scatter(x=procesos, y=tiempos_medios, mode='lines+markers'))

# Personalizar el gráfico
fig.update_layout(
    title='Tiempos medios de procesamiento por número de procesos',
    xaxis_title='Número de procesos',
    yaxis_title='Tiempo medio de procesamiento (segundos)',
    font=dict(size=18),
    margin=dict(l=50, r=50, t=50, b=50),
)

# Guardar la gráfica como un archivo HTML
fig.write_html('grafica.html', auto_open=True)

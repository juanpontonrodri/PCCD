import sys

def procesar_registro(registro):
    campos = registro.split(';')
    tipo_proceso = campos[2].lower()
    if tipo_proceso == 'reserva/anulacion':
        return 'reservas_anulaciones'
    elif tipo_proceso == 'reserva/administracion':
        return 'reservas_administracion'
    elif tipo_proceso == 'anulacion':
        return 'anulacion'
    elif tipo_proceso == 'consulta':
        return 'consulta'
    elif tipo_proceso == 'pago':
        return 'pagos'
    else:
        return 'otro'

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print(f"Uso: {sys.argv[0]} archivo.log")
        sys.exit(1)
    
    archivo_entrada = sys.argv[1]
    
    try:
        with open(archivo_entrada) as f:
            registros = f.readlines()
    except FileNotFoundError:
        print(f"No se encontró el archivo {archivo_entrada}")
        sys.exit(1)
    
    reservas_anulaciones = []
    reservas_administracion = []
    anulacion = []
    consulta = []
    pagos = []
    otro = []
    
    for registro in registros:
        tipo_proceso = procesar_registro(registro)
        if tipo_proceso == 'reservas_anulaciones':
            reservas_anulaciones.append(registro)
        elif tipo_proceso == 'reservas_administracion':
            reservas_administracion.append(registro)
        elif tipo_proceso == 'anulacion':
            anulacion.append(registro)
        elif tipo_proceso == 'consulta':
            consulta.append(registro)
        elif tipo_proceso == 'pagos':
            pagos.append(registro)
        else:
            otro.append(registro)
    
    for tipo_proceso, registros in [('reservas_anulaciones', reservas_anulaciones), ('reservas_administracion', reservas_administracion), ('anulacion', anulacion), ('consulta', consulta), ('pagos', pagos), ('otro', otro)]:
        archivo_salida = archivo_entrada.replace('.log', f'_{tipo_proceso}.log')
        with open(archivo_salida, 'w') as f:
            f.writelines(registros)

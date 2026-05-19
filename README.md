# Reto Calamar - RPC (Remote Procedure Call)

Implementación en C de un sistema distribuido inspirado en el juego del **"Puente de Cristal de Squid Game"**, utilizando el estándar **ONC/RPC** para la comunicación entre cliente y servidor.

---

# Requisitos del Sistema y Compatibilidad

Debido al uso del estándar de red **ONC/RPC** (`rpcbind`, `rpcgen` y `libtirpc`), este proyecto **no es compatible de forma nativa con Windows**. Para la correcta ejecución, se requiere un entorno basado en Unix/Linux.

## Sistemas Compatibles

### Windows 10 / 11
Activación obligatoria de **WSL (Windows Subsystem for Linux)** utilizando cualquier distribución Linux, por ejemplo:

- Ubuntu
- Debian
- Kali Linux

### Linux
Compatible con cualquier distribución moderna:

- Ubuntu
- Debian
- Fedora
- Arch Linux
- Linux Mint

### macOS
Soporte limitado debido a la depreciación de las librerías RPC nativas por parte de Apple.

Para correr el juego es recomendable tener alguno de los siguientes:

- Docker
- Máquina virtual Linux
- WSL remoto

---

# Dependencias de Software

Antes de compilar el proyecto, es necesario instalar las herramientas RPC y librerías requeridas.

## Instalación en Ubuntu/Debian

```bash
sudo apt update
sudo apt install rpcbind rpcsvc-proto build-essential libtirpc-dev libnsl-dev
```

---

# Estructura del Proyecto

```text
reto_calamar/
│
├── squid.x
├── squid_server.c
├── squid_client.c
├── Makefile.squid
```

## Descripción de Archivos

### `squid.x`
Archivo de interfaz RPC.

Define:

- Estructuras de datos:
  - `MoveReq`
  - `GameState`
- Procedimientos remotos:
  - `JOIN_GAME`
  - `MOVE`
  - `GET_STATE`

---

### `squid_server.c`
Código fuente del servidor.

Responsabilidades:

- Administración del estado global del juego
- Generación aleatoria del mapa
- Manejo del temporizador global
- Exclusión mutua mediante `pthread_mutex`
- Gestión de colisiones y jugadores

---

### `squid_client.c`
Código fuente del cliente.

Funciones principales:

- Interfaz de usuario por terminal
- Renderizado dinámico del mapa
- Comunicación RPC con el servidor
- Envío de movimientos del jugador

---

### `Makefile.squid`
Script de compilación configurado para enlazar correctamente:

- `libtirpc`
- `libnsl`
- `pthread`

---

# Compilación

Para compilar el proyecto y generar los ejecutables del cliente y servidor, ejecuta:

```bash
make -f Makefile.squid
```

---

# ¿Cómo Jugar?

## Ejecutación del Servidor

Antes de iniciar el servidor, el servicio `rpcbind` debe estar activo.

Ejecuta los siguientes comandos en una terminal:

```bash
sudo service rpcbind start

cd ~/reto_calamar

./squid_server
```

> Nota:
> La terminal del servidor permanecerá bloqueada escuchando solicitudes RPC entrantes.

---

## Ejecución Cliente

Abrir una segunda terminal independiente y ejecutar:

```bash
cd ~/reto_calamar

./squid_client localhost
```

---

# Mecánica del Juego

## Registro del Jugador

Al iniciar, el cliente solicita al servidor:

- Un ID único de jugador
- Sincronización del estado del juego

---

## Estructura del Tablero

El mapa está compuesto por:

- 3 puentes independientes
- 10 peldaños por puente

El camino correcto es generado aleatoriamente por el servidor.

---

## Navegación

El jugador debe seleccionar:

### Índice del puente

```text
0, 1 o 2
```

### Longitud del salto

```text
1 o 2 peldaños
```

---

# Nomenclatura del Mapa

| Símbolo | Significado |
|----------|-------------|
| `[?]` | Peldaño inexplorado |
| `[P]` | Peldaño seguro (Cristal templado) |
| `[X]` | Cristal roto (eliminación inmediata) |

---

# Condición de Victoria

Para ganar se debe cumplir estas acciones:

Cruzar exitosamente los 10 peldaños  
Llegar al extremo final del puente  
Antes de que el temporizador global de **60 segundos** llegue a cero

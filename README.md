# 🐚 Minishell - Sistema Operativo Avanzado

**Una implementación de shell Unix/Linux en C++**

![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

## 🚀 Características

- **Comandos internos**: `cd`, `pwd`, `help`, `salir`
- **Comandos externos**: Ejecución de programas del sistema
- **Redirección de E/S**: `>` (salida) y `<` (entrada)
- **Procesos en segundo plano**: Soporte para `&`
- **Manejo de señales**: Control robusto de procesos

## 🛠️ Instalación

### Prerrequisitos
```bash
# Ubuntu/Debian
sudo apt install build-essential
```

### Compilación
```bash
# Clonar repositorio
git clone https://github.com/Tazuto/proyecto-SO.git
cd proyecto-SO

# Compilar
make all

# Ejecutar
./minishell
```

## 📖 Uso Básico

```bash
$ ./minishell
[minishell:~]$ help
Comandos disponibles: cd, pwd, help, salir

[minishell:~]$ pwd
/home/usuario

[minishell:~]$ cd Documents
[minishell:~/Documents]$ pwd
/home/usuario/Documents

# Redirección
[minishell:~]$ ls > archivos.txt
[minishell:~]$ cat < archivos.txt

# Proceso en background
[minishell:~]$ sleep 10 &
[Proceso en segundo plano: 1234]

[minishell:~]$ salir
```

## 📁 Estructura del Proyecto

```
proyecto-SO/
├── include/          # Headers (.hpp)
├── src/             # Código fuente (.cpp)
├── Makefile         # Sistema de build
└── README.md        # Documentación
```


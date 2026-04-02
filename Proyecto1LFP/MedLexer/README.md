# MedLexer — Analizador Léxico para Gestión Hospitalaria

![Qt](https://img.shields.io/badge/Qt-6.8.3-green)
![C++](https://img.shields.io/badge/C++-17-blue)
![License](https://img.shields.io/badge/License-Academic-orange)

Proyecto 1 — Lenguajes Formales y de Programación  
Universidad San Carlos de Guatemala — Facultad de Ingeniería  
Ingeniería en Ciencias y Sistemas — 1er Semestre 2026

---

## Descripción

MedLexer es una aplicación de escritorio desarrollada en C++17 con Qt 6.8.3 
que implementa un analizador léxico manual mediante un Autómata Finito 
Determinista (AFD) para procesar archivos `.med` escritos en el mini-lenguaje 
MedLang. El sistema reconoce más de 30 tipos de tokens, detecta errores léxicos 
con posición exacta de línea y columna, y genera cuatro reportes HTML con 
estadísticas hospitalarias además de un diagrama de relaciones en formato 
Graphviz DOT.

---

## Características

- AFD implementado manualmente sin uso de `std::regex`
- Reconocimiento de 30+ tipos de tokens MedLang
- Detección de errores con línea y columna exacta
- Recuperación de errores: el análisis continúa tras cada error
- 4 reportes HTML con CSS embebido
- Detección automática de conflictos de horario
- Generación de diagrama Graphviz (.dot)
- Interfaz gráfica con Qt 6.8.3

---

## Requisitos

| Herramienta | Versión |
|-------------|---------|
| Qt Framework | 6.8.3 LTS |
| MinGW | 13.1.0 (64-bit) |
| Qt Creator | 13.0+ |
| CMake | 3.16+ |
| Sistema Operativo | Windows 10/11 (64-bit) |

---

## Instalación de Dependencias

### Paso 1 — Descargar Qt

Ve a la página oficial de Qt y descarga el instalador:
```
https://www.qt.io/download-qt-installer
```

### Paso 2 — Seleccionar componentes

Durante la instalación selecciona:
```
☑ Qt 6.8.3 LTS
   ☑ MinGW 13.1.0 64-bit
   ☑ Qt 5 Compatibility Module
☑ Qt Creator
☑ Developer and Designer Tools
   ☑ MinGW 13.1.0 64-bit
   ☑ CMake
   ☑ Ninja
```

### Paso 3 — Verificar instalación

Abre Qt Creator y verifica que aparece el kit:
```
Desktop Qt 6.8.3 MinGW 64-bit
```

---

## Compilación y Ejecución

### Opción 1 — Qt Creator (Recomendada)
```bash
# Paso 1: Clonar el repositorio
git clone https://github.com/TU_USUARIO/LFP_1S2026_#Carnet.git

# Paso 2: Entrar a la carpeta del proyecto
cd LFP_1S2026_#Carnet/Proyecto1/MedLexer

# Paso 3: Abrir Qt Creator
# File → Open File or Project → seleccionar MedLexer.pro

# Paso 4: Seleccionar kit
# Desktop Qt 6.8.3 MinGW 64-bit

# Paso 5: Compilar y ejecutar
# Ctrl+R o botón verde ▶
```

### Opción 2 — Línea de Comandos
```bash
# Paso 1: Clonar el repositorio
git clone https://github.com/TU_USUARIO/LFP_1S2026_#Carnet.git

# Paso 2: Entrar a la carpeta del proyecto
cd LFP_1S2026_#Carnet/Proyecto1/MedLexer

# Paso 3: Generar Makefile con qmake
qmake MedLexer.pro

# Paso 4: Compilar
mingw32-make

# Paso 5: Ejecutar
.\debug\MedLexer.exe
```

---

## Estructura del Proyecto
```
LFP_1S2026_#Carnet/
└── Proyecto1/
    └── MedLexer/
        ├── MedLexer.pro
        ├── README.md
        ├── .gitignore
        ├── main.cpp
        ├── mainwindow.cpp
        ├── mainwindow.h
        ├── mainwindow.ui
        ├── src/
        │   ├── model/
        │   │   ├── Token.h
        │   │   └── Token.cpp
        │   ├── lexer/
        │   │   ├── LexicalAnalyzer.h
        │   │   └── LexicalAnalyzer.cpp
        │   ├── errors/
        │   │   └── ErrorManager.h
        │   └── reports/
        │       ├── ReportGenerator.h
        │       └── ReportGenerator.cpp
        ├── docs/
        │   ├── ManualTecnico.pdf
        │   └── ManualUsuario.pdf
        ├── tests/
        │   ├── caso1_valido.med
        │   ├── caso2_token_invalido.med
        │   ├── caso3_cadena_sin_cerrar.med
        │   ├── caso4_fecha_mes_invalido.med
        │   ├── caso5_hora_invalida.med
        │   ├── caso6_especialidad_invalida.med
        │   ├── caso7_multiples_errores.med
        │   ├── caso8_conflicto_horario.med
        │   ├── caso9_sin_diagnostico.med
        │   ├── caso10_minimo.med
        │   └── CasosDePrueba.pdf
        └── examples/
            ├── ejemploValido.med
            └── ejemploErrores.med
```

---

## Uso de la Aplicación

### 1. Cargar un archivo .med
```
Click en "Cargar Archivo .med"
→ Seleccionar archivo desde el explorador
→ El contenido aparece en el área de texto
```

### 2. Analizar el archivo
```
Click en "Analizar"
→ La tabla superior muestra todos los tokens
→ La tabla inferior muestra los errores léxicos
```

### 3. Generar reportes
```
Click en "Reporte 1 - Pacientes"    → Historial de pacientes
Click en "Reporte 2 - Medicos"      → Carga por especialidad
Click en "Reporte 3 - Citas"        → Agenda con conflictos
Click en "Reporte 4 - Estadistico"  → KPIs del hospital
Click en "Reporte Errores"          → Tabla de errores HTML
Click en "Generar Graphviz"         → Archivo hospital.dot
```

### 4. Ver diagrama Graphviz
```
1. Abrir hospital.dot con el Bloc de notas
2. Copiar el contenido
3. Ir a: https://dreampuf.github.io/GraphvizOnline
4. Pegar el contenido y ver el diagrama
```

---

## Estructura del Lenguaje MedLang
```
HOSPITAL "Nombre del Hospital" {
    PACIENTES {
        paciente: "Nombre" [edad: N, tipo_sangre: "X+", habitacion: N],
    };
    MEDICOS {
        medico: "Nombre" [especialidad: ESPECIALIDAD, codigo: "MED-XXX"],
    };
    CITAS {
        cita: "Paciente" con "Medico" [fecha: AAAA-MM-DD, hora: HH:MM],
    };
    DIAGNOSTICOS {
        diagnostico: "Paciente" [condicion: "texto", medicamento: "texto", dosis: FRECUENCIA],
    };
};
```

### Especialidades válidas
```
CARDIOLOGIA | NEUROLOGIA | PEDIATRIA | 
CIRUGIA | MEDICINA_GENERAL | ONCOLOGIA
```

### Frecuencias de dosis válidas
```
DIARIA | CADA_8_HORAS | CADA_12_HORAS | SEMANAL
```

### Tipos de sangre válidos
```
"A+" | "A-" | "B+" | "B-" | "O+" | "O-" | "AB+" | "AB-"
```

---

## Tokens Reconocidos

| Categoría | Tokens |
|-----------|--------|
| Palabras reservadas | HOSPITAL, PACIENTES, MEDICOS, CITAS, DIAGNOSTICOS |
| Elementos | paciente, medico, cita, diagnostico |
| Atributos | edad, tipo_sangre, habitacion, especialidad, codigo, fecha, hora, con, condicion, medicamento, dosis |
| Especialidades | CARDIOLOGIA, NEUROLOGIA, PEDIATRIA, CIRUGIA, MEDICINA_GENERAL, ONCOLOGIA |
| Dosis | DIARIA, CADA_8_HORAS, CADA_12_HORAS, SEMANAL |
| Literales | NUMBER, STRING, DATE, TIME, ID_CODE |
| Delimitadores | SYMBOL ( `{` `}` `[` `]` `:` `,` `;` ) |

---

## Errores Detectados

| Tipo de Error | Gravedad | Ejemplo |
|---------------|----------|---------|
| Token inválido | ERROR | `@`, `#`, `!` |
| Cadena sin cerrar | CRITICO | `"texto sin cerrar` |
| Fecha con mes inválido | ERROR | `2025-13-01` |
| Fecha con día inválido | ERROR | `2025-04-32` |
| Hora fuera de rango | ERROR | `25:00`, `14:60` |
| Especialidad no reconocida | UNKNOWN | `CARDIO` |

---

## Reportes Generados

| Reporte | Archivo | Descripción |
|---------|---------|-------------|
| Tokens | tabla en GUI | Todos los tokens con posición |
| Errores | tabla en GUI | Errores con tipo y gravedad |
| Reporte 1 | reporte1_pacientes.html | Historial con estado del paciente |
| Reporte 2 | reporte2_medicos.html | Carga de trabajo por médico |
| Reporte 3 | reporte3_citas.html | Agenda con detección de conflictos |
| Reporte 4 | reporte4_estadistico.html | KPIs y distribución por especialidad |
| Errores HTML | reporte_errores.html | Tabla de errores léxicos |
| Graphviz | hospital.dot | Diagrama de relaciones |

---

## Autor

**Nombre:** Tu Nombre Completo  
**Carnet:** Tu Carnet  
**Sección:** Tu Sección  
**Curso:** Lenguajes Formales y de Programación  
**Universidad:** Universidad San Carlos de Guatemala  
**Fecha:** Abril 2026

---

## Licencia

Proyecto académico desarrollado para el curso de Lenguajes Formales y de 
Programación de la Universidad San Carlos de Guatemala. 
Prohibida su reproducción total o parcial.
```

---

## También crea el `.gitignore`

Crea un archivo `.gitignore` en la raíz del proyecto y pega esto:
```
# Qt
*.pro.user
*.pro.user.*
*.qbs.user
*.qbs.user.*
*.moc
moc_*.cpp
moc_*.h
qrc_*.cpp
ui_*.h
Makefile*
*_resource.rc
*.aps

# Build directories
build/
debug/
release/
.qtcreator/

# Compiled files
*.o
*.obj
*.exe
*.dll
*.so
*.dylib

# Generated reports
*.html
*.dot

# OS files
.DS_Store
Thumbs.db
desktop.ini

# IDE files
.vscode/
.idea/
*.swp
*.swo
```

---

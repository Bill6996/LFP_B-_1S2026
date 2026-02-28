#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <iomanip>

using namespace std;

// ============================================
// ESTRUCTURAS DE DATOS
// ============================================

struct Estudiante {
    int carnet;
    string nombre;
    string apellido;
    string carrera;
    int semestre;
};

struct Curso {
    int codigo;
    string nombre;
    int creditos;
    int semestre;
    string carrera;
};

struct Nota {
    int carnet;
    int codigo_curso;
    double nota;
    string ciclo;
    int anio;
};

// ============================================
// VECTORES GLOBALES (almacenan todos los datos)
// ============================================

vector<Estudiante> estudiantes;
vector<Curso> cursos;
vector<Nota> notas;

// ============================================
// FUNCION AUXILIAR: separar string por comas
// ============================================

string trim(const string& s) {
    int inicio = 0;
    int fin = s.size() - 1;
    while (inicio <= fin && (s[inicio] == ' ' || s[inicio] == '\r' || s[inicio] == '\n'))
        inicio++;
    while (fin >= inicio && (s[fin] == ' ' || s[fin] == '\r' || s[fin] == '\n'))
        fin--;
    return s.substr(inicio, fin - inicio + 1);
}

vector<string> split(const string& linea, char delimitador) {
    vector<string> tokens;
    string token;
    istringstream stream(linea);
    while (getline(stream, token, delimitador)) {
        tokens.push_back(trim(token));
    }
    return tokens;
}

// ============================================
// REPORTE 1: ESTADISTICAS POR CURSO
// ============================================

void reporte1() {
    if (cursos.empty() || notas.empty()) {
        cout << "ERROR: Debe cargar los archivos primero.\n";
        return;
    }

    ofstream html("reporte1.html");

    html << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html << "<title>Estadisticas por Curso</title>";
    html << "<style>";
    html << "body { font-family: Arial, sans-serif; margin: 30px; background: #f4f4f4; }";
    html << "h1 { color: #003366; }";
    html << "table { border-collapse: collapse; width: 100%; background: white; }";
    html << "th { background: #003366; color: white; padding: 10px; }";
    html << "td { border: 1px solid #ccc; padding: 8px; text-align: center; }";
    html << "tr:nth-child(even) { background: #eef; }";
    html << "</style></head><body>";
    html << "<h1>Reporte 1: Estadisticas Generales por Curso</h1>";
    html << "<table><tr><th>Curso</th><th>Estudiantes</th><th>Promedio</th>";
    html << "<th>Maxima</th><th>Minima</th><th>Desv. Estandar</th><th>Mediana</th></tr>";

    for (auto& curso : cursos) {
        // recolectar notas de este curso
        vector<double> notasCurso;
        for (auto& n : notas) {
            if (n.codigo_curso == curso.codigo) {
                notasCurso.push_back(n.nota);
            }
        }

        if (notasCurso.empty()) continue;

        // promedio
        double suma = 0;
        for (double n : notasCurso) suma += n;
        double promedio = suma / notasCurso.size();

        // maxima y minima
        double maxima = notasCurso[0];
        double minima = notasCurso[0];
        for (double n : notasCurso) {
            if (n > maxima) maxima = n;
            if (n < minima) minima = n;
        }

        // desviacion estandar
        double sumaDif = 0;
        for (double n : notasCurso) sumaDif += pow(n - promedio, 2);
        double desviacion = sqrt(sumaDif / notasCurso.size());

        // mediana
        vector<double> ordenadas = notasCurso;
        sort(ordenadas.begin(), ordenadas.end());
        double mediana;
        int tam = ordenadas.size();
        if (tam % 2 == 0)
            mediana = (ordenadas[tam / 2 - 1] + ordenadas[tam / 2]) / 2.0;
        else
            mediana = ordenadas[tam / 2];

        html << fixed << setprecision(2);
        html << "<tr>";
        html << "<td>" << curso.nombre << "</td>";
        html << "<td>" << notasCurso.size() << "</td>";
        html << "<td>" << promedio << "</td>";
        html << "<td>" << maxima << "</td>";
        html << "<td>" << minima << "</td>";
        html << "<td>" << desviacion << "</td>";
        html << "<td>" << mediana << "</td>";
        html << "</tr>";
    }

    html << "</table></body></html>";
    html.close();

    cout << ">> Reporte 1 generado: reporte1.html\n";
    cout << ">> Ubicacion: C:\\Users\\bilyf\\OneDrive\\Escritorio\\LFP_PRACTICAUNICA\\Practica1\\\n";
}

// ============================================
// REPORTE 2: RENDIMIENTO POR ESTUDIANTE
// ============================================

void reporte2() {
    if (estudiantes.empty() || notas.empty() || cursos.empty()) {
        cout << "ERROR: Debe cargar los archivos primero.\n";
        return;
    }

    ofstream html("reporte2.html");

    html << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html << "<title>Rendimiento por Estudiante</title>";
    html << "<style>";
    html << "body { font-family: Arial, sans-serif; margin: 30px; background: #f4f4f4; }";
    html << "h1 { color: #003366; }";
    html << "table { border-collapse: collapse; width: 100%; background: white; }";
    html << "th { background: #003366; color: white; padding: 10px; }";
    html << "td { border: 1px solid #ccc; padding: 8px; text-align: center; }";
    html << "tr:nth-child(even) { background: #eef; }";
    html << ".aprobado { color: green; font-weight: bold; }";
    html << ".reprobado { color: red; font-weight: bold; }";
    html << "</style></head><body>";
    html << "<h1>Reporte 2: Rendimiento por Estudiante</h1>";
    html << "<table><tr>";
    html << "<th>Carnet</th><th>Nombre</th><th>Carrera</th><th>Semestre</th>";
    html << "<th>Promedio</th><th>Aprobados</th><th>Reprobados</th><th>Creditos</th>";
    html << "</tr>";

    for (auto& e : estudiantes) {
        vector<double> notasEst;
        int aprobados = 0;
        int reprobados = 0;
        int creditos = 0;

        for (auto& n : notas) {
            if (n.carnet == e.carnet) {
                notasEst.push_back(n.nota);

                if (n.nota >= 61) {
                    aprobados++;
                    // buscar creditos del curso
                    for (auto& c : cursos) {
                        if (c.codigo == n.codigo_curso) {
                            creditos += c.creditos;
                            break;
                        }
                    }
                }
                else {
                    reprobados++;
                }
            }
        }

        if (notasEst.empty()) continue;

        double suma = 0;
        for (double n : notasEst) suma += n;
        double promedio = suma / notasEst.size();

        html << fixed << setprecision(2);
        html << "<tr>";
        html << "<td>" << e.carnet << "</td>";
        html << "<td>" << e.nombre << " " << e.apellido << "</td>";
        html << "<td>" << e.carrera << "</td>";
        html << "<td>" << e.semestre << "</td>";
        html << "<td>" << promedio << "</td>";
        html << "<td class='aprobado'>" << aprobados << "</td>";
        html << "<td class='reprobado'>" << reprobados << "</td>";
        html << "<td>" << creditos << "</td>";
        html << "</tr>";
    }

    html << "</table></body></html>";
    html.close();

    cout << ">> Reporte 2 generado: reporte2.html\n";
}

// ============================================
// REPORTE 3: TOP 10 MEJORES ESTUDIANTES
// ============================================

void reporte3() {
    if (estudiantes.empty() || notas.empty()) {
        cout << "ERROR: Debe cargar los archivos primero.\n";
        return;
    }

    // calcular promedio de cada estudiante
    vector<pair<double, Estudiante>> promedios;

    for (auto& e : estudiantes) {
        vector<double> notasEst;
        for (auto& n : notas) {
            if (n.carnet == e.carnet)
                notasEst.push_back(n.nota);
        }
        if (notasEst.empty()) continue;

        double suma = 0;
        for (double n : notasEst) suma += n;
        double promedio = suma / notasEst.size();

        promedios.push_back({ promedio, e });
    }

    // ordenar de mayor a menor
    sort(promedios.begin(), promedios.end(), [](auto& a, auto& b) {
        return a.first > b.first;
        });

    ofstream html("reporte3.html");

    html << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html << "<title>Top 10 Mejores Estudiantes</title>";
    html << "<style>";
    html << "body { font-family: Arial, sans-serif; margin: 30px; background: #f4f4f4; }";
    html << "h1 { color: #003366; }";
    html << "table { border-collapse: collapse; width: 100%; background: white; }";
    html << "th { background: #003366; color: white; padding: 10px; }";
    html << "td { border: 1px solid #ccc; padding: 8px; text-align: center; }";
    html << "tr:nth-child(even) { background: #eef; }";
    html << ".gold { background: #FFD700 !important; font-weight: bold; }";
    html << ".silver { background: #C0C0C0 !important; font-weight: bold; }";
    html << ".bronze { background: #CD7F32 !important; font-weight: bold; }";
    html << "</style></head><body>";
    html << "<h1>Reporte 3: Top 10 Mejores Estudiantes</h1>";
    html << "<table><tr>";
    html << "<th>Posicion</th><th>Carnet</th><th>Nombre</th>";
    html << "<th>Carrera</th><th>Semestre</th><th>Promedio</th>";
    html << "</tr>";

    int limite = min((int)promedios.size(), 10);
    for (int i = 0; i < limite; i++) {
        string clase = "";
        if (i == 0) clase = "gold";
        else if (i == 1) clase = "silver";
        else if (i == 2) clase = "bronze";

        html << fixed << setprecision(2);
        html << "<tr class='" << clase << "'>";
        html << "<td>" << (i + 1) << "</td>";
        html << "<td>" << promedios[i].second.carnet << "</td>";
        html << "<td>" << promedios[i].second.nombre << " " << promedios[i].second.apellido << "</td>";
        html << "<td>" << promedios[i].second.carrera << "</td>";
        html << "<td>" << promedios[i].second.semestre << "</td>";
        html << "<td>" << promedios[i].first << "</td>";
        html << "</tr>";
    }

    html << "</table></body></html>";
    html.close();

    cout << ">> Reporte 3 generado: reporte3.html\n";
}

// ============================================
// REPORTE 4: CURSOS CON MAYOR REPROBACION
// ============================================

void reporte4() {
    if (cursos.empty() || notas.empty()) {
        cout << "ERROR: Debe cargar los archivos primero.\n";
        return;
    }

    vector<tuple<double, int, int, Curso>> estadisticas;

    for (auto& curso : cursos) {
        int total = 0, reprobados = 0, aprobados = 0;

        for (auto& n : notas) {
            if (n.codigo_curso == curso.codigo) {
                total++;
                if (n.nota < 61) reprobados++;
                else aprobados++;
            }
        }

        if (total == 0) continue;

        double porcentaje = (double)reprobados / total * 100.0;
        estadisticas.push_back({ porcentaje, aprobados, reprobados, curso });
    }

    // ordenar de mayor a menor porcentaje de reprobacion
    sort(estadisticas.begin(), estadisticas.end(), [](auto& a, auto& b) {
        return get<0>(a) > get<0>(b);
        });

    ofstream html("reporte4.html");

    html << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html << "<title>Cursos con Mayor Reprobacion</title>";
    html << "<style>";
    html << "body { font-family: Arial, sans-serif; margin: 30px; background: #f4f4f4; }";
    html << "h1 { color: #003366; }";
    html << "table { border-collapse: collapse; width: 100%; background: white; }";
    html << "th { background: #003366; color: white; padding: 10px; }";
    html << "td { border: 1px solid #ccc; padding: 8px; text-align: center; }";
    html << "tr:nth-child(even) { background: #eef; }";
    html << ".alto { color: red; font-weight: bold; }";
    html << ".medio { color: orange; font-weight: bold; }";
    html << ".bajo { color: green; font-weight: bold; }";
    html << "</style></head><body>";
    html << "<h1>Reporte 4: Cursos con Mayor Indice de Reprobacion</h1>";
    html << "<table><tr>";
    html << "<th>Codigo</th><th>Curso</th><th>Total</th>";
    html << "<th>Aprobados</th><th>Reprobados</th><th>% Reprobacion</th>";
    html << "</tr>";

    for (auto& est : estadisticas) {
        double porcentaje = get<0>(est);
        int aprobados = get<1>(est);
        int reprobados = get<2>(est);
        Curso curso = get<3>(est);
        int total = aprobados + reprobados;

        string clase = "bajo";
        if (porcentaje >= 50) clase = "alto";
        else if (porcentaje >= 25) clase = "medio";

        html << fixed << setprecision(2);
        html << "<tr>";
        html << "<td>" << curso.codigo << "</td>";
        html << "<td>" << curso.nombre << "</td>";
        html << "<td>" << total << "</td>";
        html << "<td>" << aprobados << "</td>";
        html << "<td>" << reprobados << "</td>";
        html << "<td class='" << clase << "'>" << porcentaje << "%</td>";
        html << "</tr>";
    }

    html << "</table></body></html>";
    html.close();

    cout << ">> Reporte 4 generado: reporte4.html\n";
}

// ============================================
// REPORTE 5: ANALISIS POR CARRERA
// ============================================

void reporte5() {
    if (estudiantes.empty() || notas.empty() || cursos.empty()) {
        cout << "ERROR: Debe cargar los archivos primero.\n";
        return;
    }

    // obtener carreras unicas
    vector<string> carreras;
    for (auto& e : estudiantes) {
        bool existe = false;
        for (auto& c : carreras)
            if (c == e.carrera) { existe = true; break; }
        if (!existe) carreras.push_back(e.carrera);
    }

    ofstream html("reporte5.html");

    html << "<!DOCTYPE html><html><head><meta charset='UTF-8'>";
    html << "<title>Analisis por Carrera</title>";
    html << "<style>";
    html << "body { font-family: Arial, sans-serif; margin: 30px; background: #f4f4f4; }";
    html << "h1 { color: #003366; }";
    html << "h2 { color: #005599; border-bottom: 2px solid #003366; }";
    html << "table { border-collapse: collapse; width: 100%; background: white; margin-bottom: 30px; }";
    html << "th { background: #003366; color: white; padding: 10px; }";
    html << "td { border: 1px solid #ccc; padding: 8px; text-align: center; }";
    html << "tr:nth-child(even) { background: #eef; }";
    html << ".card { background: white; padding: 15px; margin-bottom: 10px; border-radius: 8px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }";
    html << "</style></head><body>";
    html << "<h1>Reporte 5: Analisis por Carrera</h1>";

    for (auto& carrera : carreras) {
        // estudiantes de esta carrera
        vector<Estudiante> estCarrera;
        for (auto& e : estudiantes)
            if (e.carrera == carrera) estCarrera.push_back(e);

        // promedio general de la carrera
        vector<double> todasNotas;
        for (auto& e : estCarrera)
            for (auto& n : notas)
                if (n.carnet == e.carnet) todasNotas.push_back(n.nota);

        double promedioCarrera = 0;
        if (!todasNotas.empty()) {
            double suma = 0;
            for (double n : todasNotas) suma += n;
            promedioCarrera = suma / todasNotas.size();
        }

        // cursos de esta carrera
        int numCursos = 0;
        for (auto& c : cursos)
            if (c.carrera == carrera) numCursos++;

        // distribucion por semestre
        int distSemestre[11] = { 0 };
        for (auto& e : estCarrera)
            if (e.semestre >= 1 && e.semestre <= 10)
                distSemestre[e.semestre]++;

        html << "<h2>" << carrera << "</h2>";
        html << "<div class='card'>";
        html << "<table><tr>";
        html << "<th>Total Estudiantes</th><th>Promedio General</th><th>Cursos Disponibles</th>";
        html << "</tr><tr>";
        html << "<td>" << estCarrera.size() << "</td>";
        html << fixed << setprecision(2);
        html << "<td>" << promedioCarrera << "</td>";
        html << "<td>" << numCursos << "</td>";
        html << "</tr></table>";

        // tabla distribucion por semestre
        html << "<h3>Distribucion por Semestre</h3>";
        html << "<table><tr>";
        for (int i = 1; i <= 10; i++)
            html << "<th>Sem " << i << "</th>";
        html << "</tr><tr>";
        for (int i = 1; i <= 10; i++)
            html << "<td>" << distSemestre[i] << "</td>";
        html << "</tr></table>";
        html << "</div>";
    }

    html << "</body></html>";
    html.close();

    cout << ">> Reporte 5 generado: reporte5.html\n";
}

// ============================================
// MENU PRINCIPAL
// ============================================

void mostrarMenu() {
    cout << "\n==========================================\n";
    cout << "     SISTEMA DE ANALISIS ACADEMICO\n";
    cout << "==========================================\n";
    cout << "1. Cargar archivo de estudiantes\n";
    cout << "2. Cargar archivo de cursos\n";
    cout << "3. Cargar archivo de notas\n";
    cout << "4. Generar Reporte: Estadisticas por Curso\n";
    cout << "5. Generar Reporte: Rendimiento por Estudiante\n";
    cout << "6. Generar Reporte: Top 10 Mejores Estudiantes\n";
    cout << "7. Generar Reporte: Cursos con Mayor Reprobacion\n";
    cout << "8. Generar Reporte: Analisis por Carrera\n";
    cout << "9. Salir\n";
    cout << "==========================================\n";
    cout << "Seleccione una opcion: ";
}

// ============================================
// LECTURA DE ARCHIVOS
// ============================================

void cargarEstudiantes() {
    string ruta;
    cout << "Ingrese el nombre del archivo (ejemplo: estudiantes.lfp): ";
    cin >> ruta;

    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo abrir el archivo " << ruta << "\n";
        return;
    }

    estudiantes.clear(); // limpiar datos anteriores
    string linea;
    int contador = 0;

    while (getline(archivo, linea)) {
        // limpiar BOM de la primera linea

           // mostrar cada caracter de la linea para ver que hay
        cout << "Linea [" << linea.size() << " chars]: ";
        for (unsigned char c : linea) {
            if (c < 32 || c > 126)
                cout << "[" << (int)c << "]";
            else
                cout << c;
        }
        cout << "\n";
        if (!linea.empty() && (unsigned char)linea[0] == 0xEF) {
            if (linea.size() >= 3) linea = linea.substr(3);
        }

        // limpiar \r al final
        if (!linea.empty() && linea.back() == '\r')
            linea.pop_back();

        if (linea.empty()) continue;

        vector<string> datos = split(linea, ',');

        if (datos.size() != 5) {
            cout << "Linea con formato incorrecto, se omite: " << linea << "\n";
            continue;
        }

        Estudiante e;
        e.carnet = stoi(datos[0]);
        e.nombre = datos[1];
        e.apellido = datos[2];
        e.carrera = datos[3];
        e.semestre = stoi(datos[4]);

        estudiantes.push_back(e);
        contador++;
    }

    archivo.close();
    cout << ">> Se cargaron " << contador << " estudiantes correctamente.\n";
}

// -------------------------------------------

void cargarCursos() {
    string ruta;
    cout << "Ingrese el nombre del archivo (ejemplo: cursos.lfp): ";
    cin >> ruta;

    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo abrir el archivo " << ruta << "\n";
        return;
    }

    cursos.clear();
    string linea;
    int contador = 0;

    while (getline(archivo, linea)) {
        if (!linea.empty() && (unsigned char)linea[0] == 0xEF)
            if (linea.size() >= 3) linea = linea.substr(3);
        if (!linea.empty() && linea.back() == '\r')
            linea.pop_back();
        if (linea.empty()) continue;

        vector<string> datos = split(linea, ',');

        if (datos.size() < 5) {
            cout << "Linea con formato incorrecto, se omite: " << linea << "\n";
            continue;
        }

        Curso c;
        c.codigo = stoi(datos[0]);
        c.nombre = datos[1];
        c.creditos = stoi(datos[2]);
        c.semestre = stoi(datos[3]);
        c.carrera = datos[4];

        cursos.push_back(c);
        contador++;
    }

    archivo.close();
    cout << ">> Se cargaron " << contador << " cursos correctamente.\n";
}

// -------------------------------------------

void cargarNotas() {
    string ruta;
    cout << "Ingrese el nombre del archivo (ejemplo: notas.lfp): ";
    cin >> ruta;

    ifstream archivo(ruta);

    if (!archivo.is_open()) {
        cout << "ERROR: No se pudo abrir el archivo " << ruta << "\n";
        return;
    }

    notas.clear();
    string linea;
    int contador = 0;

    while (getline(archivo, linea)) {
        if (!linea.empty() && (unsigned char)linea[0] == 0xEF)
            if (linea.size() >= 3) linea = linea.substr(3);
        if (!linea.empty() && linea.back() == '\r')
            linea.pop_back();
        if (linea.empty()) continue;

        vector<string> datos = split(linea, ',');

        if (datos.size() != 5) {
            cout << "Linea con formato incorrecto, se omite: " << linea << "\n";
            continue;
        }

        Nota n;
        n.carnet = stoi(datos[0]);
        n.codigo_curso = stoi(datos[1]);
        n.nota = stod(datos[2]);
        n.ciclo = datos[3];
        n.anio = stoi(datos[4]);

        notas.push_back(n);
        contador++;
    }

    archivo.close();
    cout << ">> Se cargaron " << contador << " notas correctamente.\n";
}

// ============================================
// MAIN
// ============================================

int main() {
    int opcion;

    do {
        mostrarMenu();

        try {
            string entrada;
            cin >> entrada;

            for (char c : entrada) {
                if (!isdigit(c)) {
                    throw invalid_argument("Entrada invalida");
                }
            }

            opcion = stoi(entrada);

            switch (opcion) {
            case 1: cargarEstudiantes(); break;
            case 2: cargarCursos(); break;
            case 3: cargarNotas(); break;
            case 4: reporte1(); break;
            case 5: reporte2(); break;
            case 6: reporte3(); break;
            case 7: reporte4(); break;
            case 8: reporte5(); break;
            case 9: cout << "Saliendo...\n"; break;
            default: cout << "Opcion invalida.\n";
            }

        }
        catch (invalid_argument&) {
            cout << "ERROR: Ingrese un numero valido.\n";
            cin.clear();
            opcion = 0;
        }

    } while (opcion != 9);

    return 0;
}
#include "ReportGenerator.h"
#include <fstream>
#include <map>

std::string limpiarComillas(std::string texto) {
    if (texto.size() >= 2 && texto.front() == '"' && texto.back() == '"') {
        return texto.substr(1, texto.size() - 2);
    }
    return texto;
}

std::string extraerPrioridad(const NodoArbol* nodo) {
    if (!nodo) return "";
    if (nodo->etiqueta == "<prioridad>" && !nodo->hijos.empty()) {
        return nodo->hijos[0]->etiqueta;
    }
    for (auto* hijo : nodo->hijos) {
        std::string res = extraerPrioridad(hijo);
        if (!res.empty()) return res;
    }
    return "";
}

std::string extraerValor(const NodoArbol* nodo, const std::string& target) {
    if (!nodo) return "";
    for (size_t i = 0; i < nodo->hijos.size(); i++) {
        if (nodo->hijos[i]->etiqueta == target && i + 2 < nodo->hijos.size()) {
            return limpiarComillas(nodo->hijos[i+2]->etiqueta);
        }
        std::string res = extraerValor(nodo->hijos[i], target);
        if (!res.empty()) return res;
    }
    return "";
}

struct InfoTarea {
    std::string nombre, prioridad, responsable, fecha;
};

struct InfoColumna {
    std::string nombre;
    std::vector<InfoTarea> tareas;
};

void buscarColumnas(const NodoArbol* nodo, std::vector<InfoColumna>& columnas) {
    if (!nodo) return;
    if (nodo->etiqueta == "<columna>") {
        InfoColumna col;
        for (auto* hijo : nodo->hijos) {
            if (hijo->esTerminal && hijo->etiqueta.front() == '"') {
                col.nombre = limpiarComillas(hijo->etiqueta);
                break;
            }
        }
        
        auto buscarTareas = [](const NodoArbol* n, std::vector<InfoTarea>& tareas, auto& ref) -> void {
            if (!n) return;
            if (n->etiqueta == "<tarea>") {
                InfoTarea t;
                for (auto* hijo : n->hijos) {
                    if (hijo->esTerminal && hijo->etiqueta.front() == '"') {
                        t.nombre = limpiarComillas(hijo->etiqueta);
                        break;
                    }
                }
                t.prioridad = extraerPrioridad(n);
                t.responsable = extraerValor(n, "responsable");
                t.fecha = extraerValor(n, "fecha_limite");
                tareas.push_back(t);
                return;
            }
            for (auto* hijo : n->hijos) ref(hijo, tareas, ref);
        };
        
        buscarTareas(nodo, col.tareas, buscarTareas);
        columnas.push_back(col);
        return; 
    }
    for (auto* hijo : nodo->hijos) buscarColumnas(hijo, columnas);
}

void ReportGenerator::generarReporteTokens(const std::vector<Token>& tokens) {
    std::ofstream archivo("reporte_tokens.html");
    if (!archivo.is_open()) return;
    archivo << "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>Reporte de Tokens</title>\n";
    archivo << "<style>body{font-family:Arial;} table{width:80%;margin:auto;border-collapse:collapse;} th,td{border:1px solid #ccc;padding:8px;text-align:center;} th{background-color:#4CAF50;color:white;}</style></head>\n<body>\n";
    archivo << "<h1 style=\"text-align:center;\">Tabla de Tokens</h1>\n<table>\n<tr><th>No.</th><th>Lexema</th><th>Tipo</th><th>L&iacute;nea</th><th>Columna</th></tr>\n";
    int cont = 1;
    for (const auto& t : tokens) {
        if (t.getType() == TokenType::FIN_ARCHIVO) continue;
        archivo << "<tr><td>" << cont++ << "</td><td>" << t.getLexeme() << "</td><td>" << t.getTypeName() << "</td><td>" << t.getLine() << "</td><td>" << t.getColumn() << "</td></tr>\n";
    }
    archivo << "</table></body></html>";
    archivo.close();
}

void ReportGenerator::generarReporteErrores(const std::vector<ErrorInfo>& errores) {
    std::ofstream archivo("reporte_errores.html");
    if (!archivo.is_open()) return;
    archivo << "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>Reporte de Errores</title>\n";
    archivo << "<style>body{font-family:Arial;} table{width:80%;margin:auto;border-collapse:collapse;} th,td{border:1px solid #ccc;padding:8px;text-align:center;} th{background-color:#e74c3c;color:white;}</style></head>\n<body>\n";
    archivo << "<h1 style=\"text-align:center;color:#c0392b;\">Errores Encontrados</h1>\n";
    if (errores.empty()) {
        archivo << "<h3 style=\"text-align:center;color:green;\">&iexcl;An&aacute;lisis Limpio! Cero Errores.</h3>\n";
    } else {
        archivo << "<table>\n<tr><th>No.</th><th>Lexema</th><th>Tipo</th><th>Descripci&oacute;n</th><th>L&iacute;nea</th><th>Columna</th></tr>\n";
        for (size_t i = 0; i < errores.size(); i++) {
            std::string tipo = (errores[i].type == ErrorType::LEXICO) ? "L&eacute;xico" : "Sint&aacute;ctico";
            archivo << "<tr><td>" << (i+1) << "</td><td>" << errores[i].lexeme << "</td><td>" << tipo << "</td><td>" << errores[i].description << "</td><td>" << errores[i].line << "</td><td>" << errores[i].column << "</td></tr>\n";
        }
        archivo << "</table>\n";
    }
    archivo << "</body></html>";
    archivo.close();
}

void ReportGenerator::generarReporteKanban(const NodoArbol* raiz) {
    if (!raiz) return;
    std::vector<InfoColumna> tablero;
    buscarColumnas(raiz, tablero);

    std::ofstream archivo("reporte_kanban.html");
    if (!archivo.is_open()) return;

    archivo << "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>Tablero Kanban</title>\n";
    archivo << "<style>\n  body { font-family: Arial, sans-serif; background-color: #0079bf; margin: 0; padding: 20px; }\n  h1 { color: white; margin-bottom: 20px; }\n  .tablero { display: flex; gap: 20px; overflow-x: auto; align-items: flex-start; }\n  .columna { background-color: #ebecf0; border-radius: 8px; width: 300px; min-width: 300px; padding: 10px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }\n  .col-header { font-weight: bold; margin-bottom: 15px; color: #172b4d; }\n  .tarea { background-color: white; border-radius: 5px; padding: 12px; margin-bottom: 10px; box-shadow: 0 1px 2px rgba(0,0,0,0.1); }\n  .t-nombre { font-weight: bold; color: #172b4d; margin-bottom: 8px; }\n  .etiqueta { display: inline-block; padding: 4px 8px; border-radius: 3px; color: white; font-size: 12px; font-weight: bold; margin-bottom: 8px; }\n  .p-ALTA { background-color: #eb5a46; }\n  .p-MEDIA { background-color: #f2d600; color: #172b4d; }\n  .p-BAJA { background-color: #61bd4f; }\n  .t-detalle { font-size: 12px; color: #5e6c84; margin-bottom: 5px; }\n  .responsable { display: inline-block; background-color: #dfe1e6; padding: 4px 8px; border-radius: 12px; font-size: 12px; color: #172b4d; }\n</style>\n</head>\n<body>\n";
    
    std::string nombreTablero = "Proyecto LFP";
    if (raiz->hijos.size() > 1 && raiz->hijos[1]->esTerminal) {
        nombreTablero = limpiarComillas(raiz->hijos[1]->etiqueta);
    }
    
    archivo << "<h1>" << nombreTablero << "</h1>\n<div class=\"tablero\">\n";

    for (const auto& col : tablero) {
        archivo << "  <div class=\"columna\">\n    <div class=\"col-header\">" << col.nombre << " (" << col.tareas.size() << ")</div>\n";
        for (const auto& t : col.tareas) {
            archivo << "    <div class=\"tarea\">\n      <div class=\"t-nombre\">" << t.nombre << "</div>\n      <div class=\"etiqueta p-" << t.prioridad << "\">" << t.prioridad << "</div>\n      <div class=\"t-detalle\">Fecha l&iacute;mite: " << t.fecha << "</div>\n      <div class=\"responsable\">" << t.responsable << "</div>\n    </div>\n";
        }
        archivo << "  </div>\n";
    }
    archivo << "</div>\n</body>\n</html>";
}

struct ResumenCarga { int total = 0, alta = 0, media = 0, baja = 0; };

void ReportGenerator::generarReporteResponsables(const NodoArbol* raiz) {
    if (!raiz) return;
    std::vector<InfoColumna> tablero;
    buscarColumnas(raiz, tablero);
    std::map<std::string, ResumenCarga> mapaCargas;
    int totalTareasProyecto = 0;

    for (const auto& col : tablero) {
        for (const auto& t : col.tareas) {
            mapaCargas[t.responsable].total++;
            if (t.prioridad == "ALTA") mapaCargas[t.responsable].alta++;
            else if (t.prioridad == "MEDIA") mapaCargas[t.responsable].media++;
            else if (t.prioridad == "BAJA") mapaCargas[t.responsable].baja++;
            totalTareasProyecto++;
        }
    }

    std::ofstream archivo("reporte_responsables.html");
    if (!archivo.is_open()) return;

    archivo << "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>Carga por Responsable</title>\n";
    archivo << "<style>\n  body { font-family: Arial, sans-serif; background-color: #f4f5f7; padding: 20px; }\n  .contenedor { background-color: white; border-radius: 8px; padding: 20px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); width: 80%; margin: 0 auto; border: 2px solid #0079bf; }\n  h2 { color: #172b4d; }\n  table { width: 100%; border-collapse: collapse; margin-top: 15px; }\n  th, td { padding: 12px; text-align: left; border-bottom: 1px solid #dfe1e6; }\n  th { background-color: #0079bf; color: white; }\n  .barra-fondo { width: 100%; background-color: #ebecf0; border-radius: 10px; overflow: hidden; display: flex; align-items: center; }\n  .barra-progreso { background-color: #0079bf; height: 20px; color: white; font-size: 12px; text-align: center; line-height: 20px; border-radius: 10px; }\n</style>\n</head>\n<body>\n";
    
    archivo << "<div class=\"contenedor\">\n  <h2>Carga por Responsable</h2>\n  <table>\n    <tr><th>Responsable</th><th>Tareas Asignadas</th><th>Alta</th><th>Media</th><th>Baja</th><th>Distribuci&oacute;n</th></tr>\n";

    for (const auto& par : mapaCargas) {
        int porcentaje = totalTareasProyecto > 0 ? (par.second.total * 100) / totalTareasProyecto : 0;
        archivo << "    <tr>\n      <td>" << par.first << "</td>\n      <td>" << par.second.total << "</td>\n      <td>" << par.second.alta << "</td>\n      <td>" << par.second.media << "</td>\n      <td>" << par.second.baja << "</td>\n      <td>\n        <div class=\"barra-fondo\">\n          <div class=\"barra-progreso\" style=\"width: " << porcentaje << "%;\">" << porcentaje << "%</div>\n        </div>\n      </td>\n    </tr>\n";
    }
    archivo << "  </table>\n</div>\n</body>\n</html>";
}
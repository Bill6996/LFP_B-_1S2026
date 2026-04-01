#include "ReportGenerator.h"
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <algorithm>

using namespace std;

// ── CSS base compartido ──────────────────────────────────────────
static string baseCss() {
    return R"(
        body { font-family: Arial, sans-serif; margin: 20px; background: #f5f5f5; }
        h1 { color: #1A4731; border-bottom: 3px solid #1A4731; padding-bottom: 8px; }
        h2 { color: #2E7D52; }
        table { border-collapse: collapse; width: 100%; background: white;
                box-shadow: 0 2px 4px rgba(0,0,0,0.1); margin-bottom: 30px; }
        th { background: #1A4731; color: white; padding: 10px; text-align: left; }
        td { border: 1px solid #ddd; padding: 8px; }
        tr:nth-child(even) { background: #f9f9f9; }
        .activo   { background: #28a745; color: white; padding: 3px 8px;
                    border-radius: 4px; font-weight: bold; }
        .sindiag  { background: #fd7e14; color: white; padding: 3px 8px;
                    border-radius: 4px; font-weight: bold; }
        .critico  { background: #dc3545; color: white; padding: 3px 8px;
                    border-radius: 4px; font-weight: bold; }
        .baja     { background: #007bff; color: white; padding: 3px 8px;
                    border-radius: 4px; font-weight: bold; }
        .normal   { background: #28a745; color: white; padding: 3px 8px;
                    border-radius: 4px; font-weight: bold; }
        .alta     { background: #fd7e14; color: white; padding: 3px 8px;
                    border-radius: 4px; font-weight: bold; }
        .saturada { background: #dc3545; color: white; padding: 3px 8px;
                    border-radius: 4px; font-weight: bold; }
        .confirmada { background: #28a745; color: white; padding: 3px 8px;
                      border-radius: 4px; font-weight: bold; }
        .conflicto  { background: #dc3545; color: white; padding: 3px 8px;
                      border-radius: 4px; font-weight: bold; }
        .pendiente  { background: #fd7e14; color: white; padding: 3px 8px;
                      border-radius: 4px; font-weight: bold; }
        .kpi-grid { display: grid; grid-template-columns: repeat(3,1fr);
                    gap: 16px; margin-bottom: 30px; }
        .kpi-card { background: white; border-radius: 8px; padding: 16px;
                    box-shadow: 0 2px 4px rgba(0,0,0,0.1);
                    border-left: 4px solid #1A4731; }
        .kpi-label { font-size: 12px; color: #666; margin-bottom: 4px; }
        .kpi-value { font-size: 22px; font-weight: bold; color: #1A4731; }
        .bar-bg { background: #e0e0e0; border-radius: 4px; height: 18px; width: 200px; }
        .bar-fill { height: 18px; border-radius: 4px; background: #2E7D52; }
        .bar-red  { background: #dc3545; }
    )";
}

// ── Helpers para extraer datos de tokens ─────────────────────────
struct Patient {
    string name, bloodType;
    int age = 0, room = 0;
    string condition, medicine, dose;
    bool hasDiag = false;
};

struct Doctor {
    string name, specialty, code;
};

struct Appointment {
    string patient, doctor, date, time;
};

struct Diagnosis {
    string patient, condition, medicine, dose;
};

static vector<Patient> extractPatients(const vector<Token>& tokens) {
    vector<Patient> patients;
    int i = 0;
    while (i < (int)tokens.size()) {
        if (tokens[i].type == TokenType::PACIENTE) {
            Patient p;
            p.age = -1;
            p.room = -1;
            p.bloodType = "N/A";
            p.name = "N/A";
            i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::SYMBOL) i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::STRING) {
                string n = tokens[i].lexeme;
                if (n.size() >= 2) n = n.substr(1, n.size()-2);
                // Verificar que el nombre no contenga corchetes
                // lo que indicaria una cadena sin cerrar
                if (n.find('[') != string::npos || n.find(':') != string::npos) {
                    p.name = "N/A (cadena sin cerrar)";
                } else {
                    p.name = n;
                }
                i++;
            } else {
                // Si no hay STRING valido el nombre es invalido
                p.name = "N/A (cadena sin cerrar)";
            }
            while (i < (int)tokens.size() && tokens[i].lexeme != "]") {
                if (tokens[i].type == TokenType::EDAD) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::NUMBER) {
                            p.age = stoi(tokens[i].lexeme);
                        } else {
                            p.age = -1;
                        }
                        i++;
                    }
                } else if (tokens[i].type == TokenType::TIPO_SANGRE) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::STRING) {
                            string s = tokens[i].lexeme;
                            if (s.size() >= 2) s = s.substr(1, s.size()-2);
                            p.bloodType = s;
                        } else {
                            p.bloodType = "N/A";
                        }
                        i++;
                    }
                } else if (tokens[i].type == TokenType::HABITACION) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::NUMBER) {
                            p.room = stoi(tokens[i].lexeme);
                        } else {
                            p.room = -1;
                        }
                        i++;
                    }
                } else { i++; }
            }
            patients.push_back(p);
        } else { i++; }
    }
    return patients;
}

static vector<Doctor> extractDoctors(const vector<Token>& tokens) {
    vector<Doctor> doctors;
    int i = 0;
    while (i < (int)tokens.size()) {
        if (tokens[i].type == TokenType::MEDICO) {
            Doctor d;
            d.name = "N/A";
            d.specialty = "N/A";
            d.code = "N/A";
            i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::SYMBOL) i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::STRING) {
                string n = tokens[i].lexeme;
                if (n.size() >= 2) n = n.substr(1, n.size()-2);
                d.name = n; i++;
            }
            while (i < (int)tokens.size() && tokens[i].lexeme != "]") {
                if (tokens[i].type == TokenType::ESPECIALIDAD) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        // Verificar que sea una especialidad valida
                        TokenType t = tokens[i].type;
                        if (t == TokenType::CARDIOLOGIA ||
                            t == TokenType::NEUROLOGIA  ||
                            t == TokenType::PEDIATRIA   ||
                            t == TokenType::CIRUGIA     ||
                            t == TokenType::MEDICINA_GENERAL ||
                            t == TokenType::ONCOLOGIA) {
                            d.specialty = tokens[i].lexeme;
                        } else {
                            d.specialty = "N/A";
                        }
                        i++;
                    }
                } else if (tokens[i].type == TokenType::CODIGO) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::STRING) {
                            string s = tokens[i].lexeme;
                            if (s.size() >= 2) s = s.substr(1, s.size()-2);
                            d.code = s;
                        } else {
                            d.code = "N/A";
                        }
                        i++;
                    }
                } else { i++; }
            }
            doctors.push_back(d);
        } else { i++; }
    }
    return doctors;
}

static vector<Appointment> extractAppointments(const vector<Token>& tokens) {
    vector<Appointment> appts;
    int i = 0;
    while (i < (int)tokens.size()) {
        if (tokens[i].type == TokenType::CITA) {
            Appointment a;
            a.patient = "N/A";
            a.doctor  = "N/A";
            a.date    = "N/A";
            a.time    = "N/A";
            i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::SYMBOL) i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::STRING) {
                string n = tokens[i].lexeme;
                if (n.size() >= 2) n = n.substr(1, n.size()-2);
                a.patient = n; i++;
            }
            if (i < (int)tokens.size() && tokens[i].type == TokenType::CON) i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::STRING) {
                string n = tokens[i].lexeme;
                if (n.size() >= 2) n = n.substr(1, n.size()-2);
                a.doctor = n; i++;
            }
            while (i < (int)tokens.size() && tokens[i].lexeme != "]") {
                if (tokens[i].type == TokenType::FECHA_KW) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::DATE) {
                            a.date = tokens[i].lexeme;
                        } else {
                            a.date = "FECHA-INVALIDA";
                        }
                        i++;
                    }
                } else if (tokens[i].type == TokenType::HORA_KW) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::TIME) {
                            a.time = tokens[i].lexeme;
                        } else {
                            a.time = "HORA-INVALIDA";
                        }
                        i++;
                    }
                } else { i++; }
            }
            appts.push_back(a);
        } else { i++; }
    }
    return appts;
}

static vector<Diagnosis> extractDiagnoses(const vector<Token>& tokens) {
    vector<Diagnosis> diags;
    int i = 0;
    while (i < (int)tokens.size()) {
        if (tokens[i].type == TokenType::DIAGNOSTICO) {
            Diagnosis d;
            d.patient   = "N/A";
            d.condition = "N/A";
            d.medicine  = "N/A";
            d.dose      = "N/A";
            i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::SYMBOL) i++;
            if (i < (int)tokens.size() && tokens[i].type == TokenType::STRING) {
                string n = tokens[i].lexeme;
                if (n.size() >= 2) n = n.substr(1, n.size()-2);
                d.patient = n; i++;
            }
            while (i < (int)tokens.size() && tokens[i].lexeme != "]") {
                if (tokens[i].type == TokenType::CONDICION) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::STRING) {
                            string s = tokens[i].lexeme;
                            if (s.size() >= 2) s = s.substr(1, s.size()-2);
                            d.condition = s;
                        } else {
                            d.condition = "N/A";
                        }
                        i++;
                    }
                } else if (tokens[i].type == TokenType::MEDICAMENTO) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        if (tokens[i].type == TokenType::STRING) {
                            string s = tokens[i].lexeme;
                            if (s.size() >= 2) s = s.substr(1, s.size()-2);
                            d.medicine = s;
                        } else {
                            d.medicine = "N/A";
                        }
                        i++;
                    }
                } else if (tokens[i].type == TokenType::DOSIS_KW) {
                    i += 2;
                    if (i < (int)tokens.size()) {
                        TokenType t = tokens[i].type;
                        if (t == TokenType::DIARIA      ||
                            t == TokenType::CADA_8_HORAS ||
                            t == TokenType::CADA_12_HORAS||
                            t == TokenType::SEMANAL) {
                            d.dose = tokens[i].lexeme;
                        } else {
                            d.dose = "N/A";
                        }
                        i++;
                    }
                } else { i++; }
            }
            diags.push_back(d);
        } else { i++; }
    }
    return diags;
}

// ── Reporte de Tokens ────────────────────────────────────────────
void ReportGenerator::generateTokenReport(
    const vector<Token>& tokens, const string& filename) {
    ofstream f(filename);
    f << "<html><head><meta charset='UTF-8'><style>" << baseCss() << "</style></head><body>";
    f << "<h1>Tabla de Tokens</h1>";
    f << "<table><tr><th>No.</th><th>Lexema</th><th>Tipo</th><th>Linea</th><th>Columna</th></tr>";
    int n = 1;
    for (auto& t : tokens)
        f << "<tr><td>" << n++ << "</td><td>" << t.lexeme << "</td><td>"
          << t.getTypeString() << "</td><td>" << t.line << "</td><td>" << t.column << "</td></tr>";
    f << "</table></body></html>";
    f.close();
}

// ── Reporte de Errores ───────────────────────────────────────────
void ReportGenerator::generateErrorReport(
    const vector<LexicalError>& errors, const string& filename) {
    ofstream f(filename);
    f << "<html><head><meta charset='UTF-8'><style>" << baseCss() << "</style></head><body>";
    f << "<h1>Tabla de Errores Lexicos</h1>";
    f << "<table><tr><th>No.</th><th>Lexema</th><th>Tipo de Error</th>"
      << "<th>Descripcion</th><th>Linea</th><th>Columna</th><th>Gravedad</th></tr>";
    int n = 1;
    for (auto& e : errors) {
        string cls = (e.severity == "CRITICO") ? "critico" : "conflicto";
        f << "<tr><td>" << n++ << "</td><td>" << e.lexeme << "</td><td>"
          << e.errorType << "</td><td>" << e.description << "</td><td>"
          << e.line << "</td><td>" << e.column << "</td><td><span class='"
          << cls << "'>" << e.severity << "</span></td></tr>";
    }
    f << "</table></body></html>";
    f.close();
}

// ── Reporte 1: Historial de Pacientes ───────────────────────────
void ReportGenerator::generatePatientReport(
    const vector<Token>& tokens, const string& filename) {
    auto patients = extractPatients(tokens);
    auto diags    = extractDiagnoses(tokens);

    map<string,Diagnosis> diagMap;
    for (auto& d : diags) diagMap[d.patient] = d;

    ofstream f(filename);
    f << "<html><head><meta charset='UTF-8'><style>" << baseCss() << "</style></head><body>";
    f << "<h1>Reporte 1 - Historial de Pacientes</h1>";
    f << "<table><tr><th>Paciente</th><th>Edad</th><th>Sangre</th>"
      << "<th>Diagnostico activo</th><th>Medicamento / Dosis</th><th>Estado</th></tr>";

    for (auto& p : patients) {
        string estado, cls, diag = "Sin diagnostico registrado", med = "—";
        if (diagMap.count(p.name)) {
            auto& d = diagMap[p.name];
            diag  = d.condition;
            med   = d.medicine + " / " + d.dose;
            estado = "ACTIVO"; cls = "activo";
        } else {
            estado = "SIN DIAG."; cls = "sindiag";
        }
        string edadStr  = (p.age  == -1) ? "N/A" : to_string(p.age);
        string roomStr  = (p.room == -1) ? "N/A" : to_string(p.room);
        f << "<tr><td>" << p.name << "</td><td>" << edadStr << "</td><td>"          << p.bloodType << "</td><td>" << diag << "</td><td>" << med
          << "</td><td><span class='" << cls << "'>" << estado << "</span></td></tr>";
    }
    f << "</table></body></html>";
    f.close();
}

// ── Reporte 2: Carga de Medicos ──────────────────────────────────
void ReportGenerator::generateDoctorReport(
    const vector<Token>& tokens, const string& filename) {
    auto doctors = extractDoctors(tokens);
    auto appts   = extractAppointments(tokens);

    ofstream f(filename);
    f << "<html><head><meta charset='UTF-8'><style>" << baseCss() << "</style></head><body>";
    f << "<h1>Reporte 2 - Carga de Medicos por Especialidad</h1>";
    f << "<table><tr><th>Medico</th><th>Codigo</th><th>Especialidad</th>"
      << "<th>Citas</th><th>Pacientes</th><th>Nivel de Carga</th></tr>";

    for (auto& doc : doctors) {
        int citas = 0; set<string> pacs;
        for (auto& a : appts)
            if (a.doctor == doc.name) { citas++; pacs.insert(a.patient); }

        string nivel, cls;
        if      (citas <= 3) { nivel = "BAJA";     cls = "baja"; }
        else if (citas <= 6) { nivel = "NORMAL";   cls = "normal"; }
        else if (citas <= 8) { nivel = "ALTA";     cls = "alta"; }
        else                 { nivel = "SATURADA"; cls = "saturada"; }

        f << "<tr><td>" << doc.name << "</td><td>" << doc.code << "</td><td>"
          << doc.specialty << "</td><td>" << citas << "</td><td>" << pacs.size()
          << "</td><td><span class='" << cls << "'>" << nivel << "</span></td></tr>";
    }
    f << "</table></body></html>";
    f.close();
}

// ── Reporte 3: Agenda de Citas ───────────────────────────────────
void ReportGenerator::generateAppointmentReport(
    const vector<Token>& tokens, const string& filename) {
    auto appts   = extractAppointments(tokens);
    auto doctors = extractDoctors(tokens);

    map<string,string> docSpec;
    for (auto& d : doctors) docSpec[d.name] = d.specialty;

    // Detectar conflictos
    map<string,int> slotCount;
    for (auto& a : appts) slotCount[a.doctor + a.date + a.time]++;

    // Ordenar por fecha y hora
    sort(appts.begin(), appts.end(), [](const Appointment& a, const Appointment& b){
        return (a.date + a.time) < (b.date + b.time);
    });

    ofstream f(filename);
    f << "<html><head><meta charset='UTF-8'><style>" << baseCss() << "</style></head><body>";
    f << "<h1>Reporte 3 - Agenda de Citas</h1>";
    f << "<table><tr><th>Fecha</th><th>Hora</th><th>Paciente</th>"
      << "<th>Medico</th><th>Especialidad</th><th>Estado</th></tr>";

    for (auto& a : appts) {
        string key = a.doctor + a.date + a.time;
        string estado, cls;
        if (slotCount[key] > 1) {
            estado = "CONFLICTO"; cls = "conflicto";
        } else {
            estado = "CONFIRMADA"; cls = "confirmada";
        }
        string spec = docSpec.count(a.doctor) ? docSpec[a.doctor] : "—";
        f << "<tr><td>" << a.date << "</td><td>" << a.time << "</td><td>"
          << a.patient << "</td><td>" << a.doctor << "</td><td>" << spec
          << "</td><td><span class='" << cls << "'>" << estado << "</span></td></tr>";
    }
    f << "</table></body></html>";
    f.close();
}

// ── Reporte 4: Estadistico General ──────────────────────────────
void ReportGenerator::generateStatsReport(
    const vector<Token>& tokens, const string& filename) {
    auto patients = extractPatients(tokens);
    auto doctors  = extractDoctors(tokens);
    auto appts    = extractAppointments(tokens);
    auto diags    = extractDiagnoses(tokens);

    // KPIs
    int totalPac  = patients.size();
    int totalMed  = doctors.size();
    int totalCitas = appts.size();

    map<string,int> slotCount;
    for (auto& a : appts) slotCount[a.doctor + a.date + a.time]++;
    int conflictos = 0;
    for (auto& kv : slotCount) if (kv.second > 1) conflictos++;

    int conDiag = diags.size();
    double pct  = totalPac > 0 ? (conDiag * 100.0 / totalPac) : 0;

    map<string,int> medCount;
    for (auto& d : diags) medCount[d.medicine]++;
    string topMed = "—"; int topMedN = 0;
    for (auto& kv : medCount) if (kv.second > topMedN) { topMedN = kv.second; topMed = kv.first; }

    map<string,int> docCitas;
    for (auto& a : appts) docCitas[a.doctor]++;
    string topDoc = "—", topSpec = "—"; int topDocN = 0;
    for (auto& d : doctors)
        if (docCitas.count(d.name) && docCitas[d.name] > topDocN) {
            topDocN = docCitas[d.name]; topDoc = d.name; topSpec = d.specialty; }

    double avgAge = 0;
    for (auto& p : patients) avgAge += p.age;
    if (totalPac > 0) avgAge /= totalPac;

    ofstream f(filename);
    f << "<html><head><meta charset='UTF-8'><style>" << baseCss() << "</style></head><body>";
    f << "<h1>Reporte 4 - Estadistico General del Hospital</h1>";
    f << "<h2>Seccion A - Indicadores Clave</h2>";
    f << "<div class='kpi-grid'>";

    auto kpi = [&](string label, string value) {
        f << "<div class='kpi-card'><div class='kpi-label'>" << label
          << "</div><div class='kpi-value'>" << value << "</div></div>";
    };

    kpi("Total de pacientes",  to_string(totalPac));
    kpi("Total de medicos",    to_string(totalMed));
    kpi("Total de citas",      to_string(totalCitas));
    kpi("Citas con conflicto", to_string(conflictos));
    kpi("Pacientes con diagnostico", to_string(conDiag) + " de " +
                                         to_string(totalPac) + " (" + to_string((int)pct) + "%)");
    kpi("Medicamento mas prescrito", topMed + " (" + to_string(topMedN) + ")");
    kpi("Especialidad con mas citas", topSpec + " - " + topDoc);

    ostringstream avgStr;
    avgStr.precision(1); avgStr << fixed << avgAge;
    kpi("Promedio de edad", avgStr.str() + " anos");
    f << "</div>";

    // Seccion B
    f << "<h2>Seccion B - Distribucion por Especialidad</h2>";
    f << "<table><tr><th>Especialidad</th><th>Medicos</th><th>Citas</th>"
      << "<th>Pacientes</th><th>Ocupacion</th></tr>";

    map<string,int> specMeds, specCitas;
    map<string,set<string>> specPacs;
    for (auto& d : doctors) specMeds[d.specialty]++;
    for (auto& a : appts) {
        for (auto& d : doctors)
            if (d.name == a.doctor) {
                specCitas[d.specialty]++;
                specPacs[d.specialty].insert(a.patient);
            }
    }

    for (auto& kv : specMeds) {
        int c = specCitas.count(kv.first) ? specCitas[kv.first] : 0;
        int pacs = specPacs.count(kv.first) ? specPacs[kv.first].size() : 0;
        int pct2 = totalCitas > 0 ? (c * 100 / totalCitas) : 0;
        string barCls = pct2 >= 80 ? "bar-red" : "";
        f << "<tr><td>" << kv.first << "</td><td>" << kv.second << "</td><td>"
          << c << "</td><td>" << pacs << "</td><td>"
          << "<div class='bar-bg'><div class='bar-fill " << barCls
          << "' style='width:" << pct2 << "%'></div></div> " << pct2 << "%"
          << "</td></tr>";
    }
    f << "</table></body></html>";
    f.close();
}

// ── Graphviz DOT ─────────────────────────────────────────────────
void ReportGenerator::generateGraphviz(
    const vector<Token>& tokens, const string& filename) {
    auto patients = extractPatients(tokens);
    auto doctors  = extractDoctors(tokens);
    auto appts    = extractAppointments(tokens);
    auto diags    = extractDiagnoses(tokens);

    ofstream f(filename);
    f << "digraph Hospital {\n";
    f << "  rankdir=TB;\n";
    f << "  node [shape=box, style=filled, fontname=\"Arial\"];\n\n";
    f << "  H [label=\"Hospital\", fillcolor=\"#1A4731\", fontcolor=white, shape=ellipse];\n";
    f << "  P [label=\"PACIENTES\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    f << "  M [label=\"MEDICOS\",   fillcolor=\"#2E7D52\", fontcolor=white];\n";
    f << "  C [label=\"CITAS\",     fillcolor=\"#2E7D52\", fontcolor=white];\n";
    f << "  D [label=\"DIAGNOSTICOS\", fillcolor=\"#2E7D52\", fontcolor=white];\n";
    f << "  H -> P; H -> M; H -> C; H -> D;\n\n";

    for (int i = 0; i < (int)patients.size(); i++) {
        f << "  p" << i << " [label=\"" << patients[i].name
          << "\\n" << patients[i].bloodType
          << "\", fillcolor=\"#D4EDDA\"];\n";
        f << "  P -> p" << i << ";\n";
    }

    for (int i = 0; i < (int)doctors.size(); i++) {
        f << "  m" << i << " [label=\"" << doctors[i].name
          << "\\n" << doctors[i].code << " | " << doctors[i].specialty
          << "\", fillcolor=\"#D6EAF8\"];\n";
        f << "  M -> m" << i << ";\n";
    }

    for (auto& a : appts) {
        int pi = -1, mi = -1;
        for (int i = 0; i < (int)patients.size(); i++)
            if (patients[i].name == a.patient) { pi = i; break; }
        for (int i = 0; i < (int)doctors.size(); i++)
            if (doctors[i].name == a.doctor) { mi = i; break; }
        if (pi >= 0 && mi >= 0)
            f << "  p" << pi << " -> m" << mi
              << " [label=\"" << a.date << " " << a.time
              << "\", color=\"#E67E22\", style=dashed];\n";
    }

    for (int i = 0; i < (int)diags.size(); i++) {
        f << "  d" << i << " [label=\"" << diags[i].condition
          << "\\n" << diags[i].medicine
          << "\", fillcolor=\"#FDEBD0\"];\n";
        f << "  D -> d" << i << ";\n";
        for (int j = 0; j < (int)patients.size(); j++)
            if (patients[j].name == diags[i].patient)
                f << "  d" << i << " -> p" << j
                  << " [label=\"diagnostico activo\", color=\"#C0392B\"];\n";
    }

    f << "}\n";
    f.close();
}
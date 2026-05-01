#include "LexicalAnalyzer.h"
#include "SyntaxAnalyzer.h"
#include "ErrorManager.h"
#include "ReportGenerator.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cctype> 

//para windows
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #define SOCKET int
    #define INVALID_SOCKET -1
    #define closesocket close
#endif

using namespace std;

string escJson(const string& s) {
    string out;
    for (char c : s) {
        if (c == '"') out += "\\\"";
        else if (c == '\\') out += "\\\\";
        else if (c == '\n') out += "\\n";
        else if (c == '\r') out += "\\r";
        else if (c == '\t') out += "\\t";
        else out += c;
    }
    return out;
}

int dotCounter = 0;
void dotNodo(ostringstream& out, const NodoArbol* nodo, int padre) {
    if (!nodo) return;
    int id = ++dotCounter;
    string color = nodo->esTerminal ? "#D6EAF8" : "#2E75B6";
    string fcolor = nodo->esTerminal ? "black" : "white";
    
    out << "  n" << id << " [label=\"" << escJson(nodo->etiqueta) << "\", fillcolor=\"" << color << "\", fontcolor=\"" << fcolor << "\"];\n";
    if (padre >= 0) out << "  n" << padre << " -> n" << id << ";\n";
    for (auto* hijo : nodo->hijos) {
        dotNodo(out, hijo, id);
    }
}

string generarDot(const NodoArbol* raiz) {
    if (!raiz) return "";
    dotCounter = 0;
    ostringstream out;
    out << "digraph ArbolDerivacion {\n  rankdir=TB;\n  node [shape=box, style=filled, fontname=\"Arial\"];\n";
    dotNodo(out, raiz, -1);
    out << "}\n";
    return out.str();
}

string analizarYResponder(const string& source) {
    remove("reporte_tokens.html");
    remove("reporte_errores.html");
    remove("reporte_kanban.html");
    remove("reporte_responsables.html");

    ErrorManager errMgr;
    LexicalAnalyzer lexer(source);
    vector<Token> tokens = lexer.analyze();
    
    vector<Token> tokensParaParser;
    for (const auto& t : tokens) {
        if (t.getType() == TokenType::ERROR_LEXICO) {
            errMgr.addError(ErrorType::LEXICO, t.getLexeme(), "Caracter invalido o secuencia no reconocida", t.getLine(), t.getColumn());
            
            string lex = t.getLexeme();
            if (!lex.empty()) {
                if (lex.front() == '"') {
                    // Usamos emplace_back para evadir el conflicto de nombres de Windows
                    tokensParaParser.emplace_back(TokenType::LIT_CADENA, lex, t.getLine(), t.getColumn());
                } else if (isdigit(static_cast<unsigned char>(lex.front()))) {
                    if (lex.find('-') != string::npos) {
                        tokensParaParser.emplace_back(TokenType::LIT_FECHA, lex, t.getLine(), t.getColumn());
                    } else {
                        tokensParaParser.emplace_back(TokenType::LIT_ENTERO, lex, t.getLine(), t.getColumn());
                    }
                }
            }
        } else {
            tokensParaParser.push_back(t);
        }
    }

    NodoArbol* raiz = nullptr;
    if (!tokensParaParser.empty()) {
        SyntaxAnalyzer parser(tokensParaParser, errMgr);
        raiz = parser.parse();
    }

    bool hayErrorSintactico = false;
    for (const auto& err : errMgr.getErrors()) {
        if (err.type == ErrorType::SINTACTICO) {
            hayErrorSintactico = true;
            break;
        }
    }

    if (!hayErrorSintactico) {
        ReportGenerator reporteador;
        reporteador.generarReporteTokens(tokens); 
        reporteador.generarReporteErrores(errMgr.getErrors());
        if (raiz != nullptr) {
            reporteador.generarReporteKanban(raiz);
            reporteador.generarReporteResponsables(raiz);
        }
    }

    ostringstream json;
    json << "{\n  \"tokens\": [\n";
    for (size_t i = 0; i < tokens.size(); i++) {
        const auto& t = tokens[i];
        if (t.getType() == TokenType::FIN_ARCHIVO) continue;
        json << "    {\"numero\":" << (i+1) << ",\"lexema\":\"" << escJson(t.getLexeme()) << "\",\"tipo\":\"" << t.getTypeName() << "\",\"linea\":" << t.getLine() << ",\"columna\":" << t.getColumn() << "}";
        if (i + 1 < tokens.size() && tokens[i+1].getType() != TokenType::FIN_ARCHIVO) json << ",\n";
        else json << "\n";
    }
    json << "  ],\n  \"errores\": [\n";
    
    const auto& listaErrores = errMgr.getErrors();
    for (size_t i = 0; i < listaErrores.size(); i++) {
        const auto& e = listaErrores[i];
        string tipoStr = (e.type == ErrorType::LEXICO) ? "Lexico" : "Sintactico";
        json << "    {\"numero\":" << (i+1) << ",\"lexema\":\"" << escJson(e.lexeme) << "\",\"tipo\":\"" << tipoStr << "\",\"descripcion\":\"" << escJson(e.description) << "\",\"linea\":" << e.line << ",\"columna\":" << e.column << "}";
        if (i + 1 < listaErrores.size()) json << ",\n";
        else json << "\n";
    }
    
    json << "  ],\n  \"dot\": \"" << escJson(generarDot(raiz)) << "\",\n";
    json << "  \"exitoso\": " << (hayErrorSintactico ? "false" : "true") << "\n}";
    
    if (raiz) delete raiz;
    return json.str();
}

string extraerBody(const string& request) {
    size_t pos = request.find("\r\n\r\n");
    if (pos == string::npos) return "";
    return request.substr(pos + 4);
}

string extraerSource(const string& body) {
    size_t pos = body.find("\"source\"");
    if (pos == string::npos) return "";
    pos = body.find(':', pos);
    if (pos == string::npos) return "";
    pos = body.find('"', pos);
    if (pos == string::npos) return "";
    pos++; 
    string result;
    while (pos < body.size() && body[pos] != '"') {
        if (body[pos] == '\\' && pos + 1 < body.size()) {
            char next = body[++pos];
            if (next == 'n') result += '\n';
            else if (next == 'r') result += '\r';
            else if (next == 't') result += '\t';
            else if (next == '"') result += '"';
            else if (next == '\\') result += '\\';
            else result += next;
        } else {
            result += body[pos];
        }
        pos++;
    }
    return result;
}

int main() {
#ifdef _WIN32
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    SOCKET serverFd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    bind(serverFd, (sockaddr*)&addr, sizeof(addr));
    listen(serverFd, 10);
    cout << "Servidor en http://localhost:8080\n";

    while (true) {
        SOCKET clientFd = accept(serverFd, nullptr, nullptr);
        if (clientFd == INVALID_SOCKET) continue;

#ifdef _WIN32
        DWORD timeout = 1000;
        setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout));
#else
        struct timeval tv;
        tv.tv_sec = 1; tv.tv_usec = 0;
        setsockopt(clientFd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
#endif

        char buf[8192];
        int n = recv(clientFd, buf, sizeof(buf) - 1, 0);
        if (n <= 0) { closesocket(clientFd); continue; }
        
        buf[n] = '\0';
        string request(buf);

        size_t headerEnd = request.find("\r\n\r\n");
        if (headerEnd != string::npos) {
            size_t clPos = request.find("Content-Length: ");
            if (clPos != string::npos) {
                size_t clEnd = request.find("\r", clPos);
                int contentLength = stoi(request.substr(clPos + 16, clEnd - (clPos + 16)));
                int bodyReceived = request.length() - (headerEnd + 4);
                
                while (bodyReceived < contentLength) {
                    char temp[4096];
                    int bytes = recv(clientFd, temp, sizeof(temp) - 1, 0);
                    if (bytes <= 0) break;
                    temp[bytes] = '\0';
                    request += temp;
                    bodyReceived += bytes;
                }
            }
        }

        string responseBody;
        string contentType = "application/json";
        int statusCode = 200;
        string statusText = "OK";
        
        string cors = "Access-Control-Allow-Origin: *\r\nAccess-Control-Allow-Methods: POST, GET, OPTIONS\r\nAccess-Control-Allow-Headers: Content-Type\r\n";

        if (request.find("OPTIONS") == 0) {
            string resp = "HTTP/1.1 204 No Content\r\nConnection: close\r\n" + cors + "\r\n";
            send(clientFd, resp.c_str(), resp.size(), 0);
            closesocket(clientFd);
            continue;
        }

        auto servirArchivo = [&](const string& ruta) {
            ifstream archivo(ruta);
            if(archivo.is_open()){
                ostringstream ss; ss << archivo.rdbuf();
                responseBody = ss.str();
                contentType = "text/html; charset=UTF-8";
            } else {
                responseBody = "<h1 style=\"font-family: Arial; text-align: center; color: #c0392b; margin-top: 50px;\">Archivo Bloqueado o No Generado</h1><p style=\"font-family: Arial; text-align: center;\">Se detecto un error sintactico y la generacion de reportes fue abortada por seguridad.</p>";
                contentType = "text/html; charset=UTF-8";
                statusCode = 404;
                statusText = "Not Found";
            }
        };

        if (request.find("POST /analyze") != string::npos) {
            string body = extraerBody(request);
            string source = extraerSource(body);
            responseBody = analizarYResponder(source);
        } 
        else if (request.find("GET /reporte_tokens") != string::npos) servirArchivo("reporte_tokens.html");
        else if (request.find("GET /reporte_errores") != string::npos) servirArchivo("reporte_errores.html");
        else if (request.find("GET /reporte_kanban") != string::npos) servirArchivo("reporte_kanban.html");
        else if (request.find("GET /reporte_responsables") != string::npos) servirArchivo("reporte_responsables.html");
        else {
            responseBody = "";
            statusCode = 404;
            statusText = "Not Found";
        }

        string header = "HTTP/1.1 " + to_string(statusCode) + " " + statusText + "\r\n"
                        "Connection: close\r\n" + 
                        cors + 
                        "Content-Type: " + contentType + "\r\n"
                        "Content-Length: " + to_string(responseBody.size()) + "\r\n\r\n";
                        
        string full = header + responseBody;
        send(clientFd, full.c_str(), full.size(), 0);
        closesocket(clientFd);
    }
    
    closesocket(serverFd);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
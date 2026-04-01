#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <fstream>
#include <sstream>

#include "src/lexer/LexicalAnalyzer.h"
#include "src/reports/ReportGenerator.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("MedLexer - Analizador Lexico Hospitalario");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ── Cargar Archivo ───────────────────────────────────────────────
void MainWindow::on_btnCargarArchivo_clicked()
{
    QString ruta = QFileDialog::getOpenFileName(
        this,
        "Seleccionar archivo MedLang",
        "",
        "Archivos MedLang (*.med);;Todos los archivos (*)"
        );

    if (ruta.isEmpty()) return;

    archivoActual = ruta;

    // Leer con Qt directamente en lugar de ifstream
    QFile file(ruta);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    contenidoArchivo = in.readAll();
    file.close();

    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "El archivo esta vacio.");
        return;
    }

    ui->textArea->setText(contenidoArchivo);
    ui->textArea->setStyleSheet(
        "color: #00ff00; background-color: #1e1e1e; font-family: Courier;"
        );

    setWindowTitle("MedLexer | " + ruta);
    QMessageBox::information(this, "Listo",
                             "Archivo cargado: " + QString::number(contenidoArchivo.size()) + " caracteres.");
}

// ── Analizar ─────────────────────────────────────────────────────
void MainWindow::on_btnAnalizar_clicked()
{
    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero carga un archivo .med");
        return;
    }

    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input);
    lexer.analyze();

    actualizarTablaTokens();
    actualizarTablaErrores();

    QString resumen = QString("Analisis completado:\n"
                              "Tokens encontrados: %1\n"
                              "Errores lexicos: %2")
                          .arg(lexer.tokens.size())
                          .arg(lexer.errors.size());

    QMessageBox::information(this, "Analisis completado", resumen);
}

// ── Actualizar tabla de tokens ───────────────────────────────────
void MainWindow::actualizarTablaTokens()
{
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input);
    lexer.analyze();

    ui->tablaTokens->setRowCount(0);
    ui->tablaTokens->setColumnCount(5);
    ui->tablaTokens->setHorizontalHeaderLabels(
        {"No.", "Lexema", "Tipo", "Linea", "Columna"}
        );

    int row = 0;
    for (auto& t : lexer.tokens) {
        ui->tablaTokens->insertRow(row);
        ui->tablaTokens->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        ui->tablaTokens->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(t.lexeme)));
        ui->tablaTokens->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(t.getTypeString())));
        ui->tablaTokens->setItem(row, 3, new QTableWidgetItem(QString::number(t.line)));
        ui->tablaTokens->setItem(row, 4, new QTableWidgetItem(QString::number(t.column)));
        row++;
    }
    ui->tablaTokens->resizeColumnsToContents();
}

// ── Actualizar tabla de errores ──────────────────────────────────
void MainWindow::actualizarTablaErrores()
{
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input);
    lexer.analyze();

    ui->tablaErrores->setRowCount(0);
    ui->tablaErrores->setColumnCount(6);
    ui->tablaErrores->setHorizontalHeaderLabels(
        {"No.", "Lexema", "Tipo de Error", "Descripcion", "Linea", "Columna"}
        );

    int row = 0;
    for (auto& e : lexer.errors) {
        ui->tablaErrores->insertRow(row);
        ui->tablaErrores->setItem(row, 0, new QTableWidgetItem(QString::number(row + 1)));
        ui->tablaErrores->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(e.lexeme)));
        ui->tablaErrores->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(e.errorType)));
        ui->tablaErrores->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(e.description)));
        ui->tablaErrores->setItem(row, 4, new QTableWidgetItem(QString::number(e.line)));
        ui->tablaErrores->setItem(row, 5, new QTableWidgetItem(QString::number(e.column)));
        row++;
    }
    ui->tablaErrores->resizeColumnsToContents();
}

// ── Reportes HTML ────────────────────────────────────────────────
void MainWindow::on_btnReporte1_clicked()
{
    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero carga y analiza un archivo."); return;
    }
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input); lexer.analyze();
    QString path = QDir::currentPath() + "/reporte1_pacientes.html";
    ReportGenerator::generatePatientReport(lexer.tokens, path.toStdString());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::on_btnReporte2_clicked()
{
    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero carga y analiza un archivo."); return;
    }
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input); lexer.analyze();
    QString path = QDir::currentPath() + "/reporte2_medicos.html";
    ReportGenerator::generateDoctorReport(lexer.tokens, path.toStdString());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::on_btnReporte3_clicked()
{
    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero carga y analiza un archivo."); return;
    }
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input); lexer.analyze();
    QString path = QDir::currentPath() + "/reporte3_citas.html";
    ReportGenerator::generateAppointmentReport(lexer.tokens, path.toStdString());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::on_btnReporte4_clicked()
{
    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero carga y analiza un archivo."); return;
    }
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input); lexer.analyze();
    QString path = QDir::currentPath() + "/reporte4_estadistico.html";
    ReportGenerator::generateStatsReport(lexer.tokens, path.toStdString());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void MainWindow::on_btnGraphviz_clicked()
{
    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero carga y analiza un archivo."); return;
    }
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input); lexer.analyze();
    QString path = QDir::currentPath() + "/hospital.dot";
    ReportGenerator::generateGraphviz(lexer.tokens, path.toStdString());
    QMessageBox::information(this, "Graphviz",
                             "Archivo hospital.dot generado en:\n" + path);
}

void MainWindow::on_btnErrores_clicked()
{
    if (contenidoArchivo.isEmpty()) {
        QMessageBox::warning(this, "Aviso", "Primero carga y analiza un archivo."); return;
    }
    std::string input = contenidoArchivo.toStdString();
    LexicalAnalyzer lexer(input); lexer.analyze();
    QString path = QDir::currentPath() + "/reporte_errores.html";
    ReportGenerator::generateErrorReport(lexer.errors, path.toStdString());
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}
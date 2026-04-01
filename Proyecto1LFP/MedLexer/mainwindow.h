#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnCargarArchivo_clicked();
    void on_btnAnalizar_clicked();
    void on_btnReporte1_clicked();
    void on_btnReporte2_clicked();
    void on_btnReporte3_clicked();
    void on_btnReporte4_clicked();
    void on_btnGraphviz_clicked();
    void on_btnErrores_clicked();

private:
    Ui::MainWindow *ui;
    QString archivoActual;
    QString contenidoArchivo;

    void actualizarTablaTokens();
    void actualizarTablaErrores();
};

#endif
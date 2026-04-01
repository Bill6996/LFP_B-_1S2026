/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.8.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *btnCargarArchivo;
    QPushButton *btnAnalizar;
    QPushButton *btnReporte1;
    QPushButton *btnReporte2;
    QPushButton *btnReporte3;
    QPushButton *btnReporte4;
    QPushButton *btnGraphviz;
    QPushButton *btnErrores;
    QTextEdit *textArea;
    QTableWidget *tablaTokens;
    QTableWidget *tablaErrores;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(1100, 750);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        btnCargarArchivo = new QPushButton(centralwidget);
        btnCargarArchivo->setObjectName("btnCargarArchivo");
        btnCargarArchivo->setGeometry(QRect(10, 10, 150, 40));
        btnAnalizar = new QPushButton(centralwidget);
        btnAnalizar->setObjectName("btnAnalizar");
        btnAnalizar->setGeometry(QRect(170, 10, 150, 40));
        btnReporte1 = new QPushButton(centralwidget);
        btnReporte1->setObjectName("btnReporte1");
        btnReporte1->setGeometry(QRect(10, 60, 150, 35));
        btnReporte2 = new QPushButton(centralwidget);
        btnReporte2->setObjectName("btnReporte2");
        btnReporte2->setGeometry(QRect(170, 60, 150, 35));
        btnReporte3 = new QPushButton(centralwidget);
        btnReporte3->setObjectName("btnReporte3");
        btnReporte3->setGeometry(QRect(330, 60, 150, 35));
        btnReporte4 = new QPushButton(centralwidget);
        btnReporte4->setObjectName("btnReporte4");
        btnReporte4->setGeometry(QRect(490, 60, 160, 35));
        btnGraphviz = new QPushButton(centralwidget);
        btnGraphviz->setObjectName("btnGraphviz");
        btnGraphviz->setGeometry(QRect(660, 60, 150, 35));
        btnErrores = new QPushButton(centralwidget);
        btnErrores->setObjectName("btnErrores");
        btnErrores->setGeometry(QRect(820, 60, 150, 35));
        textArea = new QTextEdit(centralwidget);
        textArea->setObjectName("textArea");
        textArea->setGeometry(QRect(10, 105, 500, 600));
        tablaTokens = new QTableWidget(centralwidget);
        tablaTokens->setObjectName("tablaTokens");
        tablaTokens->setGeometry(QRect(520, 105, 560, 290));
        tablaErrores = new QTableWidget(centralwidget);
        tablaErrores->setObjectName("tablaErrores");
        tablaErrores->setGeometry(QRect(520, 405, 560, 300));
        MainWindow->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MedLexer", nullptr));
        btnCargarArchivo->setText(QCoreApplication::translate("MainWindow", "Cargar Archivo .med", nullptr));
        btnAnalizar->setText(QCoreApplication::translate("MainWindow", "Analizar", nullptr));
        btnReporte1->setText(QCoreApplication::translate("MainWindow", "Reporte 1 - Pacientes", nullptr));
        btnReporte2->setText(QCoreApplication::translate("MainWindow", "Reporte 2 - Medicos", nullptr));
        btnReporte3->setText(QCoreApplication::translate("MainWindow", "Reporte 3 - Citas", nullptr));
        btnReporte4->setText(QCoreApplication::translate("MainWindow", "Reporte 4 - Estadistico", nullptr));
        btnGraphviz->setText(QCoreApplication::translate("MainWindow", "Generar Graphviz", nullptr));
        btnErrores->setText(QCoreApplication::translate("MainWindow", "Reporte Errores", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H

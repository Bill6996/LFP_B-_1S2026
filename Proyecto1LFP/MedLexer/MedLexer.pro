QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    src/lexer/LexicalAnalyzer.cpp \
    src/model/Token.cpp \
    src/reports/ReportGenerator.cpp

HEADERS += \
    mainwindow.h \
    src/errors/ErrorManager.h \
    src/lexer/LexicalAnalyzer.h \
    src/model/Token.h \
    src/reports/ReportGenerator.h

FORMS += \
    mainwindow.ui
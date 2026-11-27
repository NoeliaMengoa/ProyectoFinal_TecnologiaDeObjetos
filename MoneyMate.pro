QT += core gui widgets sql
CONFIG += c++17
TARGET = MoneyMate
TEMPLATE = app
INCLUDEPATH += include

HEADERS += \
    include/ExcepcionFinanciera.h \
    include/Utilidades.h \
    include/Movimiento.h \
    include/MovimientoFactory.h \
    include/Presupuesto.h \
    include/MetaAhorro.h \
    include/Recordatorio.h \
    include/Observer.h \
    include/IRepositorio.h \
    include/RepositorioSQLite.h \
    include/GestorFinanciero.h \
    include/Reporte.h \
    include/Controlador.h \
    include/GraficoBarras.h \
    include/VentanaPrincipal.h

SOURCES += \
    src/main.cpp \
    src/RepositorioSQLite.cpp \
    src/GestorFinanciero.cpp \
    src/Reporte.cpp \
    src/Controlador.cpp \
    src/GraficoBarras.cpp \
    src/VentanaPrincipal.cpp

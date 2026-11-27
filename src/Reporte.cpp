#include "Reporte.h"
#include "Utilidades.h"
#include <QMap>
#include <QPair>

QString Reporte::generarTexto() const {
    QString t;
    t += "=== BALANCE GENERAL ===\n";
    t += "Ingresos totales: " + formatearSoles(gestor.totalIngresos()) + "\n";
    t += "Gastos totales:   " + formatearSoles(gestor.totalGastos()) + "\n";
    t += "Saldo actual:     " + formatearSoles(gestor.saldoActual()) + "\n\n";

    t += "=== GASTOS POR CATEGORÍA ===\n";
    QMap<QString, double> porCategoria = gestor.gastosPorCategoria();
    QString mayor;
    double mayorMonto = 0;
    for (auto it = porCategoria.begin(); it != porCategoria.end(); ++it) {
        t += it.key() + ": " + formatearSoles(it.value()) + "\n";
        if (it.value() > mayorMonto) { mayorMonto = it.value(); mayor = it.key(); }
    }
    if (porCategoria.isEmpty()) t += "(sin gastos registrados)\n";
    else t += "\nCategoría con mayor consumo: " + mayor + " (" + formatearSoles(mayorMonto) + ")\n";

    t += "\n=== COMPARACIÓN MENSUAL (ingresos / gastos) ===\n";
    QMap<QString, QPair<double, double>> meses;   // "2026-11" -> (ingresos, gastos)
    for (const auto& m : gestor.movimientos()) {
        QString clave = m->getFecha().toString("yyyy-MM");
        if (m->tipo() == "Ingreso") meses[clave].first += m->getMonto();
        else meses[clave].second += m->getMonto();
    }
    for (auto it = meses.begin(); it != meses.end(); ++it)
        t += it.key() + ":  " + formatearSoles(it.value().first) + "  /  " +
             formatearSoles(it.value().second) + "\n";
    if (meses.isEmpty()) t += "(sin movimientos)\n";
    return t;
}

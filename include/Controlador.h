#pragma once
#include <QString>
#include <QDate>
#include "GestorFinanciero.h"

// Resultado de una operación: si salió bien y un mensaje (error o advertencia).
struct Resultado {
    bool ok;
    QString mensaje;
};

// CONTROLADOR (MVC): recibe las acciones de la Vista, las pasa al Modelo
// y devuelve un Resultado. Aquí se atrapan las excepciones.
class Controlador {
    GestorFinanciero& gestor;

public:
    explicit Controlador(GestorFinanciero& g) : gestor(g) {}

    const GestorFinanciero& modelo() const { return gestor; }
    void agregarObservador(IObservador* o) { gestor.agregarObservador(o); }

    Resultado registrarMovimiento(const QString& tipo, double monto, const QString& descripcion,
                                  const QString& categoria, const QDate& fecha);
    Resultado editarMovimiento(int id, double monto, const QString& descripcion);
    Resultado eliminarMovimiento(int id);
    Resultado cambiarSaldoInicial(double monto);
    Resultado agregarCategoria(const QString& nombre);
    Resultado crearPresupuesto(const QString& categoria, double limite);
    Resultado eliminarPresupuesto(int id);
    Resultado crearMeta(const QString& nombre, double objetivo);
    Resultado aportarMeta(int id, double monto);
    Resultado eliminarMeta(int id);
    Resultado crearRecordatorio(const QString& descripcion, const QDate& vencimiento);
    Resultado marcarPagado(int id);
    Resultado eliminarRecordatorio(int id);
};

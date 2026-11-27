#pragma once
#include <memory>
#include <vector>
#include <QStringList>
#include "Movimiento.h"
#include "Presupuesto.h"
#include "MetaAhorro.h"
#include "Recordatorio.h"

// INTERFAZ (Inversión de dependencias - SOLID "D"):
// el gestor depende de esta interfaz, no de SQLite directamente.
class IRepositorio {
public:
    virtual ~IRepositorio() = default;

    virtual int guardarMovimiento(const Movimiento& m) = 0;
    virtual void actualizarMovimiento(int id, double monto, const QString& descripcion) = 0;
    virtual void eliminarMovimiento(int id) = 0;
    virtual std::vector<std::unique_ptr<Movimiento>> cargarMovimientos() = 0;

    virtual QStringList cargarCategorias() = 0;
    virtual void guardarCategoria(const QString& nombre) = 0;

    virtual int guardarPresupuesto(const Presupuesto& p) = 0;
    virtual void eliminarPresupuesto(int id) = 0;
    virtual std::vector<Presupuesto> cargarPresupuestos() = 0;

    virtual int guardarMeta(const MetaAhorro& m) = 0;
    virtual void actualizarMeta(const MetaAhorro& m) = 0;
    virtual void eliminarMeta(int id) = 0;
    virtual std::vector<MetaAhorro> cargarMetas() = 0;

    virtual int guardarRecordatorio(const Recordatorio& r) = 0;
    virtual void actualizarRecordatorio(const Recordatorio& r) = 0;
    virtual void eliminarRecordatorio(int id) = 0;
    virtual std::vector<Recordatorio> cargarRecordatorios() = 0;

    virtual double cargarSaldoInicial() = 0;
    virtual void guardarSaldoInicial(double monto) = 0;
};

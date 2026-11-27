#pragma once
#include <memory>
#include <vector>
#include <QMap>
#include "Observer.h"
#include "IRepositorio.h"
#include "MovimientoFactory.h"

// MODELO (MVC): reglas de negocio y datos del sistema.
// Es el "Sujeto" del patrón Observer: avisa a la vista cuando algo cambia.
class GestorFinanciero : public Sujeto {
    std::unique_ptr<IRepositorio> repo;
    std::vector<std::unique_ptr<Movimiento>> movimientos_;  // polimorfismo + punteros inteligentes
    std::vector<Presupuesto> presupuestos_;
    std::vector<MetaAhorro> metas_;
    std::vector<Recordatorio> recordatorios_;
    QStringList categorias_;
    double saldoInicial_ = 0;

    QString revisarPresupuesto(const QString& categoria, double monto, const QDate& fecha) const;

public:
    explicit GestorFinanciero(std::unique_ptr<IRepositorio> repositorio);

    // Movimientos
    QString registrarMovimiento(const QString& tipo, double monto, const QString& descripcion,
                                const QString& categoria, const QDate& fecha);
    void editarMovimiento(int id, double monto, const QString& descripcion);
    void eliminarMovimiento(int id);
    const Movimiento* buscarMovimiento(int id) const;
    const std::vector<std::unique_ptr<Movimiento>>& movimientos() const { return movimientos_; }

    // Saldo y totales
    double saldoInicial() const { return saldoInicial_; }
    void cambiarSaldoInicial(double monto);
    double saldoActual() const;
    double totalIngresos() const;
    double totalGastos() const;
    double totalAhorrado() const;
    double gastoDelMes(const QString& categoria, const QDate& referencia) const;
    QMap<QString, double> gastosPorCategoria() const;

    // Categorías
    const QStringList& categorias() const { return categorias_; }
    void agregarCategoria(const QString& nombre);

    // Presupuestos
    const std::vector<Presupuesto>& presupuestos() const { return presupuestos_; }
    void crearPresupuesto(const QString& categoria, double limite);
    void eliminarPresupuesto(int id);

    // Metas de ahorro
    const std::vector<MetaAhorro>& metas() const { return metas_; }
    void crearMeta(const QString& nombre, double objetivo);
    void aportarMeta(int id, double monto);
    void eliminarMeta(int id);

    // Agenda
    const std::vector<Recordatorio>& recordatorios() const { return recordatorios_; }
    void crearRecordatorio(const QString& descripcion, const QDate& vencimiento);
    void marcarPagado(int id);
    void eliminarRecordatorio(int id);
};

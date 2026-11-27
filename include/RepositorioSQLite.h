#pragma once
#include <QSqlDatabase>
#include "IRepositorio.h"

// Implementación de IRepositorio que guarda todo en una base SQLite.
class RepositorioSQLite : public IRepositorio {
    QSqlDatabase db;
    void crearTablas();

public:
    explicit RepositorioSQLite(const QString& rutaArchivo);

    int guardarMovimiento(const Movimiento& m) override;
    void actualizarMovimiento(int id, double monto, const QString& descripcion) override;
    void eliminarMovimiento(int id) override;
    std::vector<std::unique_ptr<Movimiento>> cargarMovimientos() override;

    QStringList cargarCategorias() override;
    void guardarCategoria(const QString& nombre) override;

    int guardarPresupuesto(const Presupuesto& p) override;
    void eliminarPresupuesto(int id) override;
    std::vector<Presupuesto> cargarPresupuestos() override;

    int guardarMeta(const MetaAhorro& m) override;
    void actualizarMeta(const MetaAhorro& m) override;
    void eliminarMeta(int id) override;
    std::vector<MetaAhorro> cargarMetas() override;

    int guardarRecordatorio(const Recordatorio& r) override;
    void actualizarRecordatorio(const Recordatorio& r) override;
    void eliminarRecordatorio(int id) override;
    std::vector<Recordatorio> cargarRecordatorios() override;

    double cargarSaldoInicial() override;
    void guardarSaldoInicial(double monto) override;
};

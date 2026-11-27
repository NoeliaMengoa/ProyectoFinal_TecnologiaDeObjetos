#pragma once
#include <memory>
#include "Movimiento.h"

// PATRÓN FACTORY METHOD: centraliza la creación de movimientos.
class MovimientoFactory {
public:
    static std::unique_ptr<Movimiento> crear(const QString& tipo, double monto,
                                             const QString& descripcion,
                                             const QString& categoria,
                                             const QDate& fecha, int id = 0) {
        if (tipo == "Ingreso")
            return std::make_unique<Ingreso>(monto, descripcion, categoria, fecha, id);
        if (tipo == "Gasto")
            return std::make_unique<Gasto>(monto, descripcion, categoria, fecha, id);
        throw ExcepcionFinanciera("Tipo de movimiento desconocido: " + tipo);
    }
};

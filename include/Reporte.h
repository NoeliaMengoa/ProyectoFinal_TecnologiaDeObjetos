#pragma once
#include <QString>
#include "GestorFinanciero.h"

// Genera estadísticas y resúmenes (solo lee datos, no los modifica).
class Reporte {
    const GestorFinanciero& gestor;

public:
    explicit Reporte(const GestorFinanciero& g) : gestor(g) {}
    QString generarTexto() const;
};

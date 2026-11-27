#pragma once
#include <QString>
#include "ExcepcionFinanciera.h"

// Límite mensual de gasto para una categoría.
class Presupuesto {
    int id;
    QString categoria;
    double limite;

public:
    Presupuesto(const QString& categoria, double limite, int id = 0)
        : id(id), categoria(categoria.trimmed()), limite(limite) {
        if (this->categoria.isEmpty()) throw ExcepcionFinanciera("El presupuesto necesita una categoría.");
        if (limite <= 0) throw ExcepcionFinanciera("El límite del presupuesto debe ser mayor que cero.");
    }
    int getId() const { return id; }
    void setId(int nuevoId) { id = nuevoId; }
    QString getCategoria() const { return categoria; }
    double getLimite() const { return limite; }
};

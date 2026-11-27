#pragma once
#include <QString>
#include <QDate>
#include "ExcepcionFinanciera.h"

// Pago pendiente con fecha de vencimiento.
class Recordatorio {
    int id;
    QString descripcion;
    QDate vencimiento;
    bool pagado;

public:
    Recordatorio(const QString& descripcion, const QDate& vencimiento, bool pagado = false, int id = 0)
        : id(id), descripcion(descripcion.trimmed()), vencimiento(vencimiento), pagado(pagado) {
        if (this->descripcion.isEmpty()) throw ExcepcionFinanciera("El recordatorio necesita una descripción.");
        if (!vencimiento.isValid()) throw ExcepcionFinanciera("El recordatorio necesita una fecha de vencimiento válida.");
    }
    int getId() const { return id; }
    void setId(int nuevoId) { id = nuevoId; }
    QString getDescripcion() const { return descripcion; }
    QDate getVencimiento() const { return vencimiento; }
    bool estaPagado() const { return pagado; }

    void marcarPagado() {
        if (pagado) throw ExcepcionFinanciera("Este pago ya fue marcado como pagado.");
        pagado = true;
    }
};

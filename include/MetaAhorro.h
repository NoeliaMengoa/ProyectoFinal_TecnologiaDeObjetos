#pragma once
#include <QString>
#include "ExcepcionFinanciera.h"

// Objetivo de ahorro (celular, laptop, viaje...).
class MetaAhorro {
    int id;
    QString nombre;
    double objetivo;
    double ahorrado;

public:
    MetaAhorro(const QString& nombre, double objetivo, double ahorrado = 0, int id = 0)
        : id(id), nombre(nombre.trimmed()), objetivo(objetivo), ahorrado(ahorrado) {
        if (this->nombre.isEmpty()) throw ExcepcionFinanciera("La meta necesita un nombre.");
        if (objetivo <= 0) throw ExcepcionFinanciera("El objetivo de la meta debe ser mayor que cero.");
    }
    int getId() const { return id; }
    void setId(int nuevoId) { id = nuevoId; }
    QString getNombre() const { return nombre; }
    double getObjetivo() const { return objetivo; }
    double getAhorrado() const { return ahorrado; }
    double falta() const { return objetivo > ahorrado ? objetivo - ahorrado : 0; }
    double porcentaje() const { return ahorrado >= objetivo ? 100 : ahorrado * 100 / objetivo; }

    void aportar(double monto) {
        if (monto <= 0) throw ExcepcionFinanciera("El aporte debe ser mayor que cero.");
        ahorrado += monto;
    }
};

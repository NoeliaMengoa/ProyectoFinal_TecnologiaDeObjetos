#pragma once
#include <ostream>
#include <QString>
#include <QDate>
#include "ExcepcionFinanciera.h"

// CLASE ABSTRACTA: representa cualquier movimiento de dinero.
class Movimiento {
protected:
    int id;
    double monto;
    QString descripcion;
    QString categoria;
    QDate fecha;

public:
    Movimiento(double monto, const QString& descripcion, const QString& categoria,
               const QDate& fecha, int id = 0)
        : id(id), monto(0) {
        // Los setters validan: si algo es inválido lanzan una excepción.
        setMonto(monto);
        setDescripcion(descripcion);
        setCategoria(categoria);
        setFecha(fecha);
    }

    // DESTRUCTOR VIRTUAL: permite destruir bien objetos derivados.
    virtual ~Movimiento() = default;

    // FUNCIONES VIRTUALES PURAS (abstractas): cada hijo las implementa.
    virtual double calcularImpactoSaldo() const = 0;
    virtual QString tipo() const = 0;

    // Getters
    int getId() const { return id; }
    double getMonto() const { return monto; }
    QString getDescripcion() const { return descripcion; }
    QString getCategoria() const { return categoria; }
    QDate getFecha() const { return fecha; }

    // Setters con RESTRICCIONES
    void setId(int nuevoId) { id = nuevoId; }
    void setMonto(double m) {
        if (m <= 0) throw ExcepcionFinanciera("El monto debe ser mayor que cero.");
        monto = m;
    }
    void setDescripcion(const QString& d) {
        if (d.trimmed().isEmpty()) throw ExcepcionFinanciera("La descripción no puede estar vacía.");
        descripcion = d.trimmed();
    }
    void setCategoria(const QString& c) {
        if (c.trimmed().isEmpty()) throw ExcepcionFinanciera("La categoría es obligatoria.");
        categoria = c.trimmed();
    }
    void setFecha(const QDate& f) {
        if (!f.isValid()) throw ExcepcionFinanciera("La fecha no es válida.");
        fecha = f;
    }

    // SOBRECARGA DE OPERADOR: comparar movimientos por fecha.
    bool operator<(const Movimiento& otro) const { return fecha < otro.fecha; }

    // FUNCIÓN FRIEND: accede a datos protegidos para imprimir el movimiento.
    friend std::ostream& operator<<(std::ostream& salida, const Movimiento& m);
};

inline std::ostream& operator<<(std::ostream& salida, const Movimiento& m) {
    salida << m.tipo().toStdString() << " | S/ " << m.monto << " | "
           << m.descripcion.toStdString() << " | " << m.categoria.toStdString();
    return salida;
}

// HERENCIA: Ingreso es un Movimiento que suma al saldo.
class Ingreso : public Movimiento {
public:
    using Movimiento::Movimiento;
    double calcularImpactoSaldo() const override { return monto; }
    QString tipo() const override { return "Ingreso"; }
};

// HERENCIA: Gasto es un Movimiento que resta del saldo.
class Gasto : public Movimiento {
public:
    using Movimiento::Movimiento;
    double calcularImpactoSaldo() const override { return -monto; }
    QString tipo() const override { return "Gasto"; }
};

#pragma once
#include <stdexcept>
#include <QString>

// Excepción propia: se lanza cuando alguna regla del sistema no se cumple.
class ExcepcionFinanciera : public std::runtime_error {
public:
    explicit ExcepcionFinanciera(const QString& mensaje)
        : std::runtime_error(mensaje.toStdString()) {}
};

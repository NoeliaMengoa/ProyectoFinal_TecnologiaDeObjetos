#pragma once
#include <QString>

// FUNCIÓN INLINE: formatea un número como soles (ej. "S/ 25.50").
inline QString formatearSoles(double valor) {
    return QString("S/ %1").arg(valor, 0, 'f', 2);
}

// TEMPLATE: suma cualquier valor numérico de una colección,
// sin importar el tipo de contenedor ni de elemento.
// "obtener" es una función (normalmente una lambda) que dice qué sumar.
template <typename Contenedor, typename Funcion>
double sumarPor(const Contenedor& coleccion, Funcion obtener) {
    double total = 0;
    for (const auto& elemento : coleccion)
        total += obtener(elemento);
    return total;
}

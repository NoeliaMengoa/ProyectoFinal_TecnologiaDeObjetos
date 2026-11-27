#include "Controlador.h"

// Ejecuta una acción (lambda) y convierte las excepciones en un Resultado.
template <typename Accion>
static Resultado ejecutar(Accion accion) {
    try {
        return {true, accion()};
    } catch (const ExcepcionFinanciera& e) {
        return {false, QString::fromStdString(e.what())};
    }
}

Resultado Controlador::registrarMovimiento(const QString& tipo, double monto, const QString& descripcion,
                                           const QString& categoria, const QDate& fecha) {
    return ejecutar([&]() { return gestor.registrarMovimiento(tipo, monto, descripcion, categoria, fecha); });
}
Resultado Controlador::editarMovimiento(int id, double monto, const QString& descripcion) {
    return ejecutar([&]() { gestor.editarMovimiento(id, monto, descripcion); return QString(); });
}
Resultado Controlador::eliminarMovimiento(int id) {
    return ejecutar([&]() { gestor.eliminarMovimiento(id); return QString(); });
}
Resultado Controlador::cambiarSaldoInicial(double monto) {
    return ejecutar([&]() { gestor.cambiarSaldoInicial(monto); return QString(); });
}
Resultado Controlador::agregarCategoria(const QString& nombre) {
    return ejecutar([&]() { gestor.agregarCategoria(nombre); return QString(); });
}
Resultado Controlador::crearPresupuesto(const QString& categoria, double limite) {
    return ejecutar([&]() { gestor.crearPresupuesto(categoria, limite); return QString(); });
}
Resultado Controlador::eliminarPresupuesto(int id) {
    return ejecutar([&]() { gestor.eliminarPresupuesto(id); return QString(); });
}
Resultado Controlador::crearMeta(const QString& nombre, double objetivo) {
    return ejecutar([&]() { gestor.crearMeta(nombre, objetivo); return QString(); });
}
Resultado Controlador::aportarMeta(int id, double monto) {
    return ejecutar([&]() { gestor.aportarMeta(id, monto); return QString(); });
}
Resultado Controlador::eliminarMeta(int id) {
    return ejecutar([&]() { gestor.eliminarMeta(id); return QString(); });
}
Resultado Controlador::crearRecordatorio(const QString& descripcion, const QDate& vencimiento) {
    return ejecutar([&]() { gestor.crearRecordatorio(descripcion, vencimiento); return QString(); });
}
Resultado Controlador::marcarPagado(int id) {
    return ejecutar([&]() { gestor.marcarPagado(id); return QString(); });
}
Resultado Controlador::eliminarRecordatorio(int id) {
    return ejecutar([&]() { gestor.eliminarRecordatorio(id); return QString(); });
}

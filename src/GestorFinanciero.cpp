#include "GestorFinanciero.h"
#include "Utilidades.h"
#include <algorithm>

GestorFinanciero::GestorFinanciero(std::unique_ptr<IRepositorio> repositorio)
    : repo(std::move(repositorio)) {
    movimientos_ = repo->cargarMovimientos();
    presupuestos_ = repo->cargarPresupuestos();
    metas_ = repo->cargarMetas();
    recordatorios_ = repo->cargarRecordatorios();
    categorias_ = repo->cargarCategorias();
    saldoInicial_ = repo->cargarSaldoInicial();
}

// ---------- Movimientos ----------
QString GestorFinanciero::registrarMovimiento(const QString& tipo, double monto,
                                              const QString& descripcion,
                                              const QString& categoria, const QDate& fecha) {
    // El Factory crea el objeto correcto y las validaciones lanzan excepciones.
    auto mov = MovimientoFactory::crear(tipo, monto, descripcion, categoria, fecha);
    if (!categorias_.contains(mov->getCategoria()))
        throw ExcepcionFinanciera("La categoría no existe.");

    QString aviso;
    if (tipo == "Gasto") {
        if (monto > saldoActual())
            aviso += "Advertencia: este gasto supera tu saldo disponible (el saldo quedará negativo).\n";
        aviso += revisarPresupuesto(mov->getCategoria(), monto, fecha);
    }

    mov->setId(repo->guardarMovimiento(*mov));
    movimientos_.push_back(std::move(mov));
    notificar();
    return aviso.trimmed();
}

QString GestorFinanciero::revisarPresupuesto(const QString& categoria, double monto,
                                             const QDate& fecha) const {
    for (const Presupuesto& p : presupuestos_) {
        if (p.getCategoria() != categoria) continue;
        double total = gastoDelMes(categoria, fecha) + monto;
        if (total > p.getLimite())
            return "Superaste el presupuesto de " + categoria + " (" + formatearSoles(total) +
                   " de " + formatearSoles(p.getLimite()) + ").\n";
        if (total >= 0.8 * p.getLimite())
            return "Alcanzaste el 80% del presupuesto de " + categoria + ".\n";
    }
    return "";
}

const Movimiento* GestorFinanciero::buscarMovimiento(int id) const {
    auto it = std::find_if(movimientos_.begin(), movimientos_.end(),
                           [id](const std::unique_ptr<Movimiento>& m) { return m->getId() == id; });
    return it == movimientos_.end() ? nullptr : it->get();
}

void GestorFinanciero::editarMovimiento(int id, double monto, const QString& descripcion) {
    auto it = std::find_if(movimientos_.begin(), movimientos_.end(),
                           [id](const std::unique_ptr<Movimiento>& m) { return m->getId() == id; });
    if (it == movimientos_.end()) throw ExcepcionFinanciera("No se encontró el movimiento.");

    Movimiento& m = **it;
    double montoViejo = m.getMonto();
    QString descViejo = m.getDescripcion();
    try {
        m.setMonto(monto);
        m.setDescripcion(descripcion);
    } catch (...) {          // si algo falla, dejamos todo como estaba
        m.setMonto(montoViejo);
        m.setDescripcion(descViejo);
        throw;
    }
    repo->actualizarMovimiento(id, m.getMonto(), m.getDescripcion());
    notificar();
}

void GestorFinanciero::eliminarMovimiento(int id) {
    auto it = std::remove_if(movimientos_.begin(), movimientos_.end(),
                             [id](const std::unique_ptr<Movimiento>& m) { return m->getId() == id; });
    if (it == movimientos_.end()) throw ExcepcionFinanciera("No se encontró el movimiento.");
    movimientos_.erase(it, movimientos_.end());
    repo->eliminarMovimiento(id);
    notificar();
}

// ---------- Saldo y totales ----------
void GestorFinanciero::cambiarSaldoInicial(double monto) {
    if (monto < 0) throw ExcepcionFinanciera("El saldo inicial no puede ser negativo.");
    saldoInicial_ = monto;
    repo->guardarSaldoInicial(monto);
    notificar();
}

// El saldo se CALCULA (saldo inicial + impacto de cada movimiento), nunca se guarda.
double GestorFinanciero::saldoActual() const {
    return saldoInicial_ + sumarPor(movimientos_, [](const std::unique_ptr<Movimiento>& m) {
               return m->calcularImpactoSaldo();   // polimorfismo
           });
}

double GestorFinanciero::totalIngresos() const {
    return sumarPor(movimientos_, [](const std::unique_ptr<Movimiento>& m) {
        return m->tipo() == "Ingreso" ? m->getMonto() : 0.0;
    });
}

double GestorFinanciero::totalGastos() const {
    return sumarPor(movimientos_, [](const std::unique_ptr<Movimiento>& m) {
        return m->tipo() == "Gasto" ? m->getMonto() : 0.0;
    });
}

double GestorFinanciero::totalAhorrado() const {
    return sumarPor(metas_, [](const MetaAhorro& m) { return m.getAhorrado(); });
}

double GestorFinanciero::gastoDelMes(const QString& categoria, const QDate& ref) const {
    return sumarPor(movimientos_, [&](const std::unique_ptr<Movimiento>& m) {
        bool coincide = m->tipo() == "Gasto" && m->getCategoria() == categoria &&
                        m->getFecha().year() == ref.year() && m->getFecha().month() == ref.month();
        return coincide ? m->getMonto() : 0.0;
    });
}

QMap<QString, double> GestorFinanciero::gastosPorCategoria() const {
    QMap<QString, double> resultado;
    for (const auto& m : movimientos_)
        if (m->tipo() == "Gasto") resultado[m->getCategoria()] += m->getMonto();
    return resultado;
}

// ---------- Categorías ----------
void GestorFinanciero::agregarCategoria(const QString& nombre) {
    QString limpio = nombre.trimmed();
    if (limpio.isEmpty()) throw ExcepcionFinanciera("El nombre de la categoría no puede estar vacío.");
    for (const QString& c : categorias_)
        if (c.compare(limpio, Qt::CaseInsensitive) == 0)
            throw ExcepcionFinanciera("Ya existe una categoría con ese nombre.");
    repo->guardarCategoria(limpio);
    categorias_ << limpio;
    notificar();
}

// ---------- Presupuestos ----------
void GestorFinanciero::crearPresupuesto(const QString& categoria, double limite) {
    Presupuesto p(categoria, limite);   // valida
    for (const Presupuesto& existente : presupuestos_)
        if (existente.getCategoria() == p.getCategoria())
            throw ExcepcionFinanciera("Ya existe un presupuesto para esa categoría.");
    p.setId(repo->guardarPresupuesto(p));
    presupuestos_.push_back(p);
    notificar();
}

void GestorFinanciero::eliminarPresupuesto(int id) {
    auto it = std::remove_if(presupuestos_.begin(), presupuestos_.end(),
                             [id](const Presupuesto& p) { return p.getId() == id; });
    if (it == presupuestos_.end()) throw ExcepcionFinanciera("No se encontró el presupuesto.");
    presupuestos_.erase(it, presupuestos_.end());
    repo->eliminarPresupuesto(id);
    notificar();
}

// ---------- Metas ----------
void GestorFinanciero::crearMeta(const QString& nombre, double objetivo) {
    MetaAhorro m(nombre, objetivo);
    m.setId(repo->guardarMeta(m));
    metas_.push_back(m);
    notificar();
}

void GestorFinanciero::aportarMeta(int id, double monto) {
    auto it = std::find_if(metas_.begin(), metas_.end(),
                           [id](const MetaAhorro& m) { return m.getId() == id; });
    if (it == metas_.end()) throw ExcepcionFinanciera("No se encontró la meta.");
    it->aportar(monto);
    repo->actualizarMeta(*it);
    notificar();
}

void GestorFinanciero::eliminarMeta(int id) {
    auto it = std::remove_if(metas_.begin(), metas_.end(),
                             [id](const MetaAhorro& m) { return m.getId() == id; });
    if (it == metas_.end()) throw ExcepcionFinanciera("No se encontró la meta.");
    metas_.erase(it, metas_.end());
    repo->eliminarMeta(id);
    notificar();
}

// ---------- Agenda ----------
void GestorFinanciero::crearRecordatorio(const QString& descripcion, const QDate& vencimiento) {
    Recordatorio r(descripcion, vencimiento);
    r.setId(repo->guardarRecordatorio(r));
    recordatorios_.push_back(r);
    std::sort(recordatorios_.begin(), recordatorios_.end(),
              [](const Recordatorio& a, const Recordatorio& b) { return a.getVencimiento() < b.getVencimiento(); });
    notificar();
}

void GestorFinanciero::marcarPagado(int id) {
    auto it = std::find_if(recordatorios_.begin(), recordatorios_.end(),
                           [id](const Recordatorio& r) { return r.getId() == id; });
    if (it == recordatorios_.end()) throw ExcepcionFinanciera("No se encontró el recordatorio.");
    it->marcarPagado();
    repo->actualizarRecordatorio(*it);
    notificar();
}

void GestorFinanciero::eliminarRecordatorio(int id) {
    auto it = std::remove_if(recordatorios_.begin(), recordatorios_.end(),
                             [id](const Recordatorio& r) { return r.getId() == id; });
    if (it == recordatorios_.end()) throw ExcepcionFinanciera("No se encontró el recordatorio.");
    recordatorios_.erase(it, recordatorios_.end());
    repo->eliminarRecordatorio(id);
    notificar();
}

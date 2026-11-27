#include "RepositorioSQLite.h"
#include "MovimientoFactory.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

// Ejecuta una consulta y lanza excepción si falla.
static void ejecutar(QSqlQuery& q) {
    if (!q.exec())
        throw ExcepcionFinanciera("Error de base de datos: " + q.lastError().text());
}

RepositorioSQLite::RepositorioSQLite(const QString& rutaArchivo) {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(rutaArchivo);
    if (!db.open())
        throw ExcepcionFinanciera("No se pudo abrir la base de datos.");
    crearTablas();
}

void RepositorioSQLite::crearTablas() {
    const QStringList sentencias = {
        "CREATE TABLE IF NOT EXISTS movimiento(id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "tipo TEXT, monto REAL, descripcion TEXT, categoria TEXT, fecha TEXT)",
        "CREATE TABLE IF NOT EXISTS categoria(nombre TEXT PRIMARY KEY)",
        "CREATE TABLE IF NOT EXISTS presupuesto(id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "categoria TEXT UNIQUE, limite REAL)",
        "CREATE TABLE IF NOT EXISTS meta(id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nombre TEXT, objetivo REAL, ahorrado REAL)",
        "CREATE TABLE IF NOT EXISTS recordatorio(id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "descripcion TEXT, vencimiento TEXT, pagado INTEGER)",
        "CREATE TABLE IF NOT EXISTS config(clave TEXT PRIMARY KEY, valor TEXT)"
    };
    for (const QString& sql : sentencias) {
        QSqlQuery q(db);
        q.prepare(sql);
        ejecutar(q);
    }

    // Categorías por defecto la primera vez.
    if (cargarCategorias().isEmpty()) {
        const QStringList base = {"Alimentación", "Transporte", "Educación", "Ropa", "Salud",
                                  "Hogar", "Entretenimiento", "Mascotas", "Tecnología",
                                  "Belleza", "Trabajo", "Sueldo", "Otros"};
        for (const QString& c : base) guardarCategoria(c);
    }
}

// ---------- Movimientos ----------
int RepositorioSQLite::guardarMovimiento(const Movimiento& m) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO movimiento(tipo,monto,descripcion,categoria,fecha) VALUES(?,?,?,?,?)");
    q.addBindValue(m.tipo());
    q.addBindValue(m.getMonto());
    q.addBindValue(m.getDescripcion());
    q.addBindValue(m.getCategoria());
    q.addBindValue(m.getFecha().toString(Qt::ISODate));
    ejecutar(q);
    return q.lastInsertId().toInt();
}

void RepositorioSQLite::actualizarMovimiento(int id, double monto, const QString& descripcion) {
    QSqlQuery q(db);
    q.prepare("UPDATE movimiento SET monto=?, descripcion=? WHERE id=?");
    q.addBindValue(monto);
    q.addBindValue(descripcion);
    q.addBindValue(id);
    ejecutar(q);
}

void RepositorioSQLite::eliminarMovimiento(int id) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM movimiento WHERE id=?");
    q.addBindValue(id);
    ejecutar(q);
}

std::vector<std::unique_ptr<Movimiento>> RepositorioSQLite::cargarMovimientos() {
    std::vector<std::unique_ptr<Movimiento>> lista;
    QSqlQuery q(db);
    q.prepare("SELECT id,tipo,monto,descripcion,categoria,fecha FROM movimiento ORDER BY fecha");
    ejecutar(q);
    while (q.next()) {
        lista.push_back(MovimientoFactory::crear(
            q.value(1).toString(), q.value(2).toDouble(), q.value(3).toString(),
            q.value(4).toString(), QDate::fromString(q.value(5).toString(), Qt::ISODate),
            q.value(0).toInt()));
    }
    return lista;
}

// ---------- Categorías ----------
QStringList RepositorioSQLite::cargarCategorias() {
    QStringList lista;
    QSqlQuery q(db);
    q.prepare("SELECT nombre FROM categoria ORDER BY rowid");
    ejecutar(q);
    while (q.next()) lista << q.value(0).toString();
    return lista;
}

void RepositorioSQLite::guardarCategoria(const QString& nombre) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO categoria(nombre) VALUES(?)");
    q.addBindValue(nombre);
    ejecutar(q);
}

// ---------- Presupuestos ----------
int RepositorioSQLite::guardarPresupuesto(const Presupuesto& p) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO presupuesto(categoria,limite) VALUES(?,?)");
    q.addBindValue(p.getCategoria());
    q.addBindValue(p.getLimite());
    ejecutar(q);
    return q.lastInsertId().toInt();
}

void RepositorioSQLite::eliminarPresupuesto(int id) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM presupuesto WHERE id=?");
    q.addBindValue(id);
    ejecutar(q);
}

std::vector<Presupuesto> RepositorioSQLite::cargarPresupuestos() {
    std::vector<Presupuesto> lista;
    QSqlQuery q(db);
    q.prepare("SELECT id,categoria,limite FROM presupuesto");
    ejecutar(q);
    while (q.next())
        lista.emplace_back(q.value(1).toString(), q.value(2).toDouble(), q.value(0).toInt());
    return lista;
}

// ---------- Metas ----------
int RepositorioSQLite::guardarMeta(const MetaAhorro& m) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO meta(nombre,objetivo,ahorrado) VALUES(?,?,?)");
    q.addBindValue(m.getNombre());
    q.addBindValue(m.getObjetivo());
    q.addBindValue(m.getAhorrado());
    ejecutar(q);
    return q.lastInsertId().toInt();
}

void RepositorioSQLite::actualizarMeta(const MetaAhorro& m) {
    QSqlQuery q(db);
    q.prepare("UPDATE meta SET ahorrado=? WHERE id=?");
    q.addBindValue(m.getAhorrado());
    q.addBindValue(m.getId());
    ejecutar(q);
}

void RepositorioSQLite::eliminarMeta(int id) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM meta WHERE id=?");
    q.addBindValue(id);
    ejecutar(q);
}

std::vector<MetaAhorro> RepositorioSQLite::cargarMetas() {
    std::vector<MetaAhorro> lista;
    QSqlQuery q(db);
    q.prepare("SELECT id,nombre,objetivo,ahorrado FROM meta");
    ejecutar(q);
    while (q.next())
        lista.emplace_back(q.value(1).toString(), q.value(2).toDouble(),
                           q.value(3).toDouble(), q.value(0).toInt());
    return lista;
}

// ---------- Recordatorios ----------
int RepositorioSQLite::guardarRecordatorio(const Recordatorio& r) {
    QSqlQuery q(db);
    q.prepare("INSERT INTO recordatorio(descripcion,vencimiento,pagado) VALUES(?,?,?)");
    q.addBindValue(r.getDescripcion());
    q.addBindValue(r.getVencimiento().toString(Qt::ISODate));
    q.addBindValue(r.estaPagado() ? 1 : 0);
    ejecutar(q);
    return q.lastInsertId().toInt();
}

void RepositorioSQLite::actualizarRecordatorio(const Recordatorio& r) {
    QSqlQuery q(db);
    q.prepare("UPDATE recordatorio SET pagado=? WHERE id=?");
    q.addBindValue(r.estaPagado() ? 1 : 0);
    q.addBindValue(r.getId());
    ejecutar(q);
}

void RepositorioSQLite::eliminarRecordatorio(int id) {
    QSqlQuery q(db);
    q.prepare("DELETE FROM recordatorio WHERE id=?");
    q.addBindValue(id);
    ejecutar(q);
}

std::vector<Recordatorio> RepositorioSQLite::cargarRecordatorios() {
    std::vector<Recordatorio> lista;
    QSqlQuery q(db);
    q.prepare("SELECT id,descripcion,vencimiento,pagado FROM recordatorio ORDER BY vencimiento");
    ejecutar(q);
    while (q.next())
        lista.emplace_back(q.value(1).toString(),
                           QDate::fromString(q.value(2).toString(), Qt::ISODate),
                           q.value(3).toInt() == 1, q.value(0).toInt());
    return lista;
}

// ---------- Saldo inicial ----------
double RepositorioSQLite::cargarSaldoInicial() {
    QSqlQuery q(db);
    q.prepare("SELECT valor FROM config WHERE clave='saldo_inicial'");
    ejecutar(q);
    return q.next() ? q.value(0).toDouble() : 0.0;
}

void RepositorioSQLite::guardarSaldoInicial(double monto) {
    QSqlQuery q(db);
    q.prepare("INSERT OR REPLACE INTO config(clave,valor) VALUES('saldo_inicial',?)");
    q.addBindValue(QString::number(monto, 'f', 2));
    ejecutar(q);
}

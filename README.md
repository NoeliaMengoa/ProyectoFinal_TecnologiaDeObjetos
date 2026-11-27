# Money Mate – Tu dinero, bajo control

Aplicación de escritorio de finanzas personales hecha en **C++17 con Qt (Widgets + SQLite)**.
Proyecto final de Tecnología de Objetos.

## Cómo abrirlo
1. Abre `MoneyMate.pro` con Qt Creator.
2. Elige un kit (Qt 5.15 o Qt 6) y pulsa **Run**.
3. La base de datos `moneymate.db` se crea sola en la carpeta de datos del usuario.

## Módulos
Dashboard · Movimientos (ingresos/gastos, categorías propias, editar, eliminar, búsqueda) ·
Presupuestos (alertas al 80 % y al superar el límite) · Metas de ahorro · Agenda de pagos · Reportes.

## Estructura
- `include/` y `src/`: código fuente.
- Modelo: `Movimiento`, `Ingreso`, `Gasto`, `Presupuesto`, `MetaAhorro`, `Recordatorio`, `GestorFinanciero`, `Reporte`.
- Vista: `VentanaPrincipal`, `GraficoBarras`.
- Controlador: `Controlador`.
- Persistencia: `IRepositorio` (interfaz) + `RepositorioSQLite`.

## Dónde está cada concepto
| Concepto | Dónde |
|---|---|
| MVC | `GestorFinanciero` (M), `VentanaPrincipal` (V), `Controlador` (C) |
| Clase abstracta / funciones virtuales puras | `Movimiento` (`calcularImpactoSaldo`, `tipo`) |
| Herencia y polimorfismo | `Ingreso`, `Gasto`; `saldoActual()` en `GestorFinanciero.cpp` |
| Destructor virtual | `Movimiento`, `IRepositorio`, `IObservador` |
| Encapsulamiento | atributos protegidos/privados con getters y setters |
| Punteros inteligentes / memoria dinámica | `std::vector<std::unique_ptr<Movimiento>>` |
| Función `friend` | `operator<<` en `Movimiento.h` |
| Sobrecarga de operadores | `operator<` y `operator<<` en `Movimiento.h` |
| Funciones lambda | `connect(...)` en `VentanaPrincipal.cpp`, `sort`, `find_if`, `sumarPor` |
| Template | `sumarPor` en `Utilidades.h` y `ejecutar` en `Controlador.cpp` |
| Función inline | `formatearSoles` en `Utilidades.h` |
| Excepciones | `ExcepcionFinanciera` (lanzada en setters/gestor, atrapada en `Controlador`) |
| Restricciones | monto > 0, descripción y categoría obligatorias, fecha válida, categorías únicas, un presupuesto por categoría, aportes > 0, no pagar dos veces, confirmar al eliminar |
| SOLID | S: una clase, una responsabilidad · O: nuevos tipos de movimiento sin tocar la base · L: `Ingreso`/`Gasto` sustituyen a `Movimiento` · I: interfaces pequeñas (`IObservador`, `IRepositorio`) · D: el gestor depende de `IRepositorio` |
| Factory Method | `MovimientoFactory` |
| Observer | `Observer.h`: `GestorFinanciero` (Sujeto) y `VentanaPrincipal` (Observador) |
| SQLite | `RepositorioSQLite.cpp` |

El saldo no se guarda: se calcula como saldo inicial + impacto de cada movimiento.
Un gasto mayor al saldo se permite con una advertencia (saldo negativo = gasto financiado).

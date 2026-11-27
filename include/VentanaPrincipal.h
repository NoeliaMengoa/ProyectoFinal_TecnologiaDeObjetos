#pragma once
#include <QMainWindow>
#include "Observer.h"
#include "Controlador.h"

class QTableWidget;
class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QDateEdit;
class QLabel;
class QTextEdit;
class GraficoBarras;
class QVBoxLayout;
class GraficoMensualWidget;

// VISTA (MVC): ventana con pestañas. También es OBSERVADOR del modelo:
// cuando el modelo cambia, se llama a actualizar() y la vista se redibuja.
class VentanaPrincipal : public QMainWindow, public IObservador {
    Controlador& ctrl;
    GraficoMensualWidget* graficoMensualReporte = nullptr;

    // Dashboard
    QLabel *lblSaldo = nullptr, *lblIngresos = nullptr;
    QLabel *lblGastos = nullptr, *lblAhorros = nullptr;

    QLabel* lblActividadDashboard = nullptr;
    QLabel* lblMetasDashboard = nullptr;

    QDoubleSpinBox* spinSaldoInicial = nullptr;
    GraficoBarras* grafico = nullptr;
    // Movimientos
    QComboBox *cmbTipo = nullptr, *cmbCategoria = nullptr;
    QDoubleSpinBox* spinMonto = nullptr;
    QLineEdit *txtDescripcion = nullptr, *txtFiltro = nullptr;
    QDateEdit* dateFecha = nullptr;
    QTableWidget* tablaMov = nullptr;
    // Presupuestos
    QComboBox* cmbCatPresupuesto = nullptr;
    QDoubleSpinBox* spinLimite = nullptr;
    QTableWidget* tablaPresupuestos = nullptr;
    // Metas
    QLineEdit* txtMeta = nullptr;
    QDoubleSpinBox *spinObjetivo = nullptr, *spinAporte = nullptr;
    QTableWidget* tablaMetas = nullptr;
    // Agenda
    QLineEdit* txtRecordatorio = nullptr;
    QDateEdit* dateVencimiento = nullptr;
    QTableWidget* tablaAgenda = nullptr;
    // Reportes
    QLabel* lblReporteIngresos = nullptr;
    QLabel* lblReporteGastos = nullptr;
    QLabel* lblReporteSaldo = nullptr;

    QVBoxLayout* layoutCategoriasReporte = nullptr;
    QVBoxLayout* layoutMensualReporte = nullptr;

    void refrescarReportes();

    QWidget* crearDashboard();
    QWidget* crearMovimientos();
    QWidget* crearPresupuestos();
    QWidget* crearMetas();
    QWidget* crearAgenda();
    QWidget* crearReportes();
    QWidget* crearTarjeta(const QString& titulo, QLabel*& valor);
    QTableWidget* crearTabla(const QStringList& columnas);
    QDoubleSpinBox* crearSpinSoles();

    void refrescarDashboard();
    void refrescarCategorias();
    void refrescarMovimientos();
    void refrescarPresupuestos();
    void refrescarMetas();
    void refrescarAgenda();

    int idSeleccionado(QTableWidget* tabla) const;
    bool confirmar(const QString& pregunta);
    void mostrarResultado(const Resultado& r);

public:
    explicit VentanaPrincipal(Controlador& c, QWidget* parent = nullptr);
    void actualizar() override;   // Observer
};

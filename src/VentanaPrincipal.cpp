#include "VentanaPrincipal.h"
#include "GraficoBarras.h"
#include "Reporte.h"
#include "Utilidades.h"

#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QDateEdit>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QInputDialog>
#include <QProgressBar>
#include <algorithm>

#include <QFrame>
#include <QProgressBar>
#include <QMap>
#include <QPair>
#include <QScrollArea>

#include <QPainter>
#include <QFontMetrics>
#include <QMap>
#include <QPair>
#include <QWidget>
#include <algorithm>

static QTableWidgetItem* celda(const QString& texto) {
    return new QTableWidgetItem(texto);
}

class GraficoMensualWidget : public QWidget {
private:
    QMap<QString, QPair<double, double>> datos;

public:
    explicit GraficoMensualWidget(QWidget* parent = nullptr)
        : QWidget(parent) {

        setMinimumHeight(300);
        setMinimumWidth(250);
    }

    void setDatos(
        const QMap<QString, QPair<double, double>>& nuevosDatos) {

        datos = nuevosDatos;
        update();
    }

protected:
    void paintEvent(QPaintEvent*) override {

        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing);

        p.fillRect(rect(), Qt::white);

        // Leyenda
        p.setPen(QColor("#29965a"));
        p.setBrush(QColor("#29965a"));
        p.drawRoundedRect(18, 15, 12, 12, 3, 3);

        p.setPen(QColor("#333333"));
        p.drawText(37, 26, "Ingresos");

        p.setPen(QColor("#a90078"));
        p.setBrush(QColor("#a90078"));
        p.drawRoundedRect(115, 15, 12, 12, 3, 3);

        p.setPen(QColor("#333333"));
        p.drawText(134, 26, "Gastos");

        if (datos.isEmpty()) {
            p.drawText(rect(), Qt::AlignCenter,
                       "No hay movimientos registrados");
            return;
        }

        const int izquierda = 48;
        const int derecha = 15;
        const int arriba = 65;
        const int abajo = 55;

        int ancho = width() - izquierda - derecha;
        int alto = height() - arriba - abajo;

        if (ancho <= 0 || alto <= 0)
            return;

        double maximo = 0;

        for (auto it = datos.begin(); it != datos.end(); ++it) {
            maximo = qMax(maximo, it.value().first);
            maximo = qMax(maximo, it.value().second);
        }

        if (maximo <= 0)
            maximo = 1;

        // Líneas horizontales de referencia
        p.setPen(QPen(QColor("#eee6f1"), 1));

        for (int i = 0; i <= 4; ++i) {
            int y = arriba + alto * i / 4;
            p.drawLine(izquierda, y, width() - derecha, y);
        }

        // Eje inferior
        p.setPen(QPen(QColor("#999999"), 1));
        p.drawLine(izquierda, arriba + alto,
                   width() - derecha, arriba + alto);

        int cantidad = datos.size();
        double espacio = double(ancho) / cantidad;

        int anchoBarra = qBound(
            8, int(espacio * 0.24), 32);

        int indice = 0;

        for (auto it = datos.begin(); it != datos.end(); ++it) {

            double ingresos = it.value().first;
            double gastos = it.value().second;

            int centro = izquierda +
                         int(espacio * (indice + 0.5));

            int altoIngreso = int(
                ingresos / maximo * alto);

            int altoGasto = int(
                gastos / maximo * alto);

            int xIngreso = centro - anchoBarra - 3;
            int xGasto = centro + 3;

            int yIngreso = arriba + alto - altoIngreso;
            int yGasto = arriba + alto - altoGasto;

            // Barra de ingresos
            p.setPen(Qt::NoPen);
            p.setBrush(QColor("#29965a"));

            p.drawRoundedRect(
                xIngreso, yIngreso,
                anchoBarra, qMax(0, altoIngreso),
                5, 5);

            // Barra de gastos
            p.setBrush(QColor("#a90078"));

            p.drawRoundedRect(
                xGasto, yGasto,
                anchoBarra, qMax(0, altoGasto),
                5, 5);

            // Etiquetas de valores
            p.setFont(QFont("Arial", 8, QFont::Bold));

            p.setPen(QColor("#29965a"));

            if (ingresos > 0) {
                p.drawText(
                    xIngreso - 20,
                    qMax(52, yIngreso - 5),
                    anchoBarra + 40,
                    15,
                    Qt::AlignCenter,
                    QString::number(ingresos, 'f', 0));
            }

            p.setPen(QColor("#a90078"));

            if (gastos > 0) {
                p.drawText(
                    xGasto - 20,
                    qMax(52, yGasto - 5),
                    anchoBarra + 40,
                    15,
                    Qt::AlignCenter,
                    QString::number(gastos, 'f', 0));
            }

            // Mes
            p.setPen(QColor("#444444"));
            p.setFont(QFont("Arial", 8));

            p.drawText(
                int(izquierda + espacio * indice),
                arriba + alto + 12,
                int(espacio),
                30,
                Qt::AlignHCenter | Qt::AlignTop,
                it.key());

            indice++;
        }
    }
};


VentanaPrincipal::VentanaPrincipal(Controlador& c, QWidget* parent)
    : QMainWindow(parent), ctrl(c) {
    setWindowTitle("Money Mate - Tu dinero, bajo control");
    resize(1280, 800);
    setMinimumSize(1000, 650);
    setStyleSheet(R"(
    QMainWindow {
        background-color: #f5f2f8;
    }

    QWidget#pagina {
        background-color: #f5f2f8;
    }

    QTabWidget::pane {
        border: none;
        background-color: #f5f2f8;
    }

    QTabBar::tab {
        background-color: transparent;
        color: #333333;
        padding: 15px 22px;
        font-size: 14px;
        font-weight: 600;
        border: none;
    }

    QTabBar::tab:selected {
        background-color: #a90078;
        color: white;
        border-radius: 9px;
    }

    QTabBar::tab:hover:!selected {
        background-color: #f0dcec;
        color: #92006b;
    }

    QPushButton {
        background-color: #873bb1;
        color: white;
        border: none;
        border-radius: 12px;
        padding: 11px 20px;
        font-size: 14px;
        font-weight: bold;
    }

    QPushButton:hover {
        background-color: #a90078;
    }

    QGroupBox {
        background-color: white;
        border: 1px solid #ded4e5;
        border-radius: 15px;
        margin-top: 14px;
        padding: 16px;
        font-size: 15px;
        font-weight: bold;
        color: #29202f;
    }

    QGroupBox::title {
        subcontrol-origin: margin;
        left: 14px;
        padding: 0 5px;
    }

    QLineEdit, QDoubleSpinBox, QComboBox, QDateEdit {
        background-color: white;
        border: 1px solid #d6cbdc;
        border-radius: 9px;
        padding: 9px;
        color: #302536;
    }

    QLabel {
        color: #302536;
    }

    QTableWidget {
        background-color: white;
        alternate-background-color: #faf6fb;
        border: 1px solid #e3d9e9;
        border-radius: 10px;
        gridline-color: #eee6f1;
    }

    QHeaderView::section {
        background-color: #f0e3f0;
        color: #6e1767;
        padding: 9px;
        border: none;
        font-weight: bold;
    }

    QProgressBar {
        border: none;
        border-radius: 6px;
        background-color: #eee5f0;
        text-align: center;
    }

    QProgressBar::chunk {
        background-color: #a90078;
        border-radius: 6px;
    }
)");

    auto* pestanas = new QTabWidget(this);
    pestanas->addTab(crearDashboard(), "Dashboard");
    pestanas->addTab(crearMovimientos(), "Movimientos");
    pestanas->addTab(crearPresupuestos(), "Presupuestos");
    pestanas->addTab(crearMetas(), "Metas de ahorro");
    pestanas->addTab(crearAgenda(), "Agenda");
    pestanas->addTab(crearReportes(), "Reportes");
    setCentralWidget(pestanas);

    ctrl.agregarObservador(this);   // nos suscribimos al modelo
    actualizar();
}

// OBSERVER: el modelo cambió, redibujamos todo.
void VentanaPrincipal::actualizar() {
    refrescarCategorias();
    refrescarDashboard();
    refrescarMovimientos();
    refrescarPresupuestos();
    refrescarMetas();
    refrescarAgenda();
    refrescarReportes();
}

// ---------- Utilidades de la vista ----------
QWidget* VentanaPrincipal::crearTarjeta(
    const QString& titulo, QLabel*& valor) {

    auto* caja = new QGroupBox;
    caja->setTitle("");

    caja->setStyleSheet(R"(
        QGroupBox {
            background-color: white;
            border: 2px solid #a90078;
            border-radius: 15px;
            padding: 12px;
        }
    )");

    auto* layout = new QVBoxLayout(caja);

    auto* encabezado = new QLabel(titulo);
    encabezado->setStyleSheet(
        "font-size: 15px;"
        "font-weight: 500;"
        "color: #35303a;"
        "border: none;"
        );

    valor = new QLabel("S/ 0.00");
    valor->setStyleSheet(
        "font-size: 27px;"
        "font-weight: bold;"
        "color: #481052;"
        "border: none;"
        );

    layout->addWidget(encabezado);
    layout->addWidget(valor);
    layout->addStretch();

    caja->setMinimumHeight(115);

    return caja;
}

QTableWidget* VentanaPrincipal::crearTabla(const QStringList& columnas) {
    auto* t = new QTableWidget(0, columnas.size());
    t->setHorizontalHeaderLabels(columnas);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->horizontalHeader()->setStretchLastSection(true);
    t->verticalHeader()->setVisible(false);
    return t;
}

QDoubleSpinBox* VentanaPrincipal::crearSpinSoles() {
    auto* s = new QDoubleSpinBox;
    s->setRange(0, 10000000);
    s->setDecimals(2);
    s->setPrefix("S/ ");
    return s;
}

int VentanaPrincipal::idSeleccionado(QTableWidget* tabla) const {
    int fila = tabla->currentRow();
    if (fila < 0) return -1;
    return tabla->item(fila, 0)->text().toInt();
}

bool VentanaPrincipal::confirmar(const QString& pregunta) {
    return QMessageBox::question(this, "Confirmar", pregunta) == QMessageBox::Yes;
}

void VentanaPrincipal::mostrarResultado(const Resultado& r) {
    if (!r.ok) QMessageBox::critical(this, "Error", r.mensaje);
    else if (!r.mensaje.isEmpty()) QMessageBox::warning(this, "Aviso", r.mensaje);
}

// ---------- Dashboard ----------

QWidget* VentanaPrincipal::crearDashboard() {

    auto* w = new QWidget;
    w->setObjectName("pagina");

    auto* principal = new QVBoxLayout(w);
    principal->setContentsMargins(20, 20, 20, 20);
    principal->setSpacing(18);

    // =====================================
    // 1. TARJETAS FINANCIERAS
    // =====================================

    auto* tarjetas = new QHBoxLayout;
    tarjetas->setSpacing(16);

    tarjetas->addWidget(
        crearTarjeta("Saldo disponible", lblSaldo));

    tarjetas->addWidget(
        crearTarjeta("Total de ingresos", lblIngresos));

    tarjetas->addWidget(
        crearTarjeta("Total de gastos", lblGastos));

    tarjetas->addWidget(
        crearTarjeta("Ahorros acumulados", lblAhorros));

    principal->addLayout(tarjetas);

    // =====================================
    // 2. SALDO INICIAL
    // =====================================

    auto* filaSaldo = new QHBoxLayout;

    auto* textoSaldo = new QLabel("Saldo inicial:");

    textoSaldo->setStyleSheet(
        "font-size: 15px; font-weight: bold;");

    spinSaldoInicial = crearSpinSoles();
    spinSaldoInicial->setMaximumWidth(180);

    auto* btnGuardar = new QPushButton(
        "Guardar saldo inicial");

    filaSaldo->addWidget(textoSaldo);
    filaSaldo->addWidget(spinSaldoInicial);
    filaSaldo->addWidget(btnGuardar);
    filaSaldo->addStretch();

    principal->addLayout(filaSaldo);

    connect(btnGuardar, &QPushButton::clicked, this, [this]() {
        mostrarResultado(
            ctrl.cambiarSaldoInicial(
                spinSaldoInicial->value()));
    });

    // =====================================
    // 3. GRÁFICO Y ACTIVIDAD RECIENTE
    // =====================================

    auto* filaContenido = new QHBoxLayout;
    filaContenido->setSpacing(18);

    // Panel del gráfico

    auto* panelGrafico = new QGroupBox(
        "Gastos por categoría");

    auto* layoutGrafico = new QVBoxLayout(panelGrafico);

    grafico = new GraficoBarras;
    grafico->setMinimumHeight(230);

    layoutGrafico->addWidget(grafico);

    filaContenido->addWidget(panelGrafico, 3);

    // Panel de actividad reciente

    auto* panelActividad = new QGroupBox(
        "Actividad reciente");

    auto* layoutActividad = new QVBoxLayout(
        panelActividad);

    lblActividadDashboard = new QLabel(
        "Todavía no hay movimientos registrados.");

    lblActividadDashboard->setWordWrap(true);
    lblActividadDashboard->setAlignment(
        Qt::AlignTop | Qt::AlignLeft);

    lblActividadDashboard->setStyleSheet(
        "font-size: 13px; color: #514457;");

    layoutActividad->addWidget(
        lblActividadDashboard);

    filaContenido->addWidget(panelActividad, 1);

    principal->addLayout(filaContenido, 2);

    // =====================================
    // 4. PANEL DE METAS
    // =====================================

    auto* panelMetas = new QGroupBox("Metas de ahorro");

    auto* layoutMetas = new QVBoxLayout(panelMetas);

    lblMetasDashboard = new QLabel(
        "Todavía no tienes metas de ahorro.");

    lblMetasDashboard->setWordWrap(true);
    lblMetasDashboard->setStyleSheet(
        "font-size: 13px; color: #514457;");

    layoutMetas->addWidget(lblMetasDashboard);

    principal->addWidget(panelMetas);

    principal->addStretch();

    return w;
}



void VentanaPrincipal::refrescarDashboard() {

    const GestorFinanciero& g = ctrl.modelo();

    // Actualizar tarjetas financieras

    lblSaldo->setText(
        formatearSoles(g.saldoActual()));

    lblIngresos->setText(
        formatearSoles(g.totalIngresos()));

    lblGastos->setText(
        formatearSoles(g.totalGastos()));

    lblAhorros->setText(
        formatearSoles(g.totalAhorrado()));

    lblSaldo->setStyleSheet(
        g.saldoActual() < 0
            ? "font-size: 27px; font-weight: bold; color: #c62828;"
            : "font-size: 27px; font-weight: bold; color: #481052;"
        );

    spinSaldoInicial->setValue(g.saldoInicial());

    // Actualizar gráfico

    grafico->setDatos(g.gastosPorCategoria());

    // =====================================
    // ACTIVIDAD RECIENTE
    // =====================================

    QString actividad;

    const auto& movimientos = g.movimientos();

    int contador = 0;

    for (auto it = movimientos.rbegin();
         it != movimientos.rend() && contador < 5;
         ++it) {

        const Movimiento* m = it->get();

        QString color = m->tipo() == "Ingreso"
                            ? "#23965b" : "#c13b67";

        QString signo = m->tipo() == "Ingreso"
                            ? "+" : "-";

        actividad +=
            "<div style='margin-bottom:12px;'>"
            "<b style='color:" + color + ";'>" +
            signo + " " + formatearSoles(m->getMonto()) +
            "</b><br>" +
            m->getDescripcion().toHtmlEscaped() +
            "<br><span style='color:#999; font-size:11px;'>" +
            m->getFecha().toString("dd/MM/yyyy") +
            "</span></div>";

        contador++;
    }

    if (actividad.isEmpty()) {
        actividad = "Todavía no hay movimientos registrados.";
    }

    lblActividadDashboard->setText(actividad);

    // =====================================
    // METAS DE AHORRO
    // =====================================

    QString metasTexto;

    for (const MetaAhorro& m : g.metas()) {

        metasTexto +=
            "<div style='margin-bottom:12px;'>"
            "<b style='color:#7e278d;'>" +
            m.getNombre().toHtmlEscaped() +
            "</b><br>" +
            formatearSoles(m.getAhorrado()) +
            " / " +
            formatearSoles(m.getObjetivo()) +
            "<br>"
            "<span style='color:#23965b;'>Progreso: " +
            QString::number(m.porcentaje(), 'f', 1) +
            "%</span></div>";
    }

    if (metasTexto.isEmpty()) {
        metasTexto = "Todavía no tienes metas de ahorro.";
    }

    lblMetasDashboard->setText(metasTexto);
}


// ---------- Movimientos ----------
QWidget* VentanaPrincipal::crearMovimientos() {
    auto* w = new QWidget;
    w->setObjectName("pagina");
    auto* v = new QVBoxLayout(w);

    cmbTipo = new QComboBox;
    cmbTipo->addItems({"Ingreso", "Gasto"});
    spinMonto = crearSpinSoles();
    txtDescripcion = new QLineEdit;
    txtDescripcion->setPlaceholderText("Ej. Almuerzo, sueldo, pasaje...");
    cmbCategoria = new QComboBox;
    dateFecha = new QDateEdit(QDate::currentDate());
    dateFecha->setCalendarPopup(true);

    auto* form = new QFormLayout;
    form->addRow("Tipo:", cmbTipo);
    form->addRow("Monto:", spinMonto);
    form->addRow("Descripción:", txtDescripcion);
    form->addRow("Categoría:", cmbCategoria);
    form->addRow("Fecha:", dateFecha);
    v->addLayout(form);

    auto* botones = new QHBoxLayout;
    auto* btnRegistrar = new QPushButton("Registrar movimiento");
    auto* btnCategoria = new QPushButton("Nueva categoría");
    auto* btnEditar = new QPushButton("Editar seleccionado");
    auto* btnEliminar = new QPushButton("Eliminar seleccionado");
    botones->addWidget(btnRegistrar);
    botones->addWidget(btnCategoria);
    botones->addWidget(btnEditar);
    botones->addWidget(btnEliminar);
    v->addLayout(botones);

    txtFiltro = new QLineEdit;
    txtFiltro->setPlaceholderText("Buscar en el historial (descripción o categoría)...");
    v->addWidget(txtFiltro);

    tablaMov = crearTabla({"ID", "Fecha", "Tipo", "Categoría", "Descripción", "Monto"});
    v->addWidget(tablaMov, 1);

    // LAMBDAS conectadas a los botones
    connect(btnRegistrar, &QPushButton::clicked, this, [this]() {
        Resultado r = ctrl.registrarMovimiento(cmbTipo->currentText(), spinMonto->value(),
                                               txtDescripcion->text(), cmbCategoria->currentText(),
                                               dateFecha->date());
        mostrarResultado(r);
        if (r.ok) { spinMonto->setValue(0); txtDescripcion->clear(); }
    });
    connect(btnCategoria, &QPushButton::clicked, this, [this]() {
        bool ok = false;
        QString nombre = QInputDialog::getText(this, "Nueva categoría", "Nombre:", QLineEdit::Normal, "", &ok);
        if (ok) mostrarResultado(ctrl.agregarCategoria(nombre));
    });
    connect(btnEditar, &QPushButton::clicked, this, [this]() {
        int id = idSeleccionado(tablaMov);
        const Movimiento* m = ctrl.modelo().buscarMovimiento(id);
        if (!m) { QMessageBox::information(this, "Editar", "Selecciona un movimiento."); return; }
        bool ok1 = false, ok2 = false;
        double monto = QInputDialog::getDouble(this, "Editar", "Nuevo monto:", m->getMonto(), 0, 10000000, 2, &ok1);
        if (!ok1) return;
        QString desc = QInputDialog::getText(this, "Editar", "Nueva descripción:", QLineEdit::Normal,
                                             m->getDescripcion(), &ok2);
        if (ok2) mostrarResultado(ctrl.editarMovimiento(id, monto, desc));
    });
    connect(btnEliminar, &QPushButton::clicked, this, [this]() {
        int id = idSeleccionado(tablaMov);
        if (id < 0) { QMessageBox::information(this, "Eliminar", "Selecciona un movimiento."); return; }
        if (confirmar("¿Eliminar este movimiento?")) mostrarResultado(ctrl.eliminarMovimiento(id));
    });
    connect(txtFiltro, &QLineEdit::textChanged, this, [this]() { refrescarMovimientos(); });
    return w;
}

void VentanaPrincipal::refrescarCategorias() {
    const QStringList& cats = ctrl.modelo().categorias();
    for (QComboBox* combo : {cmbCategoria, cmbCatPresupuesto}) {
        QString actual = combo->currentText();
        combo->clear();
        combo->addItems(cats);
        if (!actual.isEmpty()) combo->setCurrentText(actual);
    }
}

void VentanaPrincipal::refrescarMovimientos() {
    QString filtro = txtFiltro->text().trimmed().toLower();

    // Filtramos con una lambda...
    std::vector<const Movimiento*> lista;
    for (const auto& m : ctrl.modelo().movimientos()) {
        auto coincide = [&](const Movimiento& x) {
            return filtro.isEmpty() || x.getDescripcion().toLower().contains(filtro) ||
                   x.getCategoria().toLower().contains(filtro);
        };
        if (coincide(*m)) lista.push_back(m.get());
    }
    // ...y ordenamos (más recientes primero) usando el operador < sobrecargado.
    std::sort(lista.begin(), lista.end(),
              [](const Movimiento* a, const Movimiento* b) { return *b < *a; });

    tablaMov->setRowCount(0);
    tablaMov->setRowCount(int(lista.size()));
    for (int i = 0; i < int(lista.size()); ++i) {
        const Movimiento* m = lista[i];
        tablaMov->setItem(i, 0, celda(QString::number(m->getId())));
        tablaMov->setItem(i, 1, celda(m->getFecha().toString("dd/MM/yyyy")));
        tablaMov->setItem(i, 2, celda(m->tipo()));
        tablaMov->setItem(i, 3, celda(m->getCategoria()));
        tablaMov->setItem(i, 4, celda(m->getDescripcion()));
        auto* monto = celda(formatearSoles(m->getMonto()));
        monto->setForeground(QColor(m->tipo() == "Ingreso" ? "#2e7d32" : "#c62828"));
        tablaMov->setItem(i, 5, monto);
    }
}

// ---------- Presupuestos ----------
QWidget* VentanaPrincipal::crearPresupuestos() {
    auto* w = new QWidget;
    w->setObjectName("pagina");
    auto* v = new QVBoxLayout(w);

    cmbCatPresupuesto = new QComboBox;
    spinLimite = crearSpinSoles();
    auto* form = new QFormLayout;
    form->addRow("Categoría:", cmbCatPresupuesto);
    form->addRow("Límite mensual:", spinLimite);
    v->addLayout(form);

    auto* botones = new QHBoxLayout;
    auto* btnCrear = new QPushButton("Crear presupuesto");
    auto* btnEliminar = new QPushButton("Eliminar seleccionado");
    botones->addWidget(btnCrear);
    botones->addWidget(btnEliminar);
    botones->addStretch();
    v->addLayout(botones);

    tablaPresupuestos = crearTabla({"ID", "Categoría", "Límite", "Gastado este mes", "Estado"});
    v->addWidget(tablaPresupuestos, 1);

    connect(btnCrear, &QPushButton::clicked, this, [this]() {
        mostrarResultado(ctrl.crearPresupuesto(cmbCatPresupuesto->currentText(), spinLimite->value()));
    });
    connect(btnEliminar, &QPushButton::clicked, this, [this]() {
        int id = idSeleccionado(tablaPresupuestos);
        if (id < 0) { QMessageBox::information(this, "Eliminar", "Selecciona un presupuesto."); return; }
        if (confirmar("¿Eliminar este presupuesto?")) mostrarResultado(ctrl.eliminarPresupuesto(id));
    });
    return w;
}

void VentanaPrincipal::refrescarPresupuestos() {
    const auto& lista = ctrl.modelo().presupuestos();
    tablaPresupuestos->setRowCount(0);
    tablaPresupuestos->setRowCount(int(lista.size()));
    for (int i = 0; i < int(lista.size()); ++i) {
        const Presupuesto& p = lista[i];
        double gastado = ctrl.modelo().gastoDelMes(p.getCategoria(), QDate::currentDate());
        QString estado = "Dentro del límite";
        QColor color("#2e7d32");
        if (gastado > p.getLimite()) { estado = "Límite superado"; color = QColor("#c62828"); }
        else if (gastado >= 0.8 * p.getLimite()) { estado = "Cerca del límite (80%)"; color = QColor("#ef6c00"); }

        tablaPresupuestos->setItem(i, 0, celda(QString::number(p.getId())));
        tablaPresupuestos->setItem(i, 1, celda(p.getCategoria()));
        tablaPresupuestos->setItem(i, 2, celda(formatearSoles(p.getLimite())));
        tablaPresupuestos->setItem(i, 3, celda(formatearSoles(gastado)));
        auto* it = celda(estado);
        it->setForeground(color);
        tablaPresupuestos->setItem(i, 4, it);
    }
}

// ---------- Metas de ahorro ----------
QWidget* VentanaPrincipal::crearMetas() {
    auto* w = new QWidget;
    w->setObjectName("pagina");
    auto* v = new QVBoxLayout(w);

    txtMeta = new QLineEdit;
    txtMeta->setPlaceholderText("Ej. Laptop, viaje, celular...");
    spinObjetivo = crearSpinSoles();
    auto* form = new QFormLayout;
    form->addRow("Nombre de la meta:", txtMeta);
    form->addRow("Monto objetivo:", spinObjetivo);
    v->addLayout(form);

    auto* fila = new QHBoxLayout;
    auto* btnCrear = new QPushButton("Crear meta");
    spinAporte = crearSpinSoles();
    auto* btnAportar = new QPushButton("Aportar a la meta seleccionada");
    auto* btnEliminar = new QPushButton("Eliminar seleccionada");
    fila->addWidget(btnCrear);
    fila->addWidget(spinAporte);
    fila->addWidget(btnAportar);
    fila->addWidget(btnEliminar);
    v->addLayout(fila);

    tablaMetas = crearTabla({"ID", "Meta", "Objetivo", "Ahorrado", "Falta", "Progreso"});
    v->addWidget(tablaMetas, 1);

    connect(btnCrear, &QPushButton::clicked, this, [this]() {
        Resultado r = ctrl.crearMeta(txtMeta->text(), spinObjetivo->value());
        mostrarResultado(r);
        if (r.ok) { txtMeta->clear(); spinObjetivo->setValue(0); }
    });
    connect(btnAportar, &QPushButton::clicked, this, [this]() {
        int id = idSeleccionado(tablaMetas);
        if (id < 0) { QMessageBox::information(this, "Aportar", "Selecciona una meta."); return; }
        Resultado r = ctrl.aportarMeta(id, spinAporte->value());
        mostrarResultado(r);
        if (r.ok) spinAporte->setValue(0);
    });
    connect(btnEliminar, &QPushButton::clicked, this, [this]() {
        int id = idSeleccionado(tablaMetas);
        if (id < 0) { QMessageBox::information(this, "Eliminar", "Selecciona una meta."); return; }
        if (confirmar("¿Eliminar esta meta?")) mostrarResultado(ctrl.eliminarMeta(id));
    });
    return w;
}

void VentanaPrincipal::refrescarMetas() {
    const auto& lista = ctrl.modelo().metas();
    tablaMetas->setRowCount(0);
    tablaMetas->setRowCount(int(lista.size()));
    for (int i = 0; i < int(lista.size()); ++i) {
        const MetaAhorro& m = lista[i];
        tablaMetas->setItem(i, 0, celda(QString::number(m.getId())));
        tablaMetas->setItem(i, 1, celda(m.getNombre()));
        tablaMetas->setItem(i, 2, celda(formatearSoles(m.getObjetivo())));
        tablaMetas->setItem(i, 3, celda(formatearSoles(m.getAhorrado())));
        tablaMetas->setItem(i, 4, celda(formatearSoles(m.falta())));
        auto* barra = new QProgressBar;
        barra->setRange(0, 100);
        barra->setValue(int(m.porcentaje()));
        tablaMetas->setCellWidget(i, 5, barra);
    }
}

// ---------- Agenda ----------
QWidget* VentanaPrincipal::crearAgenda() {
    auto* w = new QWidget;
    w->setObjectName("pagina");
    auto* v = new QVBoxLayout(w);

    txtRecordatorio = new QLineEdit;
    txtRecordatorio->setPlaceholderText("Ej. Pagar la luz, cuota de la universidad...");
    dateVencimiento = new QDateEdit(QDate::currentDate().addDays(7));
    dateVencimiento->setCalendarPopup(true);
    auto* form = new QFormLayout;
    form->addRow("Pago pendiente:", txtRecordatorio);
    form->addRow("Vence el:", dateVencimiento);
    v->addLayout(form);

    auto* botones = new QHBoxLayout;
    auto* btnAgregar = new QPushButton("Agregar recordatorio");
    auto* btnPagado = new QPushButton("Marcar como pagado");
    auto* btnEliminar = new QPushButton("Eliminar seleccionado");
    botones->addWidget(btnAgregar);
    botones->addWidget(btnPagado);
    botones->addWidget(btnEliminar);
    v->addLayout(botones);

    tablaAgenda = crearTabla({"ID", "Descripción", "Vencimiento", "Estado"});
    v->addWidget(tablaAgenda, 1);

    connect(btnAgregar, &QPushButton::clicked, this, [this]() {
        Resultado r = ctrl.crearRecordatorio(txtRecordatorio->text(), dateVencimiento->date());
        mostrarResultado(r);
        if (r.ok) txtRecordatorio->clear();
    });
    connect(btnPagado, &QPushButton::clicked, this, [this]() {
        int id = idSeleccionado(tablaAgenda);
        if (id < 0) { QMessageBox::information(this, "Agenda", "Selecciona un recordatorio."); return; }
        mostrarResultado(ctrl.marcarPagado(id));
    });
    connect(btnEliminar, &QPushButton::clicked, this, [this]() {
        int id = idSeleccionado(tablaAgenda);
        if (id < 0) { QMessageBox::information(this, "Eliminar", "Selecciona un recordatorio."); return; }
        if (confirmar("¿Eliminar este recordatorio?")) mostrarResultado(ctrl.eliminarRecordatorio(id));
    });
    return w;
}

void VentanaPrincipal::refrescarAgenda() {
    const auto& lista = ctrl.modelo().recordatorios();
    tablaAgenda->setRowCount(0);
    tablaAgenda->setRowCount(int(lista.size()));
    for (int i = 0; i < int(lista.size()); ++i) {
        const Recordatorio& r = lista[i];
        QString estado = "Pendiente";
        QColor color("#ef6c00");
        if (r.estaPagado()) { estado = "Pagado"; color = QColor("#2e7d32"); }
        else if (r.getVencimiento() < QDate::currentDate()) { estado = "Vencido"; color = QColor("#c62828"); }

        tablaAgenda->setItem(i, 0, celda(QString::number(r.getId())));
        tablaAgenda->setItem(i, 1, celda(r.getDescripcion()));
        tablaAgenda->setItem(i, 2, celda(r.getVencimiento().toString("dd/MM/yyyy")));
        auto* it = celda(estado);
        it->setForeground(color);
        tablaAgenda->setItem(i, 3, it);
    }
}

// ---------- Reportes ----------

QWidget* VentanaPrincipal::crearReportes() {

    auto* w = new QWidget;
    w->setObjectName("pagina");

    auto* principal = new QVBoxLayout(w);
    principal->setContentsMargins(20, 20, 20, 20);
    principal->setSpacing(18);

    // ESTILO DE LOS PANELES
    const QString estiloPanel = R"(
        QGroupBox {
            background-color: white;
            border: 1px solid #ded4e5;
            border-radius: 15px;
            margin-top: 12px;
            padding: 15px;
            font-size: 16px;
            font-weight: bold;
            color: #29202f;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 5px;
        }
    )";

    // =====================================
    // 1. ENCABEZADO
    // =====================================

    auto* encabezado = new QHBoxLayout;

    auto* titulo = new QLabel("Resumen financiero");
    titulo->setStyleSheet(
        "font-size: 22px; font-weight: bold; color: #481052;");

    encabezado->addWidget(titulo);
    encabezado->addStretch();

    principal->addLayout(encabezado);

    // =====================================
    // 2. BALANCE GENERAL
    // =====================================

    auto* balance = new QGroupBox("BALANCE GENERAL");
    balance->setStyleSheet(estiloPanel);

    auto* filaBalance = new QHBoxLayout(balance);
    filaBalance->setSpacing(25);

    auto crearIndicador = [](const QString& nombre,
                             QLabel*& valor,
                             const QString& color) {

        auto* contenedor = new QWidget;
        auto* layout = new QVBoxLayout(contenedor);
        layout->setContentsMargins(0, 5, 0, 5);

        auto* etiqueta = new QLabel(nombre);
        etiqueta->setStyleSheet(
            "font-size: 15px; color: #333333;");

        valor = new QLabel("S/ 0.00");
        valor->setStyleSheet(
            "font-size: 27px; font-weight: bold; color: "
            + color + ";");

        layout->addWidget(etiqueta);
        layout->addWidget(valor);

        return contenedor;
    };

    filaBalance->addWidget(
        crearIndicador("Ingresos totales", lblReporteIngresos,
                       "#29965a"));

    filaBalance->addWidget(
        crearIndicador("Gastos totales", lblReporteGastos,
                       "#a13b49"));

    filaBalance->addWidget(
        crearIndicador("Saldo actual", lblReporteSaldo,
                       "#596981"));

    principal->addWidget(balance);

    // =====================================
    // 3. GASTOS POR CATEGORÍA
    // =====================================

    auto* filaGraficos = new QHBoxLayout;
    filaGraficos->setSpacing(18);

    auto* panelCategorias = new QGroupBox(
        "GASTOS POR CATEGORÍA");

    panelCategorias->setStyleSheet(estiloPanel);

    layoutCategoriasReporte = new QVBoxLayout(
        panelCategorias);

    layoutCategoriasReporte->setSpacing(12);

    filaGraficos->addWidget(panelCategorias, 3);

    // =====================================
    // 4. COMPARACIÓN MENSUAL
    // =====================================

    auto* panelMensual = new QGroupBox(
        "COMPARACIÓN MENSUAL");

    panelMensual->setStyleSheet(estiloPanel);

    layoutMensualReporte = new QVBoxLayout(panelMensual);

    layoutMensualReporte->setContentsMargins(10, 15, 10, 10);

    graficoMensualReporte = new GraficoMensualWidget;

    layoutMensualReporte->addWidget(graficoMensualReporte);

    filaGraficos->addWidget(panelMensual, 1);

    principal->addLayout(filaGraficos, 1);

    principal->addStretch();

    return w;
}


void VentanaPrincipal::refrescarReportes() {

    const GestorFinanciero& g = ctrl.modelo();

    // =====================================
    // 1. BALANCE GENERAL
    // =====================================

    lblReporteIngresos->setText(
        formatearSoles(g.totalIngresos()));

    lblReporteGastos->setText(
        formatearSoles(g.totalGastos()));

    lblReporteSaldo->setText(
        formatearSoles(g.saldoActual()));

    // =====================================
    // 2. LIMPIAR CATEGORÍAS ANTERIORES
    // =====================================

    while (QLayoutItem* item =
           layoutCategoriasReporte->takeAt(0)) {

        delete item->widget();
        delete item;
    }

    QMap<QString, double> categorias =
        g.gastosPorCategoria();

    double mayorGasto = 0;

    for (auto it = categorias.begin();
         it != categorias.end(); ++it) {

        mayorGasto = qMax(mayorGasto, it.value());
    }

    if (categorias.isEmpty()) {

        auto* mensaje = new QLabel(
            "Todavía no hay gastos registrados.");

        layoutCategoriasReporte->addWidget(mensaje);

    } else {

        for (auto it = categorias.begin();
             it != categorias.end(); ++it) {

            auto* nombre = new QLabel(
                it.key() + "     " +
                formatearSoles(it.value()));

            nombre->setStyleSheet(
                "font-size: 14px;"
                "font-weight: 600;"
                "color: #42334a;");

            auto* barra = new QProgressBar;

            barra->setRange(0, 100);

            int porcentaje = mayorGasto > 0
                                 ? int(it.value() * 100.0 / mayorGasto)
                                 : 0;

            barra->setValue(porcentaje);
            barra->setTextVisible(false);
            barra->setFixedHeight(13);

            barra->setStyleSheet(R"(
                QProgressBar {
                    background-color: #eee5f0;
                    border: none;
                    border-radius: 6px;
                }

                QProgressBar::chunk {
                    background-color: #a90078;
                    border-radius: 6px;
                }
            )");

            layoutCategoriasReporte->addWidget(nombre);
            layoutCategoriasReporte->addWidget(barra);
        }
    }

    // =====================================
    // 3. COMPARACIÓN MENSUAL
    // =====================================

    QMap<QString, QPair<double, double>> meses;

    for (const auto& m : g.movimientos()) {

        QString clave =
            m->getFecha().toString("yyyy-MM");

        if (m->tipo() == "Ingreso") {
            meses[clave].first += m->getMonto();
        }
        else {
            meses[clave].second += m->getMonto();
        }
    }

    // Enviar los datos al gráfico vertical
    graficoMensualReporte->setDatos(meses);

}


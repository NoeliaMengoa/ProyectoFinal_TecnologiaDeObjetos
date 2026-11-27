#include "GraficoBarras.h"
#include "Utilidades.h"
#include <QPainter>
#include <algorithm>

GraficoBarras::GraficoBarras(QWidget* parent) : QWidget(parent) {
    setMinimumHeight(180);
}

void GraficoBarras::setDatos(const QMap<QString, double>& nuevos) {
    datos = nuevos;
    update();
}

void GraficoBarras::paintEvent(QPaintEvent*) {
    QPainter p(this);
    if (datos.isEmpty()) {
        p.drawText(rect(), Qt::AlignCenter, "Aún no hay gastos registrados");
        return;
    }
    double maximo = 0;
    for (double v : datos) maximo = std::max(maximo, v);

    int alto = std::min(28, height() / int(datos.size()) - 4);
    int y = 4;
    int anchoMax = std::max(50, width() - 280);
    for (auto it = datos.begin(); it != datos.end(); ++it) {
        int ancho = int(anchoMax * it.value() / maximo);
        p.drawText(QRect(4, y, 120, alto), Qt::AlignVCenter, it.key());
        p.fillRect(130, y, ancho, alto, QColor("#7e57c2"));
        p.drawText(QRect(136 + ancho, y, 130, alto), Qt::AlignVCenter, formatearSoles(it.value()));
        y += alto + 4;
    }
}

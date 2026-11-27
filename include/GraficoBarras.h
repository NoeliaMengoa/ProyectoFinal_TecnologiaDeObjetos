#pragma once
#include <QWidget>
#include <QMap>

// Widget simple que dibuja barras horizontales (gastos por categoría).
class GraficoBarras : public QWidget {
    QMap<QString, double> datos;

public:
    explicit GraficoBarras(QWidget* parent = nullptr);
    void setDatos(const QMap<QString, double>& nuevos);

protected:
    void paintEvent(QPaintEvent*) override;
};

#include <QApplication>
#include <QMessageBox>
#include <QStandardPaths>
#include <QDir>
#include <memory>
#include "RepositorioSQLite.h"
#include "GestorFinanciero.h"
#include "Controlador.h"
#include "VentanaPrincipal.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("MoneyMate");

    try {
        // La base de datos se guarda en la carpeta de datos del usuario.
        QString carpeta = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir().mkpath(carpeta);

        // Armamos MVC: Modelo (gestor) -> Controlador -> Vista (ventana).
        GestorFinanciero gestor(std::make_unique<RepositorioSQLite>(carpeta + "/moneymate.db"));
        Controlador controlador(gestor);
        VentanaPrincipal ventana(controlador);
        ventana.show();
        return app.exec();
    } catch (const ExcepcionFinanciera& e) {
        QMessageBox::critical(nullptr, "Money Mate", QString::fromStdString(e.what()));
        return 1;
    }
}

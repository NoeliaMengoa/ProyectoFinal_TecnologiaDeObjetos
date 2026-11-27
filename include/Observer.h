#pragma once
#include <vector>

// PATRÓN OBSERVER
// IObservador: quien quiere enterarse de los cambios (la ventana).
class IObservador {
public:
    virtual ~IObservador() = default;
    virtual void actualizar() = 0;
};

// Sujeto: quien avisa a todos sus observadores (el gestor financiero).
class Sujeto {
    std::vector<IObservador*> observadores;

public:
    virtual ~Sujeto() = default;
    void agregarObservador(IObservador* o) { observadores.push_back(o); }
    void notificar() {
        for (auto* o : observadores) o->actualizar();
    }
};

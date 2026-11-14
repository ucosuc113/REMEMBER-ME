#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Scene/SceneService.h"

#include "Estado.h"

using namespace strace;

class Salida : public strace::IScript {
    ObjectField jugador{this, "jugador"};
    ObjectField fundido{this, "fundido"};
    TextField siguiente{this, "siguiente", "Hab2"};
    FloatField blanco{this, "blanco", 2.0f};
    FloatField espera{this, "espera", 1.0f};

    bool m_activa = false;
    float m_t = 0.0f;
    bool m_pedida = false;

public:
    void OnStart() override {
        m_activa = false;
        m_pedida = false;
        m_t = 0.0f;
    }

    void OnUpdate(float deltaTime) override {
        if (!m_activa) {
            GameObject* j = jugador.Get();
            remember::Caja3 yo, zona;
            if (!j || !remember::CajaDe(*j, yo) || !remember::CajaDe(*gameObject, zona)) return;
            if (!remember::Tocan(yo, zona)) return;
            m_activa = true;
            m_t = 0.0f;
        }

        m_t += deltaTime;
        float a = remember::Suave(m_t / std::max(0.01f, blanco.Get()));
        remember::Pintar(fundido.Get(), 1, 1, 1, a);

        if (!m_pedida && m_t >= blanco.Get() + espera.Get()) {
            m_pedida = true;
            if (!siguiente.Get().empty()) SceneService::LoadScene(siguiente.Get());
        }
    }
};

STRACE_REGISTER_SCRIPT(Salida)
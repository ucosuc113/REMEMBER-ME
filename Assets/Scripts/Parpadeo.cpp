#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"

using namespace strace;

class Parpadeo : public strace::IScript {
    FloatField bpm{this, "bpm", 110.0f};
    FloatField pulsos{this, "pulsos", 2.0f};
    ObjectField plano{this, "plano"};

    float m_tiempo = 0.0f;
    bool m_visible = true;

public:
    void OnStart() override {
        m_tiempo = 0.0f;
        m_visible = true;
        if (GameObject* p = plano.Get()) p->active = true;
    }

    void OnUpdate(float deltaTime) override {
        GameObject* p = plano.Get();
        if (!p || bpm.Get() <= 0.0f) return;

        float duracion = pulsos.Get() * 60.0f / bpm.Get();
        m_tiempo += deltaTime;

        while (m_tiempo >= duracion) {
            m_tiempo -= duracion;
            m_visible = !m_visible;
            p->active = m_visible;
        }
    }
};

STRACE_REGISTER_SCRIPT(Parpadeo)
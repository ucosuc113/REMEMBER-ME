#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/UI/SceneUi.h"
#include "Engine/Input/Cursor.h"
#include "Engine/Scene/SceneService.h"

#include "Estado.h"

using namespace strace;

class BotonJugar : public strace::IScript {
    ObjectField boton{this, "boton"};
    ObjectField fundido{this, "fundido"};
    TextField escena{this, "escena", "Escena Principal"};
    FloatField duracion{this, "duracion", 1.0f};

    bool m_saliendo = false;
    bool m_pedida = false;
    float m_t = 0.0f;

public:
    void OnStart() override {
        Cursor::SetMode(CursorMode::Visible);
        remember::controlBloqueado = false;
        remember::Pintar(fundido.Get(), 0, 0, 0, 0);
        m_saliendo = false;
        m_pedida = false;
        m_t = 0.0f;
    }

    void OnUpdate(float deltaTime) override {
        if (!m_saliendo) {
            GameObject* b = boton.Get();
            if (b && SceneUi::Clicked(*b)) {
                m_saliendo = true;
                m_t = 0.0f;
            }
            return;
        }

        m_t += deltaTime;
        remember::Pintar(fundido.Get(), 0, 0, 0, remember::Suave(m_t / std::max(0.01f, duracion.Get())));
        if (!m_pedida && m_t >= duracion.Get()) {
            m_pedida = true;
            SceneService::LoadScene(escena.Get());
        }
    }
};

STRACE_REGISTER_SCRIPT(BotonJugar)
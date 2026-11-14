#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Scene/SceneService.h"
#include "Engine/Audio/AudioService.h"
#include "Engine/Text.h"

using namespace strace;

class Parpadeo : public strace::IScript {
    FloatField bpm{this, "bpm", 110.0f};
    FloatField pulsos{this, "pulsos", 1.0f};
    ObjectField plano{this, "plano"};
    TextField sonido{this, "sonido", "Assets/Audio/El beeper.mp3"};
    FloatField volumen{this, "volumen", -6.0f};

    float m_tiempo = 0.0f;
    bool m_visible = true;
    GameObject* m_objetivo = nullptr;
    float m_escala[3] = {1.0f, 1.0f, 1.0f};

    void Mostrar(bool visible) {
        if (!m_objetivo) return;
        for (int i = 0; i < 3; ++i) m_objetivo->transform.scale[i] = visible ? m_escala[i] : 0.0f;
    }

public:
    void OnStart() override {
        m_objetivo = plano.Get() ? plano.Get() : gameObject;
        for (int i = 0; i < 3; ++i) m_escala[i] = m_objetivo->transform.scale[i];
        m_objetivo->active = true;
        m_tiempo = 0.0f;
        m_visible = true;
        Mostrar(true);

        if (sonido.Get().empty()) return;
        std::wstring root = SceneService::ProjectRoot();
        if (!root.empty() && root.back() != L'/' && root.back() != L'\\') root += L'/';
        AudioService::PlayTrackFile(root + Utf8ToWide(sonido.Get()), volumen.Get(), true);
    }

    void OnUpdate(float deltaTime) override {
        if (!m_objetivo || bpm.Get() <= 0.0f) return;

        float duracion = pulsos.Get() * 60.0f / bpm.Get();
        m_tiempo += deltaTime;

        while (m_tiempo >= duracion) {
            m_tiempo -= duracion;
            m_visible = !m_visible;
            Mostrar(m_visible);
        }
    }
};

STRACE_REGISTER_SCRIPT(Parpadeo)
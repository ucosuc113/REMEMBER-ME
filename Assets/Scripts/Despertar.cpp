#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Components/Physics/Rigidbody.h"

#include "Estado.h"

#include <cmath>

using namespace strace;

class Despertar : public strace::IScript {
    ObjectField jugador{this, "jugador"};
    ObjectField camara{this, "camara"};
    ObjectField fundido{this, "fundido"};
    ObjectField puntoCama{this, "puntoCama"};
    ObjectField puntoPie{this, "puntoPie"};
    FloatField mirarArriba{this, "mirarArriba", -80.0f};
    FloatField negro{this, "negro", 1.5f};
    FloatField aparecer{this, "aparecer", 3.0f};
    FloatField esperar{this, "esperar", 1.5f};
    FloatField levantarse{this, "levantarse", 2.5f};

    int m_fase = 0;
    float m_t = 0.0f;
    float m_desde[3] = {0, 0, 0};
    float m_hasta[3] = {0, 0, 0};
    float m_yawDesde = 0.0f;
    float m_yawHasta = 0.0f;

    static float Angulo(float desde, float hasta, float t) {
        float d = std::fmod(hasta - desde + 540.0f, 360.0f) - 180.0f;
        return desde + d * t;
    }

    void Pose(float t) {
        GameObject* j = jugador.Get();
        if (!j) return;
        float s = remember::Suave(t);
        for (int i = 0; i < 3; ++i) j->transform.position[i] = m_desde[i] + (m_hasta[i] - m_desde[i]) * s;
        j->transform.position[1] += std::sin(t * 3.14159265f) * 0.12f;
        j->transform.rotation[1] = Angulo(m_yawDesde, m_yawHasta, s);
        if (GameObject* c = camara.Get()) {
            float sp = remember::Suave(std::min(1.0f, t * 1.4f));
            c->transform.rotation[0] = mirarArriba.Get() * (1.0f - sp);
        }
    }

public:
    void OnStart() override {
        remember::controlBloqueado = true;
        remember::Pintar(fundido.Get(), 0, 0, 0, 1);

        GameObject* j = jugador.Get();
        GameObject* a = puntoCama.Get();
        GameObject* b = puntoPie.Get();
        if (!j || !a || !b) {
            remember::controlBloqueado = false;
            remember::Pintar(fundido.Get(), 0, 0, 0, 0);
            m_fase = 4;
            return;
        }
        for (int i = 0; i < 3; ++i) {
            m_desde[i] = a->transform.position[i];
            m_hasta[i] = b->transform.position[i];
        }
        m_yawDesde = a->transform.rotation[1];
        m_yawHasta = b->transform.rotation[1];
        if (Rigidbody* rb = GetComponent<Rigidbody>(*j)) {
            rb->isKinematic = true;
            rb->velocity = {0, 0, 0};
        }
        Pose(0.0f);
        m_fase = 0;
        m_t = 0.0f;
    }

    void OnUpdate(float deltaTime) override {
        if (m_fase >= 4) return;
        m_t += deltaTime;

        if (m_fase == 0) {
            Pose(0.0f);
            if (m_t >= negro.Get()) { m_fase = 1; m_t = 0.0f; }
        } else if (m_fase == 1) {
            Pose(0.0f);
            float a = 1.0f - remember::Suave(m_t / std::max(0.01f, aparecer.Get()));
            remember::Pintar(fundido.Get(), 0, 0, 0, a);
            if (m_t >= aparecer.Get()) { m_fase = 2; m_t = 0.0f; }
        } else if (m_fase == 2) {
            Pose(0.0f);
            if (m_t >= esperar.Get()) { m_fase = 3; m_t = 0.0f; }
        } else if (m_fase == 3) {
            float t = std::min(1.0f, m_t / std::max(0.01f, levantarse.Get()));
            Pose(t);
            if (t >= 1.0f) {
                if (GameObject* j = jugador.Get()) {
                    if (Rigidbody* rb = GetComponent<Rigidbody>(*j)) {
                        rb->isKinematic = false;
                        rb->velocity = {0, 0, 0};
                    }
                }
                remember::controlBloqueado = false;
                m_fase = 4;
            }
        }
    }
};

STRACE_REGISTER_SCRIPT(Despertar)
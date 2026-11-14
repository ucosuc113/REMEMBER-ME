#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Input/InputService.h"
#include "Engine/Input/Cursor.h"
#include "Engine/Scene/SceneService.h"
#include "Engine/Components/Physics/Rigidbody.h"

#include "Estado.h"

#include <cmath>
#include <algorithm>
#include <functional>
#include <vector>

using namespace strace;

class Control : public strace::IScript {
    FloatField speed{this, "speed", 2.0f};
    FloatField jump{this, "jump", 4.8f};
    FloatField sensibilidad{this, "sensibilidad", 0.15f};
    BoolField invertirY{this, "invertirY", false};
    FloatField empuje{this, "empuje", 0.7f};
    ObjectField camara{this, "camara"};

    float m_pitch = 0.0f;
    bool m_fijo = true;
    std::vector<GameObject*> m_cajas;

    void BuscarCajas() {
        m_cajas.clear();
        std::function<void(GameObject&)> ver = [&](GameObject& n) {
            if (n.name.rfind("Caja", 0) == 0 && GetComponent<Rigidbody>(n) && GetComponent<BoxCollider3D>(n))
                m_cajas.push_back(&n);
            for (auto& c : n.children) ver(*c);
        };
        for (auto& r : SceneService::MutableRoots()) ver(*r);
    }

    void Empujar(float vx, float vz) {
        remember::Caja3 yo;
        if (!remember::CajaDe(*gameObject, yo)) return;
        const float margen = 0.08f;
        float px = (yo.min[0] + yo.max[0]) * 0.5f;
        float pz = (yo.min[2] + yo.max[2]) * 0.5f;
        float rx = (yo.max[0] - yo.min[0]) * 0.5f;
        float rz = (yo.max[2] - yo.min[2]) * 0.5f;

        for (GameObject* caja : m_cajas) {
            Rigidbody* rb = GetComponent<Rigidbody>(*caja);
            remember::Caja3 c;
            if (!caja->active || !rb || !remember::CajaDe(*caja, c)) continue;

            float bx = (c.min[0] + c.max[0]) * 0.5f;
            float bz = (c.min[2] + c.max[2]) * 0.5f;
            float hx = (c.max[0] - c.min[0]) * 0.5f;
            float hz = (c.max[2] - c.min[2]) * 0.5f;
            float gx = std::fabs(px - bx) - (hx + rx);
            float gz = std::fabs(pz - bz) - (hz + rz);
            bool encima = yo.min[1] >= c.max[1] - 0.05f;
            bool alLado = !encima && yo.max[1] > c.min[1] && yo.min[1] < c.max[1];

            float nx = 0.0f, nz = 0.0f;
            if (alLado && gx < margen && gz < margen) {
                if (gx > gz) {
                    float dir = bx > px ? 1.0f : -1.0f;
                    if (vx * dir > 0.0f) nx = vx * empuje.Get();
                } else {
                    float dir = bz > pz ? 1.0f : -1.0f;
                    if (vz * dir > 0.0f) nz = vz * empuje.Get();
                }
            }
            rb->velocity.x = nx;
            rb->velocity.z = nz;
        }
    }

public:
    void OnStart() override {
        InputActions& in = InputService::Actions();
        in.BindKey("saltar", Key::Space);
        in.BindKey("soltar_mouse", Key::Escape);
        in.BindMouseButton("tomar_mouse", MouseButton::Left);
        Cursor::SetMode(CursorMode::Locked);
        m_fijo = true;
        BuscarCajas();
    }

    void OnUpdate(float deltaTime) override {
        Rigidbody* rb = GetComponent<Rigidbody>(*gameObject);
        if (!rb) return;

        InputActions& in = InputService::Actions();
        Transform& t = gameObject->transform;
        GameObject* cam = camara.Get();

        if (remember::controlBloqueado) {
            rb->velocity.x = 0.0f;
            rb->velocity.z = 0.0f;
            if (cam) m_pitch = -cam->transform.rotation[0];
            Empujar(0.0f, 0.0f);
            return;
        }

        if (m_fijo && in.Pressed("soltar_mouse")) {
            m_fijo = false;
            Cursor::SetMode(CursorMode::Visible);
        } else if (!m_fijo && in.Pressed("tomar_mouse")) {
            m_fijo = true;
            Cursor::SetMode(CursorMode::Locked);
        }

        if (m_fijo) {
            float signo = invertirY.Get() ? 1.0f : -1.0f;
            t.rotation[1] += in.LookDeltaX() * sensibilidad.Get();
            m_pitch = std::clamp(m_pitch + signo * in.LookDeltaY() * sensibilidad.Get(), -85.0f, 85.0f);
        }

        if (cam) cam->transform.rotation[0] = -m_pitch;

        float avanzar = in.Axis("move_back", "move_forward");
        float lateral = in.Axis("move_left", "move_right");

        float yaw = t.rotation[1] * 3.14159265f / 180.0f;
        float fx = std::sin(yaw);
        float fz = std::cos(yaw);
        float rx = fz;
        float rz = -fx;

        float dx = fx * avanzar + rx * lateral;
        float dz = fz * avanzar + rz * lateral;
        float len = std::sqrt(dx * dx + dz * dz);
        if (len > 1.0f) {
            dx /= len;
            dz /= len;
        }

        rb->velocity.x = dx * speed.Get();
        rb->velocity.z = dz * speed.Get();
        Empujar(rb->velocity.x, rb->velocity.z);

        if (rb->grounded && in.Pressed("saltar")) {
            rb->velocity.y = jump.Get();
        }
    }
};

STRACE_REGISTER_SCRIPT(Control)
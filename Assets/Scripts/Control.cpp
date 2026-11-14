#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Input/InputService.h"
#include "Engine/Input/Cursor.h"
#include "Engine/Scene/ComponentQuery.h"
#include "Engine/Components/Physics/Rigidbody.h"

#include <cmath>
#include <algorithm>

using namespace strace;

class Control : public strace::IScript {
    FloatField speed{this, "speed", 5.0f};
    FloatField jump{this, "jump", 7.0f};
    FloatField sensibilidad{this, "sensibilidad", 0.15f};
    BoolField invertirY{this, "invertirY", false};
    ObjectField camara{this, "camara"};

    float m_pitch = 0.0f;

public:
    void OnStart() override {
        InputActions& in = InputService::Actions();
        in.BindKey("saltar", Key::Space);
        in.BindKey("soltar_mouse", Key::Escape);
        in.BindMouseButton("tomar_mouse", MouseButton::Left);
        Cursor::SetMode(CursorMode::Hidden);
    }

    void OnUpdate(float deltaTime) override {
        Rigidbody* rb = GetComponent<Rigidbody>(*gameObject);
        if (!rb) return;

        InputActions& in = InputService::Actions();
        Transform& t = gameObject->transform;

        if (in.Pressed("soltar_mouse")) Cursor::SetMode(CursorMode::Visible);
        if (in.Pressed("tomar_mouse")) Cursor::SetMode(CursorMode::Hidden);

        if (Cursor::Mode() == CursorMode::Hidden) {
            float signo = invertirY.Get() ? 1.0f : -1.0f;
            t.rotation[1] += in.CursorDeltaX() * sensibilidad.Get();
            m_pitch = std::clamp(m_pitch + signo * in.CursorDeltaY() * sensibilidad.Get(), -85.0f, 85.0f);
        }

        if (GameObject* cam = camara.Get()) {
            cam->transform.rotation[0] = -m_pitch;
        }

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

        if (rb->grounded && in.Pressed("saltar")) {
            rb->velocity.y = jump.Get();
        }
    }
};

STRACE_REGISTER_SCRIPT(Control)
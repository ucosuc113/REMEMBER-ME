#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/UI/SceneUi.h"
#include "Engine/Input/Cursor.h"

using namespace strace;

class BotonJugar : public strace::IScript {
    ObjectField boton{this, "boton"};
    ObjectField menu{this, "menu"};
    ObjectField juego{this, "juego"};

public:
    void OnStart() override {
        Cursor::SetMode(CursorMode::Visible);
        if (GameObject* j = juego.Get()) j->active = false;
    }

    void OnUpdate(float) override {
        GameObject* b = boton.Get();
        if (!b || !SceneUi::Clicked(*b)) return;

        if (GameObject* m = menu.Get()) m->active = false;
        if (GameObject* j = juego.Get()) j->active = true;
        Cursor::SetMode(CursorMode::Locked);
    }
};

STRACE_REGISTER_SCRIPT(BotonJugar)
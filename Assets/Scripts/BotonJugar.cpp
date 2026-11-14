#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/UI/SceneUi.h"
#include "Engine/Input/Cursor.h"
#include "Engine/Scene/SceneService.h"

using namespace strace;

class BotonJugar : public strace::IScript {
    ObjectField boton{this, "boton"};
    TextField escena{this, "escena", "Escena Principal"};

public:
    void OnStart() override {
        Cursor::SetMode(CursorMode::Visible);
    }

    void OnUpdate(float) override {
        GameObject* b = boton.Get();
        if (b && SceneUi::Clicked(*b)) SceneService::LoadScene(escena.Get());
    }
};

STRACE_REGISTER_SCRIPT(BotonJugar)
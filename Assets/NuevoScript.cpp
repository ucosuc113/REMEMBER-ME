#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"

class NuevoScript : public strace::IScript {
public:
    void OnStart() override {
    }

    void OnUpdate(float deltaTime) override {
    }
};

STRACE_REGISTER_SCRIPT(NuevoScript)



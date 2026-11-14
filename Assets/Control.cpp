#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"

#include ""

class Control : public strace::IScript {
public:
    void OnStart() override {
    }

 

    void OnUpdate(float deltaTime) override {
    in.BindKey"Caminar",Key::W
    
    if(in.pressed("Caminar")){
    
    }
    }
};

STRACE_REGISTER_SCRIPT(NuevoScript)


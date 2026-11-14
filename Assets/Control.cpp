#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Input/InputService.h"

#include <cmath>

using namespace strace;

class Control : public strace::IScript {
    
FloatField speed{this,"speed",5.0f;
FloatField giro{this,"giro",2.0f;

public:

void OnUpdate(float deltaTime) override {
    InputActions& in = InputService::Actions();
 
 float avanzar = in.Axis("move_forward","move_back");
 
 Transform& t = gameObject->transform;
   
    }
};

STRACE_REGISTER_SCRIPT(Control)


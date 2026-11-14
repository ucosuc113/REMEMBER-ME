#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Input/InputService.h"

#include <cmath>

using namespace strace;

class Control : public strace::IScript {
    
FloatField speed{this,"speed",5.0f};
FloatField giro{this,"giro",2.0f};

public: 

void OnUpdate(float deltaTime) override {
    InputActions& in = InputService::Actions();
 
 float avanzar = in.Axis("move_back", "move_forward");
 
 Transform& t = gameObject->transform;
   
   float yaw = t.rotation[1] * 3.14159265f / 180.0f;
   
   t.position[0] += sin(yaw) * avanzar * speed.Get() * deltaTime;
   t.position[2] += cos(yaw) * avanzar * speed.Get() * deltaTime;
   
    }
};

STRACE_REGISTER_SCRIPT(Control)


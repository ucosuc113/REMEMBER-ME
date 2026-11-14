#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Input/InputService.h"

#include <cmath>

using namespace strace;

class Control : public strace::IScript {
    
FloatField speed{this,"speed",5.0f};
FloatField giro{this,"giro",120.0f};

public: 

void OnUpdate(float deltaTime) override {
    InputActions& in = InputService::Actions();
 
 float avanzar = in.Axis("move_back", "move_forward");
 float girar = in.Axis("move_left", "move_right");
 Transform& t = gameObject->transform;
 
   t.rotation[1] += girar * giro.Get() * deltaTime;
   
   float yaw = t.rotation[1] * 3.14159265f / 180.0f;
   
   t.position[0] += sin(yaw) * avanzar * speed.Get() * deltaTime;
   t.position[2] += cos(yaw) * avanzar * speed.Get() * deltaTime;
   
    }
};

STRACE_REGISTER_SCRIPT(Control)


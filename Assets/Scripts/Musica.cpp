#include "Engine/Scripting/IScript.h"
#include "Engine/Scripting/ScriptRegistry.h"
#include "Engine/Scripting/ScriptFields.h"
#include "Engine/Scene/SceneService.h"
#include "Engine/Audio/AudioService.h"
#include "Engine/Text.h"

using namespace strace;

class Musica : public strace::IScript {
    TextField pista{this, "pista", "Assets/Audio/Remember Me.mp3"};
    FloatField volumen{this, "volumen", -6.0f};
    BoolField repetir{this, "repetir", true};

public:
    void OnStart() override {
        std::wstring root = SceneService::ProjectRoot();
        if (!root.empty() && root.back() != L'/' && root.back() != L'\\') root += L'/';
        AudioService::PlayTrackFile(root + Utf8ToWide(pista.Get()), volumen.Get(), repetir.Get());
    }
};

STRACE_REGISTER_SCRIPT(Musica)
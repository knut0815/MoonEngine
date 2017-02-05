#pragma once

#include <EngineApp/MoonEngineCfg.h>
#include "MeshLibrary.h"
#include "ProgramLibrary.h"
#include "InstTransformLib.h"
#include "TextureLibrary.h"
#include "SamplerLibrary.h"

namespace MoonEngine
{
    struct Library
    {
        static void Init(MoonEngineCfg config);

        static void Destroy();

        static std::shared_ptr<MeshLibrary> MeshLib;
        static std::shared_ptr<ProgramLibrary> ProgramLib;
        static std::shared_ptr<InstTransformLibrary> InstTransformLib;
        static std::shared_ptr<TextureLibrary> TextureLib;
        static std::shared_ptr<SamplerLibrary> SamplerLib;
    };
}

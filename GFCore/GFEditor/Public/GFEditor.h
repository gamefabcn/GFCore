#pragma once

#include "CoreMinimal.h"
#include "AutoPackager.h"
#include "Modules/ModuleManager.h"


class FGFEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    
private:
    TUniquePtr<FAutoPackager> AutoPackagerInstance;
};

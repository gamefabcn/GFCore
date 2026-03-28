#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "Framework/Commands/UICommandList.h"
#include "IUATHelperModule.h"


class FAutoPackagerCommands : public TCommands<FAutoPackagerCommands>
{
public:
	FAutoPackagerCommands()
		: TCommands<FAutoPackagerCommands>(TEXT("AutoPackager"), NSLOCTEXT("Contexts", "AutoPackager", "AutoPackager"), NAME_None, FAppStyle::GetAppStyleSetName()) {}

	virtual void RegisterCommands() override;

	TSharedPtr<FUICommandInfo> PackageAction;
	TSharedPtr<FUICommandInfo> CleanAction;
};

class FAutoPackager
{
public:
	FAutoPackager();
	~FAutoPackager();

	void Initialize();
	void Shutdown();

private:
	void RegisterMenus();
	void OnPackageButtonClicked();
	void OnCleanButtonClicked();
	void OnPackageCompleted(FString Result, double Time);
	void Run7zCompression(const FString& BuildOutputDir);

	TSharedPtr<class FMonitoredProcess> ZipProcess;
	TSharedPtr<FUICommandList> PluginCommands;
};
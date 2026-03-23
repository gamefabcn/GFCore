#include "AutoPackager.h"
#include "IUATHelperModule.h"
#include "LevelEditor.h"
#include "ToolMenus.h"
#include "Misc/Paths.h"
#include "Misc/DateTime.h"
#include "HAL/PlatformFileManager.h"
#include "Misc/MonitoredProcess.h"
#include "Async/Async.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

#define LOCTEXT_NAMESPACE "FAutoPackager"

FAutoPackager::FAutoPackager()
{
	UE_LOG(LogTemp, Log, TEXT("AutoPackager 功能类已创建"));
}

FAutoPackager::~FAutoPackager()
{
	Shutdown();
}

void FAutoPackager::Initialize()
{
	// 1. 注册指令
	FAutoPackagerCommands::Register();
	PluginCommands = MakeShareable(new FUICommandList);

	// 2. 映射指令
	PluginCommands->MapAction(FAutoPackagerCommands::Get().PackageAction, FExecuteAction::CreateRaw(this, &FAutoPackager::OnPackageButtonClicked));
	PluginCommands->MapAction(FAutoPackagerCommands::Get().CleanAction, FExecuteAction::CreateRaw(this, &FAutoPackager::OnCleanButtonClicked));

	// 3. 注册到 LevelEditor（快捷键生效）
	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetGlobalLevelEditorActions()->Append(PluginCommands.ToSharedRef());

	// 4. 注册菜单
	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FAutoPackager::RegisterMenus));
}

void FAutoPackager::Shutdown()
{
	UToolMenus::UnRegisterStartupCallback(this);
	UToolMenus::UnregisterOwner(this);
	FAutoPackagerCommands::Unregister();
}

// 注册指令与默认快捷键
void FAutoPackagerCommands::RegisterCommands()
{
	UI_COMMAND(PackageAction, "一键打包并压缩", "自动打包并调用7z压缩", EUserInterfaceActionType::Button, FInputChord(EModifierKey::Alt, EKeys::F8));
	UI_COMMAND(CleanAction, "一键清理项目", "安全删除 Saved, Intermediate, Binaries", EUserInterfaceActionType::Button, FInputChord());
}


void FAutoPackager::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	
	// 在主菜单栏创建 GF 菜单
	UToolMenu* MainMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu");
	MainMenu->AddSubMenu("MainMenu", NAME_None, "GFMenu", LOCTEXT("GFMenu_Label", "GF"), LOCTEXT("GFMenu_Tooltip", "GF 专属工具集"));

	UToolMenu* GFMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.GFMenu");
	{
		// 使用指令列表添加按钮，这样快捷键才会生效且在 UI 上显示
		FToolMenuSection& PackageSection = GFMenu->AddSection("GFPackage", LOCTEXT("GFPackage_Label", "发布"));
		PackageSection.AddMenuEntryWithCommandList(FAutoPackagerCommands::Get().PackageAction, PluginCommands);

		FToolMenuSection& MaintenanceSection = GFMenu->AddSection("GFMaintenance", LOCTEXT("GFMaintenance_Label", "维护"));
		MaintenanceSection.AddMenuEntryWithCommandList(FAutoPackagerCommands::Get().CleanAction, PluginCommands);
	}
}

// --- 清理逻辑 ---
void FAutoPackager::OnCleanButtonClicked()
{
	FString ProjectDir = FPaths::ProjectDir();
	TArray<FString> Folders = { "Saved", "Intermediate", "Binaries" };
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	for (const FString& Folder : Folders)
	{
		FString FullPath = ProjectDir / Folder;
		if (PlatformFile.DirectoryExists(*FullPath))
		{
			PlatformFile.DeleteDirectoryRecursively(*FullPath);
		}
	}

	FNotificationInfo Info(LOCTEXT("CleanDone", "项目清理完成！"));
	Info.ExpireDuration = 5.0f;
	FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(SNotificationItem::CS_Success);
}

// --- 打包逻辑
void FAutoPackager::OnPackageButtonClicked()
{
	// 1. 获取原始路径（可能相对）
	FString RawProjectPath = FPaths::GetProjectFilePath();
	UE_LOG(LogTemp, Warning, TEXT("[AutoPackager] 原始路径: %s"), *RawProjectPath);

	// 2. 强制转为绝对路径（关键修复！）
	FString ProjectPath = FPaths::ConvertRelativePathToFull(RawProjectPath);
	UE_LOG(LogTemp, Warning, TEXT("[AutoPackager] 绝对路径: %s"), *ProjectPath);

	if (ProjectPath.IsEmpty() || !FPaths::FileExists(ProjectPath))
	{
		UE_LOG(LogTemp, Error, TEXT("[AutoPackager] 项目文件不存在！路径无效"));
		return;
	}

	// 3. 打包输出目录
	FString ArchiveDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Release/Builds"));
	UE_LOG(LogTemp, Warning, TEXT("[AutoPackager] 输出目录: %s"), *ArchiveDir);

	// 4. 构建 UAT 命令行（使用绝对路径）
	FString CommandLine = FString::Printf(
		TEXT("BuildCookRun -project=\"%s\" -noP4 -clientconfig=Shipping -serverconfig=Shipping -nocompile -nocompileeditor -installed -utf8output -platform=Win64 -build -cook -stage -archive -archivedirectory=\"%s\""),
		*ProjectPath, *ArchiveDir);

	UE_LOG(LogTemp, Warning, TEXT("[AutoPackager] 完整 UAT 命令:\n%s"), *CommandLine);

	// 5. 执行 UAT
	IUATHelperModule::Get().CreateUatTask(
		CommandLine,
		LOCTEXT("Win64", "Windows"),
		LOCTEXT("PackTask", "Packaging"),
		LOCTEXT("PackShort", "Pack"),
		nullptr, nullptr,
		[this, ArchiveDir](FString Result, double Time)
		{
			AsyncTask(ENamedThreads::GameThread, [this, ArchiveDir, Result, Time]()
			{
				this->OnPackageCompleted(Result, Time);
				if (Result.Equals(TEXT("Completed")))
				{
					this->Run7zCompression(ArchiveDir / TEXT("Windows"));
				}
			});
		}
	);
}

void FAutoPackager::OnPackageCompleted(FString Result, double Time)
{
	UE_LOG(LogTemp, Log, TEXT("打包结束: %s, 耗时: %f 秒"), *Result, Time);
}

void FAutoPackager::Run7zCompression(const FString& BuildOutputDir)
{
	FString ProjectName = FApp::GetProjectName();
	FString DateStr = FDateTime::Now().ToString(TEXT("%Y%m%d"));
	FString TargetDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() / TEXT("Release/Zips"));

	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.DirectoryExists(*TargetDir)) PlatformFile.CreateDirectory(*TargetDir);

	int32 Index = 1;
	FString ZipFilePath;
	do {
		ZipFilePath = TargetDir / FString::Printf(TEXT("%s_%s_v%d.7z"), *ProjectName, *DateStr, Index++);
	} while (PlatformFile.FileExists(*ZipFilePath));

	FString SevenZipPath = TEXT("C:\\Program Files\\7-Zip\\7z.exe");
	if (!PlatformFile.FileExists(*SevenZipPath)) return;

	FString Args = FString::Printf(TEXT("a -t7z \"%s\" \"%s\\*\""), *ZipFilePath, *BuildOutputDir);

	ZipProcess = MakeShareable(new FMonitoredProcess(SevenZipPath, Args, true));
	ZipProcess->OnCompleted().BindLambda([ZipFilePath](int32 ReturnCode)
	{
		AsyncTask(ENamedThreads::GameThread, [ZipFilePath, ReturnCode]()
		{
			/*
			FNotificationInfo Info(ReturnCode == 0 ? LOCTEXT("ZipDone", "压缩完成！") : LOCTEXT("ZipFail", "压缩失败！"));
			Info.ExpireDuration = 5.0f;
			FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(ReturnCode == 0 ? SNotificationItem::CS_Success : SNotificationItem::CS_Fail);
			*/
		
			if (ReturnCode == 0)
			{
				// 弹出通知
				FNotificationInfo Info(FText::Format(LOCTEXT("ZipDone", "压缩完成!\n{0}"), FText::FromString(ZipFilePath)));
				Info.ExpireDuration = 8.0f;
				Info.bUseSuccessFailIcons = true;
				FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(SNotificationItem::CS_Success);

				UE_LOG(LogTemp, Log, TEXT("7z 压缩成功: %s"), *ZipFilePath);
				FPlatformProcess::ExploreFolder(*ZipFilePath);
			}
			else
			{
				FNotificationInfo Info(LOCTEXT("ZipFail", "7z 压缩失败，请检查输出日志！"));
				Info.ExpireDuration = 8.0f;
				FSlateNotificationManager::Get().AddNotification(Info)->SetCompletionState(SNotificationItem::CS_Fail);
			}	
		});
	});

	ZipProcess->Launch();
	
	FNotificationInfo StartInfo(LOCTEXT("ZipStart", "正在执行 7z 压缩..."));
	StartInfo.ExpireDuration = 5.0f;
	FSlateNotificationManager::Get().AddNotification(StartInfo)->SetCompletionState(SNotificationItem::CS_Pending);
}
#undef LOCTEXT_NAMESPACE
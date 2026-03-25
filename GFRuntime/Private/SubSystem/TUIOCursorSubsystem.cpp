#include "SubSystem/TUIOCursorSubsystem.h"

#include "GFRuntime.h"
#include "OSCServer.h"
#include "OSCMessage.h"
#include "OSCAddress.h"

void UTUIOCursorSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitOSCServer();
	UE_LOG(LogTemp, Log, TEXT("[TUIO] Subsystem 初始化完成"));
}

void UTUIOCursorSubsystem::Deinitialize()
{
	if (TUIOServer)
	{
		TUIOServer->Stop();
		TUIOServer = nullptr;
	}
	Super::Deinitialize();
}

void UTUIOCursorSubsystem::InitOSCServer()
{
	UGFConfigEngineSubsystem* ConfigSys = GEngine ? GEngine->GetEngineSubsystem<UGFConfigEngineSubsystem>() : nullptr;
	if (!ConfigSys)
	{
		UE_LOG(LogTemp, Error, TEXT("[TUIO] 无法获取 GFConfigEngineSubsystem！"));
		return;
	}

	ConfigSys->LoadConfig();

	ListenAddress = ConfigSys->GetTuioIp();
	ListenPort = ConfigSys->GetTuioPort();
	ListenAddress = "127.0.0.1";
	
	TUIOServer = UOSCManager::CreateOSCServer(ListenAddress, ListenPort, true, true, FString("TUIOReceiver"));
	if (TUIOServer)
	{
		TUIOServer->OnOscMessageReceived.AddDynamic(this, &UTUIOCursorSubsystem::OnOscMessageReceived);
		UE_LOG(LogTemp, Log, TEXT("[TUIO] Server 启动成功并绑定回调！正在监听 %s:%d"), *ListenAddress, ListenPort);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[TUIO] Server 创建失败！请检查端口是否被占用"));
	}
}

void UTUIOCursorSubsystem::OnOscMessageReceived(const FOSCMessage& Message, const FString& IpAddress, int32 Port)
{
	// === 最关键的调试日志：任何 OSC 消息都会打印 ===
	FString Address = Message.GetAddress().GetFullPath();

	// === 广播给外部 ===
	OnOSCMessageReceived.Broadcast(Message, IpAddress, Port);

	// === TUIO 专用处理 ===
	if (Address == TEXT("/tuio/2Dcur"))
	{
		FString Command;
		UOSCManager::GetString(Message, 0, Command);

		if (Command == "set")
		{
			int32 SessionId = 0;
			float x = 0.0f, y = 0.0f;

			UOSCManager::GetInt32(Message, 1, SessionId);
			UOSCManager::GetFloat(Message, 2, x);
			UOSCManager::GetFloat(Message, 3, y);

			FVector2D ScreenSize = GetViewportSize();
			float ScreenX = x * ScreenSize.X;
			float ScreenY = y * ScreenSize.Y;

			UE_LOG(LogTemp, Log, TEXT("[TUIO] 成功解析 Cursor %d → Screen (%.0f, %.0f)"), SessionId, ScreenX, ScreenY);

			OnTUIOCursorUpdated.Broadcast(SessionId, ScreenX, ScreenY);
		}
	}
}

FVector2D UTUIOCursorSubsystem::GetViewportSize() const
{
	FVector2D Size(1920.0f, 1080.0f);
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(Size);
	}
	return Size;
}

//运行时重启 Server ===
void UTUIOCursorSubsystem::RestartTUIOServer()
{
	Deinitialize();   // 先停止旧 Server
	InitOSCServer();  // 重新创建
	UE_LOG(LogTemp, Log, TEXT("[TUIO] Server 已手动重启"));
}
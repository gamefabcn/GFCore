#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "OSCManager.h"
#include "GFConfigEngineSubsystem.h"
#include "TUIOCursorSubsystem.generated.h"

// === 通用 OSC 消息事件（3 参数）===
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnOSCMessageReceived, const FOSCMessage&, Message, const FString&, IPAddress, int32, Port);

// TUIO 专用事件（保留，向后兼容）
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTUIOCursorUpdated, int32, SessionId, float, ScreenX, float, ScreenY);

UCLASS()
class UTUIOCursorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// === 通用 OSC 消息事件 ===
	UPROPERTY(BlueprintAssignable, Category = "OSC Events")
	FOnOSCMessageReceived OnOSCMessageReceived;

	// === TUIO 专用事件 ===
	UPROPERTY(BlueprintAssignable, Category = "TUIO Events")
	FOnTUIOCursorUpdated OnTUIOCursorUpdated;

	// === 新增：运行时重启 Server（商业必备）===
	UFUNCTION(BlueprintCallable, Category = "TUIO")
	void RestartTUIOServer();

private:
	UOSCServer* TUIOServer = nullptr;
	int32 ListenPort = 3333;
	FString ListenAddress;

	void InitOSCServer();
	FVector2D GetViewportSize() const;

	UFUNCTION()
	void OnOscMessageReceived(const FOSCMessage& Message, const FString& IpAddress, int32 Port);
};
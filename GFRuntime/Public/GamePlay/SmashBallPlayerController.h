// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SmashBallPlayerController.generated.h"

/**
 * 砸球类型游戏控制
 */

struct FTuioContainerStruct;
class UTUIOReceiverComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBroadcastHit, FVector2D, ScreenPos, AActor*, HitActor);
UCLASS()
class GFRUNTIME_API ASmashBallPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ASmashBallPlayerController();
	virtual void BeginPlay() override;

	void OnDebugLeftMousePressed();
	virtual void SetupInputComponent() override;
	
	// tuio 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TUIO")
	UTUIOReceiverComponent* TuioReceiver;
	
	
	/** 扩展：随时切换到UI模式（暂停菜单用） */
	UFUNCTION(BlueprintCallable, Category = "Input|Mouse")
	void SetUIMode();

	/** 切换回游戏鼠标模式（砸球主模式） */
	UFUNCTION(BlueprintCallable, Category = "Input|Mouse")
	void SetGameMouseMode();
	
protected:
	UFUNCTION()
	void OnTuioCursorAdded(const FTuioContainerStruct Data);
	
private:
	FBroadcastHit OnBroadcastHit;
	
	UFUNCTION()
	void Click(FVector2D screenPos);
};

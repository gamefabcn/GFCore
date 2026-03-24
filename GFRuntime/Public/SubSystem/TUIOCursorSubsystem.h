// Fill out your copyright notice in the Description page of Project Settings.

/**
 * 负责tuio数据解析，基于osc
 */
#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TUIOCursorSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnTUIOCursorUpdated, int32, SessionId, float, X, float, Y);
UCLASS()
class GFRUNTIME_API UTUIOCursorSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	public:
	virtual void Deinitialize() override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UPROPERTY(BlueprintAssignable, Category = "TUIO")
	FOnTUIOCursorUpdated OnTUIOCursorUpdated;
	
	
};

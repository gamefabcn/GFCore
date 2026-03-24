// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "SmashBallGameMode.generated.h"

/**
 * 砸球游戏模式，核心设置在PlayerController里，GameMode主要负责整体游戏规则和流程管理
 */
UCLASS()
class GFRUNTIME_API ASmashBallGameMode : public AGameMode
{
	GENERATED_BODY()
public:
	ASmashBallGameMode();
protected:
	virtual void BeginPlay() override;
};

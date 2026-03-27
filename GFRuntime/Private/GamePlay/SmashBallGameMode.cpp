// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePlay/SmashBallGameMode.h"

#include "GamePlay/SmashBallPlayerController.h"

ASmashBallGameMode::ASmashBallGameMode()
{
	PlayerControllerClass = ASmashBallPlayerController::StaticClass();
}

void ASmashBallGameMode::BeginPlay()
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (ASmashBallPlayerController* MyPC = Cast<ASmashBallPlayerController>(PC))
		{
			//MyPC->SetGameMouseMode();
		}
	}
}

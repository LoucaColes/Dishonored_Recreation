// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 * 
 */
UCLASS()
class DISHONORED_API ADPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> HUDClass;

	// Keep a pointer to be able to hide it
	UPROPERTY()
	TObjectPtr<UUserWidget> HUD;

	// Begin Actor interface
protected:

	virtual void BeginPlay() override;

	// End Actor interface
};

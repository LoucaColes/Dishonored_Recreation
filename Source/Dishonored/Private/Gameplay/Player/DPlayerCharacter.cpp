// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Player/DPlayerCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ADPlayerCharacter::ADPlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	bIsSprinting = false;
	walkSpeed = 600;
	sprintSpeed = 900;
}

// Called when the game starts or when spawned
void ADPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->MaxWalkSpeed = walkSpeed;
	
	if (CameraTiltCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("TiltCamera"));
		TimelineFinishedCallback.BindUFunction(this, FName{ TEXT("TiltCameraFinished") });
		CameraTiltTimeline.AddInterpFloat(CameraTiltCurve, TimelineCallback);
		CameraTiltTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}

void ADPlayerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ADPlayerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ADPlayerCharacter::DetermineCrouchOrSlide()
{
	// Check if we are falling and if so do nothing
	if (GetMovementComponent()->IsFalling()) { return;}

	// Check if we are sprinting to decide whether to crouch or not
	if (!bIsSprinting)
	{
		ToggleCrouch();
	}
	else
	{
		StartSliding();
	}
}

void ADPlayerCharacter::ToggleCrouch()
{
	if (!bIsCrouched)
	{
		Crouch();
		OnCrouchChangedDelegate.Broadcast(true);
	}
	else
	{
		UnCrouch();
		OnCrouchChangedDelegate.Broadcast(false);
	}
}

void ADPlayerCharacter::StartSliding()
{
	CameraTiltTimeline.Play();
}

void ADPlayerCharacter::StartSprinting()
{
	bIsSprinting = true;
	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->MaxWalkSpeed = sprintSpeed;
}

void ADPlayerCharacter::StopSprinting()
{
	bIsSprinting = false;
	UCharacterMovementComponent* CharacterMovementComp = GetCharacterMovement();
	CharacterMovementComp->MaxWalkSpeed = walkSpeed;
}

// Called every frame
void ADPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CameraTiltTimeline.TickTimeline(DeltaTime);
}

// Called to bind functionality to input
void ADPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADPlayerCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADPlayerCharacter::Look);

		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &ADPlayerCharacter::DetermineCrouchOrSlide);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ADPlayerCharacter::StartSprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ADPlayerCharacter::StopSprinting);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void ADPlayerCharacter::TiltCamera()
{
	float TimelineValue = CameraTiltTimeline.GetPlaybackPosition();
	float CurveFloatValue = CameraTiltCurve->GetFloatValue(TimelineValue);

	FRotator CurrentRotation = GetController()->GetControlRotation();
	FRotator NewRotation = FRotator(CurveFloatValue, CurrentRotation.Pitch, CurrentRotation.Yaw);
	GetController()->SetControlRotation(NewRotation);
}

void ADPlayerCharacter::TiltCameraFinished()
{
	//if (CameraTiltTimeline.GetTimelineDirectionEnum() == 
}

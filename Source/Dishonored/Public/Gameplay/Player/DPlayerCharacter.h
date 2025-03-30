// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/TimelineComponent.h"
#include "DPlayerCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;
class UTimelineComponent;
class FOnTimelineEvent;
struct FTimerHandle;

UENUM(BlueprintType)
enum EMovementState
{
	Walk,
	Sprint,
	Crouch,
	Slide
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCrouchChangedSignature, bool, isCrouching);

UCLASS()
class DISHONORED_API ADPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

#pragma region Components
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;
#pragma endregion


#pragma region Input
	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
#pragma endregion


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float walkSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float sprintSpeed;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Slide", meta = (AllowPrivateAccess = "true"))
	float SlideHalfHeight = 35.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement | Slide", meta = (AllowPrivateAccess = "true"))
	float SlideZOffset = 25.f;

	UPROPERTY(EditAnywhere, Category = "Movement | Slide", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* CameraTiltCurve;
	UPROPERTY(EditAnywhere, Category = "Movement | Slide", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* SlideCurve;
	

public:
	// Sets default values for this character's properties
	ADPlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void DetermineCrouchOrSlide();
	void ToggleCrouch();
	void StartSliding();
	UFUNCTION()
	void StopSliding();

	void StartSprinting();
	void StopSprinting();

	
	UFUNCTION()
	void TiltCamera();

	UFUNCTION()
	void SlidePlayer();


private:
	FTimerHandle SlideTimerHandle;
	float StandingHalfHeight;
	EMovementState MovementState;
	FTimeline CameraTiltTimeline;
	FTimeline SlideTimeline;
	float StandingZOffset;

	bool ShouldConsiderMoveInput();
public:	
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	UPROPERTY(BlueprintAssignable)
	FOnCrouchChangedSignature OnCrouchChangedDelegate;
};

#include "Character/RPBaseCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Animation/AnimMontage.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"
#include "RPTestItemActor.h"

ARPBaseCharacter::ARPBaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// SceneComponent 생성
	Sound = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	Sound->SetupAttachment(GetMesh());

	// SoundComponent 생성
	SC_FootSteps = CreateDefaultSubobject<UAudioComponent>(TEXT("MySound"));
	SC_FootSteps->SetupAttachment(Sound);
	SC_FootSteps->bAutoActivate = false;
}

void ARPBaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}
void ARPBaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARPBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARPBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ARPBaseCharacter, PlayerHudRef);
	DOREPLIFETIME(ARPBaseCharacter, ControlRotationRep);
	DOREPLIFETIME(ARPBaseCharacter, LeanValue);
	DOREPLIFETIME(ARPBaseCharacter, LeanLeftArm);
	DOREPLIFETIME(ARPBaseCharacter, LeanRightArm);
}


void ARPBaseCharacter::PlayMontage(UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition)
{
	if (GetMesh() && GetMesh()->GetAnimInstance() && MontageToPlay)
	{
		GetMesh()->GetAnimInstance()->Montage_Play(MontageToPlay, PlayRate, EMontagePlayReturnType::Duration, StartingPosition);
	}
}

void ARPBaseCharacter::ServerPlayMontage_Implementation(UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition)
{
	MulticastPlayMontage(MontageToPlay, PlayRate, StartingPosition);
}

bool ARPBaseCharacter::ServerPlayMontage_Validate(UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition)
{
	return true;
}

void ARPBaseCharacter::MulticastPlayMontage_Implementation(UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition)
{
	PlayMontage(MontageToPlay, PlayRate, StartingPosition);
}

void ARPBaseCharacter::ServerPlayerSpeed_Implementation(float NewSpeed)
{
	MulticastSpeed(NewSpeed);
}

bool ARPBaseCharacter::ServerPlayerSpeed_Validate(float NewSpeed)
{
	return NewSpeed >= 0.0f && NewSpeed <= 1500.0f;
}

void ARPBaseCharacter::MulticastSpeed_Implementation(float NewSpeed)
{
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->MaxWalkSpeed = NewSpeed;
	}
}

void ARPBaseCharacter::Sliding(bool bEnable)
{
	// IfThenElse 노드: bEnable이 true일 때만 로직 실행
	if (bEnable)
	{
		// DoOnce 매크로: bCanSlide가 true일 때만 로직 실행
		if (bCanSlide)
		{
			bCanSlide = false;

			// ServerPlayMontage 호출
			if (SlideMontage)
			{
				ServerPlayMontage(SlideMontage, 1.0f, 0.0f);
			}

			// Delay 노드: 0.2초 대기
			FTimerHandle SlideTimerHandle;
			float DelayDuration = 0.2f;
			GetWorldTimerManager().SetTimer(SlideTimerHandle, this, &ARPBaseCharacter::OnSlideCompleted, DelayDuration, false);
		}
	}
}

bool ARPBaseCharacter::CheckForSlide()
{
	// 1. 속도 확인
	bool bIsFastEnough = GetVelocity().Size() > SlideSpeed;
	
	// 2. 지면에 붙어있는지 확인.
	bool bIsOnGround = !GetCharacterMovement()->IsFalling();

	return bIsFastEnough && IsSprinting && IsSprintingForward && bIsOnGround;
}

bool ARPBaseCharacter::CheckCrouching()
{
	return !GetCharacterMovement()->IsCrouching();
}

bool ARPBaseCharacter::CheckFalling()
{
	return GetCharacterMovement()->IsFalling();
}

void ARPBaseCharacter::OnSlideCompleted()
{   
	// DoOnce 매크로의 Reset 기능
	bCanSlide = true;
}





// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "RPBaseCharacter.generated.h"

UCLASS()
class RESETPROTOCOL_API ARPBaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ARPBaseCharacter();

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

public:
	// 일반적인 애니메이션 몽타주 재생 함수
	void PlayMontage(UAnimMontage* MontageToPlay, float PlayRate = 1.0f, float StartingPosition = 0.0f);
	
	// 서버 함수: 클라이언트가 서버에 애니메이션 재생을 요청
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Replication")
	void ServerPlayMontage(UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition);

	// 멀티캐스트 함수: 서버가 모든 클라이언트에 애니메이션 재생을 동기화
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void MulticastPlayMontage(UAnimMontage* MontageToPlay, float PlayRate, float StartingPosition);

	// 서버 함수: 클라이언트가 서버에 속도 변경을 요청
	UFUNCTION(Server, Reliable, WithValidation, BlueprintCallable, Category = "Replication")
	void ServerPlayerSpeed(float NewSpeed);

	// 멀티캐스트 함수: 서버가 모든 클라이언트에 속도 변경을 동기화
	UFUNCTION(NetMulticast, Reliable, Category = "Replication")
	void MulticastSpeed(float NewSpeed);

	// 슬라이딩 기능을 시작하는 함수
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Sliding(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "Slide")
	bool CheckForSlide();

	UFUNCTION(BlueprintCallable, Category = "Crouch")
	bool CheckCrouching();

	UFUNCTION(BlueprintCallable, Category = "Crouch")
	bool CheckFalling();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* Sound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UAudioComponent* SC_FootSteps;

public:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadWrite, Category = "Reference")
	UUserWidget* PlayerHudRef;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Replication")
	FRotator ControlRotationRep;

	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Leaning")
	float LeanValue;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Leaning")
	float LeanLeftArm;
	UPROPERTY(EditAnywhere, Replicated, BlueprintReadWrite, Category = "Leaning")
	float LeanRightArm;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Leaning")
	bool IsLeaning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zoom")
	bool Zoomed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Land")
	bool Landed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	bool IsSprintingForward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	bool IsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Slide")
	float SlideSpeed;

private:
	// DoOnce 매크로를 대체할 변수
	UPROPERTY()
	bool bCanSlide = true;

	// 슬라이딩 몽타주를 저장할 변수
	UPROPERTY(EditDefaultsOnly, Category = "Montage")
	UAnimMontage* SlideMontage;

	// 지연 시간 후 실행될 함수
	void OnSlideCompleted();






};
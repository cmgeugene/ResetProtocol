#pragma once

#include "CoreMinimal.h"
#include "Character/RPBaseCharacter.h"
#include "Component/RPInteractorComponent.h"
#include "RPPlayerCharacter.generated.h"

UCLASS()
class RESETPROTOCOL_API ARPPlayerCharacter : public ARPBaseCharacter
{
	GENERATED_BODY()

public:
	ARPPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	FHitResult& GetHitResult() { return InteractHitResult; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Component")
	URPInteractorComponent* InteractorComponent;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Interact")
	FHitResult InteractHitResult;
	
};

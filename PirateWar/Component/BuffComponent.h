#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIRATEWAR_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffComponent();
	friend class APirateCharacter;
	void Heal(float HealAmount, float HealingTime);
	void HealRampUp(float DeltaTime);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class APirateCharacter* Character;

	bool bHealing = false;
	float HealingRate = 0.f;
	float AmountToHeal = 0.f;
};

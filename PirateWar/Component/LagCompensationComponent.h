#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PirateWar/Character/PirateCharacter.h"
#include "LagCompensationComponent.generated.h"

USTRUCT(BlueprintType)
struct FBoxInformation
{
	GENERATED_BODY()

	UPROPERTY()
	FVector Location;

	UPROPERTY()
	FRotator Rotation;

	UPROPERTY()
	FVector BoxExtent; 
};

USTRUCT(BlueprintType)
struct FFramePackage
{
	GENERATED_BODY()

	UPROPERTY()
	float Time;

	UPROPERTY()
	TMap<FName, FBoxInformation> HitBoxInfo;

	UPROPERTY()
	APirateCharacter* Character;
};

USTRUCT(BlueprintType)
struct FServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	bool bHitConfirmed;

	UPROPERTY()
	bool bHeadShot;
};

USTRUCT(BlueprintType)
struct FShotgunServerSideRewindResult
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<APirateCharacter*, uint32> HeadShots;

	UPROPERTY()
	TMap<APirateCharacter*, uint32> BodyShots;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIRATEWAR_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULagCompensationComponent();
	friend class APirateCharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void ShowFramePackage(const FFramePackage& Package, FColor Color);
	
	FServerSideRewindResult ServerSideRewind(class APirateCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime
	);

	FServerSideRewindResult ProjectileServerSideRewind(
		class APirateCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ServerScoreRequest(
		APirateCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation,
		float HitTime,
		class AWeapon* DamageCauser
	);

	UFUNCTION(Server, Reliable)
	void ProjectileServerScoreRequest(
		APirateCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
	
	FShotgunServerSideRewindResult ShotgunServerSideRewind(
		const TArray<APirateCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);

	UFUNCTION(Server, Reliable)
	void ShotgunServerScoreRequest(
		const TArray<APirateCharacter*>& HitCharacters,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations,
		float HitTime
	);
	
protected:
	virtual void BeginPlay() override;
	virtual void SaveFramePackage(FFramePackage& Package);
	
	FFramePackage InterpBetweenFrames(
		const FFramePackage& OlderFrame,
		const FFramePackage& YoungerFrame,
		float HitTime);
	
	FServerSideRewindResult ConfirmHit(
		const FFramePackage&	Package,
		APirateCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize& HitLocation
	);

	FServerSideRewindResult ProjectileConfirmHit(
		const FFramePackage& Package,
		APirateCharacter* HitCharacter,
		const FVector_NetQuantize& TraceStart,
		const FVector_NetQuantize100& InitialVelocity,
		float HitTime
	);
	
	FShotgunServerSideRewindResult ShotgunConfirmHit(
		const TArray<FFramePackage>& FramePackages,
		const FVector_NetQuantize& TraceStart,
		const TArray<FVector_NetQuantize>& HitLocations
	);
	
	void CacheBoxPositions(APirateCharacter* HitCharacter, FFramePackage& OutFramePackage);
	void MoveBoxes(APirateCharacter* HitCharacter, const FFramePackage& Package);
	void ResetHitBoxes(APirateCharacter* HitCharacter, const FFramePackage& Package);
	void EnableCharacterMeshCollision(APirateCharacter* HitCharacter, ECollisionEnabled::Type CollisionEnabled);
	void SaveFramePackage();
	FFramePackage GetFrameToCheck(APirateCharacter* HitCharacter, float HitTime);

	
	
private:
	UPROPERTY()
	APirateCharacter* Character;

	UPROPERTY()
	class APiratePlayerController* Controller;

	TDoubleLinkedList<FFramePackage> FrameHistory;

	UPROPERTY(EditAnywhere)
	float MaxRecordTime = 4.f;
};

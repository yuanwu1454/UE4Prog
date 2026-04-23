// Nothing ...

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "KingWeaponBase.generated.h"

class UBoxComponent;

UCLASS()
class MYGAME_API AKingWeaponBase : public AWeaponBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AKingWeaponBase();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapons")
	UStaticMeshComponent* WeaponMesh;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapons")
	UBoxComponent* WeaponCollisionBox;
public:
	FORCEINLINE UBoxComponent* GetWeaponCollisionBox() const {return WeaponCollisionBox;}
	
};

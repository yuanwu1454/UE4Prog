// Nothing ...


#include "KingHeroAnimInstance.h"

#include "King/KingCharacter.h"

void UKingHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (OwningCharacter)
	{
		OwningKingCharacter = Cast<AKingCharacter>(OwningCharacter);
	}
}

void UKingHeroAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	if (bHasAcceleration)
	{
		IdleElpasedTime = 0.f;
		bShouldEnterRelaxState = false;
	}else
	{

		IdleElpasedTime += DeltaSeconds;
		bShouldEnterRelaxState = (IdleElpasedTime>=EnterRelaxStateThreshold);
	}
}
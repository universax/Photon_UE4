// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LoadBalancing-cpp/inc/Client.h"
#include "Components/ActorComponent.h"
#include "ListnerBase.h"
#include "PhotonListnerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTestDelegate);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PHOTON_API UPhotonListnerComponent : public UActorComponent, public ListnerBase
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPhotonListnerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


public:
	UPROPERTY(BlueprintAssignable, Category = "Debug")
	FTestDelegate delegateTest;
	
	void DelegateTest();
};

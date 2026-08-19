// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/ATAWorldItem.h"

// Sets default values
AATAWorldItem::AATAWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AATAWorldItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AATAWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


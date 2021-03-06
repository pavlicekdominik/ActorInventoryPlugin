// Copyright Dominik Pavlicek 2022. All Rights Reserved.

#include "Components/ActorInventoryComponent.h"

#include "Definitions/InventoryItem.h"
#include "Helpers/ActorInventoryBPFLibrary.h"
#include "Helpers/ActorInventoryPluginLog.h"

UActorInventoryComponent::UActorInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UActorInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	InventoryManager = UActorInventoryBPFLibrary::GetInventoryManager(this);
}

void UActorInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UActorInventoryComponent::AddItemToInventory(UInventoryItem* Item)
{
	if (IsItemInInventory(Item))
	{
		return;
	}
	
	InventoryItems.Emplace(Item);
}

void UActorInventoryComponent::AddItemsToInventory(const TArray<UInventoryItem*>& ListOfItems)
{
	for (UInventoryItem* Itr : ListOfItems)
	{
		AddItemToInventory(Itr);
	}
}

bool UActorInventoryComponent::IsItemInInventory(const UInventoryItem* Item) const
{
	return InventoryItems.Contains(Item);
}

void UActorInventoryComponent::LoadInventoryContent(const UDataTable* SourceTable)
{
	if (SourceTable && SourceTable->RowStruct)
	{
		const UScriptStruct* InventoryRowStruct = SourceTable->GetRowStruct();
		if (InventoryRowStruct->IsChildOf(FInventoryItemData::StaticStruct()))
		{
			static const FString ContextString(TEXT("ContextString"));
			for (const FName Itr : SourceTable->GetRowNames())
			{
				const FInventoryItemData* Row = SourceTable->FindRow<FInventoryItemData>(Itr, ContextString);
				
				AInvP_LOG(Warning, TEXT("%s"), *ContextString)
				
				if (Row)
				{
					UInventoryItem* NewItem = NewObject<UInventoryItem>(UInventoryItem::StaticClass());

					if (NewItem)
					{
						NewItem->SetItem(*Row);
						AddItemToInventory(NewItem);
					}
				}
			}
		} 
	}
}

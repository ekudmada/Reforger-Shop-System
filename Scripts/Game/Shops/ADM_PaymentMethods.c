class ADM_PaymentMethodBase: ScriptAndConfig
{
	bool CheckPayment(IEntity player) { return false; }
	bool CollectPayment(IEntity player) { return false; }
	bool ReturnPayment(IEntity player) { return false; }
}

[BaseContainerProps()]
class ADM_PaymentMethodItem: ADM_PaymentMethodBase
{
	[Attribute(defvalue: "", desc: "Prefab of item", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_ItemPrefab;
	
	[Attribute(defvalue: "1", desc: "Number of item to sell", params: "1 inf")]
	protected int m_ItemQuantity;
	
	ResourceName GetItemPrefab()
	{
		return m_ItemPrefab;
	}
	
	int GetItemQuantity()
	{
		return m_ItemQuantity;
	}
	
	array<IEntity> GetPaymentItemsInInventory(SCR_InventoryStorageManagerComponent inventory)
	{
		if (!inventory)
			return null;
		
		array<IEntity> allInventoryItems = {};
		inventory.GetItems(allInventoryItems);
		
		array<IEntity> desiredItems = {};
		foreach (IEntity inventoryItem : allInventoryItems)
		{
			EntityPrefabData prefabData = inventoryItem.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			
			if (prefabName == m_ItemPrefab) desiredItems.Insert(inventoryItem);	
		}
		
		return desiredItems;
	}
	
	override bool CheckPayment(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		array<IEntity> paymentItems = this.GetPaymentItemsInInventory(inventory);
		if (!paymentItems) return false;
		
		bool haveEnough = paymentItems.Count() >= m_ItemQuantity;
		return haveEnough;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override bool CollectPayment(IEntity player)
	{
		//check if player has the desired payment
		if (!CheckPayment(player)) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		// Keep track of what is removed, as it is removed.
		// cant use ReturnPayment() b/c then we would give EVERYTHING back,
		// if we fail in the middle the player would get something for free
		// vs. only returning what was taken.
		array<ResourceName> removedItems = {};
		array<bool> didRemoveItems = {};
		array<IEntity> paymentItems = this.GetPaymentItemsInInventory(inventory);
		foreach (IEntity item : paymentItems)
		{
			EntityPrefabData prefabData = item.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			
			bool didRemoveItem = inventory.TryDeleteItem(item, null);
			didRemoveItems.Insert(didRemoveItem);
			
			if (didRemoveItem) removedItems.Insert(prefabName);
			if (didRemoveItems.Count() == m_ItemQuantity)
				break;
		}
		
		if (didRemoveItems.Contains(false))
		{
			Print("Error! Couldn't remove items for payment. Returning items already taken", LogLevel.ERROR);
			
			foreach (ResourceName returnItemPrefab : removedItems)
			{
				IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(returnItemPrefab));
				bool inserted = ADM_ItemShop.InsertAutoEquipItem(inventory, item);
			}
			
			return false;
		}
			
		return true;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override bool ReturnPayment(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		for (int i = 0; i < m_ItemQuantity; i++)
		{
			IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_ItemPrefab));
			bool insertedItem = ADM_ItemShop.InsertAutoEquipItem(inventory, item);
			return insertedItem;
		}
			
		return true;
	}
}

[BaseContainerProps()]
class ADM_PaymentMethodCurrency: ADM_PaymentMethodBase
{
	[Attribute()]
	int m_Quantity;
	
	int GetQuantity()
	{
		return m_Quantity;
	}
	
	override bool CheckPayment(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		int totalCurrency = ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventory);
		if (totalCurrency < m_Quantity) return false;
		
		return true;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override bool CollectPayment(IEntity player)
	{
		//check if player has the desired payment
		if (!CheckPayment(player)) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		bool didRemoveCurrency = ADM_CurrencyComponent.RemoveCurrencyFromInventory(inventory, m_Quantity);
		return didRemoveCurrency;
	}
}
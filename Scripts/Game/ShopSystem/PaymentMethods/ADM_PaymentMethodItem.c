class ADM_PaymentMethodItem: ADM_PaymentMethodBase
{
	[Attribute(defvalue: "", desc: "Prefab of item", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_ItemPrefab;
	
	[Attribute(defvalue: "1", desc: "Number of item for payment", params: "1 inf")]
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
			
			if (prefabName == m_ItemPrefab) 
				desiredItems.Insert(inventoryItem);	
		}
		
		return desiredItems;
	}
	
	override bool CheckPayment(IEntity player, int quantity = 1)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		array<IEntity> paymentItems = this.GetPaymentItemsInInventory(inventory);
		if (!paymentItems) 
			return false;
		
		return (paymentItems.Count() >= m_ItemQuantity*quantity);
	}
	
	override bool CollectPayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer() || !CheckPayment(player)) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		// Keep track of what is removed, as it is removed.
		// cant use ReturnPayment() b/c then we would give EVERYTHING back,
		// if we fail in the middle of removing payments the player would get something for free.
		// versus only returning what was taken.
		array<ResourceName> removedItems = {};
		array<bool> didRemoveItems = {};
		array<IEntity> paymentItems = this.GetPaymentItemsInInventory(inventory);
		foreach (IEntity item : paymentItems)
		{
			EntityPrefabData prefabData = item.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			
			// TODO: check if the payment has storages or attachments and drop them on the ground
			// things like magazines, sights, etc.
			
			bool didRemoveItem = inventory.TryDeleteItem(item, null);
			didRemoveItems.Insert(didRemoveItem);
			
			if (didRemoveItem) 
				removedItems.Insert(prefabName);
			
			if (didRemoveItems.Count() == m_ItemQuantity*quantity) 
				break;
		}
		
		if (didRemoveItems.Contains(false) || didRemoveItems.Count() < m_ItemQuantity*quantity)
		{
			Print("Error! Couldn't remove items for payment. Returning items already taken", LogLevel.ERROR);
			
			foreach (ResourceName returnItemPrefab : removedItems)
			{
				IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(returnItemPrefab));
				bool inserted = ADM_Utils.InsertAutoEquipItem(inventory, item);
				if (!inserted) 
					Print("Error! Couldn't return an item for payment.", LogLevel.ERROR); //TODO: not sure best way to handle this if it occurs, maybe drop it to ground?
			}
			
			return false;
		}
			
		return true;
	}
	
	override bool DistributePayment(IEntity player, int quantity = 1)
	{
		if (!Replication.IsServer()) 
			return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) 
			return false;
		
		bool returned = true;
		for (int i = 0; i < m_ItemQuantity * quantity; i++)
		{
			IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_ItemPrefab));
			bool insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
			if (!insertedItem) 
				returned = false;
		}
			
		return returned;
	}
	
	override string GetDisplayName(int quantity = 1)
	{
		return string.Format("%1 x%2", ADM_Utils.GetPrefabDisplayName(m_ItemPrefab), Math.AbsInt(m_ItemQuantity * quantity));
	}
	
	override ResourceName GetDisplayEntity()
	{
		return m_ItemPrefab;
	}
	
	override bool Equals(ADM_PaymentMethodBase other)
	{
		ADM_PaymentMethodItem otherItem = ADM_PaymentMethodItem.Cast(other);
		if (!otherItem)
			return false;
		
		if (otherItem.GetItemPrefab() != this.GetItemPrefab())
			return false;
		
		return true;
	}
	
	override bool Add(ADM_PaymentMethodBase other, int quantity = 1)
	{
		if (!this.Equals(other))
			return false;
		
		ADM_PaymentMethodItem otherItem = ADM_PaymentMethodItem.Cast(other);
		if (!otherItem)
			return false;
				
		this.m_ItemQuantity += otherItem.m_ItemQuantity * quantity;
		
		return true;
	}
}
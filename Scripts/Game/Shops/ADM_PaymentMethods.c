class ADM_PaymentMethodBase: ScriptAndConfig
{
	bool CheckPayment(IEntity player) { return false; };
	bool CollectPayment(IEntity player) { return false; };
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
	
	array<IEntity> GetPaymentItemsFromInventory(SCR_InventoryStorageManagerComponent inventory)
	{
		if (!inventory)
			return null;
		
		array<IEntity> items = {};
		inventory.GetItems(items);
		
		array<IEntity> desiredItems = {};
		foreach (IEntity item : items)
		{
			EntityPrefabData prefabData = item.GetPrefabData();
			ResourceName prefabName = prefabData.GetPrefabName();
			
			if (prefabName == m_ItemPrefab)
				desiredItems.Insert(item);	
		}
		
		return desiredItems;
	}
	
	override bool CheckPayment(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		array<IEntity> paymentItems = this.GetPaymentItemsFromInventory(inventory);
		if (!paymentItems)
			return false;
		
		return (paymentItems.Count() >= m_ItemQuantity);
	}
	
	override bool CollectPayment(IEntity player)
	{
		//check if player has the desired payment
		if (!CheckPayment(player)) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		array<bool> didRemoveItems = {};
		array<IEntity> paymentItems = this.GetPaymentItemsFromInventory(inventory);
		foreach (IEntity item : paymentItems)
		{
			bool didRemoveItem = inventory.TryDeleteItem(item, null);
			didRemoveItems.Insert(didRemoveItem);
						
			if (didRemoveItems.Count() == m_ItemQuantity)
				break;
		}
		
		if (didRemoveItems.Contains(false))
		{
			Print("Error! Couldn't remove items for payment.", LogLevel.ERROR);
			return false;
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
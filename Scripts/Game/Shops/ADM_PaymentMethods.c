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
	
	override bool CheckPayment(IEntity player)
	{
		//Print("check payment for item payment");
		return true;
	}
	
	override bool CollectPayment(IEntity player)
	{
		//Print("collect payment ItemPayment");
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
		//essentially the same as ADM_PaymentMethodItem but account for all currency types
		
		//check if player has the desired payment
		if (!CheckPayment(player)) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		//keep track of everything we have removed, if a single one fails we should give player everything back
		//and return false; (though CheckPayment should theoretically prevent this, who knows what could go wrong)
		//TryRemoveItemFromInventory
		//get all items in inventory which match our payment
		
		
		return true;
	}
}
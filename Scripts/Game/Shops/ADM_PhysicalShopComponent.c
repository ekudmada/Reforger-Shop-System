class ADM_PaymentMethod: ScriptAndConfig
{
	bool CheckPayment(IEntity player) { return false; };
	bool CollectPayment(IEntity player) { return false; };
}

[BaseContainerProps()]
class ADM_PaymentMethodItem: ADM_PaymentMethod
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
};

[BaseContainerProps()]
class ADM_PaymentMethodCurrency: ADM_PaymentMethod
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
};

class ADM_PhysicalShopComponentClass: ScriptComponentClass
{
};

//! A brief explanation of what this component does.
//! The explanation can be spread across multiple lines.
//! This should help with quickly understanding the script's purpose.
class ADM_PhysicalShopComponent: ScriptComponent
{
	[Attribute(defvalue: "", desc: "Prefab of item to sell", uiwidget: UIWidgets.Object, params: "et", category: "Physical Shop")]
	protected ref ADM_PaymentMethodItem m_ItemForSale;
	
	[Attribute(category: "Physical Shop")]
	protected ref array<ref ADM_PaymentMethod> m_RequiredPayment;
	
	[Attribute(defvalue: "0", desc: "How many seconds for item to respawn after it has been purchased. (-1 for no respawning)", uiwidget: UIWidgets.EditBox, params: "et", category: "Physical Shop")]
	protected float m_RespawnTime;
	
	[Attribute(defvalue: "1", desc: "If this item is equipable, and there is space available to equip it, then the item will auto-equip.", uiwidget: UIWidgets.CheckBox, params: "et", category: "Physical Shop")]
	protected bool m_AutoEquip;
	
	[Attribute(defvalue: "0", desc: "If true then items will drop on ground if player cannot equip item or place in inventory. If false the sale will not be allowed.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_AllowSaleWithFullInventory;
	
	//------------------------------------------------------------------------------------------------
	void UpdateMesh(IEntity owner)
	{
		if (!m_ItemForSale) return;
		
		ResourceName modelPath;
		string remapPath;
		
		bool foundModelPath = SCR_Global.GetModelAndRemapFromResource(m_ItemForSale.GetItemPrefab(), modelPath, remapPath);
		if (!foundModelPath) return;
		
		Resource resource = Resource.Load(modelPath);
		if (!resource.IsValid()) return;
		
		MeshObject model = resource.GetResource().ToMeshObject();
		owner.SetObject(model, string.Empty);
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ADM_PaymentMethod> GetRequiredPayment()
	{
		return m_RequiredPayment;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPaymentOnlyCurrency()
	{
		if (m_RequiredPayment.Count() != 1)	return false;
		if (m_RequiredPayment[0].ClassName() != "ADM_PaymentMethodCurrency") return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	ADM_PaymentMethodItem GetItemForSale()
	{
		return m_ItemForSale;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRespawnTime()
	{
		return m_RespawnTime;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanPurchase(IEntity player)
	{
		bool canPurchase = true;
		foreach (ADM_PaymentMethod payment : m_RequiredPayment)
		{
			canPurchase = payment.CheckPayment(player);
			if (!canPurchase) break;
		}
		
		return canPurchase;
	}
	
	//------------------------------------------------------------------------------------------------
	void PurchaseItemAction()
	{
		Rpc(RpcAsk_PurchaseItem);
	}
	
	//------------------------------------------------------------------------------------------------
	void ViewPayment()
	{
		Print("open dialog view payment");
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_PurchaseItem(int playerId)
	{
		Print(playerId.ToString() + ": requested to buy item " + m_ItemForSale.GetItemPrefab());
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!player) {
			Rpc(RpcDo_PurchaseItem, "Couldn't find player entity");
			return;
		}
		
		Rpc(RpcDo_PurchaseItem, "success");
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_PurchaseItem(string message)
	{
		Print(message);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		UpdateMesh(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	void ADM_PhysicalShopComponent(IEntityComponentSource src, IEntity ent, IEntity parent) 
	{
		
	}
};
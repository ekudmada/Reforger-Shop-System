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
		return false;
	}
	
	override bool CollectPayment(IEntity player)
	{
		//Print("collect payment ItemPayment");
		return false;
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
		//Print("check payment for currency");
		return false;
	}
	
	override bool CollectPayment(IEntity player)
	{
		//Print("collect paymenet currency");
		return false;
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
		Rpc(PurchaseItem);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void PurchaseItem(int playerId)
	{
		// Check if player has m_Payment
		// If they do, check if they have space to store the purchased item m_ItemForSale
		// If they have the storage, continue
		// If they don't have storage but m_AllowSaleWithFullInventory then continue
		// Remove m_Paymentkm
		// Then give them the items
		Print(playerId.ToString() + ": requested to buy item " + m_ItemForSale.GetItemPrefab());
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
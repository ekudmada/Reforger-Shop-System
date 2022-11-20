class ADM_PhysicalShopBase: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab of thing to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_Prefab;
	
	ResourceName GetPrefab()
	{
		return m_Prefab;
	}
	
	bool CanDeliver(IEntity player) { return false; }
	bool Deliver(IEntity player) { return false; }
	bool CanRespawn(ADM_PhysicalShopComponent shop) { return false; }
}

[BaseContainerProps()]
class ADM_ItemShop: ADM_PhysicalShopBase
{
	[Attribute(defvalue: "0", desc: "If true then shop will spawn in place of the shop if the player cannot equip item or place in inventory. If false the sale will not be allowed.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_AllowSaleWithFullInventory;
	
	override bool CanDeliver(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		//TODO: Check if player can fit the item in their inventory
		
		
		return true;
	}
	
	override bool Deliver(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player);
		if (!canDeliver) return false;
		
		// give item/weapon/magazine/clothing to player	
		IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_Prefab));
		bool insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
		
		EStoragePurpose purpose = EStoragePurpose.PURPOSE_ANY;
		if (item.FindComponent(WeaponComponent))
			purpose = EStoragePurpose.PURPOSE_WEAPON_PROXY;

		if (item.FindComponent(BaseLoadoutClothComponent))
			purpose = EStoragePurpose.PURPOSE_LOADOUT_PROXY;
		
		if (item.FindComponent(SCR_GadgetComponent))
			purpose = EStoragePurpose.PURPOSE_GADGET_PROXY;
		
		BaseInventoryStorageComponent storageComp = inventory.FindStorageForItem(item, purpose);
		if (storageComp)
		{
			//TODO: drop current item in slot or put in inventory
			inventory.EquipAny(storageComp, item, -1, null);
		}	
		
		return insertedItem;		
	}
	
	override bool CanRespawn(ADM_PhysicalShopComponent shop)
	{
		return true;
	}
}
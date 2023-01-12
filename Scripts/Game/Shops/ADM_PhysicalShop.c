class ADM_PhysicalShopBase: ScriptAndConfig
{
	[Attribute(defvalue: "", desc: "Prefab of thing to sell", uiwidget: UIWidgets.ResourceNamePicker, params: "et")]
	protected ResourceName m_Prefab;
	
	ResourceName GetPrefab()
	{
		return m_Prefab;
	}
	
	bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
	bool Deliver(IEntity player, ADM_PhysicalShopComponent shop) { return false; }
	bool CanRespawn(ADM_PhysicalShopComponent shop) { return false; }
}

[BaseContainerProps()]
class ADM_ItemShop: ADM_PhysicalShopBase
{
	[Attribute(defvalue: "0", desc: "If true then shop will spawn in place of the shop if the player cannot equip item or place in inventory. If false the sale will not be allowed.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_AllowSaleWithFullInventory;
	
	bool CanFitItemInInventory(IEntity player)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		SCR_CharacterInventoryStorageComponent charInventory = inventory.GetCharacterStorage();
		if (!charInventory)
			return false;
		
		array<SCR_UniversalInventoryStorageComponent> inventoryStorages = {};
		charInventory.GetStorages(inventoryStorages); // this isn't including my backpack or weapon storage. Why?
		
		array<bool> checks = {};
		foreach (SCR_UniversalInventoryStorageComponent storage : inventoryStorages)
		{
			float weight = 0;
			float volume = 0;
			
			// check volume
			bool weightFits = (storage.GetWeight() + weight) <= storage.GetMaxWeight();
			//Print(string.Format("%1N out of %2N", storage.GetWeight() + weight, storage.GetMaxWeight()));
			
			// check mass
			bool volumeFits = (storage.GetVolume2() + volume) <= storage.GetMaxVolume();
			//Print(string.Format("%1m^3 out of %2m^3", storage.GetVolume2() + volume, storage.GetMaxVolume()));
			
			checks.Insert(weightFits && volumeFits);
		}
		
		// if any of the storages can fit it, then we are good
		return checks.Contains(true);
	}
	
	override bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		if (!m_AllowSaleWithFullInventory)
			return CanFitItemInInventory(player);
		
		return true;
	}
	
	static bool InsertAutoEquipItem(SCR_InventoryStorageManagerComponent inventory, IEntity item)
	{
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
			bool equipped = inventory.EquipAny(storageComp, item, -1, null);
		}
		
		return insertedItem;
	}
	
	override bool Deliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop);
		if (!canDeliver) return false;
		
		// give item/weapon/magazine/clothing to player	
		IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_Prefab));		
		if (m_AllowSaleWithFullInventory && !CanFitItemInInventory(player))
		{
			// Move item to ground location of shop
			vector shopMat[4];
			shop.GetOwner().GetTransform(shopMat);
			item.SetTransform(shopMat);
			
			return true;
		} else {
			return InsertAutoEquipItem(inventory, item);	
		}
	}
	
	override bool CanRespawn(ADM_PhysicalShopComponent shop)
	{
		//TODO: Don't respawn unless no other items are in the way
				
		return true;
	}
}

[BaseContainerProps()]
class ADM_VehicleShop: ADM_PhysicalShopBase
{
	override bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		return true;
	}
	
	override bool Deliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop);
		if (!canDeliver) return false;
		
		// spawn vehicle
		
		
		return true;
	}
	
	override bool CanRespawn(ADM_PhysicalShopComponent shop)
	{
		//TODO: Don't respawn the shop vehicle unless no other vehicles are in the way
				
		return true;
	}
}
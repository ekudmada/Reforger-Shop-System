[BaseContainerProps()]
class ADM_MerchandiseItem: ADM_MerchandiseBase
{
	[Attribute(defvalue: "0", desc: "If true then shop will spawn in place of the shop if the player cannot equip item or place in inventory. If false the sale will not be allowed.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_AllowSaleWithFullInventory;
	
	bool CanFitItemInInventory(IEntity player)
	{
		/*SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory)
			return false;
		
		SCR_CharacterInventoryStorageComponent charInventory = inventory.GetCharacterStorage();
		if (!charInventory)
			return false;
		
		array<SCR_UniversalInventoryStorageComponent> inventoryStorages = {};
		charInventory.GetStorages(inventoryStorages); // this isn't including my backpack or weapon storage. Why?
		
		array<bool> checks = {true};
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
		}*/
		
		// if any of the storages can fit it, then we are good
		return true;
	}
	
	override bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		if (!m_AllowSaleWithFullInventory) return CanFitItemInInventory(player);
		
		return true;
	}
	
	override bool Deliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		if (RplSession.Mode() == RplMode.Client) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop);
		if (!canDeliver) return false;
		
		// give item/weapon/magazine/clothing to player	
		IEntity item = GetGame().SpawnEntityPrefab(m_PrefabResource);	
		bool putInInventory = ADM_Utils.InsertAutoEquipItem(inventory, item);
		
		// Move item to location of shop if we can't fit in inventory and we allow sale with full inventory
		if (m_AllowSaleWithFullInventory && !putInInventory)
		{
			vector shopMat[4];
			shop.GetOwner().GetTransform(shopMat);
			item.SetTransform(shopMat);
		}
		
		// If we cant do with both conditions false, delete it
		if (!m_AllowSaleWithFullInventory && !putInInventory)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(item);
			return false;
		}
		
		return true;
	}
	
	override bool CanRespawn(ADM_PhysicalShopComponent shop) { return true; }
}
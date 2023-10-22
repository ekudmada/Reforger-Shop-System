[BaseContainerProps()]
class ADM_MerchandiseItem: ADM_MerchandiseType
{
	[Attribute(defvalue: "0", desc: "If true then shop will spawn in place of the shop if the player cannot equip item or place in inventory. If false the sale will not be allowed.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_AllowSaleWithFullInventory;
	
	bool CanFitItemInInventory(IEntity player, int quantity = 1)
	{
		//TODO
		return true;
	}
	
	override bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null)
	{
		if (!m_AllowSaleWithFullInventory) 
			return CanFitItemInInventory(player);
		
		return true;
	}
	
	override bool Deliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1)
	{
		if (!Replication.IsServer()) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop, quantity);
		if (!canDeliver) return false;
		
		array<IEntity> deliveredItems = {};
		for (int i = 0; i < quantity; i++)
		{
			// give item/weapon/magazine/clothing to player	
			IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_sPrefab));	
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
			} else {
				deliveredItems.Insert(item);
			}
		}
		
		if (deliveredItems.Count() != quantity)
		{
			foreach (IEntity item : deliveredItems)
			{
				SCR_EntityHelper.DeleteEntityAndChildren(item);
			}
			
			return false;
		}
		
		return true;
	}
	
	override bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null) 
	{ 
		return true; 
	}
}
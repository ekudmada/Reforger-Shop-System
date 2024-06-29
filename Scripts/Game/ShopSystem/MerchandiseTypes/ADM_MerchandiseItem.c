[BaseContainerProps()]
class ADM_MerchandiseItem: ADM_MerchandisePrefab
{
	[Attribute(defvalue: "0", desc: "If true then shop will spawn merchandise on the ground at the player if the player cannot equip item or place in inventory. If false the sale will not be allowed.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_bAllowPurchaseWithFullInventory;
	
	[Attribute(defvalue: "0", desc: "If true then shop will spawn payment on the ground at the player if the player cannot equip item or place in inventory. If false the sale will not be allowed.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_bAllowSaleWithFullInventory;
	
	[Attribute(defvalue: "0", desc: "if true, then attachements and all sub-storage items will be dropped on the ground when the player sells an item and does not have enough storage for the contents.", uiwidget: UIWidgets.CheckBox, category: "Physical Shop")]
	protected bool m_bDropSubStorageItemsOnSell;
	
	override bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null) 
	{ 
		// I think this will always be true, maybe for items with geometry that is large it should do a space check so no collisions occur?
		return true; 
	}
	
	array<IEntity> FindItemsToSell(IEntity player, ResourceName itemResource, int quantity = 1)
	{
		// cool gamemode feature might be to check for storages nearby that the player owns (such as vehicles)
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return null;
		
		array<IEntity> items = {};
		int numItems = inventory.GetItems(items);
		
		array<IEntity> foundItems = {};
		foreach (IEntity item : items)
		{
			if (item.GetPrefabData().GetPrefabName() == itemResource)
			{
				foundItems.Insert(item);
				
				if (foundItems.Count() >= quantity)
				{
					break;
				}
			}
		}
		
		return foundItems;
	}
	
	override bool CanSell(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1) 
	{ 
		array<IEntity> itemsToSell = FindItemsToSell(player, GetPrefab(), quantity);
		if (!itemsToSell || itemsToSell.IsEmpty() || itemsToSell.Count() < quantity)
		{
			return false;
		}
						
		return true; 
	}
	
	override bool CanCollectMerchandise(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1) 
	{ 
		// same as CanSell for now. maybe in future check for limited quantity or pulling from a specific storage
		
		 
		return CanSell(player, shop, merchandise, quantity); 
	}
		
	override bool CollectMerchandise(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1) 
	{ 
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return null;
		
		int numItemsToSell = quantity;
		array<IEntity> itemsToSell = FindItemsToSell(player, GetPrefab(), quantity);
		array<IEntity> itemsRemovedFromInventory = {};
		array<bool> itemRemovalSuccess = {};
		while (numItemsToSell > 0)
		{
			int lastIdx = itemsToSell.Count()-1;
			IEntity item = itemsToSell[lastIdx];
			
			bool success = inventory.TryRemoveItemFromInventory(item);
			if (success)
			{
				itemsToSell.Remove(lastIdx);
				itemsRemovedFromInventory.Insert(item);
				itemRemovalSuccess.Insert(true);
				numItemsToSell--;
			} else {
				itemRemovalSuccess.Insert(false);
				break;
			}
			
		}
		
		// if anything failed to remove, and we already removed some items, then return everything and cancel the transaction
		if (itemRemovalSuccess.Contains(false))
		{
			foreach(IEntity item : itemsRemovedFromInventory)
			{
				inventory.TryInsertItem(item);
			}
			
			return false;
		}
		
		// if everything was removed, then delete all (eventually add option to place into a specific storage)
		foreach(IEntity item : itemsRemovedFromInventory)
		{
			SCR_EntityHelper.DeleteEntityAndChildren(item);
		}
						
		return true; 
	}
	
	override bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1, array<IEntity> ignoreEntities = null)
	{
		//TODO: better logic here, i'm lazy. need to check if everything can fit in players inventory
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		ADM_MerchandiseItem merchItem = ADM_MerchandiseItem.Cast(merchandise.GetType());
		if (!merchItem) return false;
		
		return inventory.CanInsertResource(merchItem.GetPrefab(), EStoragePurpose.PURPOSE_ANY, quantity) || m_bAllowSaleWithFullInventory;
	}
	
	override bool Deliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		if (!Replication.IsServer()) return false;
		
		SCR_InventoryStorageManagerComponent inventory = SCR_InventoryStorageManagerComponent.Cast(player.FindComponent(SCR_InventoryStorageManagerComponent));
		if (!inventory) return false;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop, merchandise, quantity);
		if (!canDeliver) return false;
		
		array<IEntity> deliveredItems = {};
		for (int i = 0; i < quantity; i++)
		{
			// give item/weapon/magazine/clothing to player	
			IEntity item = GetGame().SpawnEntityPrefab(Resource.Load(m_sPrefab));	
			vector mat[4];
			player.GetTransform(mat);
			item.SetTransform(mat);
		
			bool putInInventory = ADM_Utils.InsertAutoEquipItem(inventory, item);
			
			// If we cant do with both conditions false, delete it
			if (!m_bAllowPurchaseWithFullInventory && !putInInventory)
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
}
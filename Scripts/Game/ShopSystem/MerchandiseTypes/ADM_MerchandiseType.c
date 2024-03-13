class ADM_MerchandiseType: ScriptAndConfig
{
	[Attribute(defvalue: "-1", params: "0 inf")]
	protected int m_iMaxPurchaseQuantity;
	
	[Attribute(defvalue: "-1", params: "0 inf")]
	protected int m_iMaxSellQuantity;
	
	/*
		How to display this payment method in menus
	*/
	string GetDisplayName(int quantity = 1) { return string.Empty; }
	
	/*
		How to display this payment method in menus
	*/
	ResourceName GetDisplayEntity() { return string.Empty; }
	
	/*
		For physical shops this checks if the item can be spawned in the world. For vehicles it checks collisions so no explosions or objects within eachother,
		and for items it always returns true.
	*/
	bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null) { return false; }
	
	/*
		Maximum amount of mechandise in a single purchase transaction.
	*/
	int GetMaxPurchaseQuantity() { return m_iMaxPurchaseQuantity; }
	
	/*
		When a player requests to buy merchandise, this is called before the shop system collects payment from the player. This should be used for 
		physical clearance checks (vehicles), inventory checks (items), or any other gamemode specific logic.
	*/
	bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1, array<IEntity> ignoreEntities = null) { return false; }
	
	/*
		When a player requests to buy merchandise, this is called after the shop system has processed the payment from the player, and is now delivering 
		the merchandise to the player. (place the item in the player inventory, spawn the vehicle, or some custom gamemode specific logic when the 
		merchandise is purchased)
		This should be called only on the server!
	*/
	bool Deliver(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1) { return false; }
	
	/*
		Maximum amount of mechandise in a single selling transaction.
	*/
	int GetMaxSellQuantity() { return m_iMaxSellQuantity; }
	
	/*
		When a player requests to sell merchandise, this is called before the shop system removes the item from the player. This should be used
		for checks such as does the player have enough inventory space to deliver the shop payment, does the player have the merchandise, or any 
		other gamemode specific logic.
	*/
	bool CanSell(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1) { return false; }
		
	/*
		When a player requests to sell merchandise, this is called to check if the merchandise can be removed. For inventory items this is probably
		not needed, though for things such as vehicles it may be desired to check if the inventory is empty, or if there are occupants, or any other
		gamemode specific logic.
	*/
	bool CanCollectMerchandise(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1) { return false; }
		
	/*
		When a player requests to sell merchandise, this is called to remove the merchandise from the player. For inventory items it will remove
		items from the player inventory. But for example a vehicle shop that allows players to sell vehicles may need custom logic that searches
		near the shop for vehicles the player owns (custom gamemode logic) or just any vehicle (risky if an open trader)	
	*/
	bool CollectMerchandise(IEntity player, ADM_ShopBaseComponent shop, ADM_ShopMerchandise merchandise, int quantity = 1) { return false; }
}
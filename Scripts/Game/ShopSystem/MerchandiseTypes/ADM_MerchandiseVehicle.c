[BaseContainerProps()]
class ADM_MerchandiseVehicle: ADM_MerchandiseType
{
	[Attribute()]
	protected ref PointInfo m_SpawnPosition;
	
	[Attribute(desc: "Ignore player when checking if spawn area is clear")]
	protected bool m_bIgnorePlayer;
	
	EntitySpawnParams GetVehicleSpawnTransform(ADM_ShopBaseComponent shop)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		
		if (m_SpawnPosition && !shop.IsInherited(ADM_PhysicalShopComponent)) {
			m_SpawnPosition.GetWorldTransform(params.Transform);
			params.Transform[3] = shop.GetOwner().CoordToParent(params.Transform[3]);
		} else {
			shop.GetOwner().GetTransform(params.Transform);
		}		
		
		return params;
	}
	
	int lastCheckTime = -1;
	bool canRespawnCache = false;
	override bool CanRespawn(ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null)
	{
		int curTick = System.GetTickCount();
		if (curTick - lastCheckTime >= 1000)
		{
			IEntity shopEntity = shop.GetOwner();
			array<IEntity> excludeEntities = {shopEntity};
			
			if (ignoreEntities)
				excludeEntities.InsertAll(ignoreEntities);
			
			canRespawnCache = ADM_Utils.IsSpawnPositionClean(m_sPrefab, GetVehicleSpawnTransform(shop), excludeEntities);
			lastCheckTime = curTick;
		}
		
		return canRespawnCache;
	}
	
	override bool CanDeliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1, array<IEntity> ignoreEntities = null)
	{
		if (quantity > 1) quantity = 1;
		
		array<IEntity> excludeEntities = {};
		if (m_bIgnorePlayer)
		{
			ADM_Utils.GetChildrenRecursive(player, excludeEntities);
			excludeEntities.Insert(player);
		}
			
		if (ignoreEntities)
			excludeEntities.InsertAll(ignoreEntities);
		
		// same check for respawning and delivering
		return CanRespawn(shop, quantity, excludeEntities); 
	}
	
	override bool Deliver(IEntity player, ADM_ShopBaseComponent shop, int quantity = 1)
	{
		if (!Replication.IsServer()) return false;
		if (quantity > 1) quantity = 1;
		
		bool canDeliver = this.CanDeliver(player, shop, quantity, null);
		if (!canDeliver) 
			return false;
		
		// spawn vehicle
		EntitySpawnParams params = GetVehicleSpawnTransform(shop);
		IEntity entity = GetGame().SpawnEntityPrefab(Resource.Load(m_sPrefab), shop.GetOwner().GetWorld(), params);
		
		return true;
	}
}
[BaseContainerProps()]
class ADM_MerchandiseVehicle: ADM_MerchandiseBase
{
	EntitySpawnParams GetVehicleSpawnTransform(ADM_PhysicalShopComponent shop)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = shop.GetOwner().GetOrigin();
		
		return params;
	}
	
	int lastCheckTime = -1;
	bool canRespawnCache = false;
	override bool CanRespawn(ADM_PhysicalShopComponent shop)
	{
		int curTick = System.GetTickCount();
		if (curTick - lastCheckTime >= 1000)
		{
			canRespawnCache = ADM_Utils.IsSpawnPositionClean(m_PrefabResource, GetVehicleSpawnTransform(shop), {shop.GetOwner()});
			lastCheckTime = curTick;
		}
		
		return canRespawnCache;
	}
	
	override bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		// Don't need to check any player specific things like inventory storage, just need to check if the space is clear from objects that could blow it up.
		return CanRespawn(shop);
	}
	
	override bool Deliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		if (RplSession.Mode() == RplMode.Client) return false;
		
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop);
		if (!canDeliver) return false;
		
		// spawn vehicle
		IEntity entity = GetGame().SpawnEntityPrefab(m_PrefabResource, null, GetVehicleSpawnTransform(shop));
		
		return true;
	}
}
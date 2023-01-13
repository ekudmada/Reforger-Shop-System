[BaseContainerProps()]
class ADM_VehicleShop: ADM_PhysicalShopBase
{
	override bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		return true;
	}
	
	override bool Deliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		// double check we can deliver
		bool canDeliver = this.CanDeliver(player, shop);
		if (!canDeliver) return false;
		
		// spawn vehicle
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = shop.GetOwner().GetOrigin();
		
		Resource resource = Resource.Load(m_Prefab);
		IEntity entity = GetGame().SpawnEntityPrefab(resource, null, params);
		
		return true;
	}
	
	override bool CanRespawn(ADM_PhysicalShopComponent shop)
	{
		//TODO: Don't respawn the shop vehicle unless no other vehicles are in the way
				
		return true;
	}
}
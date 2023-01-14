[BaseContainerProps()]
class ADM_VehicleShop: ADM_PhysicalShopBase
{
	override bool CanRespawn(ADM_PhysicalShopComponent shop)
	{
		//TODO: Don't respawn the shop vehicle unless no other vehicles are in the way
		
		return true;
	}
	
	override bool CanDeliver(IEntity player, ADM_PhysicalShopComponent shop)
	{
		// Don't need to check any player specific things like inventory storage, just need to check if the space is clear from objects that could blow it up.
		return CanRespawn(shop);
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
}
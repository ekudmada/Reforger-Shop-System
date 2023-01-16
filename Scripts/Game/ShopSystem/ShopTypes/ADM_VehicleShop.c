[BaseContainerProps()]
class ADM_VehicleShop: ADM_ShopBase
{
	static bool DEBUG = true;
	
	EntitySpawnParams GetVehicleSpawnTransform(ADM_PhysicalShopComponent shop)
	{
		EntitySpawnParams params = EntitySpawnParams();
		params.TransformMode = ETransformMode.WORLD;
		params.Transform[3] = shop.GetOwner().GetOrigin();
		
		return params;
	}
	
	// static version of SCR_EntitySpawnerComponent::IsSpawnPositionClean
	static autoptr Shape debugBB, debugIntersect;
	static bool IsSpawnPositionClean(Resource resource, EntitySpawnParams params, array<IEntity> excludeArray, bool removeWrecks = true, BaseWorld world = null, vector heightOffset = vector.Zero)
	{
		if (!world)
			world = GetGame().GetWorld();
		
		if (!resource || !resource.IsValid())
			return false;
		
		//Currently, material is still used for preview, even thought it shouldn't be seen, as function doesn't work correctly without it
		SCR_PrefabPreviewEntity previewEntity = SCR_PrefabPreviewEntity.Cast(SCR_PrefabPreviewEntity.SpawnPreviewFromPrefab(resource, "SCR_PrefabPreviewEntity", world, params, "{56EBF5038622AC95}Assets/Conflict/CanBuild.emat"));
		if (!previewEntity)
			return false;
		
		excludeArray.Insert(previewEntity);
		
		TraceOBB paramOBB = new TraceOBB();
		Math3D.MatrixIdentity3(paramOBB.Mat);
		vector currentMat[4];
		previewEntity.GetTransform(currentMat);
		paramOBB.Mat[0] = currentMat[0];
		paramOBB.Mat[1] = currentMat[1];
		paramOBB.Mat[2] = currentMat[2];
		paramOBB.Start = currentMat[3] + heightOffset;
		paramOBB.Flags = TraceFlags.ENTS;
		paramOBB.ExcludeArray = excludeArray;
		paramOBB.LayerMask = EPhysicsLayerPresets.Projectile;
		previewEntity.GetPreviewBounds(paramOBB.Mins, paramOBB.Maxs);
		
		if (DEBUG)
		{	
			debugBB = Shape.Create(ShapeType.BBOX, COLOR_BLUE_A, ShapeFlags.VISIBLE | ShapeFlags.NOZBUFFER | ShapeFlags.WIREFRAME, paramOBB.Mins, paramOBB.Maxs);
			debugBB.SetMatrix(currentMat);
		}
			
		GetGame().GetWorld().TracePosition(paramOBB, null);
		SCR_EntityHelper.DeleteEntityAndChildren(previewEntity);
		
		//If tracePosition found colliding entity, further checks will be done to determine whether can be actually something spawned
		if (paramOBB.TraceEnt)
		{
			if (DEBUG)
			{
				vector mat[4];
				paramOBB.TraceEnt.GetTransform(mat);
				debugIntersect = Shape.CreateSphere(COLOR_RED, ShapeFlags.VISIBLE | ShapeFlags.NOZBUFFER | ShapeFlags.WIREFRAME, paramOBB.TraceEnt.GetOrigin(), 0.1);
				debugIntersect.SetMatrix(mat);
			}
				
			return false;
		}
			
		return true;
	}
	
	override bool CanRespawn(ADM_PhysicalShopComponent shop)
	{
		return ADM_VehicleShop.IsSpawnPositionClean(Resource.Load(m_Prefab), GetVehicleSpawnTransform(shop), {shop.GetOwner()});
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
		EntitySpawnParams params = GetVehicleSpawnTransform(shop);
		Resource resource = Resource.Load(m_Prefab);
		IEntity entity = GetGame().SpawnEntityPrefab(resource, null, params);
		ExtraDeliverLogic(player, shop);
		
		return true;
	}
	
	void ExtraDeliverLogic(IEntity player, ADM_PhysicalShopComponent shop)
	{
		// override to add in custom gameplay logic such as ownership or locking
	}
}
class ADM_Utils
{
	static bool DEBUG = false;
	protected static ref map<ResourceName, ref UIInfo> s_mItemUIInfo = new map<ResourceName, ref UIInfo>();
	protected static ref map<ResourceName, ref SCR_EditableVehicleUIInfo> s_mVehicleUIInfo = new map<ResourceName, ref SCR_EditableVehicleUIInfo>();
	
	//------------------------------------------------------------------------------------------------
	// The following function is MIT license and is from Everon Life
	// https://github.com/EveronLife/EveronLife/blob/98e1f4061d185c31ad68dfa2e3316870a1cece0e/src/Scripts/Game/Core/EL_UIInfoUtils.c
	static UIInfo GetItemUIInfo(ResourceName prefab)
	{
		UIInfo resultInfo = s_mItemUIInfo.Get(prefab);

		if (!resultInfo)
		{
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(Resource.Load(prefab));
			if (entitySource)
			{
			    for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
			    {
			        IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
			        if(componentSource.GetClassName().ToType().IsInherited(InventoryItemComponent))
			        {
			            BaseContainer attributesContainer = componentSource.GetObject("Attributes");
			            if (attributesContainer)
			            {
			                BaseContainer itemDisplayNameContainer = attributesContainer.GetObject("ItemDisplayName");
			                if (itemDisplayNameContainer)
			                {
			                    resultInfo = UIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(itemDisplayNameContainer));
			                    break;
			                }
			            }
			        }
			    }
			}
			
			s_mItemUIInfo.Set(prefab, resultInfo);
		}

		return resultInfo;
	}
	
	//------------------------------------------------------------------------------------------------
	static SCR_EditableVehicleUIInfo GetVehicleUIInfo(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo resultInfo = s_mVehicleUIInfo.Get(prefab);

		if (!resultInfo)
		{
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(Resource.Load(prefab));
			if (entitySource)
			{
			    for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
			    {
			        IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
			        if(componentSource.GetClassName().ToType().IsInherited(SCR_EditableVehicleComponent))
			        {
						BaseContainer baseUIInfo = componentSource.GetObject("m_UIInfo");
						if (baseUIInfo)
						{
		                    resultInfo = SCR_EditableVehicleUIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(baseUIInfo));
							break;
						}
			        }
			    }
			}
			
			s_mVehicleUIInfo.Set(prefab, resultInfo);
		}

		return resultInfo;
	}
	
	static string GetPrefabDisplayName(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo vehicleUIInfo = ADM_Utils.GetVehicleUIInfo(prefab);
		if (vehicleUIInfo) 
			return vehicleUIInfo.GetName();
		
		UIInfo itemUIInfo = ADM_Utils.GetItemUIInfo(prefab);
		if (itemUIInfo) 
			return itemUIInfo.GetName();
		
		return prefab;
	}
	
	static string GetPrefabDescription(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo vehicleUIInfo = ADM_Utils.GetVehicleUIInfo(prefab);
		if (vehicleUIInfo) 
			return vehicleUIInfo.GetDescription();
		
		UIInfo itemUIInfo = ADM_Utils.GetItemUIInfo(prefab);
		if (itemUIInfo) 
			return itemUIInfo.GetDescription();
		
		return prefab;
	}
	
	static ResourceName GetPrefabDisplayIcon(ResourceName prefab)
	{
		SCR_EditableVehicleUIInfo vehicleUIInfo = ADM_Utils.GetVehicleUIInfo(prefab);
		if (vehicleUIInfo) 
			return vehicleUIInfo.GetIconPath();
		
		UIInfo itemUIInfo = ADM_Utils.GetItemUIInfo(prefab);
		if (itemUIInfo) 
			return itemUIInfo.GetIconPath();
		
		return string.Empty;
	}
	
	static bool InsertAutoEquipItem(SCR_InventoryStorageManagerComponent inventory, IEntity item)
	{
		EStoragePurpose purpose = EStoragePurpose.PURPOSE_ANY;
		if (item.FindComponent(WeaponComponent)) purpose = EStoragePurpose.PURPOSE_WEAPON_PROXY;
		if (item.FindComponent(BaseLoadoutClothComponent)) purpose = EStoragePurpose.PURPOSE_LOADOUT_PROXY;
		if (item.FindComponent(SCR_GadgetComponent)) purpose = EStoragePurpose.PURPOSE_GADGET_PROXY;
		
		bool insertedItem = inventory.TryInsertItem(item, purpose, null);
		if (!insertedItem) 
			insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
		
		return insertedItem;
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
}
class ADM_Utils
{
	static bool DEBUG = false;
	protected static ref map<ResourceName, ref string> s_mPrefabDisplayName = new map<ResourceName, ref string>();

	//------------------------------------------------------------------------------------------------
	// Modified from EL_UIInfoUtils from Everon Life. By Arkensor
	static string GetPrefabDisplayName(ResourceName prefab)
	{
		string resultDisplayName = s_mPrefabDisplayName.Get(prefab);

		if (!resultDisplayName)
		{
			IEntitySource entitySource = SCR_BaseContainerTools.FindEntitySource(Resource.Load(prefab));
			if (entitySource)
			{
				// Default to InventoryItemComponent, but lets prioritize other types like SCR_EditableVehicleComponent
				// priority is low ----> high
				array<string> classPriority = {"InventoryItemComponent", "SCR_EditableVehicleComponent"};
				string classToCheck = classPriority[0];
				int prevFoundIndex = -1;
			    for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
			    {
			        IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
					
					int foundIndex = classPriority.Find(componentSource.GetClassName());
					if (foundIndex != -1 && foundIndex > prevFoundIndex)
					{
						classToCheck = componentSource.GetClassName();
						prevFoundIndex = foundIndex;
					}
			    }
				
				for(int nComponent, componentCount = entitySource.GetComponentCount(); nComponent < componentCount; nComponent++)
				{
					IEntityComponentSource componentSource = entitySource.GetComponent(nComponent);
					
					switch(classToCheck)
					{
						case classPriority[0]:
						{
							if(componentSource.GetClassName().ToType().IsInherited(InventoryItemComponent))
					        {
					            BaseContainer attributesContainer = componentSource.GetObject("Attributes");
					            if (attributesContainer)
					            {
					                BaseContainer itemDisplayNameContainer = attributesContainer.GetObject("ItemDisplayName");
					                if (itemDisplayNameContainer)
					                {
					                    UIInfo resultInfo = UIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(itemDisplayNameContainer));
										resultDisplayName = resultInfo.GetName();
					                    break;
					                }
					            }
					        }
							break;
						}
						case classPriority[1]:
						{
							if(componentSource.GetClassName().ToType().IsInherited(SCR_EditableVehicleComponent))
					        {
								BaseContainer baseUIInfo = componentSource.GetObject("m_UIInfo");
								if (baseUIInfo)
								{
				                    SCR_EditableVehicleUIInfo vehicleResultInfo = SCR_EditableVehicleUIInfo.Cast(BaseContainerTools.CreateInstanceFromContainer(baseUIInfo));
									resultDisplayName = vehicleResultInfo.GetName();
				                    break;
								}
							}
							break;
						}
					}
				}
			}
			
			s_mPrefabDisplayName.Set(prefab, resultDisplayName);
		}

		return resultDisplayName;
	}
	
	static bool InsertAutoEquipItem(SCR_InventoryStorageManagerComponent inventory, IEntity item)
	{
		EStoragePurpose purpose = EStoragePurpose.PURPOSE_ANY;
		if (item.FindComponent(WeaponComponent)) purpose = EStoragePurpose.PURPOSE_WEAPON_PROXY;
		if (item.FindComponent(BaseLoadoutClothComponent)) purpose = EStoragePurpose.PURPOSE_LOADOUT_PROXY;
		if (item.FindComponent(SCR_GadgetComponent)) purpose = EStoragePurpose.PURPOSE_GADGET_PROXY;
		
		bool insertedItem = inventory.TryInsertItem(item, purpose, null);
		if (!insertedItem) insertedItem = inventory.TryInsertItem(item, EStoragePurpose.PURPOSE_ANY, null);
		
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
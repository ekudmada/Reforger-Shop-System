class ADM_PhysicalShopComponentClass: ADM_ShopBaseComponentClass {}

//TODO: look into SCR_PreviewEntityComponent
//TODO: fix respawn time = 0 bug (vehicle check doesnt work)
class ADM_PhysicalShopComponent: ADM_ShopBaseComponent
{	
	[Attribute(category: "Shop")]
	protected ref ADM_ShopMerchandise m_PhysicalMerchandise;
	
	[Attribute(defvalue: "0", desc: "How many seconds for item to respawn after it has been purchased. (-1 for no respawning)", uiwidget: UIWidgets.Slider, params: "0.1 1000 et", category: "Shop")]
	protected float m_fRespawnTime;
	
	[Attribute(desc: "Show bounding box for respawn check. DEBUG ONLY")]
	protected bool m_bDebugBB;
	
	[RplProp(onRplName: "OnStateChange")]
	protected bool m_bState = true;
	
	// Only used for server to determine when to spawn a new vehicle
	protected float m_fLastStateChangeTime = -1;
	
	void UpdateMesh(IEntity owner)
	{
		//TODO: look into using SCR_PreviewEntity or replicating it. Display prefab 1:1 with all slots and such
		if (!owner || !m_Merchandise || m_Merchandise.Count() <= 0) 
			return;
		
		ResourceName modelPath;
		string remapPath;
		
		ADM_MerchandisePrefab merchandise = ADM_MerchandisePrefab.Cast(m_Merchandise[0].GetType());
		if (!merchandise) 
			return;
		
		bool foundModelPath = SCR_Global.GetModelAndRemapFromResource(merchandise.GetPrefab(), modelPath, remapPath);
		if (!foundModelPath) 
			return;
		
		Resource resource = Resource.Load(modelPath);
		if (!resource.IsValid()) 
			return;
		
		MeshObject model = resource.GetResource().ToMeshObject();
		owner.SetObject(model, string.Empty);
		
		Physics oldPhysics = owner.GetPhysics();
		if (oldPhysics) 
			oldPhysics.Destroy();
		
		Physics.CreateStatic(owner, 0xffffffff);
	}
	
	void SetState(bool state)
	{
		m_bState = !m_bState;
		m_fLastStateChangeTime = System.GetTickCount();	
		Replication.BumpMe();
		
		OnStateChange();
	}
	
	void OnStateChange()
	{
		IEntity owner = GetOwner();
		if (m_bState)
		{
			UpdateMesh(owner);
		} else {
			owner.SetObject(null, string.Empty);
			Physics physics = owner.GetPhysics();
			if (physics) 
				physics.Destroy();
		}
	}
	
	// Return amount of seconds to respawn
	float GetRespawnTime()
	{
		return m_fRespawnTime;
	}
	
	// Amount of time in milliseconds until respawn
	float GetTimeUntilRespawn()
	{
		return System.GetTickCount() - m_fLastStateChangeTime;
	}
	
	void ViewPayment(ADM_PhysicalShopAction action)
	{
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ADM_ViewPaymentMenu); 
		ADM_ViewPaymentUI menu = ADM_ViewPaymentUI.Cast(menuBase);
		menu.SetShop(this, action);
	}
	
	override bool AskPurchase(IEntity player, ADM_PlayerShopManagerComponent playerManager, ADM_ShopMerchandise merchandise, int quantity)
	{
		bool success = super.AskPurchase(player, playerManager, merchandise, quantity);
		if (!success) 
			return false;
		
		SetState(false);
		
		return true;
	}
	
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.FRAME);
		OnStateChange();
		
		ConnectToDiagSystem(owner);
	}
	
	static autoptr Shape debugBB;
	override void EOnDiag(IEntity owner, float timeSlice)
	{
		if (!m_PhysicalMerchandise)
			return;
		
		ADM_MerchandisePrefab merchPrefab = ADM_MerchandisePrefab.Cast(m_PhysicalMerchandise.GetType());
		if (!merchPrefab)
			return;
		
		if (m_PhysicalMerchandise && ADM_Utils.m_CachedBoundingBoxes && ADM_Utils.m_CachedBoundingBoxes.Contains(merchPrefab.GetPrefab()))
		{
			vector traceMat[4];
			owner.GetTransform(traceMat);
			
			TraceOBB paramOBB = new TraceOBB();
			Math3D.MatrixIdentity3(paramOBB.Mat);
			paramOBB.Mat[0] = traceMat[0];
			paramOBB.Mat[1] = traceMat[1];
			paramOBB.Mat[2] = traceMat[2];
			paramOBB.Start = traceMat[3];
			paramOBB.Flags = TraceFlags.ENTS;
			paramOBB.LayerMask = EPhysicsLayerPresets.Projectile;
			paramOBB.Mins = ADM_Utils.m_CachedBoundingBoxes.Get(merchPrefab.GetPrefab())[0];
			paramOBB.Maxs = ADM_Utils.m_CachedBoundingBoxes.Get(merchPrefab.GetPrefab())[1];
			debugBB = Shape.Create(ShapeType.BBOX, COLOR_BLUE_A, ShapeFlags.VISIBLE | ShapeFlags.NOZBUFFER | ShapeFlags.WIREFRAME, paramOBB.Mins, paramOBB.Maxs);
			debugBB.SetMatrix(traceMat);
		}
	}
	
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (!Replication.IsServer() || m_fLastStateChangeTime == -1 || m_fRespawnTime == -1) 
			return;
		
		//TODO: better understand the delay from buying -> spawning and what could cause the physical shop respawn detection to not work
		float dt = GetTimeUntilRespawn();
		if (m_fRespawnTime < 1) 
			m_fRespawnTime = 1;
		
		if (dt > m_fRespawnTime * 1000 && m_Merchandise.Count() > 0 && m_Merchandise[0].GetType().CanRespawn(this))
		{
			SetState(true);
			m_fLastStateChangeTime = -1;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void ADM_PhysicalShopComponent(IEntityComponentSource src, IEntity ent, IEntity parent)
	{
		if (m_PhysicalMerchandise) 
			m_Merchandise = {m_PhysicalMerchandise};
	}
}
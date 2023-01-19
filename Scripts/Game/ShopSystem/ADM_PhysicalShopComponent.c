class ADM_PhysicalShopComponentClass: ScriptComponentClass {}

//TODO: look into SCR_PreviewEntityComponent
class ADM_PhysicalShopComponent: ADM_ShopComponent
{	
	[Attribute(defvalue: "0", desc: "How many seconds for item to respawn after it has been purchased. (-1 for no respawning)", uiwidget: UIWidgets.EditBox, params: "et", category: "Physical Shop")]
	protected float m_RespawnTime;
	
	protected float m_LastStateChangeTime = -1;
	
	//------------------------------------------------------------------------------------------------
	void UpdateMesh(IEntity owner)
	{
		//TODO: look into using SCR_PreviewEntity or replicating it. Display prefab 1:1 with all slots and such
		if (!m_Merchandise || m_Merchandise.Count() <= 0) return;
		
		ResourceName modelPath;
		string remapPath;
		
		ADM_MerchandiseType merchandise = m_Merchandise[0].GetMerchandise();
		if (!merchandise) return;
		
		bool foundModelPath = SCR_Global.GetModelAndRemapFromResource(merchandise.GetPrefab(), modelPath, remapPath);
		if (!foundModelPath) return;
		
		Resource resource = Resource.Load(modelPath);
		if (!resource.IsValid()) return;
		
		MeshObject model = resource.GetResource().ToMeshObject();
		owner.SetObject(model, string.Empty);
		
		Physics oldPhysics = owner.GetPhysics();
		if (oldPhysics) oldPhysics.Destroy();
		
		Physics.CreateStatic(owner, 0xffffffff);
	}
	
	//------------------------------------------------------------------------------------------------
	void SetState(bool state)
	{
		IEntity owner = GetOwner();
		if (!owner) return;
		
		if (state)
		{
			UpdateMesh(owner);
			m_LastStateChangeTime = -1;
		} else {
			Physics physics = owner.GetPhysics();
			if (physics) physics.Destroy();
			
			owner.SetObject(null, string.Empty);
			m_LastStateChangeTime = System.GetTickCount();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	// Return amount of seconds to respawn
	float GetRespawnTime()
	{
		return m_RespawnTime;
	}
	
	//------------------------------------------------------------------------------------------------
	// Amount of time in milliseconds until respawn
	float GetTimeUntilRespawn()
	{
		return System.GetTickCount() - m_LastStateChangeTime;
	}
	
	//------------------------------------------------------------------------------------------------
	void ViewPayment()
	{
		MenuBase menuBase = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.ADM_ViewPaymentMenu); 
		ADM_ViewPaymentUI menu = ADM_ViewPaymentUI.Cast(menuBase);
		menu.SetShop(this);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	override void RpcAsk_Purchase(int playerId, ADM_ShopMerchandise merchandise, int quantity)
	{
		super.RpcAsk_Purchase(playerId, merchandise, quantity);
		this.SetState(false);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		this.SetState(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_LastStateChangeTime == -1) return;
		
		float dt = GetTimeUntilRespawn();
		if (dt >= m_RespawnTime * 1000 && m_Merchandise.Count() > 0 && m_Merchandise[0].GetMerchandise().CanRespawn(this))
		{
			Print(dt);
			this.SetState(true);
		}
	}
	
	void ADM_PhysicalShopComponent()
	{
		if (!m_Merchandise || m_Merchandise.Count() <= 0) return;
	
		m_Merchandise.Resize(1);
	}
}
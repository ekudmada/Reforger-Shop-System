class ADM_PhysicalShopComponentClass: ScriptComponentClass {}

//! A brief explanation of what this component does.
//! The explanation can be spread across multiple lines.
//! This should help with quickly understanding the script's purpose.
//TODO: look into SCR_PreviewEntityComponent
class ADM_PhysicalShopComponent: ScriptComponent
{
	[Attribute(defvalue: "", desc: "Physical Shop Configuration", uiwidget: UIWidgets.Object, params: "et", category: "Physical Shop")]
	protected ref ADM_ShopBase m_ShopConfig;
	
	[Attribute(category: "Physical Shop")]
	protected ref array<ref ADM_PaymentMethodBase> m_RequiredPayment;
	
	[Attribute(defvalue: "0", desc: "How many seconds for item to respawn after it has been purchased. (-1 for no respawning)", uiwidget: UIWidgets.EditBox, params: "et", category: "Physical Shop")]
	protected float m_RespawnTime;
	
	protected float m_LastStateChangeTime = -1;
	
	//------------------------------------------------------------------------------------------------
	void UpdateMesh(IEntity owner)
	{
		if (!m_ShopConfig) return;
		
		ResourceName modelPath;
		string remapPath;
		
		bool foundModelPath = SCR_Global.GetModelAndRemapFromResource(m_ShopConfig.GetPrefab(), modelPath, remapPath);
		if (!foundModelPath) return;
		
		Resource resource = Resource.Load(modelPath);
		if (!resource.IsValid()) return;
		
		MeshObject model = resource.GetResource().ToMeshObject();
		owner.SetObject(model, string.Empty);
		
		Physics oldPhysics = owner.GetPhysics();
		if (!oldPhysics) return;
		oldPhysics.Destroy();
		
		Physics newPhysics = Physics.CreateStatic(owner, -1);
		
		/*PhysicsGeom geom = PhysicsGeom.CreateBox(Vector(1,1,1));
		vector frame[4];
		Math3D.MatrixIdentity4(frame);
		frame[3] = vector.Up;
		string material = "{D745FD8FC67DB26A}Common/Materials/Game/stone.gamemat";
		int idx = newPhysics.AddGeom("Box", geom, frame, material, 0xffffffff);*/
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ADM_PaymentMethodBase> GetRequiredPayment()
	{
		return m_RequiredPayment;
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsPaymentOnlyCurrency()
	{
		if (m_RequiredPayment.Count() != 1)	return false;
		if (m_RequiredPayment[0].ClassName() != "ADM_PaymentMethodCurrency") return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	ADM_ShopBase GetShopConfig()
	{
		return m_ShopConfig;
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
	bool CanPurchase(IEntity player)
	{
		bool canPurchase = true;
		foreach (ADM_PaymentMethodBase payment : m_RequiredPayment)
		{
			canPurchase = payment.CheckPayment(player);
			if (!canPurchase) break;
		}
		
		return canPurchase;
	}
	
	//------------------------------------------------------------------------------------------------
	void AskPurchase()
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		Rpc(RpcAsk_Transaction, playerId);
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
	void RpcAsk_Transaction(int playerId)
	{
		//TODO:
		//  - Purchase multiple quantities of items
		//  - View payment if not currency
		//  - Add supply/demand ability (player shops will be real supply)
		//  - Add interface for "ownership" of shop (where the money goes, if its a player shop it should go to player, if NPC it should go to either nowhere or some sort of federal system)
		
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!player) 
		{
			Rpc(RpcDo_Purchase, "Couldn't find player entity");
			return;
		}
		
		bool canPay = CanPurchase(player);
		if (!canPay) 
		{
			Rpc(RpcDo_Purchase, "Payment not met");
			return;
		}
		
		bool canDeliver = m_ShopConfig.CanDeliver(player, this);
		if (!canDeliver) 
		{
			Rpc(RpcDo_Purchase, "Can't deliver item");
			return;
		}
		
		array<ADM_PaymentMethodBase> collectedPaymentMethods = {};
		array<bool> didCollectPayments = {};
		for (int i = 0; i < m_RequiredPayment.Count(); i++) 
		{
			ADM_PaymentMethodBase paymentMethod = m_RequiredPayment[i];
			
			// If collect payment returns false, it is assumed that the payment was NOT removed from the player
			// this will be up to each payment method coding logic to ensure is true.
			bool didCollectPayment = paymentMethod.CollectPayment(player);
			didCollectPayments.Insert(didCollectPayment);
			
			if (didCollectPayment) collectedPaymentMethods.Insert(paymentMethod);
		}
		
		if (didCollectPayments.Contains(false))
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player);
				if (!returnedPayment) PrintFormat("Error returning payment! %s", paymentMethod.Type().ToString());
			}
			
			Rpc(RpcDo_Purchase, "Error collecting payment");
			return;
		}
		
		bool deliver = m_ShopConfig.Deliver(player, this);
		if (!deliver) 
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player);
				if (!returnedPayment) PrintFormat("Error returning payment! %1", paymentMethod.Type().ToString());
			}
			
			Rpc(RpcDo_Purchase, "Error delivering item");
			return;
		}
		
		// Hide shop
		Physics physics = GetOwner().GetPhysics();
		if (physics) physics.Destroy();
		
		GetOwner().SetObject(null, string.Empty);
		m_LastStateChangeTime = System.GetTickCount();
		
		Rpc(RpcDo_Purchase, "success");
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Purchase(string message)
	{
		SCR_HintManagerComponent.GetInstance().ShowCustom(message);
		Print(message);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		if (m_LastStateChangeTime == -1) return;
		
		float dt = GetTimeUntilRespawn();
		if (dt >= m_RespawnTime * 1000 && m_ShopConfig.CanRespawn(this))
		{
			UpdateMesh(owner);
			m_LastStateChangeTime = -1;
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		UpdateMesh(owner);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
}
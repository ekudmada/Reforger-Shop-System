class ADM_PhysicalShopComponentClass: ScriptComponentClass {}

//! A brief explanation of what this component does.
//! The explanation can be spread across multiple lines.
//! This should help with quickly understanding the script's purpose.
class ADM_PhysicalShopComponent: ScriptComponent
{
	[Attribute(defvalue: "", desc: "Physical Shop Configuration", uiwidget: UIWidgets.Object, params: "et", category: "Physical Shop")]
	protected ref ADM_PhysicalShopBase m_ShopConfig;
	
	[Attribute(category: "Physical Shop")]
	protected ref array<ref ADM_PaymentMethodBase> m_RequiredPayment;
	
	[Attribute(defvalue: "0", desc: "How many seconds for item to respawn after it has been purchased. (-1 for no respawning)", uiwidget: UIWidgets.EditBox, params: "et", category: "Physical Shop")]
	protected float m_RespawnTime;
	
	protected float m_LastRespawnTime = -1;
	
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
	ADM_PhysicalShopBase GetShopConfig()
	{
		return m_ShopConfig;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetRespawnTime()
	{
		return m_RespawnTime;
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
		Rpc(RpcAsk_Purchase, GetGame().GetPlayerController().GetPlayerId());
	}
	
	//------------------------------------------------------------------------------------------------
	void ViewPayment()
	{
		//TODO
		Print("open dialog view payment");
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_Purchase(int playerId)
	{
		//TODO:
		//  - Return payment methods already collected if one of them fails
		//  - Check for storage before trying to insert an item
		//  - Purchase multiple quantities of items
		//  - Drop current item in slot or put in inventory
		//  - Don't respawn unless no other items are in the way
		//  - View payment if not currency
		//  - Add supply/demand ability (player shops will be real supply)
		//  - Add interface for "ownership" of shop (where the money goes, if its a player shop it should go to playr, if NPC it should go to either nowhere or some sort of federal system)
		
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
		
		array<bool> didCollectPayments = {};
		for (int i = 0; i < m_RequiredPayment.Count(); i++) 
		{
			ADM_PaymentMethodBase paymentMethod = m_RequiredPayment[i];
			bool didCollectPayment = paymentMethod.CollectPayment(player);
			didCollectPayments.Insert(didCollectPayment);
		}
		
		if (didCollectPayments.Contains(false))
		{
			Rpc(RpcDo_Purchase, "Error collecting payment");
			//TODO: Ensure that player doesn't lose anything
			return;
		}
		
		bool deliver = m_ShopConfig.Deliver(player, this);
		if (!deliver) 
		{
			Rpc(RpcDo_Purchase, "Error delivering item");
			return;
		}
		
		// Hide shop mesh
		GetOwner().SetObject(null, string.Empty);
		m_LastRespawnTime = System.GetTickCount();
		
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
		if (m_LastRespawnTime == -1) return;
		
		float dt = System.GetTickCount() - m_LastRespawnTime;
		if (dt >= m_RespawnTime * 1000 && m_ShopConfig.CanRespawn(this))
		{
			UpdateMesh(owner);
			m_LastRespawnTime = -1;
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
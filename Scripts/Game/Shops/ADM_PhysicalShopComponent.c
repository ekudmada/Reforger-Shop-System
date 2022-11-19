class ADM_PhysicalShopComponentClass: ScriptComponentClass {};

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
		Print("open dialog view payment");
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_Purchase(int playerId)
	{
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!player) {
			Rpc(RpcDo_Purchase, "Couldn't find player entity");
			return;
		}
		
		bool canPay = CanPurchase(player);
		if (!canPay) {
			Rpc(RpcDo_Purchase, "Payment not met");
			return;
		}
		
		bool canDeliver = m_ShopConfig.CanDeliver(player);
		if (!canDeliver) {
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
		
		bool deliver = m_ShopConfig.Deliver(player);
		if (!deliver) {
			Rpc(RpcDo_Purchase, "Error delivering item");
		}
		
		Rpc(RpcDo_Purchase, "success");
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Purchase(string message)
	{
		Print(message);
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
		SetEventMask(owner, EntityEvent.INIT);
	}
};
class ADM_ShopComponentClass: ScriptComponentClass {}

class ADM_ShopComponent: ScriptComponent
{
	[Attribute(defvalue: "", desc: "Merchandise to sell", uiwidget: UIWidgets.Object, params: "et", category: "Shop")]
	protected ref array<ref ADM_ShopMerchandise> m_Merchandise;
	
	//------------------------------------------------------------------------------------------------
	bool IsPaymentOnlyCurrency(ADM_ShopMerchandise merchandise)
	{
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetRequiredPaymentToBuy();
		if (requiredPayment.Count() != 1)	return false;
		if (requiredPayment[0].ClassName().ToType() != ADM_PaymentMethodCurrency) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ADM_ShopMerchandise> GetMerchandise()
	{
		return m_Merchandise;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanPurchase(IEntity player, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		bool canPurchase = true;
		
		int quantityCanBuy = merchandise.GetQuantityCanBuy();
		if (quantityCanBuy < 0 && quantityCanBuy != -1) return false;
		if (quantityCanBuy - quantity < 0) return false;
		
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetRequiredPaymentToBuy();
		foreach (ADM_PaymentMethodBase payment : requiredPayment)
		{
			canPurchase = payment.CheckPayment(player, quantity);
			if (!canPurchase) break;
		}
		
		return canPurchase;
	}
	
	//------------------------------------------------------------------------------------------------
	void AskPurchase(ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		int playerId = GetGame().GetPlayerController().GetPlayerId();
		Rpc(RpcAsk_Purchase, playerId, merchandise, quantity);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void RpcAsk_Purchase(int playerId, ADM_ShopMerchandise merchandise, int quantity)
	{
		//TODO:
		//  - Add interface for "ownership" of shop (where the money goes, if its a player shop it should go to player, if NPC it should go to either nowhere or some sort of federal system)
		
		IEntity player = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (!player) 
		{
			Rpc(RpcDo_Transaction, "Couldn't find player entity");
			return;
		}
		
		bool canPay = CanPurchase(player, merchandise, quantity);
		if (!canPay) 
		{
			Rpc(RpcDo_Transaction, "Payment not met");
			return;
		}
		
		bool canDeliver = merchandise.GetMerchandise().CanDeliver(player, this, quantity);
		if (!canDeliver) 
		{
			Rpc(RpcDo_Transaction, "Can't deliver item");
			return;
		}
		
		array<ADM_PaymentMethodBase> collectedPaymentMethods = {};
		array<bool> didCollectPayments = {};
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetRequiredPaymentToBuy();
		foreach (ADM_PaymentMethodBase paymentMethod : requiredPayment) 
		{
			bool didCollectPayment = paymentMethod.CollectPayment(player, quantity);
			didCollectPayments.Insert(didCollectPayment);
			
			if (didCollectPayment) collectedPaymentMethods.Insert(paymentMethod);
		}
		
		if (didCollectPayments.Contains(false))
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player, quantity);
				if (!returnedPayment) PrintFormat("Error returning payment! %s", paymentMethod.Type().ToString());
			}
			
			Rpc(RpcDo_Transaction, "Error collecting payment");
			return;
		}
		
		bool deliver = merchandise.GetMerchandise().Deliver(player, this, quantity);
		if (!deliver) 
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player, quantity);
				if (!returnedPayment) PrintFormat("Error returning payment! %1", paymentMethod.Type().ToString());
			}
			
			Rpc(RpcDo_Transaction, "Error delivering item");
			return;
		}
		
		Rpc(RpcDo_Transaction, "success");
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void RpcDo_Transaction(string message)
	{
		SCR_HintManagerComponent.GetInstance().ShowCustom(message);
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnFrame(IEntity owner, float timeSlice)
	{
		
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		foreach (ADM_ShopMerchandise merchandise : m_Merchandise)
		{
			if (!merchandise) continue;
			 
			ADM_MerchandiseType merchandiseType = merchandise.GetMerchandise();
			if (!merchandiseType) continue;
			
			merchandiseType.SetPrefabResource(Resource.Load(merchandiseType.GetPrefab()));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT | EntityEvent.FRAME);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
}
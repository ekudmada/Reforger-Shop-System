class ADM_ShopBaseComponentClass: ScriptComponentClass {}
class ADM_ShopBaseComponent: ScriptComponent
{
	/*
		TODO:
			- Add hook for gamemodes to control costs
			- Hook for gamemodes to send payments somewhere
				- e.g player owned shops
			- Rather than delete payment or sold item have option for it to go into storage
	*/
	
	/* 
		Selling Ideas:
			- Add a function to check for shop storage when selling to the shop, probably default to always true (this will allow overrides for player owned shops with limited storage capacity)
			- How to deal with items that have attachements
	*/
	
	[Attribute(category: "Shop"), RplProp()]
	protected string m_ShopName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Config", "conf", category: "Shop")]
	protected ResourceName m_ShopConfig;
	
	protected ref array<ref ADM_ShopMerchandise> m_Merchandise = {};
	private ref ScriptInvoker Event_OnPostPurchase = new ScriptInvoker();
	private ref ScriptInvoker Event_OnPostSell = new ScriptInvoker();
	
	//------------------------------------------------------------------------------------------------
	static bool IsBuyPaymentOnlyCurrency(ADM_ShopMerchandise merchandise)
	{
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetBuyPayment();
		if (requiredPayment.Count() != 1 || !requiredPayment[0].Type().IsInherited(ADM_PaymentMethodCurrency)) 
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool IsSellPaymentOnlyCurrency(ADM_ShopMerchandise merchandise)
	{
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetSellPayment();
		if (requiredPayment.Count() != 1 || requiredPayment[0].Type() != ADM_PaymentMethodCurrency) 
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_ShopName;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ADM_ShopMerchandise> GetMerchandiseAll()
	{
		return m_Merchandise;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ADM_ShopMerchandise> GetMerchandiseBuy()
	{
		array<ref ADM_ShopMerchandise> merchandiseBuy = {};
		foreach (ADM_ShopMerchandise merch : m_Merchandise)
		{
			if (merch.GetBuyPayment().Count() > 0)
			{
				merchandiseBuy.Insert(merch);
			}
		}
	
		return merchandiseBuy;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ADM_ShopMerchandise> GetMerchandiseSell()
	{
		array<ref ADM_ShopMerchandise> merchandiseSell = {};
		foreach (ADM_ShopMerchandise merch : m_Merchandise)
		{
			if (merch.GetSellPayment().Count() > 0)
			{
				merchandiseSell.Insert(merch);
			}
		}
	
		return merchandiseSell;
	}
	
	//------------------------------------------------------------------------------------------------
	static bool CanPurchase(IEntity player, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		if (!merchandise || !player)
			return false;
		
		bool canPurchase = true;
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetBuyPayment();
		foreach (ADM_PaymentMethodBase payment : requiredPayment)
		{
			canPurchase = payment.CheckPayment(player, quantity);
			if (!canPurchase) 
				break;
		}
		
		return canPurchase;
	}
	
	//------------------------------------------------------------------------------------------------
	bool AskPurchase(IEntity player, ADM_PlayerShopManagerComponent playerManager, ADM_ShopMerchandise merchandise, int quantity)
	{
		if (!player || !playerManager)
			return false;
		
		bool canPay = CanPurchase(player, merchandise, quantity);
		if (!canPay) 
		{
			playerManager.SetPurchaseMessage("Error! You do not have the required payment.");
			return false;
		}
		
		bool canDeliver = merchandise.GetType().CanDeliver(player, this, quantity);
		if (!canDeliver) 
		{
			playerManager.SetPurchaseMessage("Error! Unable to deliver item(s).");
			return false;
		}
		
		int maxQuantity = merchandise.GetType().GetMaxQuantity();
		if (maxQuantity != 0 && maxQuantity > 0 && quantity > maxQuantity)
			quantity = maxQuantity;
		
		array<ADM_PaymentMethodBase> collectedPaymentMethods = {};
		array<bool> didCollectPayments = {};
		
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetBuyPayment();
		foreach (ADM_PaymentMethodBase paymentMethod : requiredPayment) 
		{
			bool didCollectPayment = paymentMethod.CollectPayment(player, quantity);
			didCollectPayments.Insert(didCollectPayment);
			
			if (didCollectPayment) 
				collectedPaymentMethods.Insert(paymentMethod);
		}
		
		if (didCollectPayments.Contains(false))
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.DistributePayment(player, quantity);
				if (!returnedPayment) 
					PrintFormat("Error returning payment! %s", paymentMethod.Type().ToString());
			}
			
			playerManager.SetPurchaseMessage("Error! Could not collect payment.");
			return false;
		}
		
		bool deliver = merchandise.GetType().Deliver(player, this, quantity);
		if (!deliver) 
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.DistributePayment(player, quantity);
				if (!returnedPayment) 
					PrintFormat("Error returning payment! %1", paymentMethod.Type().ToString());
			}
			
			playerManager.SetPurchaseMessage("Error! Can not deliver item(s).");
			return false;
		}
		
		playerManager.SetPurchaseMessage("Purchase Successful!");
		
		// Invoke OnPostPurchase (event after a purchase is completed for gamemodes to hook to)
		// Pass in the shop, player who purchased, and collectedPaymentMethods
		Event_OnPostPurchase.Invoke(this, player, collectedPaymentMethods);
		
		return true;	
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanSell(IEntity player, ADM_ShopMerchandise merchandise, int quantity = 1)
	{
		if (!merchandise || !player)
			return false;
		
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetSellPayment();
		foreach (ADM_PaymentMethodBase payment : requiredPayment)
		{
			// TODO: instead of check payment, check if player has same prefab
			// TODO: will want to account for weapon attachments and such
			//if (!payment.CheckPayment(player, quantity))
			//	break;
		}
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	bool AskSell(IEntity player, ADM_PlayerShopManagerComponent playerManager, ADM_ShopMerchandise merchandise, int quantity)
	{
		playerManager.SetPurchaseMessage("Selling not implemented yet!");
		
		// Invoke OnPostSell (event after a purchase is completed for gamemodes to hook to)
		// Pass in the shop, player who purchased, and sold item
		Event_OnPostSell.Invoke(this, player, null);
		
		return false;
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
		
		if (!m_Merchandise) 
		{
			m_Merchandise = new array<ref ADM_ShopMerchandise>();
		}
			
		// Copy config into merchandise array
		if (m_ShopConfig != string.Empty) {
			ADM_ShopConfig shopConfig = ADM_ShopConfig.GetConfig(m_ShopConfig);
			if (shopConfig && shopConfig.m_Merchandise) {
				foreach (ADM_ShopMerchandise merch : shopConfig.m_Merchandise) {
					m_Merchandise.Insert(merch);
				}
			}
		}
		
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (rpl) 
		{
			rpl.InsertToReplication();
		}
	}		
}
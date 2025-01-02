#define ADM_SHOPSYSTEM

class ADM_ShopBaseComponentClass: ScriptComponentClass {}
class ADM_ShopBaseComponent: ScriptComponent
{
	[Attribute(category: "Shop"), RplProp()]
	protected string m_ShopName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Config", "conf", category: "Shop")]
	protected ResourceName m_ShopConfig;
	
	protected ref array<ref ADM_ShopMerchandise> m_Merchandise = {};
	private ref ScriptInvoker Event_OnPostPurchase = new ScriptInvoker();
	private ref ScriptInvoker Event_OnPostSell = new ScriptInvoker();
	
	string GetName()
	{
		return m_ShopName;
	}
	
	array<ref ADM_ShopMerchandise> GetMerchandiseAll()
	{
		return m_Merchandise;
	}
	
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
	
	static bool IsBuyPaymentOnlyCurrency(ADM_ShopMerchandise merchandise)
	{
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetBuyPayment();
		return (requiredPayment.Count() == 1 && requiredPayment[0].Type().IsInherited(ADM_PaymentMethodCurrency));
	}
	
	static bool IsSellPaymentOnlyCurrency(ADM_ShopMerchandise merchandise)
	{
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetSellPayment();
		return (requiredPayment.Count() == 1 && requiredPayment[0].Type() == ADM_PaymentMethodCurrency);
	}
	
	/*
		When a player requests to buy merchandise, this is called before the shop system collects payment from the player. This should be used for
		checking if the player has the necessary payment, or any other gamemode specific logic.
	*/
	bool CanPurchase(IEntity player, array<ref ADM_PaymentMethodBase> requiredPayment, int quantity) 
	{ 
		bool canPurchase = true;
		foreach (ADM_PaymentMethodBase payment : requiredPayment)
		{
			canPurchase = payment.CheckPayment(player, quantity);
			if (!canPurchase) 
				break;
		}
		
		return canPurchase;
	}
	
	bool AskPurchase(IEntity player, ADM_PlayerShopManagerComponent playerManager, ADM_ShopMerchandise merchandise, int quantity)
	{
		if (!Replication.IsServer() || !player || !playerManager)
		{
			return false;
		}
			
		bool canPay = CanPurchase(player, merchandise.GetBuyPayment(), quantity);
		if (!canPay) 
		{
			playerManager.SetTransactionMessage("Error! You do not have the required payment.");
			return false;
		}
		
		bool canDeliver = merchandise.GetType().CanDeliver(player, this, merchandise, quantity);
		if (!canDeliver) 
		{
			playerManager.SetTransactionMessage("Error! Unable to deliver item(s).");
			return false;
		}
		
		int maxQuantity = merchandise.GetType().GetMaxPurchaseQuantity();
		if (maxQuantity != 0 && maxQuantity > 0 && quantity > maxQuantity)
		{
			quantity = maxQuantity;
		}
			
		array<ADM_PaymentMethodBase> collectedPaymentMethods = {};
		array<bool> didCollectPayments = {};
		
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetBuyPayment();
		foreach (ADM_PaymentMethodBase paymentMethod : requiredPayment) 
		{
			bool didCollectPayment = paymentMethod.CollectPayment(player, quantity);
			didCollectPayments.Insert(didCollectPayment);
			
			if (didCollectPayment) 
			{
				collectedPaymentMethods.Insert(paymentMethod);
			}
		}
		
		if (didCollectPayments.Contains(false))
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.DistributePayment(player, quantity);
				if (!returnedPayment) 
				{
					PrintFormat("Error returning payment! %s", paymentMethod.Type().ToString());
				}
			}
			
			playerManager.SetTransactionMessage("Error! Could not collect payment.");
			return false;
		}
		
		bool deliver = merchandise.GetType().Deliver(player, this, merchandise, quantity);
		if (!deliver) 
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.DistributePayment(player, quantity);
				if (!returnedPayment)
				{
					PrintFormat("Error returning payment! %1", paymentMethod.Type().ToString());
				}
			}
			
			playerManager.SetTransactionMessage("Error! Can not deliver item(s).");
			return false;
		}
		
		playerManager.SetTransactionMessage("Purchase Successful!");
		
		// Invoke OnPostPurchase (event after a purchase is completed for gamemodes to hook to)
		// Pass in the shop, player who purchased, and collectedPaymentMethods
		Event_OnPostPurchase.Invoke(this, player, collectedPaymentMethods);
		
		return true;	
	}
	
	bool AskSell(IEntity player, ADM_PlayerShopManagerComponent playerManager, ADM_ShopMerchandise merchandise, int quantity)
	{
		if (!Replication.IsServer())
			return false;
		
		bool canSell = merchandise.GetType().CanSell(player, this, merchandise, quantity);
		if (!canSell) 
		{
			playerManager.SetTransactionMessage("Error! You do not have the required merchandise to sell!");
			return false;
		}
		
		bool canCollect = merchandise.GetType().CanCollectMerchandise(player, this, merchandise, quantity);
		if (!canCollect)
		{
			playerManager.SetTransactionMessage("Error! Merchandise could not be sold!");
			return false;
		}
		
		bool didCollect = merchandise.GetType().CollectMerchandise(player, this, merchandise, quantity);
		if (!didCollect)
		{
			playerManager.SetTransactionMessage("Error! Merchandise could not be collected!");
			return false;
		}
		
		foreach (ADM_PaymentMethodBase paymentMethod : merchandise.GetSellPayment())
		{
			bool returnedPayment = paymentMethod.DistributePayment(player, quantity);
			if (!returnedPayment)
			{
				PrintFormat("Error giving payment! %1", paymentMethod.Type().ToString());
			}
		}
		
		playerManager.SetTransactionMessage("Success! Merchandise sold.");
		
		// Invoke OnPostSell (event after a purchase is completed for gamemodes to hook to)
		// Pass in the shop, player who purchased, and sold item
		Event_OnPostSell.Invoke(this, player, null);
		
		return true;
	}
	
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
		if (m_ShopConfig != string.Empty) 
		{
			ADM_ShopConfig shopConfig = ADM_ShopConfig.GetConfig(m_ShopConfig);
			if (shopConfig && shopConfig.m_Merchandise) 
			{
				foreach (ADM_ShopMerchandise merch : shopConfig.m_Merchandise) 
				{
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
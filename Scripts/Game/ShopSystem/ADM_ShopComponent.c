class ADM_ShopComponentClass: ScriptComponentClass {}
class ADM_ShopComponent: ScriptComponent
{
	/*
		TODO:
			- Add limited supply 
				- Syncronize m_Merchandise over the network
			- Sell items
			- Supply & Demand
			- Hook for gamemodes to send payments somewhere
				- e.g player owned shops
	*/
	
	//Make RplProp so it is replicated
	[Attribute(), RplProp()]
	protected string m_ShopName;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Config", "conf", category: "Shop")]
	protected ResourceName m_ShopConfig;
	
	[Attribute(defvalue: "", desc: "Merchandise to sell", uiwidget: UIWidgets.Object, category: "Shop")]
	protected ref array<ref ADM_ShopMerchandise> m_AdditionalMerchandise;
	
	[Attribute("", UIWidgets.ResourceNamePicker, "Config", "conf", category: "Shop")]
	ref array<ResourceName> m_Categories;
	
	ref array<ref ADM_ShopCategory> m_LoadedCategories = {};
	
	protected ref array<ref ADM_ShopMerchandise> m_Merchandise;
	protected IEntity m_Owner;
	
	//------------------------------------------------------------------------------------------------
	static bool IsPaymentOnlyCurrency(ADM_ShopMerchandise merchandise)
	{
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetRequiredPaymentToBuy();
		if (requiredPayment.Count() != 1) return false;
		if (requiredPayment[0].ClassName().ToType() != ADM_PaymentMethodCurrency) return false;
		
		return true;
	}
	
	array<ref ADM_ShopCategory> GetCategories()
	{
		return m_LoadedCategories;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetName()
	{
		return m_ShopName;
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
		
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetRequiredPaymentToBuy();
		foreach (ADM_PaymentMethodBase payment : requiredPayment)
		{
			canPurchase = payment.CheckPayment(player, quantity);
			if (!canPurchase) break;
		}
		
		return canPurchase;
	}
	
	bool AskPurchase(IEntity player, ADM_ShopComponent shop, ADM_ShopMerchandise merchandise, int quantity, ADM_PlayerShopManagerComponent playerManager)
	{
		//TODO: Should I check if merchandise is actually in the shop array?
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(player);
		
		bool canPay = shop.CanPurchase(player, merchandise, quantity);
		if (!canPay) 
		{
			playerManager.SetPurchaseMessage("Error! You do not have the required payment.");
			return false;
		}
		
		bool canDeliver = merchandise.GetMerchandise().CanDeliver(player, shop, quantity);
		if (!canDeliver) 
		{
			playerManager.SetPurchaseMessage("Error! Unable to deliver item(s).");
			return false;
		}
		
		if (!merchandise.GetMerchandise().CanPurchaseMultiple() && quantity > 1)
			quantity = 1;
		
		array<ADM_PaymentMethodBase> collectedPaymentMethods = {};
		array<bool> didCollectPayments = {};
		array<ref ADM_PaymentMethodBase> requiredPayment = merchandise.GetRequiredPaymentToBuy();
		foreach (ADM_PaymentMethodBase paymentMethod : requiredPayment) 
		{
			bool didCollectPayment = paymentMethod.CollectPayment(player, quantity);
			didCollectPayments.Insert(didCollectPayment);
			
			Print(didCollectPayment);
			if (didCollectPayment) collectedPaymentMethods.Insert(paymentMethod);
		}
		
		if (didCollectPayments.Contains(false))
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player, quantity);
				if (!returnedPayment) PrintFormat("Error returning payment! %s", paymentMethod.Type().ToString());
			}
			
			playerManager.SetPurchaseMessage("Error! Could not collect payment.");
			return false;
		}
		
		bool deliver = merchandise.GetMerchandise().Deliver(player, shop, quantity);
		if (!deliver) 
		{
			foreach (ADM_PaymentMethodBase paymentMethod : collectedPaymentMethods)
			{
				bool returnedPayment = paymentMethod.ReturnPayment(player, quantity);
				if (!returnedPayment) PrintFormat("Error returning payment! %1", paymentMethod.Type().ToString());
			}
			
			playerManager.SetPurchaseMessage("Error! Can not deliver item(s).");
			return false;
		}
		
		playerManager.SetPurchaseMessage("Purchase Successful!");
		return true;	
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().GetWorldEntity())
  			return;
		
		RplComponent rpl = RplComponent.Cast(owner.FindComponent(RplComponent));
		if (rpl) rpl.InsertToReplication();
		
		foreach (ADM_ShopMerchandise merchandise : m_Merchandise)
		{
			if (!merchandise) continue;
			 
			ADM_MerchandiseType merchandiseType = merchandise.GetMerchandise();
			if (!merchandiseType) continue;
			
			merchandiseType.SetPrefabResource(Resource.Load(merchandiseType.GetPrefab()));
		}
		
		foreach (ResourceName category : m_Categories)
		{
			m_LoadedCategories.Insert(ADM_ShopCategory.GetConfig(category));
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
		owner.SetFlags(EntityFlags.ACTIVE, true);
	}
	
	void ADM_ShopComponent()
	{
		m_Owner = GetOwner();
		
		if (!m_Merchandise) m_Merchandise = new array<ref ADM_ShopMerchandise>();
		
		if (m_ShopConfig != string.Empty) {
			ADM_ShopConfig shopConfig = ADM_ShopConfig.GetConfig(m_ShopConfig);
			if (shopConfig && shopConfig.m_Merchandise) {
				foreach (ADM_ShopMerchandise merch : shopConfig.m_Merchandise) {
					m_Merchandise.Insert(merch);
				}
			}
		}
		
		foreach (ADM_ShopMerchandise merch : m_AdditionalMerchandise) {
			m_Merchandise.Insert(merch);
		}
	} 
}
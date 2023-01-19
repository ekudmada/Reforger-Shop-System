class ADM_PhysicalShopPurchaseAction : ScriptedUserAction
{
	protected IEntity m_Owner;
	protected ADM_PhysicalShopComponent m_Shop;
	
	protected string m_ItemName;
	protected int m_ItemQuantity;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_Owner = pOwnerEntity;
		m_Shop = ADM_PhysicalShopComponent.Cast(m_Owner.FindComponent(ADM_PhysicalShopComponent));
		if (!m_Shop) 
		{
			Print("Error! Could not find physical shop component for this action.", LogLevel.ERROR);
			return;
		}
		
		array<ref ADM_ShopMerchandise> shopConfigs = m_Shop.GetMerchandise();
		if (!shopConfigs) return;
		
		ADM_ShopMerchandise shopConfig;
		if (shopConfigs.Count() > 0) shopConfig = shopConfigs[0];
		if (!shopConfig) return;
		
		m_ItemName = ADM_Utils.GetPrefabDisplayName(shopConfig.GetMerchandise().GetPrefab());
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return;
		
		ADM_ShopMerchandise merchandise = m_Shop.GetMerchandise()[0];
		if (m_Shop.IsPaymentOnlyCurrency(merchandise) || !(merchandise.GetRequiredPaymentToBuy().Count() > 0))
			m_Shop.AskPurchase(merchandise);
		else
			m_Shop.ViewPayment();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		VObject model = m_Shop.GetOwner().GetVObject();
		if (!model) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		//return m_Shop.CanPurchase(user);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_Shop || m_Shop.GetMerchandise().Count() <= 0) return false;
		
		ADM_ShopMerchandise merchandise = m_Shop.GetMerchandise()[0];
		string actionName = "Purchase";
		if (!merchandise.GetRequiredPaymentToBuy().Count() > 0) actionName = "Free";
		if (m_ItemName.Length() > 0) actionName += string.Format(" %1", m_ItemName);
		
		bool currencyOnly = m_Shop.IsPaymentOnlyCurrency(merchandise);
		if (currencyOnly) {
			int cost = ADM_PaymentMethodCurrency.Cast(merchandise.GetRequiredPaymentToBuy()[0]).GetQuantity();
			actionName += string.Format(" ($%1)", cost);
		}
		outName = actionName;
		
		return true;
	}
}
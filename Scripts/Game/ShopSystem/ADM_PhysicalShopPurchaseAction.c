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
		
		ADM_ShopBase shopConfig = m_Shop.GetShopConfig();
		if (!shopConfig) return;
		
		m_ItemName = ADM_Utils.GetPrefabDisplayName(shopConfig.GetPrefab());
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		if (!m_Shop) return;
		
		if (m_Shop.IsPaymentOnlyCurrency() || !(m_Shop.GetRequiredPayment().Count() > 0))
			m_Shop.AskPurchase();
		else
			m_Shop.ViewPayment();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_Shop) return false;
		
		string actionName = "Purchase";
		if (!m_Shop.GetRequiredPayment().Count() > 0) actionName = "Free";
		if (m_ItemName.Length() > 0) actionName += string.Format(" %1", m_ItemName);
		
		bool currencyOnly = m_Shop.IsPaymentOnlyCurrency();
		if (currencyOnly) {
			int cost = ADM_PaymentMethodCurrency.Cast(m_Shop.GetRequiredPayment()[0]).GetQuantity();
			actionName += string.Format(" ($%1)", cost);
		}
		outName = actionName;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_Shop) return false;
		
		//return m_Shop.CanPurchase(user);
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_Shop) return false;
		
		VObject model = m_Shop.GetOwner().GetVObject();
		if (!model) return false;
		
		return true;
	}
};
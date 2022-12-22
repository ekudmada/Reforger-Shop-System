class ADM_PhysicalShopAction : ScriptedUserAction
{
	protected IEntity m_Owner;
	protected ADM_PhysicalShopComponent m_PhysicalShop;
	
	protected string m_ItemName;
	protected int m_ItemQuantity;
	
	string GetPrefabDisplayName(ResourceName resourcePath)
	{
		if (resourcePath == ResourceName.Empty)
			return string.Empty;

		Resource resource = Resource.Load(resourcePath);
		if (!resource)
			return string.Empty;

		BaseResourceObject prefabBase = resource.GetResource();
		if (!prefabBase)
			return string.Empty;
		
		BaseContainer prefabSrc = prefabBase.ToBaseContainer();
		if (!prefabSrc)
			return string.Empty;

		BaseContainerList components = prefabSrc.GetObjectArray("components");
		if (!components)
			return string.Empty;
		
		array<string> acceptableComponents = {"InventoryItemComponent", "WeaponComponent"};
		BaseContainer itemComponent = null;
		for (int c = components.Count() - 1; c >= 0; c--)
		{
			itemComponent = components.Get(c);
			if (acceptableComponents.Contains(itemComponent.GetClassName())) break;
			
			itemComponent = null;
		}

		if (!itemComponent) return string.Empty;

		// Getting UIInfo is broken right now
		// Currently broken as of 11/14/2022. Issue has been resolved as of 11/9/2022, need to wait for game update
		// https://feedback.bistudio.com/T167392
		UIInfo uiInfo;
		itemComponent.Get("UIInfo", uiInfo);
		string displayName = uiInfo.GetName();	
		
		return displayName;
	}
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent) 
	{
		m_Owner = pOwnerEntity;
		m_PhysicalShop = ADM_PhysicalShopComponent.Cast(m_Owner.FindComponent(ADM_PhysicalShopComponent));
		if (!m_PhysicalShop) 
		{
			Print("Error! Could not find physical shop component for this action.", LogLevel.ERROR);
			return;
		}
		
		ADM_PhysicalShopBase shopConfig = m_PhysicalShop.GetShopConfig();
		if (!shopConfig) return;
		
		m_ItemName = GetPrefabDisplayName(shopConfig.GetPrefab());
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		if (!m_PhysicalShop) return;
		
		//if (m_PhysicalShop.IsPaymentOnlyCurrency())
			m_PhysicalShop.AskPurchase();
		//else
		//	m_PhysicalShop.ViewPayment();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_PhysicalShop) return false;
		
		string actionName = "Buy";
		if (m_ItemName.Length() > 0) actionName += string.Format(" %1", m_ItemName);
		
		bool currencyOnly = m_PhysicalShop.IsPaymentOnlyCurrency();
		if (currencyOnly) {
			int cost = ADM_PaymentMethodCurrency.Cast(m_PhysicalShop.GetRequiredPayment()[0]).GetQuantity();
			actionName += string.Format(" ($%1)", cost);
		}
		outName = actionName;	
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_PhysicalShop) return false;
		
		return m_PhysicalShop.CanPurchase(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_PhysicalShop) return false;
		
		VObject model = m_PhysicalShop.GetOwner().GetVObject();
		if (!model) return false;
		
		return true;
	}
};
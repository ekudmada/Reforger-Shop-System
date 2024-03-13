class ADM_PhysicalShopAction : ScriptedUserAction
{
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "1", desc: "Adjustment Step")]
	protected float m_fAdjustmentStep;
	
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "SelectAction", desc: "Input action for increase")]
	protected string m_sActionIncrease;
	
	[Attribute( uiwidget: UIWidgets.Auto, defvalue: "", desc: "Input action for decrease")]
	protected string m_sActionDecrease;
	
	protected IEntity m_Owner;
	protected ADM_PhysicalShopComponent m_Shop;
		
	protected string m_ItemName;
	
	protected float m_fTargetValue;
	
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
		
		array<ref ADM_ShopMerchandise> shopMerchandise = m_Shop.GetMerchandiseBuy();
		if (!shopMerchandise) return;
		
		ADM_ShopMerchandise merchandise;
		if (shopMerchandise.Count() > 0) merchandise = shopMerchandise[0];
		if (!merchandise) return;
		
		ADM_MerchandiseType merchandiseType = merchandise.GetType();
		if (!merchandiseType) return;
		
		m_ItemName = merchandiseType.GetDisplayName();
		m_fTargetValue = m_fAdjustmentStep;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{	
		if (!m_Shop || m_Shop.GetMerchandiseBuy().Count() <= 0) return;
				
		SCR_PlayerController scrPlayerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
		if (!scrPlayerController || pUserEntity != scrPlayerController.GetMainEntity()) 
			return;
		
		// Physical shops should only have one item defined, so just grab the first one in the merchandise array	
		ADM_ShopMerchandise merchandise = m_Shop.GetMerchandiseBuy()[0];
		if (ADM_ShopComponent.IsBuyPaymentOnlyCurrency(merchandise) || !(merchandise.GetBuyPayment().Count() > 0))
		{
			ADM_PlayerShopManagerComponent playerShopManager = ADM_PlayerShopManagerComponent.Cast(scrPlayerController.FindComponent(ADM_PlayerShopManagerComponent));
			if (!playerShopManager) return;
			
			playerShopManager.AskPurchase(m_Shop, merchandise, m_fTargetValue);
		} else {
			m_Shop.ViewPayment(this);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandiseBuy().Count() <= 0) return false;
		
		VObject model = m_Shop.GetOwner().GetVObject();
		if (!model) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_Shop || m_Shop.GetMerchandiseBuy().Count() <= 0) return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (!m_Shop || m_Shop.GetMerchandiseBuy().Count() <= 0) return false;
		
		array<string> label = {"Purchase"};
		ADM_ShopMerchandise merchandise = m_Shop.GetMerchandiseBuy()[0];
		
		if (m_ItemName.Length() > 0) 
			label.Insert(m_ItemName);
		
		bool currencyOnly = ADM_ShopComponent.IsBuyPaymentOnlyCurrency(merchandise);
		if (currencyOnly) 
		{
			int cost = ADM_PaymentMethodCurrency.Cast(merchandise.GetBuyPayment()[0]).GetQuantity();
			if (cost > 0)
			{
				label.Insert(string.Format("($%1)", cost));
			} else {
				label[0] = "Free";
			}
		}
		
		if (m_fAdjustmentStep > 0)
			label.Insert(string.Format("x%1", m_fTargetValue));
		
		string name = "";
		foreach(string part : label)
		{
			name += part + " ";
		}
		
		outName = name.Trim();
		
		return true;
	}
	
	//----------------------------------------------------------------------------------
	override void OnActionSelected()
	{
		super.OnActionSelected();

		if (m_fAdjustmentStep <= 0)
			return;
		
		if (!m_sActionIncrease.IsEmpty())
			GetGame().GetInputManager().AddActionListener(m_sActionIncrease, EActionTrigger.VALUE, HandleActionIncrease);
		
		if (!m_sActionDecrease.IsEmpty())
			GetGame().GetInputManager().AddActionListener(m_sActionDecrease, EActionTrigger.VALUE, HandleActionDecrease);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnActionDeselected()
	{
		super.OnActionDeselected();
		
		if (m_fAdjustmentStep <= 0)
			return;
		
		if (!m_sActionIncrease.IsEmpty())
			GetGame().GetInputManager().RemoveActionListener(m_sActionIncrease, EActionTrigger.VALUE, HandleActionIncrease);
		
		if (!m_sActionDecrease.IsEmpty())
			GetGame().GetInputManager().RemoveActionListener(m_sActionDecrease, EActionTrigger.VALUE, HandleActionDecrease);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool HasLocalEffectOnlyScript()
	{
		return true;
	}
	
	//----------------------------------------------------------------------------------
	override bool CanBroadcastScript() 
	{ 
		return false; 
	};
	
	//------------------------------------------------------------------------------------------------
	void HandleActionIncrease(float value)
	{
		if (m_fAdjustmentStep <= 0)
			return;
		
		if (value > 0.5)
			value = m_fAdjustmentStep;
		else if (value < -0.5)
			value = -m_fAdjustmentStep;
		else
			return;
		
		// Limit to normalized current value +/- adjustment limit
		float previousValue = m_fTargetValue;
		m_fTargetValue += value;
		
		// Round to adjustment step
		m_fTargetValue = Math.Round(m_fTargetValue / m_fAdjustmentStep) * m_fAdjustmentStep;
		if (m_fTargetValue <= m_fAdjustmentStep) m_fTargetValue = m_fAdjustmentStep; // minimum
		
		if (!float.AlmostEqual(m_fTargetValue, previousValue))
			SetSendActionDataFlag();
	}
	
	//------------------------------------------------------------------------------------------------
	void HandleActionDecrease(float value)
	{
		HandleActionIncrease(-value);
	}
	
	//------------------------------------------------------------------------------------------------
	float GetTargetValue()
	{
		return m_fTargetValue;
	}
}
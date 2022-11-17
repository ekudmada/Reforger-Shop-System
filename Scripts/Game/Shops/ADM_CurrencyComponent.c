class SCR_CurrencyPredicate: InventorySearchPredicate
{
	void SCR_CurrencyPredicate()
	{
		QueryComponentTypes.Insert(ADM_CurrencyComponent);
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{		
		return (ADM_CurrencyComponent.Cast(queriedComponents[0])).GetValue() > 0;
	}
};

class ADM_CurrencyComponentClass: ScriptComponentClass
{
};

//! A brief explanation of what this component does.
//! The explanation can be spread across multiple lines.
//! This should help with quickly understanding the script's purpose.
class ADM_CurrencyComponent: ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.EditBox, params: "1 inf")]
	protected int m_Value;
	
	int GetValue()
	{
		return m_Value;
	}
	
	static array<IEntity> FindCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		array<IEntity> currencyItems = {};
		SCR_CurrencyPredicate predicate = new SCR_CurrencyPredicate();
		inventoryManager.FindItems(currencyItems, predicate);
		
		return currencyItems;
	}
	
	static int FindTotalCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		int total = 0;
		
		array<IEntity> currency = ADM_CurrencyComponent.FindCurrencyInInventory(inventoryManager);
		foreach (IEntity currencyEntity: currency)
		{
			ADM_CurrencyComponent currencyComponent = ADM_CurrencyComponent.Cast(currencyEntity.FindComponent(ADM_CurrencyComponent));
			if (!currencyComponent) continue;
			
			total += currencyComponent.GetValue();
		}
		
		return total;
	}
}
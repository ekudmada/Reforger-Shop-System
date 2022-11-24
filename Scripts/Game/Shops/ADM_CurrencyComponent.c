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
}

class ADM_CurrencyComponentClass: ScriptComponentClass {}

//! A brief explanation of what this component does.
//! The explanation can be spread across multiple lines.
//! This should help with quickly understanding the script's purpose.
class ADM_CurrencyComponent: ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.EditBox, params: "0 inf")]
	protected int m_Value;
	
	int GetValue()
	{
		return m_Value;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void SetValue(int value)
	{
		// TODO: reasoning for calling this function should exist. Don't just let any
		// random RPC call modify this value. Though, I am unsure how to implement such a thing.
		m_Value = value;
	}
	
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void ModifyValue(int amount, bool direction)
	{
		// TODO: reasoning for calling this function should exist. Don't just let any
		// random RPC call modify this value. Though, I am unsure how to implement such a thing.
		if (direction)
			m_Value += amount;
		else
			m_Value -= amount;
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
	
	static bool RemoveCurrencyFromInventory(SCR_InventoryStorageManagerComponent inventory, int amount)
	{
		if (!inventory) return false;
		if (amount < 1) return false;
		if (ADM_CurrencyComponent.FindTotalCurrencyInInventory(inventory) < amount) return false;
		
		int originalAmount = amount;
		array<IEntity> currencyItems = ADM_CurrencyComponent.FindCurrencyInInventory(inventory);
		foreach (IEntity item : currencyItems)
		{
			ADM_CurrencyComponent currencyComp = ADM_CurrencyComponent.Cast(item.FindComponent(ADM_CurrencyComponent));
			if (currencyComp)
			{
				int val = currencyComp.GetValue();
				if (val >= amount)
				{
					currencyComp.ModifyValue(amount, false);
					amount = 0;
				} else {
					amount -= val;
					currencyComp.SetValue(0);
				}
			}
		}
		
		if (amount > 0)
		{
			Print(string.Format("Error! Did not remove requested amount of currency from inventory. $%1 was requested, $%2 total remaining.", originalAmount, amount), LogLevel.ERROR);
			return false;
		}
		
		if (amount < 0)
		{
			Print(string.Format("Error! Removed too much currency from inventory. $%1 was requested, net: $%2.", originalAmount, amount), LogLevel.ERROR);
			return false;
		}
		
		return true;
	}
}
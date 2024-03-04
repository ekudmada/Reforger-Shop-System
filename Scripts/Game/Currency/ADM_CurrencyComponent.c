class ADM_CurrencyPredicate: InventorySearchPredicate
{
	void ADM_CurrencyPredicate()
	{
		QueryComponentTypes.Insert(ADM_CurrencyComponent);
	}

	override protected bool IsMatch(BaseInventoryStorageComponent storage, IEntity item, array<GenericComponent> queriedComponents, array<BaseItemAttributeData> queriedAttributes)
	{		
		return ADM_CurrencyComponent.Cast(queriedComponents[0]);
	}
}

class ADM_CurrencyComponentClass: ScriptComponentClass {}
class ADM_CurrencyComponent: ScriptComponent
{
	[Attribute(uiwidget: UIWidgets.EditBox, params: "0 inf"), RplProp()]
	protected int m_Value;
	
	int GetValue()
	{
		return m_Value;
	}

	void SetValue(int value)
	{
		if (!Replication.IsServer())
			return;
		
		m_Value = value;
		Replication.BumpMe();
	}
	
	bool ModifyValue(int amount, bool direction)
	{
		if (!Replication.IsServer())
			return false;
		
		if (amount < 0) return false;
		if (direction)
			m_Value += amount;
		else
			m_Value -= amount;
		
		Replication.BumpMe();
		
		return true;
	}
	
	static array<IEntity> FindCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		array<IEntity> currencyItems = {};
		ADM_CurrencyPredicate predicate = new ADM_CurrencyPredicate();
		inventoryManager.FindItems(currencyItems, predicate);
		
		return currencyItems;
	}
	
	static int FindTotalCurrencyInInventory(SCR_InventoryStorageManagerComponent inventoryManager)
	{
		int total = 0;
		
		array<IEntity> currency = ADM_CurrencyComponent.FindCurrencyInInventory(inventoryManager);
		foreach (IEntity currencyEntity : currency)
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
		if (amount < 1) return true;
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
	
	static bool AddCurrencyToInventory(SCR_InventoryStorageManagerComponent inventory, int amount)
	{
		if (!inventory) return false;
		if (amount < 1) return false;
		
		bool didAdd = false;
		array<IEntity> currencyItems = ADM_CurrencyComponent.FindCurrencyInInventory(inventory);
		foreach (IEntity item : currencyItems)
		{
			ADM_CurrencyComponent currencyComp = ADM_CurrencyComponent.Cast(item.FindComponent(ADM_CurrencyComponent));
			if (currencyComp) 
			{
				currencyComp.ModifyValue(amount, true);
				didAdd = true;
				break;
			}
		}
		
		return didAdd;
	}
}
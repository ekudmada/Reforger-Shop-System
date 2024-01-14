[BaseContainerProps()]
class ADM_PaymentMethodBase: ScriptAndConfig
{
	/*
		This function checks if the player has the desired payment available.
	*/
	bool CheckPayment(IEntity player, int quantity = 1) { return false; }
	
	/*
		This function collects the payment from the player. Used for buying from the shop.
	*/
	bool CollectPayment(IEntity player, int quantity = 1) { return false; }
	
	/*
		This function distributes payment to the player. Used for selling to the shop and in case of merchandise delivery error.
	*/
	bool DistributePayment(IEntity player, int quantity = 1) { return false; }
	
	/*
		How to display this payment method in menus
	*/
	string GetDisplayString(int quantity = 1) { return string.Empty; }
	
	/*
		How to display this payment method in menus
	*/
	ResourceName GetDisplayEntity() { return string.Empty; }
	
	/*
		Check if two payment methods are the same
	*/
	bool Equals(ADM_PaymentMethodBase other) { return false; }
	
	/*
		Add other payment method to this instance
	*/
	bool Add(ADM_PaymentMethodBase other) { return false; }
}
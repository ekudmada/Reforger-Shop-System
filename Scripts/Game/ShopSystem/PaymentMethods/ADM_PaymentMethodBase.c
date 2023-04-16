class ADM_PaymentMethodBase: ScriptAndConfig
{
	/*
		This function checks if the player has the desired payment available.
	*/
	bool CheckPayment(IEntity player, int quantity = 1) { return false; }
	
	/*
		This function collects the payment from the player.
	*/
	bool CollectPayment(IEntity player, int quantity = 1) { return false; }
	
	/*
		This function is used if there is an error during the transaction and the payment method needs to be returened.
	*/
	bool ReturnPayment(IEntity player, int quantity = 1) { return false; }
	
	/*
		How to display this payment method in menus
	*/
	string GetDisplayString(int quantity = 1) { return string.Empty; }
	
	/*
		How to display this payment method in menus
	*/
	ResourceName GetDisplayEntity() { return string.Empty; }
}
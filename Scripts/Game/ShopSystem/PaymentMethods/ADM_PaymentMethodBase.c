class ADM_PaymentMethodBase: ScriptAndConfig
{
	/*
		This function checks if the player has the desired payment available.
	*/
	bool CheckPayment(IEntity player) { return false; }
	
	/*
		This function collects the payment from the player.
	*/
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool CollectPayment(IEntity player) { return false; }
	
	/*
		This function is used if there is an error during the transaction and the payment method needs to be returened.
	*/
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	bool ReturnPayment(IEntity player) { return false; }
	
	/*
		How to display this payment method in menus
	*/
	string GetDisplayString() { return string.Empty; }
}
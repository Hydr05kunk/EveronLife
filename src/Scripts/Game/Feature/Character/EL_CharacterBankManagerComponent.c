[EntityEditorProps(category: "EveronLife/Bank", description: "Player Bank Manager", color: "0 0 255 255")]
class EL_CharacterBankManagerComponentClass: ScriptGameComponentClass
{
}

class EL_CharacterBankManagerComponent : ScriptGameComponent
{
	ref EL_BankAccount m_LocalBankAccount;

	//------------------------------------------------------------------------------------------------
	void Ask_NewTransaction(int amount, string comment)
	{
		if (Replication.IsServer())
			Do_NewTransaction(amount, comment);
		else
			Rpc(Do_NewTransaction, amount, comment);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Server)]
	void Do_NewTransaction(int amount, string comment)
	{
		bool success;
		if (amount == 0)
			return;
		if (amount > 0)
			success = m_LocalBankAccount.TryDeposit(amount, comment);
		else
			success = m_LocalBankAccount.TryWithdraw(-amount, comment);
		if (success)
		{
			Print("[EL-Bank] Transaction Success!");
			Ask_SyncProxyAccount();
		}
		else
			Print("[EL-Bank] Transaction Error!");
	}

	//------------------------------------------------------------------------------------------------
	EL_BankAccount GetAccount()
	{
		return m_LocalBankAccount;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	void SetAccount(EL_BankAccount newAccount)
	{
		m_LocalBankAccount = newAccount;
		Ask_SyncProxyAccount();
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Owner)]
	void Rpc_DoSetLocalBankAccount(EL_BankAccount account, array<string> transactionComments)
	{
		Print("[EL-Bank] Received bank account update");
		
		//Update account
		m_LocalBankAccount = account;
		
		//Update transactions comments
		foreach (int i, string comment : transactionComments)
		{
			m_LocalBankAccount.m_aTransactions[i].m_sComment = comment;
		}
		
		//Update open bank ui
		EL_BankMenu bankMenu = EL_BankMenu.Cast(GetGame().GetMenuManager().FindMenuByPreset(ChimeraMenuPreset.EL_BankMenu));
		if (bankMenu)
			bankMenu.OnMenuFocusGained();
	}
	
	//------------------------------------------------------------------------------------------------
	void Ask_SyncProxyAccount()
	{
		//Workaround to send strings
		array<string> transactionComments = {};
		foreach (EL_BankTransaction transaction : m_LocalBankAccount.m_aTransactions)
		{
			transactionComments.Insert(transaction.m_sComment);
		}
		Print("[EL-Bank] Sending Account to " + EL_Utils.GetPlayerName(GetOwner()));
		Rpc(Rpc_DoSetLocalBankAccount, m_LocalBankAccount, transactionComments);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called from Authority
	void LoadOrCreateAccount(IEntity owner)
	{
		if (m_LocalBankAccount)
		{
			Print("[EL-Bank] Found bank account.. Sending to Owner");
			Ask_SyncProxyAccount();
			return;
		}
		
		Print("[EL-Bank] Creating new Account");
		SetAccount(EL_BankAccount.Create(EL_Utils.GetPlayerUID(owner), 1000));
	}
	
	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		if (Replication.IsServer())
			GetGame().GetCallqueue().CallLater(LoadOrCreateAccount, 10000, false, owner);
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPostInit(IEntity owner)
	{
		SetEventMask(owner, EntityEvent.INIT);
	}
}


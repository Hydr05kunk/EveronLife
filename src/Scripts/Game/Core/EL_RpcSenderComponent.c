[EntityEditorProps(category: "EveronLife/Core")]
class EL_RpcSenderComponentClass : ScriptComponentClass
{
}

class EL_RpcSenderComponent : ScriptComponent
{
    //------------------------------------------------------------------------------------------------
    void AskBuyVehicle(ResourceName vehiclePrefab, int color)
    {
        Rpc(Rpc_AskBuyVehicle, vehiclePrefab, color);
    }
    
    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void Rpc_AskBuyVehicle(ResourceName vehiclePrefab, int color)
    {
        // Logic to handle buying the vehicle without the shop manager
        DoBuyVehicle(vehiclePrefab, color);
    }

    //------------------------------------------------------------------------------------------------  
    void AskLoadGarage(IEntity garageEnt)
    {
        RplComponent rplComp = EL_ComponentFinder<RplComponent>.Find(garageEnt);
        Rpc(Rpc_AskLoadGarage, rplComp.Id());
    }
    
    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void Rpc_AskLoadGarage(RplId garageId)
    {
        RplComponent garageRplComp = RplComponent.Cast(Replication.FindItem(garageId));
        EL_GarageManagerComponent garageManager = EL_ComponentFinder<EL_GarageManagerComponent>.Find(garageRplComp.GetEntity());
        
        garageManager.DoLoadGarage(GetOwner());
    }   
    
    //------------------------------------------------------------------------------------------------  
    void AskWithdrawVehicle(IEntity garageEnt, int index)
    {
        RplComponent rplComp = EL_ComponentFinder<RplComponent>.Find(garageEnt);
        Rpc(Rpc_AskWithdrawVehicle, rplComp.Id(), index);
    }
    
    //------------------------------------------------------------------------------------------------
    [RplRpc(RplChannel.Reliable, RplRcver.Server)]
    void Rpc_AskWithdrawVehicle(RplId garageId, int index)
    {
        RplComponent garageRplComp = RplComponent.Cast(Replication.FindItem(garageId));
        EL_GarageManagerComponent garageManager = EL_ComponentFinder<EL_GarageManagerComponent>.Find(garageRplComp.GetEntity());
        
        garageManager.DoWithdrawVehicle(GetOwner(), index);
    }
    
    //------------------------------------------------------------------------------------------------
    void DoBuyVehicle(ResourceName vehiclePrefab, int color)
    {
        // Implement vehicle buying logic here without the shop manager
    }
}

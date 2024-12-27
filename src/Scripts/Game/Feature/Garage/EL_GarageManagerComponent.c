[EntityEditorProps(category: "EL/Garage")]
class EL_GarageManagerComponentClass : ScriptComponentClass
{
}

[BaseContainerProps()]
class EL_GarageManagerComponent : ScriptComponent
{
    protected ref map<string, ref array<string>> m_mSavedVehicles = new map<string, ref array<string>>;
    IEntity m_UserEntity;
    EL_GarageUI m_GarageUI;

    void DoWithdrawVehicle(IEntity owner, int index)
    {
        IEntity freeSpawnPoint = EL_SpawnUtils.FindFreeSpawnPoint(SCR_EntityHelper.GetMainParent(GetOwner()));
        if (!freeSpawnPoint)
        {
            Print("[EL-Garage] No free spawn point to withdraw!", LogLevel.WARNING);
            return;
        }
        string ownerId = EL_Utils.GetPlayerUID(owner);

        array<string> storedVehicleIds = m_mSavedVehicles.Get(ownerId);
        if (!storedVehicleIds || storedVehicleIds.IsEmpty())
            return;

        string withdrawnVehicleId = storedVehicleIds.Get(index);
        storedVehicleIds.Remove(index);
        m_mSavedVehicles.Set(ownerId, storedVehicleIds);

        // Load and spawn parked vehicle
        IEntity withdrawnVehicle = EPF_PersistentWorldEntityLoader.Load(EPF_VehicleSaveData, withdrawnVehicleId);
        if (!withdrawnVehicle)
        {
            Print("[EL-Garage] Failed to load vehicle with ID: " + withdrawnVehicleId, LogLevel.ERROR);
            return;
        }

        // Teleport to free spawn point
        withdrawnVehicle.SetOrigin(freeSpawnPoint.GetOrigin());
        withdrawnVehicle.SetAngles(freeSpawnPoint.GetAngles());

        // Save data
        EPF_PersistenceComponent persistence = EPF_PersistenceComponent.Cast(withdrawnVehicle.FindComponent(EPF_PersistenceComponent));
        persistence.Save();
        Print("[EL-Garage] Vehicle with ID: " + withdrawnVehicleId + " withdrawn and teleported.");
    }

    void WithdrawVehicle(int index)
    {
        EL_RpcSenderComponent rpcSender = EL_RpcSenderComponent.Cast(m_UserEntity.FindComponent(EL_RpcSenderComponent));
        rpcSender.AskWithdrawVehicle(GetOwner(), index);
    }

    void OpenGarageUI(array<ResourceName> garageSaveDataList)
    {
        m_GarageUI = EL_GarageUI.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.EL_Garage));
        m_GarageUI.SetGarageManager(this);

        if (garageSaveDataList.IsEmpty())
            return;

        m_GarageUI.PopulateVehicleList(garageSaveDataList);
    }

    [RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
    void RPC_SetVehicleSaveData(RplId targetPlayerId, array<ResourceName> garageSaveData)
    {
        RplComponent targetRplComponent = RplComponent.Cast(Replication.FindItem(targetPlayerId));
        IEntity targetPlayer = targetRplComponent.GetEntity();
        IEntity localPlayer = SCR_PlayerController.GetLocalControlledEntity();

        if (targetPlayer != localPlayer)
            return;

        OpenGarageUI(garageSaveData);
    }

    void DoLoadGarage(IEntity pUserEntity)
    {
        array<string> allVehiclesInGarage = GetOwnedVehicles(EL_Utils.GetPlayerUID(pUserEntity));

        array<ResourceName> garageVehicleList = new array<ResourceName>();
        EDF_DbRepository<EPF_VehicleSaveData> vehicleRepo = EPF_PersistenceEntityHelper<EPF_VehicleSaveData>.GetRepository();

        if (allVehiclesInGarage)
        {
            foreach (string vehicleId : allVehiclesInGarage)
            {
                PrintFormat("[EL-Garage] Loading vehicle ID: '%1'", vehicleId);

                EPF_VehicleSaveData vehSaveData = vehicleRepo.Find(vehicleId).GetEntity();
                if (!vehSaveData)
                {
                    PrintFormat("[EL-Garage] Vehicle data for ID '%1' not found!", vehicleId, LogLevel.ERROR);
                    continue;
                }

                if (!vehSaveData.m_rPrefab)
                {
                    PrintFormat("[EL-Garage] Prefab data missing for vehicle ID '%1'!", vehicleId, LogLevel.ERROR);
                    continue;
                }

                garageVehicleList.Insert(vehSaveData.m_rPrefab);
            }
        }

        RplComponent rplC = EL_ComponentFinder<RplComponent>.Find(pUserEntity);
        if (pUserEntity == SCR_PlayerController.GetLocalControlledEntity())
            RPC_SetVehicleSaveData(rplC.Id(), garageVehicleList);
        else
            Rpc(RPC_SetVehicleSaveData, rplC.Id(), garageVehicleList);
    }

    void OpenGarage(IEntity user)
    {
        m_UserEntity = user;

        EL_RpcSenderComponent rpcSender = EL_RpcSenderComponent.Cast(m_UserEntity.FindComponent(EL_RpcSenderComponent));
        rpcSender.AskLoadGarage(GetOwner());
    }

    void AddVehicle(string vehicleId, string ownerId)
    {
        array<string> storedVehicleIds = m_mSavedVehicles.Get(ownerId);
        if (storedVehicleIds)
        {
            storedVehicleIds.Insert(vehicleId);
        }
        else
        {
            storedVehicleIds = new array<string>;
            storedVehicleIds.Insert(vehicleId);
            m_mSavedVehicles.Insert(ownerId, storedVehicleIds);
        }
        Print("Vehicle ID: " + vehicleId + " added for owner: " + ownerId);
    }

    void SetVehicles(map<string, ref array<string>> savedVehicles)
    {
        m_mSavedVehicles = savedVehicles;
        Print("Vehicles data set for the garage.");
    }

    map<string, ref array<string>> GetAllVehicles()
    {
        return m_mSavedVehicles;
    }

    array<string> GetOwnedVehicles(string ownerId)
    {
        array<string> storedVehicleIds = m_mSavedVehicles.Get(ownerId);
        if (!storedVehicleIds)
        {
            storedVehicleIds = new array<string>;
            m_mSavedVehicles.Insert(ownerId, storedVehicleIds);
        }
        Print("Retrieved " + storedVehicleIds.Count() + " vehicles for owner: " + ownerId);
        return storedVehicleIds;
    }
}

class EL_ParkVehicleAction : ScriptedUserAction
{
    [Attribute(defvalue:"10", UIWidgets.EditBox, desc: "Garage Search Radius")]
    private float m_fGarageSearchRadius;

    private EL_GarageManagerComponent m_GarageManager;

    override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
    {
        if (!EL_NetworkUtils.IsOwner(pOwnerEntity)) return;

        GetGame().GetWorld().QueryEntitiesBySphere(pOwnerEntity.GetOrigin(), m_fGarageSearchRadius, FindFirstGarage, FilterGarage);

        if (!m_GarageManager) 
        {
            Print("Garage Manager not found within radius.");
            return;
        }

        // Stop engine and eject passengers
        VehicleControllerComponent vehicleController = VehicleControllerComponent.Cast(pOwnerEntity.FindComponent(VehicleControllerComponent));
        vehicleController.StopEngine();
        Print("Engine stopped.");

        SCR_BaseCompartmentManagerComponent vehicleCompartmentManager = SCR_BaseCompartmentManagerComponent.Cast(pOwnerEntity.FindComponent(SCR_BaseCompartmentManagerComponent));
        array<IEntity> occupants = {};
        vehicleCompartmentManager.GetOccupants(occupants);

        foreach (IEntity occupant : occupants)
        {
            ChimeraCharacter character = ChimeraCharacter.Cast(occupant);
            if (!character)
                continue;

            CharacterControllerComponent controller = character.GetCharacterController();
            CompartmentAccessComponent access = character.GetCompartmentAccessComponent();
            if (!controller || !access)
                continue;

            access.IsGettingOut();
            Print("Occupant ejected: " + character.GetName());
        }

        // Save Vehicle
        EPF_PersistenceComponent persistence = EPF_PersistenceComponent.Cast(pOwnerEntity.FindComponent(EPF_PersistenceComponent));
        EPF_EntitySaveData saveData = persistence.Save();
        persistence.Delete();
        Print("Vehicle saved with ID: " + saveData.GetId());

        // Add to garage
        EL_CharacterOwnerComponent charOwnerComp = EL_CharacterOwnerComponent.Cast(GetOwner().FindComponent(EL_CharacterOwnerComponent));
        m_GarageManager.AddVehicle(saveData.GetId(), charOwnerComp.GetCharacterOwner());
        Print("Vehicle added to garage. Owner: " + charOwnerComp.GetCharacterOwner());

        SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
        Print("Vehicle entity deleted.");
    }

    override bool CanBeShownScript(IEntity user)
    {
        ChimeraCharacter char = ChimeraCharacter.Cast(user);
        return (!char.IsInVehicle() && CanBePerformedScript(user));
    }

    override bool CanBePerformedScript(IEntity user)
    {
        // Check if user is owner of this vehicle
        EL_CharacterOwnerComponent charOwnerComp = EL_CharacterOwnerComponent.Cast(GetOwner().FindComponent(EL_CharacterOwnerComponent));
        if (EL_Utils.GetPlayerUID(user) != charOwnerComp.GetCharacterOwner())
            return false;

        // Check if garage is nearby
        GetGame().GetWorld().QueryEntitiesBySphere(GetOwner().GetOrigin(), m_fGarageSearchRadius, FindFirstGarage, FilterGarage);
        return (m_GarageManager);
    }

    bool FilterGarage(IEntity ent)
    {
        return (ent.FindComponent(EL_GarageManagerComponent));
    }

    bool FindFirstGarage(IEntity ent)
    {
        m_GarageManager = EL_GarageManagerComponent.Cast(ent.FindComponent(EL_GarageManagerComponent));
        if (!m_GarageManager)
            return true; // Continue search

        return false; // Stop search
    }
}

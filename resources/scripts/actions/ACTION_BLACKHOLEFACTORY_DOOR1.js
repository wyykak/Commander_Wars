var Constructor = function()
{
    this.canBePerformed = function(action, map)
    {
        var building = action.getTargetBuilding();
        var x = building.getX() - 2;
        var y = building.getY() + 1;
        var firecount = building.getFireCount();
        if ((firecount === 1 || firecount === 3 || firecount === 5 || firecount === 7) &&
                map.onMap(x, y) && map.getTerrain(x, y).getUnit() === null &&
                ACTION_BLACKHOLEFACTORY_DOOR1.canBuildUnit(action, map))
        {
            return true;
        }
        else
        {
            return false;
        }
    };
    this.getActionText = function(map)
    {
        return qsTr("Build Door 1");
    };
    this.getIcon = function(map)
    {
        return "build";
    };
    this.isFinalStep = function(action, map)
    {
        if (action.getInputStep() === 0)
        {
            return false;
        }
        else
        {
            return true;
        }
    };

    this.getStepInputType = function(action, map)
    {
        // supported types are MENU and FIELD
        if (action.getInputStep() === 0)
        {
            return "MENU";
        }
        return "";
    };

    this.canBuildUnit = function(action, map)
    {
        var building = action.getTargetBuilding();
        var owner = building.getOwner();
        var unitLimit = map.getGameRules().getUnitLimit();
        var unitCount = owner.getUnitCount();
        if ((unitLimit <= 0 ||
             unitCount < unitLimit))
        {
            var x = building.getX() - 2;
            var y = building.getY() + 1;
            var buildlist = owner.getBuildList();
            var units = map.getAllUnitIDs();
            for (var i = 0; i < units.length; i++)
            {
                // check all units if they can move over this terrain
                if (buildlist.includes(units[i]) &&
                        Global[Global[units[i]].getMovementType()].getMovementpoints(map.getTerrain(x, y), null, map.getTerrain(x, y), true) > 0 &&
                        Global[units[i]].getCOSpecificUnit() === false)
                {
                    return true;
                }
            }
        }
        return false;
    };

    this.getStepData = function(action, data, map)
    {
        var building = action.getTargetBuilding();
        var x = building.getX() - 2;
        var y = building.getY() + 1;
        var units = map.getAllUnitIDs();
        var buildlist = building.getOwner().getBuildList();
        var terrain = map.getTerrain(x, y);
        for (var i = 0; i < units.length; i++)
        {
            // check all units if they can move over this terrain
            if (buildlist.includes(units[i]) &&
                Global[Global[units[i]].getMovementType()].getMovementpoints(terrain, null, terrain, true, map) > 0 &&
                Global[units[i]].getCOSpecificUnit() === false)
            {
                var name = Global[units[i]].getName();
                data.addData(name, units[i], units[i], 0, true);
            }
        }
    }

    this.perform = function(action, map)
    {
        action.startReading();
        var unitID = action.readDataString();
        // we need to move the unit to the target position
        var building = action.getTargetBuilding();
        var player = building.getOwner();
        var x = building.getX() - 2;
        var y = building.getY() + 1;
        building.setFireCount(building.getFireCount() - 1);
        // spawn the unit
        var unit = map.spawnUnit(x, y, unitID, player);
        map.getGameRecorder().buildUnit(player.getPlayerID(), unitID, player.getPlayerID());
        player.buildedUnit(unit);

        // achievements
        if (player.getBaseGameInput().getAiType() === GameEnums.AiTypes_Human)
        {
            ACHIEVEMENT_BUILD_UNIT.unitProduced(unitID);
        }
    };
    this.getDescription = function()
    {
        return qsTr("Orders a Black Hole Factory to produce a unit at the first door. ") +
               qsTr("The unit is able to move immediatly.");
    };
}

Constructor.prototype = ACTION;
var ACTION_BLACKHOLEFACTORY_DOOR1 = new Constructor();

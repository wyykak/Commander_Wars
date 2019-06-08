var Constructor = function()
{
    // called for loading the main sprite
    this.canBePerformed = function(action)
    {
        var unit = action.getTargetUnit();
        var actionTargetField = action.getActionTarget();
        var targetField = action.getTarget();

        var test = Qt.point(0, 1);

        if ((unit.getHasMoved() === true))
        {
            return false;
        }
        if ((actionTargetField.x === targetField.x) && (actionTargetField.y === targetField.y) ||
            (action.getMovementTarget() === null))
        {
            for (var i = 0; i < unit.getLoadedUnitCount(); i++)
            {
                if (ACTION_UNLOAD.getUnloadFields(action, i).length > 0)
                {
                    return true;
                }
            }
        }
        return false;

    };
    this.getUnloadFields = function(action, transportUnitIdx)
    {
        var targetField = action.getActionTarget();
        var targetFields = [Qt.point(targetField.x + 1, targetField.y),
                            Qt.point(targetField.x - 1, targetField.y),
                            Qt.point(targetField.x,     targetField.y - 1),
                            Qt.point(targetField.x,     targetField.y + 1)];
        var unit = action.getTargetUnit();
        var targetTerrain = map.getTerrain(targetField.x, targetField.y);
        var transportUnit = unit.getLoadedUnit(transportUnitIdx);
        var ret = [];
        // can both units move over the current terrain?
        var moveType = Global[transportUnit.getMovementType()];
        if ((moveType.getMovementpoints(targetTerrain) > 0) &&
            (Global[unit.getMovementType()].getMovementpoints(targetTerrain) > 0))
        {
            // check all neighbour terrains
            for (var i = 0; i < targetFields.length; i++)
            {
                if (map.onMap(targetFields[i].x, targetFields[i].y))
                {
                    var terrain = map.getTerrain(targetFields[i].x, targetFields[i].y);
                    var defUnit = terrain.getUnit();
                    // can the transported unit move over the terrain?
                    if ((Global[transportUnit.getMovementType()].getMovementpoints(terrain) > 0) &&
                        (defUnit === null || defUnit.isStealthed(unit.getOwner())))
                    {
                        ret.push(targetFields[i]);
                    }
                }
            }
        }
        return ret;
    };

    this.getActionText = function()
    {
        return qsTr("Unload");
    };
    this.getIcon = function()
    {
        return "load";
    };
    this.isFinalStep = function(action)
    {
        // check if the final step was a wait command
        var step = action.getInputStep();
        if (!globals.isEven(step))
        {
            action.startReading();
            for (var i = 0; i < step; i += 2)
            {
                var dataString = action.readDataString();
                if (i === step - 1)
                {
                    if (dataString === "ACTION_WAIT")
                    {
                        return true;
                    }
                }
                else
                {
                    action.readDataInt32();
                    action.readDataInt32();
                }
            }
        }
        return false;
    };

    this.getStepInputType = function(action)
    {
        // supported types are MENU and FIELD
        if (globals.isEven(action.getInputStep()))
        {
            return "MENU";
        }
        else
        {
            return "FIELD";
        }
    };

    this.getStepData = function(action, data)
    {
        action.startReading();
        var step = action.getInputStep();
        var unitIndexes = [];
        var blockedFields = [];
        var fields = null;
        var i3 = 0;
        var i4 = 0;
        var found = false;
        for (var i2 = 0; i2 < step; i2 += 2)
        {
            unitIndexes.push(parseInt(action.readDataString()));
            if (i2 !== step - 1)
            {
                var x = action.readDataInt32();
                var y= action.readDataInt32();
                blockedFields.push(Qt.point(x, y))
            }
        }
        if (globals.isEven(action.getInputStep()))
        {
            var unit = action.getTargetUnit();
            for (var i = 0; i < unit.getLoadedUnitCount(); i++)
            {
                if (unitIndexes.indexOf(i) < 0)
                {
                    fields = ACTION_UNLOAD.getUnloadFields(action, i);
                    for (i3 = 0; i3 < fields.length; i3++)
                    {
                        found = false;
                        for (i4 = 0; i4 < blockedFields.length; i4++)
                        {
                            if (blockedFields[i4] === fields[i3])
                            {
                                found = true;
                                break;
                            }
                        }
                        if (found === false)
                        {
                            var transportUnit = unit.getLoadedUnit(i);
                            data.addData(Global[transportUnit.getUnitID()].getName(), i.toString(), transportUnit.getUnitID());
                            break;
                        }
                    }
                }
            }
            data.addData(qsTr("Wait"), "ACTION_WAIT", "wait");
        }
        else
        {
            // find out which unit we want to unload
            fields = ACTION_UNLOAD.getUnloadFields(action, unitIndexes[unitIndexes.length - 1]);
            for (i3 = 0; i3 < fields.length; i3++)
            {
                found = false;
                for (i4 = 0; i4 < blockedFields.length; i4++)
                {
                    if (blockedFields[i4] === fields[i3])
                    {
                        found = true;
                        break;
                    }
                }
                if (found === false)
                {
                    data.addPoint(fields[i3]);
                }
            }
            data.setColor("#1E90FF");
        }
    };

    this.postAnimationUnit = null;
    this.postAnimationTransportUnits = [];
    this.postAnimationTransportUnitsPosX = [];
    this.postAnimationTransportUnitsPosY = [];
    this.perform = function(action)
    {
        // we need to move the unit to the target position
        ACTION_UNLOAD.postAnimationUnit = action.getTargetUnit();
        var animation = Global[ACTION_UNLOAD.postAnimationUnit.getUnitID()].doWalkingAnimation(action);
        animation.setEndOfAnimationCall("ACTION_UNLOAD", "performPostAnimation");
        // move unit to target position
        ACTION_UNLOAD.postAnimationUnit.moveUnitAction(action);
        // disable unit commandments for this turn
        ACTION_UNLOAD.postAnimationUnit.setHasMoved(true);
        action.startReading();
        var step = action.getInputStep();
        for (var i = 0; i < step; i += 2)
        {
            var dataString = action.readDataString();
            if (i === step - 1)
            {
            }
            else
            {
                var x = action.readDataInt32();
                var y = action.readDataInt32();
                ACTION_UNLOAD.postAnimationTransportUnits.push(ACTION_UNLOAD.postAnimationUnit.getLoadedUnit(parseInt(dataString)));
                ACTION_UNLOAD.postAnimationTransportUnitsPosX.push(x);
                ACTION_UNLOAD.postAnimationTransportUnitsPosY.push(y);
            }
        }
    };
    this.performPostAnimation = function(postAnimation)
    {
        // unloading the units here :)
        for (var i = 0; i < ACTION_UNLOAD.postAnimationTransportUnits.length; i++)
        {
            // check if the field is empty before unloading
            if (map.getTerrain(ACTION_UNLOAD.postAnimationTransportUnitsPosX[i],
                           ACTION_UNLOAD.postAnimationTransportUnitsPosY[i]).getUnit() === null)
            {
                ACTION_UNLOAD.postAnimationUnit.unloadUnit(ACTION_UNLOAD.postAnimationTransportUnits[i],
                                                           Qt.point(ACTION_UNLOAD.postAnimationTransportUnitsPosX[i],
                                                                    ACTION_UNLOAD.postAnimationTransportUnitsPosY[i]));
            }
        }
        audio.playSound("unload.wav");
        ACTION_UNLOAD.postAnimationUnit = null;
        ACTION_UNLOAD.postAnimationTransportUnits = [];
        ACTION_UNLOAD.postAnimationTransportUnitsPosX = [];
        ACTION_UNLOAD.postAnimationTransportUnitsPosY = [];
    };
}

Constructor.prototype = ACTION;
var ACTION_UNLOAD = new Constructor();

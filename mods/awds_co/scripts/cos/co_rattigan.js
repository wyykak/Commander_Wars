CO_RATTIGAN.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                         defender, defPosX, defPosY, isDefender)
{
    if (co.getIsCO0() === true)
    {
        var count = CO_RATTIGAN.getUnitCount(co, defPosX, defPosY);
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            if (count > 0)
            {
                return 40 + count * 5;
            }
            return 0;
        case GameEnums.PowerMode_Power:
            if (count > 0)
            {
                return 20 + count * 5;
            }
            return 0;
        default:
            if (co.inCORange(Qt.point(atkPosX, atkPosY), attacker) &&
                    count > 0)
            {
                return 25;
            }
            break;
        }
        if (count > 0)
        {
            return 5;
        }
        else
        {
            return -10;
        }
    }
};

CO_RATTIGAN.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                          defender, defPosX, defPosY, isDefender)
{
    if (co.getIsCO0() === true)
    {
        var count = CO_RATTIGAN.getUnitCount(co, defPosX, defPosY);
        switch (co.getPowerMode())
        {
        case GameEnums.PowerMode_Tagpower:
        case GameEnums.PowerMode_Superpower:
            if (count > 0)
            {
                return 20;
            }
            return 0;
        case GameEnums.PowerMode_Power:
            return 0;
        default:
            break;
        }
    }
    return 0;
};

CO_RATTIGAN.getMovementpointModifier = function(co, unit, posX, posY)
{
    if (co.getIsCO0() === true)
    {
        if (co.getPowerMode() === GameEnums.PowerMode_Power)
        {
            return 1;
        }
    }
    return 0;
};

CO_ZANDRA.init = function(co, map)
{
    co.setPowerStars(0);
    co.setSuperpowerStars(4);
};
CO_YUKIO.activateSuperpower = function(co, powerMode, map)
{
    CO_ZANDRA.activatePower(co, powerMode, map);
};
CO_ZANDRA.getSuperPowerDescription = function()
{
    return CO_ZANDRA.getPowerDescription();
};
CO_ZANDRA.getSuperPowerName = function()
{
    return CO_ZANDRA.getPowerName();
};
CO_ZANDRA.getOffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                       defender, defPosX, defPosY, isDefender, action, luckmode, map)
{
    if (co.getIsCO0() === true)
    {
        if (map !== null)
        {
            switch (co.getPowerMode())
            {
            case GameEnums.PowerMode_Tagpower:
            case GameEnums.PowerMode_Superpower:
            case GameEnums.PowerMode_Power:
                if (map.getGameRules().getCurrentWeather().getWeatherId() === "WEATHER_SANDSTORM")
                {
                    // apply sandstorm buff :)
                    return 30;
                }
                else
                {
                    return 0;
                }
            default:
                if (map.getGameRules().getCurrentWeather() !== null &&
                        map.getGameRules().getCurrentWeather().getWeatherId() === "WEATHER_SANDSTORM")
                {
                    // apply sandstorm buff :)
                    return 20;
                }
                return 0;
            }
        }
    }
    return 0;
};
CO_ZANDRA.getDeffensiveBonus = function(co, attacker, atkPosX, atkPosY,
                                        defender, defPosX, defPosY, isAttacker, action, luckmode, map)
{
    if (co.getIsCO0() === true)
    {
        if (co.getPowerMode() > GameEnums.PowerMode_Off)
        {
            return 10;
        }
    }
    return 0;
};
CO_ZANDRA.getFirerangeModifier = function(co, unit, posX, posY, map)
{
    if (co.getIsCO0() === true)
    {
        if (map.getGameRules().getCurrentWeather() !== null &&
                map.getGameRules().getCurrentWeather().getWeatherId() === "WEATHER_SANDSTORM")
        {
            if (unit.getBaseMaxRange() > 1)
            {
                switch (co.getPowerMode())
                {
                case GameEnums.PowerMode_Tagpower:
                case GameEnums.PowerMode_Superpower:
                case GameEnums.PowerMode_Power:
                    return 1;
                default:
                    return 0;
                }
            }
        }
    }
    return 0;
};
CO_ZANDRA.getWeatherImmune = function(co, map)
{
    if (co.getIsCO0() === true)
    {
        if (map.getGameRules().getCurrentWeather().getWeatherId() === "WEATHER_SANDSTORM")
        {
            return true;
        }
    }
    return false;
};

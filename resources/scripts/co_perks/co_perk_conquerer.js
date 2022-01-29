var Constructor = function()
{
    this.getCaptureBonus = function(co, unit, posX, posY, map)
    {
		if (CO_PERK.isActive(co))
		{
			return 2;
		}
        return 0;
    };
	// Perk - Intel
    this.getDescription = function()
    {
        return qsTr("Increases the capture rate by 2.");
    };
    this.getIcon = function(map)
    {
        return "invader";
    };
    this.getName = function()
    {
        return qsTr("Conquerer");
    };
};

Constructor.prototype = CO_PERK;
var CO_PERK_CONQUERER = new Constructor();

death_watch_black_sun_thug = Creature:new {
	objectName = "@mob/creature_names:mand_bunker_blksun_thug",
	randomNameType = NAME_GENERIC,
	randomNameTag = true,
	socialGroup = "death_watch",
	faction = "",
	level = 86,
	chanceHit = 0.85,
	damageMin = 570,
	damageMax = 900,
	baseXp = 8130,
	baseHAM = 65000,
	baseHAMmax = 85000,
	armor = 0,
	resists = {40,40,60,35,55,70,35,40,-1},
	meatType = "",
	meatAmount = 0,
	hideType = "",
	hideAmount = 0,
	boneType = "",
	boneAmount = 0,
	milk = 2 * 0,
	tamingChance = 0,
	ferocity = 0,
	pvpBitmask = AGGRESSIVE + ATTACKABLE + ENEMY,
	creatureBitmask = KILLER,
	optionsBitmask = AIENABLED,
	diet = HERBIVORE,
	scale = 1.15,

	templates = {"object/mobile/dressed_black_sun_thug.iff"},
	lootGroups = {
		{
			groups = {
				{group = "death_watch_bunker_commoners", chance = 10000000},
			},
			lootChance = 2500000
		},
		{
			groups = {
				{group = "death_watch_bunker_commoners", chance = 10000000},
			},
			lootChance = 2500000
		},
		{
      groups = {
        {group = "death_watch_bh_armor_schematics", chance = 10000000},
      },
      lootChance = 500000
    },  
		{
      groups = {
        {group = "black_sun_weapons", chance = 10000000},
      },
      lootChance = 500000
    },
    {
      groups = {
        {group = "blacksun_rare", chance = 10000000},
      },
      lootChance = 500000
    },
		{
			groups = {
				{group = "armor_attachments", chance = 10000000},
			},
			lootChance = 1000000
		},
		{
			groups = {
				{group = "clothing_attachments", chance = 10000000},
			},
			lootChance = 500000
		},
	},
	weapons = {"pirate_weapons_heavy"},
	conversationTemplate = "",
	attacks = merge(bountyhuntermaster,marksmanmaster,brawlermaster)
}

CreatureTemplates:addCreatureTemplate(death_watch_black_sun_thug, "death_watch_black_sun_thug")

/*
    Name    : PlayerbotDruidAI.cpp
    Complete: maybe around 30%
    Authors : rrtn, Natsukawa
    Version : 0.41
*/
#include "PlayerbotDruidAI.h"

class PlayerbotAI;

PlayerbotDruidAI::PlayerbotDruidAI(Player* const master, Player* const bot, PlayerbotAI* const ai): PlayerbotClassAI(master, bot, ai)
{
    
    MOONFIRE            = ai->getSpellId("moonfire"); // attacks
    STARFIRE            = ai->getSpellId("starfire");
    STARFALL            = ai->getSpellId("starfall");
    WRATH               = ai->getSpellId("wrath");
    ROOTS               = ai->getSpellId("roots");
	INSECT_SWARM        = ai->getSpellId("insect swarm");
	FORCE_OF_NATURE     = ai->getSpellId("force of nature");
	HURRICANE           = ai->getSpellId("hurricane");
    MARK_OF_THE_WILD    = ai->getSpellId("mark of the wild"); // buffs
    THORNS              = ai->getSpellId("thorns");
	BARKSKIN            = ai->getSpellId("barkskin");
	INNERVATE           = ai->getSpellId("innervate");
	MANA_REJUVENATION   = ai->getSpellId("mana rejuvenation");
    FAERIE_FIRE         = ai->getSpellId("faerie fire"); // debuffs
    REJUVENATION        = ai->getSpellId("rejuvenation"); // heals
    REGROWTH            = ai->getSpellId("regrowth");
	WILD_GROWTH         = ai->getSpellId("wild growth");
    LIFEBLOOM           = ai->getSpellId("lifebloom");
    NOURISH             = ai->getSpellId("nourish");
    HEALING_TOUCH       = ai->getSpellId("healing touch");
	SWIFTMEND           = ai->getSpellId("swiftmend");
    //Druid Forms
	MOONKIN_FORM        = ai->getSpellId("moonkin form");
    DIRE_BEAR_FORM      = ai->getSpellId("dire bear form");
	BEAR_FORM           = ai->getSpellId("bear form");
    CAT_FORM            = ai->getSpellId("cat form");
	TREE_OF_LIFE        = 33891;
    //Cat Attack type's
    RAKE                = ai->getSpellId("rake");            //40 energy
    CLAW                = ai->getSpellId("claw");            //45
    COWER               = ai->getSpellId("cower");           //20
    MANGLE              = ai->getSpellId("mangle");          //45
    TIGERS_FURY         = ai->getSpellId("tigers's fury");
    //Cat Finishing Move's
    RIP                 = ai->getSpellId("rip");             //30
    FEROCIOUS_BITE      = ai->getSpellId("ferocious bite");  //35
    MAIM                = ai->getSpellId("maim");            //35
	//Bear/Dire Bear Attacks
	BASH                = ai->getSpellId("bash");            //10 rage
	MAUL                = ai->getSpellId("maul");            //15
	SWIPE               = ai->getSpellId("swipe");           //20
	DEMORALIZING_ROAR   = ai->getSpellId("roar");            //10
}

PlayerbotDruidAI::~PlayerbotDruidAI() {}

void PlayerbotDruidAI::HealTarget(Unit &target, uint8 hp)
{
    PlayerbotAI* ai = GetAI();
    Player *m_bot = GetPlayerBot();

    if (hp < 70 && REJUVENATION > 0 && !target.HasAura(REJUVENATION, 0) && !target.HasAura(REGROWTH, 0) && ai->GetManaPercent() >=21)
        ai->CastSpell(REJUVENATION, target);

	if (hp < 60 && LIFEBLOOM > 0 && !target.HasAura(LIFEBLOOM, 0) && ai->GetManaPercent() >= 28)
        ai->CastSpell(LIFEBLOOM, target);

    if (hp < 55 && REGROWTH > 0 && !target.HasAura(REGROWTH, 0) && !target.HasAura(REJUVENATION, 0) && ai->GetManaPercent() >= 33)
        ai->CastSpell(REGROWTH, target);

	if (hp < 50 && SWIFTMEND > 0 && target.HasAura(REJUVENATION, 0) || target.HasAura(REGROWTH, 0) && ai->GetManaPercent() >= 16)
        ai->CastSpell(SWIFTMEND, target);

    if (hp < 45 && WILD_GROWTH > 0 && !target.HasAura(WILD_GROWTH, 0) && ai->GetManaPercent() >= 26)
        ai->CastSpell(WILD_GROWTH, target);

	if (hp < 30 && NOURISH > 0 && ai->GetManaPercent() >= 18)
        ai->CastSpell(NOURISH, target);

	if (hp < 25 && HEALING_TOUCH > 0 && ai->GetManaPercent() >= 38)
        ai->CastSpell(HEALING_TOUCH, target);
} // end HealTarget

void PlayerbotDruidAI::DoNextCombatManeuver(Unit *pTarget)
{
    PlayerbotAI* ai = GetAI();
    if (!ai)
        return;

    switch (ai->GetScenarioType())
    {
        case PlayerbotAI::SCENARIO_DUEL:
            ai->CastSpell(MOONFIRE);
            return;
    }

    uint32 masterHP = GetMaster()->GetHealth()*100 / GetMaster()->GetMaxHealth();

    ai->SetInFront( pTarget );
    Player *m_bot = GetPlayerBot();
    Unit* pVictim = pTarget->getVictim();

    if (pVictim && ai->GetHealthPercent() >= 40)
    {
        if (pVictim == m_bot)
            SpellSequence = DruidTank;
    }
    else if (pTarget->GetHealth() > pTarget->GetMaxHealth()*0.8 && pVictim)
    {
        if (pVictim != m_bot)
            SpellSequence = DruidSpell;
    }
    else if (ai->GetHealthPercent() <= 40 || GetMaster()->GetHealth() < GetMaster()->GetMaxHealth()*0.8)
        SpellSequence = DruidHeal;
    else
        SpellSequence = DruidCombat;

    switch (SpellSequence)
    {
        case DruidTank: // Its not a tank druid, only self protecting from heavy damage if got aggro.
            //ai->TellMaster("DruidTank");

            if( !m_bot->HasInArc(M_PI, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }
            if(m_bot->HasAura(CAT_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //ai->TellMaster("FormClearCat");
            }
			if (MOONKIN_FORM > 0 && !m_bot->HasAura(MOONKIN_FORM, 0))
            {
                ai->CastSpell (MOONKIN_FORM);
            }
            else if (DIRE_BEAR_FORM > 0 && !m_bot->HasAura(MOONKIN_FORM, 0) && !m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                ai->CastSpell (DIRE_BEAR_FORM);
            }
			else if (BEAR_FORM > 0 && !m_bot->HasAura(MOONKIN_FORM, 0) && !m_bot->HasAura(DIRE_BEAR_FORM, 0) && !m_bot->HasAura(BEAR_FORM, 0))
            {
                ai->CastSpell (BEAR_FORM);
            }
			else if (DEMORALIZING_ROAR > 0 && m_bot->HasAura(DIRE_BEAR_FORM, 0) || m_bot->HasAura(BEAR_FORM, 0) && !m_bot->HasAura(MOONKIN_FORM, 0) && !pTarget->HasAura(DEMORALIZING_ROAR, 0) && ai->GetRageAmount() >= 10)
            {
                ai->CastSpell(DEMORALIZING_ROAR, *pTarget);
            }
			if (FAERIE_FIRE > 0 && DruidSpellCombat < 1 && !pTarget->HasAura(FAERIE_FIRE, 0))
            {
                ai->CastSpell(FAERIE_FIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (MOONFIRE > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && DruidSpellCombat < 2 && !pTarget->HasAura(MOONFIRE, 0) && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell(MOONFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (ROOTS > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && DruidSpellCombat < 3 && !pTarget->HasAura(ROOTS, 0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(ROOTS, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (HURRICANE > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && ai->GetAttackerCount()>=5 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 91)
            {
				//ai->TellMaster("casting hurricane!");
                ai->CastSpell(HURRICANE, *pTarget);
				ai->SetIgnoreUpdateTime(10);
                DruidSpellCombat++;
                break;
            }
            else if (WRATH > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && DruidSpellCombat < 5 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(WRATH, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (INSECT_SWARM > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && DruidSpellCombat < 6 && !pTarget->HasAura(INSECT_SWARM, 0) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(INSECT_SWARM , *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (STARFIRE > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && DruidSpellCombat < 7 && ai->GetManaPercent() >= 18)
            {
                ai->CastSpell(STARFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (FORCE_OF_NATURE > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && DruidSpellCombat < 8 && ai->GetManaPercent() >= 12)
            {
				//ai->TellMaster("summoning treants.");
                ai->CastSpell(FORCE_OF_NATURE);
                DruidSpellCombat++;
                break;
            }
			else if (STARFALL > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && ai->GetAttackerCount()>=3 && DruidSpellCombat < 9 && ai->GetManaPercent() >= 39)
            {
                ai->CastSpell(STARFALL, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (BARKSKIN > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && ai->GetHealthPercent() < 50 && DruidSpellCombat < 10 && !m_bot->HasAura(BARKSKIN, 0))
            {
                ai->CastSpell(BARKSKIN, *m_bot);
                DruidSpellCombat++;
                break;
            }
			else if (INNERVATE > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && ai->GetManaPercent() < 50 && DruidSpellCombat < 11 && !m_bot->HasAura(INNERVATE, 0))
            {
                ai->CastSpell(INNERVATE, *m_bot);
                DruidSpellCombat++;
                break;
            }
			else if (THORNS > 0 && m_bot->HasAura(MOONKIN_FORM, 0) && ai->GetManaPercent() >= 19 && DruidSpellCombat < 12 && !m_bot->HasAura(THORNS, 0))
            {
                ai->CastSpell(THORNS, *m_bot);
                DruidSpellCombat++;
                break;
            }
			else if (DruidSpellCombat > 13)
            {
                DruidSpellCombat = 0;
                break;
            }
            else
            {
                DruidSpellCombat = 0;
                break;
            }
            break;

        case DruidSpell:
            //ai->TellMaster("DruidSpell");
            if(m_bot->HasAura(CAT_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //ai->TellMaster("FormClearCat");
                break;
            }
            if(m_bot->HasAura(BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(5487);
                //ai->TellMaster("FormClearBear");
                break;
            }
			if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //ai->TellMaster("FormClearDireBear");
                break;
            }
			if(m_bot->HasAura(MOONKIN_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(24858);
                //ai->TellMaster("FormClearMoonkin");
                break;
            }
            if (FAERIE_FIRE > 0 && DruidSpellCombat < 1 && !pTarget->HasAura(FAERIE_FIRE, 0) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(FAERIE_FIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (MOONFIRE > 0 && DruidSpellCombat < 2 && (!pTarget->HasAura(MOONFIRE, 0)) && ai->GetManaPercent() >= 24)
            {
                ai->CastSpell(MOONFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (ROOTS > 0 && DruidSpellCombat < 3 && (!pTarget->HasAura(ROOTS, 0)) && ai->GetManaPercent() >= 8)
            {
                ai->CastSpell(ROOTS, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (HURRICANE > 0 && ai->GetAttackerCount()>=5 && DruidSpellCombat < 4 && ai->GetManaPercent() >= 91)
            {
				//ai->TellMaster("casting hurricane!");
                ai->CastSpell(HURRICANE, *pTarget);
				ai->SetIgnoreUpdateTime(10);
                DruidSpellCombat++;
                break;
            }
			else if (INNERVATE > 0 && ai->GetManaPercent() < 50 && DruidSpellCombat < 5 && !m_bot->HasAura(INNERVATE, 0))
            {
                ai->CastSpell(INNERVATE, *m_bot);
                DruidSpellCombat++;
                break;
            }
            else if (WRATH > 0 && DruidSpellCombat < 6 && ai->GetManaPercent() >= 13)
            {
                ai->CastSpell(WRATH, *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (INSECT_SWARM > 0 && DruidSpellCombat < 7 && !pTarget->HasAura(INSECT_SWARM, 0) && ai->GetManaPercent() >= 9)
            {
                ai->CastSpell(INSECT_SWARM , *pTarget);
                DruidSpellCombat++;
                break;
            }
            else if (STARFIRE > 0 && DruidSpellCombat < 8 && ai->GetManaPercent() >= 18)
            {
                ai->CastSpell(STARFIRE, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (FORCE_OF_NATURE > 0 && DruidSpellCombat < 9 && ai->GetManaPercent() >= 12)
            {
				//ai->TellMaster("summoning treants.");
                ai->CastSpell(FORCE_OF_NATURE);
                DruidSpellCombat++;
                break;
            }
			else if (STARFALL > 0 && ai->GetAttackerCount()>=3 && DruidSpellCombat < 10 && ai->GetManaPercent() >= 39)
            {
                ai->CastSpell(STARFALL, *pTarget);
                DruidSpellCombat++;
                break;
            }
			else if (BARKSKIN > 0 && ai->GetHealthPercent() < 50 && DruidSpellCombat < 11 && !m_bot->HasAura(BARKSKIN, 0))
            {
                ai->CastSpell(BARKSKIN, *m_bot);
                DruidSpellCombat++;
                break;
            }
			else if (MANA_REJUVENATION > 0 && ai->GetManaPercent() < 50 && DruidSpellCombat < 12 && !m_bot->HasAura(MANA_REJUVENATION, 0))
            {
                ai->CastSpell(MANA_REJUVENATION, *m_bot);
                DruidSpellCombat++;
                break;
            }
			else if (DruidSpellCombat > 13)
            {
                DruidSpellCombat = 0;
                break;
            }
            else
            {
                DruidSpellCombat = 0;
                break;
            }
            break;

        case DruidHeal:
            //ai->TellMaster("DruidHeal");
            if(m_bot->HasAura(CAT_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(768);
                //ai->TellMaster("FormClearCat");
                break;
            }
            if(m_bot->HasAura(BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(5487);
                //ai->TellMaster("FormClearBear");
                break;
            }
            if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //ai->TellMaster("FormClearDireBear");
                break;
            }
			if(m_bot->HasAura(MOONKIN_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(24858);
                //ai->TellMaster("FormClearMoonkin");
                break;
            }
            if (ai->GetHealthPercent() < 70)
            {
                HealTarget (*GetPlayerBot(), ai->GetHealthPercent());
                break;
            }
            if (masterHP < 70)
            {
                HealTarget (*GetMaster(), masterHP);
                break;
            }
            else
            {
                DruidSpellCombat = 0;
                break;
            }
            break;

        case DruidCombat:
            //ai->TellMaster("DruidCombat");
            if( !m_bot->HasInArc(M_PI, pTarget))
            {
                m_bot->SetInFront(pTarget);
                if (pVictim)
                    pVictim->Attack(pTarget, true);
            }
            if(m_bot->HasAura(BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(5487);
                //ai->TellMaster("FormClearBear");
                break;
            }
			if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(9634);
                //ai->TellMaster("FormClearDireBear");
                break;
            }
			if(m_bot->HasAura(MOONKIN_FORM, 0))
            {
                m_bot->RemoveAurasDueToSpell(24858);
                //ai->TellMaster("FormClearMoonkin");
                break;
            }
            if (CAT_FORM > 0 && !m_bot->HasAura(CAT_FORM, 0))
                ai->CastSpell (CAT_FORM);
/*
            if (COWER > 0 && m_bot->GetComboPoints() == 1 && ai->GetEnergyAmount() >= 20)
            {
                ai->CastSpell(COWER);
                //ai->TellMaster("Cower");
            }*/
            if (MAIM > 0 && m_bot->GetComboPoints() >= 1 && pTarget->IsNonMeleeSpellCasted(true))
            {
                ai->CastSpell(MAIM, *pTarget);
                //ai->TellMaster("SpellPreventing Maim");
                break;
            }

            if (RAKE > 0 && m_bot->GetComboPoints() <= 1 && ai->GetEnergyAmount() >= 40)
            {
                ai->CastSpell (RAKE, *pTarget);
                //ai->TellMaster("Rake");
                break;
            }
            else if (CLAW > 0 && m_bot->GetComboPoints() <= 2 && ai->GetEnergyAmount() >= 45)
            {
                ai->CastSpell (CLAW, *pTarget);
                //ai->TellMaster("Claw");
                break;
            }
            else if (MANGLE > 0 && m_bot->GetComboPoints() <= 3 && ai->GetEnergyAmount() >= 45)
            {
                ai->CastSpell (MANGLE, *pTarget);
                //ai->TellMaster("Mangle");
                break;
            }
            else if (CLAW > 0 && m_bot->GetComboPoints() <= 4 && ai->GetEnergyAmount() >= 45)
            {
                ai->CastSpell (CLAW, *pTarget);
                //ai->TellMaster("Claw2");
                break;
            }

            if (m_bot->GetComboPoints() == 5)
            {
                if (RIP > 0 && pTarget->getClass() == CLASS_ROGUE && ai->GetEnergyAmount() >= 30)
                {
                    ai->CastSpell(RIP, *pTarget);
                    //ai->TellMaster("Rogue Rip");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_DRUID && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //ai->TellMaster("Druid Maim");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_SHAMAN && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //ai->TellMaster("Shaman Maim");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_WARLOCK && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //ai->TellMaster("Warlock Maim");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_HUNTER && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //ai->TellMaster("Hunter Ferocious Bite");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_WARRIOR && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //ai->TellMaster("Warrior Ferocious Bite");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_PALADIN && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //ai->TellMaster("Paladin Ferocious Bite");
                }
                else if (FEROCIOUS_BITE > 0 && pTarget->getClass() == CLASS_DEATH_KNIGHT && ai->GetEnergyAmount() >= 25)
                {
                    ai->CastSpell(FEROCIOUS_BITE, *pTarget);
                    //ai->TellMaster("DK Ferocious Bite");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_MAGE && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //ai->TellMaster("Mage Maim");
                }
                else if (MAIM > 0 && pTarget->getClass() == CLASS_PRIEST && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //ai->TellMaster("Priest Maim");
                }
                else if (MAIM > 0 && ai->GetEnergyAmount() >= 35)
                {
                    ai->CastSpell(MAIM, *pTarget);
                    //ai->TellMaster("Else Maim");
                }
                break;
			}
            else
            {
				DruidSpellCombat = 0;
				break;
            }
            break;
    }
} // end DoNextCombatManeuver

void PlayerbotDruidAI::DoNonCombatActions()
{
    Player * m_bot = GetPlayerBot();
    if (!m_bot)
        return;

    if(m_bot->HasAura(CAT_FORM, 0))
    {
        m_bot->RemoveAurasDueToSpell(768);
        //GetAI()->TellMaster("FormClearCat");
    }
    if(m_bot->HasAura(BEAR_FORM, 0))
    {
        m_bot->RemoveAurasDueToSpell(5487);
        //ai->TellMaster("FormClearBear");
    }
	if(m_bot->HasAura(DIRE_BEAR_FORM, 0))
    {
        m_bot->RemoveAurasDueToSpell(9634);
        //ai->TellMaster("FormClearDireBear");
    }
	if(m_bot->HasAura(MOONKIN_FORM, 0))
    {
        m_bot->RemoveAurasDueToSpell(24858);
        //ai->TellMaster("FormClearMoonkin");
    }
/*
    // mana myself with MANA_REJUVENATION (*moved to combat: interferes with drinking/eating)
	if (GetAI()->GetManaPercent() < 50 && MANA_REJUVENATION > 0 && !m_bot->HasAura(MANA_REJUVENATION, 0))
	  	GetAI()->CastSpell(MANA_REJUVENATION, *m_bot);
*/
    // buff myself with MARK_OF_THE_WILD
    if (MARK_OF_THE_WILD > 0 && !m_bot->HasAura(MARK_OF_THE_WILD, 0))
        GetAI()->CastSpell (MARK_OF_THE_WILD, *m_bot);

    // Thorns generates aggro for moonkin
    if (THORNS > 0 && !m_bot->HasAura(THORNS, 0))
        GetAI()->CastSpell (THORNS, *m_bot);
/*
    // buff master with THORNS
    if (THORNS > 0 && !GetMaster()->HasAura(THORNS,0))
        GetAI()->CastSpell (THORNS, *(GetMaster()));
*/
    // mana check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    Item* pItem = GetAI()->FindDrink();

    if (pItem != NULL && GetAI()->GetManaPercent() < 15)
    {
        GetAI()->TellMaster("I could use a drink.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }

    // hp check
    if (m_bot->getStandState() != UNIT_STAND_STATE_STAND)
        m_bot->SetStandState(UNIT_STAND_STATE_STAND);

    pItem = GetAI()->FindFood();

    if (pItem != NULL && GetAI()->GetHealthPercent() < 15)
    {
        GetAI()->TellMaster("I could use some food.");
        GetAI()->UseItem(*pItem);
        GetAI()->SetIgnoreUpdateTime(30);
        return;
    }
    
    // buff and heal master's group
    if (GetMaster()->GetGroup())
    {
        Group::MemberSlotList const& groupSlot = GetMaster()->GetGroup()->GetMemberSlots();
        for (Group::member_citerator itr = groupSlot.begin(); itr != groupSlot.end(); itr++)
        {
            Player *tPlayer = objmgr.GetPlayer(uint64 (itr->guid));
            if( !tPlayer || !tPlayer->isAlive() )
                continue;

             // buff and heal
             (!tPlayer->HasAura(MARK_OF_THE_WILD,0) && GetAI()->CastSpell (MARK_OF_THE_WILD, *tPlayer));
			 //(!tPlayer->HasAura(THORNS,0) && GetAI()->CastSpell (THORNS, *tPlayer));
             (HealTarget(*tPlayer, tPlayer->GetHealth()*100 / tPlayer->GetMaxHealth()));
        }
    }
} // end DoNonCombatActions

void PlayerbotDruidAI::BuffPlayer(Player* target)
{
    GetAI()->CastSpell(MARK_OF_THE_WILD, *target);
}

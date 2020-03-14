/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2011-2016 ArkCORE <http://www.arkania.net/>
 * Copyright (C) 2006-2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
SDName: Guards
SD%Complete: 100
SDComment:
SDCategory: Guards
EndScriptData */

/* ContentData
guard_generic
guard_shattrath_aldor
guard_shattrath_scryer
EndContentData */

#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "GuardAI.h"
#include "Player.h"
#include "SpellInfo.h"

enum GuardGeneric
{
    GENERIC_CREATURE_COOLDOWN       = 5000,

    SAY_GUARD_SIL_AGGRO             = 0,

    NPC_STORMWIND_CITY_GUARD        = 68,
    NPC_STORMWIND_ROYAL_GUARD       = 1756,
    NPC_STORMWIND_CITY_PATROLLER    = 1976,
    NPC_ORGRIMMAR_GRUNT             = 3296,
    NPC_KORKRON_ELITE               = 14304,
    NPC_CENARION_HOLD_INFANTRY      = 15184,
};

class guard_generic : public CreatureScript
{
public:
    guard_generic() : CreatureScript("guard_generic") { }

    struct guard_genericAI : public GuardAI
    {
        guard_genericAI(Creature* creature) : GuardAI(creature) { }

        void Reset() override
        {
            globalCooldown1 = 0;
            globalCooldown2 = 0;
            events.Reset();
            buffTimer = 0;
            m_timer1 = 1000;
            m_timer2 = 1000;
        }

        void EnterCombat(Unit* who) override
        {
            if (me->GetEntry() == NPC_CENARION_HOLD_INFANTRY)
                Talk(SAY_GUARD_SIL_AGGRO, who);
            if (SpellInfo const* spell = me->reachWithSpellAttack(who))
                DoCast(who, spell->Id);
        }

        void UpdateAI(uint32 diff) override
        {
            static uint64 playerguid;
            events.Update(diff);

            DoWork(diff);

             //Always decrease our global cooldown first
            if (globalCooldown1 > diff)
                globalCooldown1 -= diff;
            else
                globalCooldown1 = 0;

            if (globalCooldown2 > diff)
                globalCooldown2 -= diff;
            else
                globalCooldown2 = 0;

            //Buff for Love is in the Air
            if (me->IsAlive() && IsEventActive(8) && !me->IsInCombat() && !me->HasAura(71507) &&
                (me->GetEntry() == NPC_STORMWIND_CITY_GUARD || me->GetEntry() == NPC_STORMWIND_CITY_PATROLLER || me->GetEntry() == NPC_ORGRIMMAR_GRUNT))
                me->AddAura(71507, me);
            else if (!IsEventActive(8) && me->HasAura(71507))
                me->RemoveAura(71507);

            Player * pplayer = me->SelectNearestPlayer(20);

            //Fun emotes they do to players
            if (pplayer && pplayer->GetGUIDLow() != playerguid)
            {
                playerguid = pplayer->GetGUIDLow();
                if (me->GetDistance(pplayer->GetPositionX(), pplayer->GetPositionY(), pplayer->GetPositionZ()) <= 7 && me->IsAlive() && !me->IsInCombat() && (me->GetEntry() == NPC_STORMWIND_CITY_GUARD
                    || me->GetEntry() == NPC_STORMWIND_CITY_PATROLLER || me->GetEntry() == NPC_ORGRIMMAR_GRUNT) && globalCooldown2 == 0)
                {
                    if ((pplayer->GetQuestStatus(13188) != QUEST_STATUS_NONE && pplayer->GetQuestStatus(13188) != QUEST_STATUS_REWARDED) || (pplayer->GetQuestStatus(13189) != QUEST_STATUS_NONE && pplayer->GetQuestStatus(13189) != QUEST_STATUS_REWARDED))
                        events.ScheduleEvent(3, 100);
                    else
                        events.ScheduleEvent(urand(1, 2), 100);
                }
            }
            else if (!pplayer)
                playerguid = 0;
            while (uint32 eventId = events.ExecuteEvent())
            {
                switch (eventId)
                {
                case 1:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    events.Reset();
                    events.ScheduleEvent(4, 100);
                    break;
                case 2:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    events.Reset();
                    events.ScheduleEvent(4, 100);
                    break;
                case 4:

                    globalCooldown2 = urand(9000, 10000);
                    events.Reset();
                    break;
                case 3:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_RUDE);
                    int ur = urand(1, 3);
                    if (ur == 1)
                        me->CastSpell(pplayer, 58511, true);
                    else if (ur == 2)
                        me->CastSpell(pplayer, 58514, true);
                    else
                        me->CastSpell(pplayer, 58519, true);
                    int ura = urand(1, 5);
                    if (ura == 1)
                        me->MonsterYell("How dare you set foot in our city!", 0, 0);
                    else if (ura == 2)
                        me->MonsterYell("My family was wiped out by the scourge! MONSTER!", 0, 0);
                    else if (ura == 3)
                        me->MonsterYell("GET A ROPE!", 0, 0);
                    else if (ura == 4)
                        me->MonsterYell("Traitorous dog!", 0, 0);
                    else if (ura == 5)
                        me->MonsterYell("Monster!", 0, 0);
                    events.Reset();
                    events.ScheduleEvent(4, 100);
                    break;

                }
            }

            //Buff timer (only buff when we are alive and not in combat
            if (me->IsAlive() && !me->IsInCombat())
            {
                if (buffTimer <= diff)
                {
                    //Find a spell that targets friendly and applies an aura (these are generally buffs)
                    SpellInfo const* info = SelectSpell(me, 0, 0, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_AURA);

                    if (info && !globalCooldown1)
                    {
                        //Cast the buff spell
                        DoCast(me, info->Id);

                        //Set our global cooldown
                        globalCooldown1 = GENERIC_CREATURE_COOLDOWN;

                        //Set our timer to 10 minutes before rebuff
                        buffTimer = 600000;
                    }                                                   //Try again in 30 seconds
                    else buffTimer = 30000;
                } else buffTimer -= diff;
            }

            //Return since we have no target
            if (!UpdateVictim())
                return;

            // Make sure our attack is ready and we arn't currently casting
            if (me->isAttackReady() && !me->IsNonMeleeSpellCast(false))
            {
                //If we are within range melee the target
                if (me->IsWithinMeleeRange(me->GetVictim()))
                {
                    bool healing = false;
                    SpellInfo const* info = nullptr;

                    //Select a healing spell if less than 30% hp
                    if (me->HealthBelowPct(30))
                        info = SelectSpell(me, 0, 0, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);

                    //No healing spell available, select a hostile spell
                    if (info)
                        healing = true;
                    else
                        info = SelectSpell(me->GetVictim(), 0, 0, SELECT_TARGET_ANY_ENEMY, 0, 0, 0, 0, SELECT_EFFECT_DONTCARE);

                    //20% chance to replace our white hit with a spell
                    if (info && urand(0, 99) < 20 && !globalCooldown1)
                    {
                        //Cast the spell
                        if (healing)
                            DoCast(me, info->Id);
                        else
                            DoCastVictim(info->Id);

                        //Set our global cooldown
                        globalCooldown1 = GENERIC_CREATURE_COOLDOWN;
                    }
                    else
                        me->AttackerStateUpdate(me->GetVictim());

                    me->ResetAttackTimer();
                }
            }
            else
            {
                //Only run this code if we arn't already casting
                if (!me->IsNonMeleeSpellCast(false))
                {
                    bool healing = false;
                    SpellInfo const* info = nullptr;

                    //Select a healing spell if less than 30% hp ONLY 33% of the time
                    if (me->HealthBelowPct(30) && 33 > urand(0, 99))
                        info = SelectSpell(me, 0, 0, SELECT_TARGET_ANY_FRIEND, 0, 0, 0, 0, SELECT_EFFECT_HEALING);

                    //No healing spell available, See if we can cast a ranged spell (Range must be greater than ATTACK_DISTANCE)
                    if (info)
                        healing = true;
                    else
                        info = SelectSpell(me->GetVictim(), 0, 0, SELECT_TARGET_ANY_ENEMY, 0, 0, NOMINAL_MELEE_RANGE, 0, SELECT_EFFECT_DONTCARE);

                    //Found a spell, check if we arn't on cooldown
                    if (info && !globalCooldown1)
                    {
                        //If we are currently moving stop us and set the movement generator
                        if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != IDLE_MOTION_TYPE)
                        {
                            me->GetMotionMaster()->Clear(false);
                            me->GetMotionMaster()->MoveIdle();
                        }

                        //Cast spell
                        if (healing)
                            DoCast(me, info->Id);
                        else
                            DoCastVictim(info->Id);

                        //Set our global cooldown
                        globalCooldown1 = GENERIC_CREATURE_COOLDOWN;
                    }                                               //If no spells available and we arn't moving run to target
                    else if (me->GetMotionMaster()->GetCurrentMovementGeneratorType() != CHASE_MOTION_TYPE)
                    {
                        //Cancel our current spell and then mutate new movement generator
                        me->InterruptNonMeleeSpells(false);
                        me->GetMotionMaster()->Clear(false);
                        me->GetMotionMaster()->MoveChase(me->GetVictim());
                    }
                }
            }

            DoMeleeAttackIfReady();
        }

        void DoReplyToTextEmote(uint32 emote)
        {
            switch (emote)
            {
                case TEXT_EMOTE_KISS:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_BOW);
                    break;

                case TEXT_EMOTE_WAVE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_WAVE);
                    break;

                case TEXT_EMOTE_SALUTE:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_SALUTE);
                    break;

                case TEXT_EMOTE_SHY:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_FLEX);
                    break;

                case TEXT_EMOTE_RUDE:
                case TEXT_EMOTE_CHICKEN:
                    me->HandleEmoteCommand(EMOTE_ONESHOT_POINT);
                    break;
            }
        }

        void ReceiveEmote(Player* player, uint32 textEmote) override
        {
            switch (me->GetEntry())
            {
                case NPC_STORMWIND_CITY_GUARD:
                case NPC_STORMWIND_CITY_PATROLLER:
                case NPC_ORGRIMMAR_GRUNT:
                    break;
                default:
                    return;
            }

            if (!me->IsFriendlyTo(player))
                return;

            DoReplyToTextEmote(textEmote);
        }
        
        void DoWork(uint32 diff)
        {
            if (m_timer1 < diff)
            {
                m_timer1 = 1000;
                DoWorkForQuest13188();
            }
            else
                m_timer1 -= diff;

            if (m_timer2 < diff)
            {
                m_timer2 = 1000;
                DoWorkForQuest13189();
            }
            else
                m_timer2 -= diff;
        }

        void DoWorkForQuest13188() // ally
        {
            if (me->GetZoneId() != 1519)
                return;
            if (me->GetEntry() != 68 && me->GetEntry() != 1756 && me->GetEntry() != 1976)
                return;
            Player* player = me->FindNearestPlayer(10.0f);
            if (!player || player->GetQuestStatus(13188) != QUEST_STATUS_COMPLETE)
                return;
            switch (urand(1,25))
            {
                case 1:
                    me->CastSpell(player, 58511);
                    Talk(2);
                    m_timer1 = 8000;
                    break;
                case 2:
                    me->CastSpell(player, 58514);
                    Talk(3);
                    m_timer1 = 8000;
                    break;
                case 3:
                    me->CastSpell(player, 58519);
                    Talk(4);
                    m_timer1 = 8000;
                    break;
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                    Talk(5);
                    m_timer1 = 8000;
                    break;
             }

        }

        void DoWorkForQuest13189() // horde
        {
            if (me->GetZoneId() != 1637 || me->GetEntry() != 14304)
                return;
            Player* player = me->FindNearestPlayer(10.0f);
            if (!player || player->GetQuestStatus(13189) != QUEST_STATUS_COMPLETE)
                return;
            switch (urand(1, 25))
            {
            case 1:
                me->CastSpell(player, 58511);
                Talk(2);
                m_timer2 = 8000;
                break;
            case 2:
                me->CastSpell(player, 58514);
                Talk(3);
                m_timer2 = 8000;
                break;
            case 3:
                me->CastSpell(player, 58519);
                Talk(4);
                m_timer2 = 8000;
                break;
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                Talk(5);
                m_timer2 = 8000;
                break;
            }
        }

    private:
        EventMap events;
        uint32 globalCooldown1;
        uint32 globalCooldown2;
        uint32 buffTimer;
        uint32 m_timer1;
        uint32 m_timer2;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
       return new guard_genericAI(creature);
    }
};

enum GuardShattrath
{
    SPELL_BANISHED_SHATTRATH_A = 36642,
    SPELL_BANISHED_SHATTRATH_S = 36671,
    SPELL_BANISH_TELEPORT      = 36643,
    SPELL_EXILE                = 39533
};

class guard_shattrath_scryer : public CreatureScript
{
public:
    guard_shattrath_scryer() : CreatureScript("guard_shattrath_scryer") { }

    struct guard_shattrath_scryerAI : public GuardAI
    {
        guard_shattrath_scryerAI(Creature* creature) : GuardAI(creature) { }

        void Reset() override
        {
            banishTimer = 5000;
            exileTimer = 8500;
            playerGUID = 0;
            canTeleport = false;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (canTeleport)
            {
                if (exileTimer <= diff)
                {
					if (Unit* temp = ObjectAccessor::GetUnit(*me, playerGUID))
                    {
                        temp->CastSpell(temp, SPELL_EXILE, true);
                        temp->CastSpell(temp, SPELL_BANISH_TELEPORT, true);
                    }
                    playerGUID = 0;
                    exileTimer = 8500;
                    canTeleport = false;
                } else exileTimer -= diff;
            }
            else if (banishTimer <= diff)
            {
                Unit* temp = me->GetVictim();
                if (temp && temp->GetTypeId() == TYPEID_PLAYER)
                {
                    DoCast(temp, SPELL_BANISHED_SHATTRATH_A);
                    banishTimer = 9000;
                    playerGUID = temp->GetGUID();
                    if (playerGUID)
                        canTeleport = true;
                }
            } else banishTimer -= diff;

            DoMeleeAttackIfReady();
        }

    private:
        uint32 exileTimer;
        uint32 banishTimer;
        uint64 playerGUID;
        bool canTeleport;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new guard_shattrath_scryerAI(creature);
    }
};

class guard_shattrath_aldor : public CreatureScript
{
public:
    guard_shattrath_aldor() : CreatureScript("guard_shattrath_aldor") { }

    struct guard_shattrath_aldorAI : public GuardAI
    {
        guard_shattrath_aldorAI(Creature* creature) : GuardAI(creature) { }

        void Reset() override
        {
            banishTimer = 5000;
            exileTimer = 8500;
            playerGUID = 0;
            canTeleport = false;
        }

        void UpdateAI(uint32 diff) override
        {
            if (!UpdateVictim())
                return;

            if (canTeleport)
            {
                if (exileTimer <= diff)
                {
					if (Unit* temp = ObjectAccessor::GetUnit(*me, playerGUID))
                    {
                        temp->CastSpell(temp, SPELL_EXILE, true);
                        temp->CastSpell(temp, SPELL_BANISH_TELEPORT, true);
                    }
                    playerGUID = 0;
                    exileTimer = 8500;
                    canTeleport = false;
                } else exileTimer -= diff;
            }
            else if (banishTimer <= diff)
            {
                Unit* temp = me->GetVictim();
                if (temp && temp->GetTypeId() == TYPEID_PLAYER)
                {
                    DoCast(temp, SPELL_BANISHED_SHATTRATH_S);
                    banishTimer = 9000;
                    playerGUID = temp->GetGUID();
                    if (playerGUID)
                        canTeleport = true;
                }
            } else banishTimer -= diff;

            DoMeleeAttackIfReady();
        }
    private:
        uint32 exileTimer;
        uint32 banishTimer;
        uint64 playerGUID;
        bool canTeleport;
    };

    CreatureAI* GetAI(Creature* creature) const override
    {
        return new guard_shattrath_aldorAI(creature);
    }
};

void AddSC_guards()
{
    new guard_generic;
    new guard_shattrath_aldor;
    new guard_shattrath_scryer;
}

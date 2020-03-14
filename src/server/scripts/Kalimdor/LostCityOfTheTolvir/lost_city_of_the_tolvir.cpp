/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 * Copyright (C) 2011-2016 ArkCORE <http://www.arkania.net/>
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
 
 
#include "ScriptMgr.h"
#include "ScriptedCreature.h"
#include "lost_city_of_the_tolvir.h"
#include "Vehicle.h"
#include "Player.h"

enum Spells
{
    // Pygmy Brute
    SPELL_IMPALE                = 83783,
    SPELL_SHOCKWAVE             = 83785,

    // Wind Tunnel
    SPELL_SLIPSTREAM            = 85016,
};


const Position RopeBeamPos[8]=
{
    {-11095.3f, -1357.22f, 11.5751f, 0.0f},
    {-11093.6f, -1356.25f, 17.2975f, 0.0f},
    {-11097.3f, -1347.20f, 17.1596f, 0.0f},
    {-11099.0f, -1347.01f, 11.4107f, 0.0f},
    {-11087.8f, -1379.89f, 11.4035f, 0.0f},
    {-11086.9f, -1378.05f, 17.2635f, 0.0f},
    {-11090.6f, -1369.01f, 17.2757f, 0.0f},
    {-11092.2f, -1367.89f, 11.1261f, 0.0f},
};

const uint32 SpellRopBeam[4] = {84168, 84167, 84171, 84172};

// 45001  // 45007
class npc_enslaved_bandit_45001 : public CreatureScript
{
public:
    npc_enslaved_bandit_45001() : CreatureScript("npc_enslaved_bandit_45001") { }

    struct npc_enslaved_bandit_45001AI : public ScriptedAI
    {
        npc_enslaved_bandit_45001AI(Creature* creature) : ScriptedAI(creature)
        {
            RopeBeam();
        }

        void Reset()
        {
        }

        void RopeBeam()
        {
            if (me->GetDistance2d(RopeBeamPos[0].m_positionX, RopeBeamPos[0].m_positionY) < 10.0f)
            {
                me->SetCanFly(true);
                me->setActive(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                for (int i = 0; i < 4; ++i)
                    if (Creature* beam = me->SummonCreature(37231, RopeBeamPos[i]))
                    {
                        beam->setActive(true);
                        beam->SetCanFly(true);
                        me->CastSpell(beam, SpellRopBeam[i], true);
                    }
            }
            else if (me->GetDistance2d(RopeBeamPos[4].m_positionX, RopeBeamPos[4].m_positionY) < 10.0f)
            {
                me->SetCanFly(true);
                me->setActive(true);
                me->SetReactState(REACT_PASSIVE);
                me->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);

                for (int i = 4; i < 8; ++i)
                    if (Creature* beam = me->SummonCreature(37231, RopeBeamPos[i]))
                    {
                        beam->setActive(true);
                        beam->SetCanFly(true);
                        me->CastSpell(beam, SpellRopBeam[i - 4], true);
                    }
            }

            ScriptedAI::EnterEvadeMode();
        }

        void UpdateAI(uint32 /*diff*/)
        {
            if (!UpdateVictim())
                return;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_enslaved_bandit_45001AI(creature);
    }
};

// 44896
class npc_pygmy_brute_44896 : public CreatureScript
{
public:
    npc_pygmy_brute_44896() : CreatureScript("npc_pygmy_brute_44896") { }

    struct npc_pygmy_brute_44896AI : public ScriptedAI
    {
        npc_pygmy_brute_44896AI(Creature* creature) : ScriptedAI(creature){ }

        uint32 uiSummonTimer;
        uint32 uiImpaleTimer;
        uint32 uiShockwaveTimer;
        uint8 uiSummonMask;

        void Reset()
        {
            uiSummonTimer = 500;
            uiImpaleTimer = urand(3000, 7000);
            uiShockwaveTimer = urand(7000, 15000);
            uiSummonMask = 1;
        }

        void UpdateAI(uint32 diff)
        {
            if (uiSummonMask)
            {
                if (uiSummonTimer <= diff)
                {
                    if (Vehicle* vehicle = me->GetVehicleKit())
                    {
                        float x, y, z;
                        me->GetPosition(x, y, z);
                        uiSummonTimer = 500;

                        switch (uiSummonMask)
                        {
                            case 1:
                                {
                                    ++uiSummonMask;

                                    if (!vehicle->GetPassenger(0))
                                        if (TempSummon* pygmy = me->SummonCreature(44897, x, y, z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 30000))
                                            pygmy->EnterVehicle(me);
                                }
                                break;
                            case 2:
                                {
                                    uiSummonMask = 0;

                                    if (Unit* passenger = vehicle->GetPassenger(0))
                                        if (Vehicle* veh_pygmy = passenger->GetVehicleKit())
                                            if (!veh_pygmy->GetPassenger(0))
                                                if (Creature* pygmy = me->SummonCreature(44898, x, y, z, 0.0f, TEMPSUMMON_CORPSE_DESPAWN, 30000))
                                                    pygmy->EnterVehicle(passenger);
                                }
                                break;
                        }
                    }
                }
                else
                    uiSummonTimer -= diff;
            }

            if (!UpdateVictim())
                return;

            if (uiImpaleTimer <= diff)
            {
                uiImpaleTimer = urand(10000, 25000);
                me->CastSpell(me->GetVictim(), 83783, false);
            }
            else
                uiImpaleTimer -= diff;

            if (uiShockwaveTimer <= diff)
            {
                uiShockwaveTimer = urand(10000, 25000);
                me->CastSpell(me, 83785, false);
            }
            else
                uiShockwaveTimer -= diff;

            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_pygmy_brute_44896AI(creature);
    }
};

struct SlipstreamPlayer
{
    uint64 uiPlayerGUID;
    uint32 uiPersonalTimer;
};

class npc_wind_tunnel : public CreatureScript
{
public:
    npc_wind_tunnel() : CreatureScript("npc_wind_tunnel") { }

    struct npc_wind_tunnelAI : public ScriptedAI
    {
        npc_wind_tunnelAI(Creature* creature) : ScriptedAI(creature)
        {
            instance = creature->GetInstanceScript();
            me->setActive(true);
        }

        InstanceScript* instance;
        std::list<SlipstreamPlayer> lPlayers;
        uint8 uiIdentifier;

        void Reset()
        {
            if (instance)
            {
                if (instance->GetData(DATA_HIGH_PROPHET_BARIM) != DONE)
                    me->SetVisible(false);
                else
                    me->SetVisible(true);
            }
        }

        void PassengerBoarded(Unit* who, int8 /*seatId*/, bool apply)
        {
            if (apply && who->GetTypeId() == TYPEID_PLAYER)
            {
                SlipstreamPlayer new_player;
                new_player.uiPlayerGUID = who->GetGUID();
                new_player.uiPersonalTimer = 2000;
                lPlayers.push_back(new_player);
            }
        }

        void UpdateAI(uint32 diff)
        {
            if (lPlayers.empty())
                return;

            for (std::list<SlipstreamPlayer>::iterator itr = lPlayers.begin(); itr != lPlayers.end();)
            {
                if ((*itr).uiPersonalTimer <= diff)
                {
					if (Player* player = ObjectAccessor::GetPlayer(*me, (*itr).uiPlayerGUID))
                        if (Creature* Landing = me->FindNearestCreature(48097, 100.0f))
                        {
                            player->ExitVehicle();
                            player->CastSpell(player, SPELL_SLIPSTREAM, true);
                            player->CastSpell(Landing, 46598, true);

                            if (Aura* aura = Landing->GetAura(46598, (*itr).uiPlayerGUID))
                                aura->SetDuration(1000);
                        }

                    itr = lPlayers.erase(itr);
                }
                else
                {
                    (*itr).uiPersonalTimer -= diff;
                    ++itr;
                }
            }
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return new npc_wind_tunnelAI(creature);
    }
};

void AddSC_lost_city_of_the_tolvir()
{
    new npc_enslaved_bandit_45001();
    new npc_pygmy_brute_44896();
    new npc_wind_tunnel();
}
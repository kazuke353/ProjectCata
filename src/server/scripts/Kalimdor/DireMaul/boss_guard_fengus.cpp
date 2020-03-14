/*
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
#include "dire_maul.h"

// http://www.wowhead.com/npc=14321/guard-fengus
// guard_fengus
// GUARD_FENGUS

// 14321
class boss_guard_fengus : public CreatureScript
{
public:
    boss_guard_fengus() : CreatureScript("boss_guard_fengus") { }

    struct boss_guard_fengusAI : public BossAI
    {
        boss_guard_fengusAI(Creature* creature) : BossAI(creature, ENC_GUARD_FENGUS) { m_instance = creature->GetInstanceScript(); }

        InstanceScript* m_instance;
        EventMap m_events;
        uint32 m_phase;

        void Reset()
        {
            m_instance->SetData(ENC_GUARD_FENGUS, NOT_STARTED);
            m_events.Reset();
            m_phase = 0;
        }

        void EnterCombat(Unit* /*who*/)
        {
            m_instance->SetData(ENC_GUARD_FENGUS, IN_PROGRESS);
        }

        void JustDied(Unit* /*Killer*/)
        {
            m_instance->SetData(ENC_GUARD_FENGUS, DONE);
        }

        void UpdateAI(uint32 uiDiff)
        {
            if (!UpdateVictim())
                return;
            DoMeleeAttackIfReady();
        }
    };

    CreatureAI* GetAI(Creature* creature) const
    {
        return GetDireMaulAI<boss_guard_fengusAI>(creature);
    }
};

void AddSC_boss_guard_fengus()
{
    new boss_guard_fengus();
}



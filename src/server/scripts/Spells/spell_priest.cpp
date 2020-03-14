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

/*
 * Scripts for spells with SPELLFAMILY_PRIEST and SPELLFAMILY_GENERIC spells used by priest players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_pri_".
 */

#include "Player.h"
#include "ScriptMgr.h"
#include "SpellScript.h"
#include "SpellAuraEffects.h"
#include "GridNotifiers.h"

enum PriestSpells
{
    PRIEST_SPELL_RENEW = 139,
    PRIEST_SPELL_REVELATIONS = 88627,
    PRIEST_SPELL_SANCTUARY_4YD_DUMMY = 88667,
    PRIEST_SPELL_SANCTUARY_4YD_HEAL = 88668,
    PRIEST_SPELL_SANCTUARY_8YD_DUMMY = 88685,
    PRIEST_SPELL_SPIRIT_OF_REDEMPTION_FORM = 27795,
    PRIEST_SPELL_SPIRIT_OF_REDEMPTION_IMMUNITY = 62371,
    PRIEST_SPELL_SPIRIT_OF_REDEMPTION_ROOT = 27792,
    PRIEST_SPELL_SPIRIT_OF_REDEMPTION_SHAPESHIFT = 27827,
    PRIEST_SPELL_SPIRIT_OF_REDEMPTION_TALENT = 20711,

    SPELL_PRIEST_ABSOLUTION = 33167,
    SPELL_PRIEST_BODY_AND_SOUL_DISPEL = 64136,
    SPELL_PRIEST_BODY_AND_SOUL_SPEED = 65081,
    SPELL_PRIEST_CURE_DISEASE = 528,
    SPELL_PRIEST_DISPEL_MAGIC_FRIENDLY = 97690,
    SPELL_PRIEST_DISPEL_MAGIC_HOSTILE = 97691,
    SPELL_PRIEST_DIVINE_AEGIS = 47753,
    SPELL_PRIEST_DIVINE_TOUCH = 63544,
    SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING = 55675,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC = 55677,
    SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL = 56131,
    SPELL_PRIEST_GLYPH_OF_LIGHTWELL = 55673,
    SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL = 56161,
    SPELL_PRIEST_GLYPH_OF_SHADOW = 107906,
    SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL = 48153,
    SPELL_PRIEST_HOLY_WORD_SANCTUARY_TRIGGERED = 88686,
    SPELL_PRIEST_ITEM_EFFICIENCY = 37595,
    SPELL_PRIEST_LEAP_OF_FAITH = 73325,
    SPELL_PRIEST_LEAP_OF_FAITH_EFFECT = 92832,
    SPELL_PRIEST_LEAP_OF_FAITH_EFFECT_TRIGGER = 92833,
    SPELL_PRIEST_LEAP_OF_FAITH_TRIGGERED = 92572,
    SPELL_PRIEST_MANA_LEECH_PROC = 34650,
    SPELL_PRIEST_PENANCE_R1 = 47540,
    SPELL_PRIEST_PENANCE_R1_DAMAGE = 47758,
    SPELL_PRIEST_PENANCE_R1_HEAL = 47757,
    SPELL_PRIEST_REFLECTIVE_SHIELD_R1 = 33201,
    SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED = 33619,
    SPELL_PRIEST_SHADOW_WORD_DEATH = 32409,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH = 107904,
    SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH = 107903,
    SPELL_PRIEST_T9_HEALING_2P = 67201,
    SPELL_PRIEST_TWIN_DISCIPLINES_RANK_1 = 47586,
    SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL = 15290,
    SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL = 64085,

};

enum PriestSpellIcons
{
    PRIEST_ICON_ID_BORROWED_TIME                    = 2899,
    PRIEST_ICON_ID_DIVINE_TOUCH_TALENT              = 3021,
    PRIEST_ICON_ID_PAIN_AND_SUFFERING               = 2874
};

enum MiscSpells
{
    SPELL_GEN_REPLENISHMENT                         = 57669
};

class PowerCheck
{
    public:
        explicit PowerCheck(Powers const power) : _power(power) { }

        bool operator()(WorldObject* obj) const
        {
            if (Unit* target = obj->ToUnit())
                return target->getPowerType() != _power;

            return true;
        }

    private:
        Powers const _power;
};

class RaidCheck
{
    public:
        explicit RaidCheck(Unit const* caster) : _caster(caster) { }

        bool operator()(WorldObject* obj) const
        {
            if (Unit* target = obj->ToUnit())
                return !_caster->IsInRaidWith(target);

            return true;
        }

    private:
        Unit const* _caster;
};

// -64127
class spell_pri_body_and_soul : public SpellScriptLoader
{
    public:
        spell_pri_body_and_soul() : SpellScriptLoader("spell_pri_body_and_soul") { }

        class spell_pri_body_and_soul_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_body_and_soul_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_CURE_DISEASE) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PRIEST_BODY_AND_SOUL_DISPEL))
                    return false;
                return true;
            }

            void HandleEffectSpeedProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                // Proc only with Power Word: Shield or Leap of Faith
                if (!(eventInfo.GetDamageInfo()->GetSpellInfo()->SpellFamilyFlags[0] & 0x1 || eventInfo.GetDamageInfo()->GetSpellInfo()->SpellFamilyFlags[2] & 0x80000))
                    return;

                GetTarget()->CastCustomSpell(SPELL_PRIEST_BODY_AND_SOUL_SPEED, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void HandleEffectDispelProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                // Proc only with Cure Disease
                if (eventInfo.GetDamageInfo()->GetSpellInfo()->Id != SPELL_PRIEST_CURE_DISEASE || eventInfo.GetProcTarget() != GetTarget())
                    return;

                if (roll_chance_i(aurEff->GetAmount()))
                    GetTarget()->CastSpell(eventInfo.GetProcTarget(), SPELL_PRIEST_BODY_AND_SOUL_DISPEL, true, NULL, aurEff);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_body_and_soul_AuraScript::HandleEffectSpeedProc, EFFECT_0, SPELL_AURA_DUMMY);
                OnEffectProc += AuraEffectProcFn(spell_pri_body_and_soul_AuraScript::HandleEffectDispelProc, EFFECT_1, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_body_and_soul_AuraScript();
        }
};

// 34861 - Circle of Healing
class spell_pri_circle_of_healing : public SpellScriptLoader
{
    public:
        spell_pri_circle_of_healing() : SpellScriptLoader("spell_pri_circle_of_healing") { }

        class spell_pri_circle_of_healing_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_circle_of_healing_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING))
                    return false;
                return true;
            }

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(RaidCheck(GetCaster()));

                uint32 const maxTargets = GetCaster()->HasAura(SPELL_PRIEST_GLYPH_OF_CIRCLE_OF_HEALING) ? 6 : 5; // Glyph of Circle of Healing

                if (targets.size() > maxTargets)
                {
                    targets.sort(Trinity::HealthPctOrderPred());
                    targets.resize(maxTargets);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_circle_of_healing_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_circle_of_healing_SpellScript();
        }
};

// 527 - Dispel magic
class spell_pri_dispel_magic : public SpellScriptLoader
{
public:
    spell_pri_dispel_magic() : SpellScriptLoader("spell_pri_dispel_magic") { }

    class spell_pri_dispel_magic_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_dispel_magic_SpellScript);

        bool Validate(SpellInfo const* /*spellInfo*/)
        {
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_ABSOLUTION))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL))
                return false;
            if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC))
                return false;
            return true;
        }

        SpellCastResult CheckCast()
        {
            Unit* caster = GetCaster();
            Unit* target = GetExplTargetUnit();

            if (!target || (!caster->HasAura(SPELL_PRIEST_ABSOLUTION) && caster != target && target->IsFriendlyTo(caster)))
                return SPELL_FAILED_BAD_TARGETS;
            return SPELL_CAST_OK;
        }

        void AfterEffectHit(SpellEffIndex /*effIndex*/)
        {
            if (GetHitUnit()->IsFriendlyTo(GetCaster()))
            {
                GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_DISPEL_MAGIC_FRIENDLY, true);
                if (AuraEffect const* aurEff = GetHitUnit()->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC, EFFECT_0))
                {
                    int32 heal = GetHitUnit()->CountPctFromMaxHealth(aurEff->GetAmount());
                    GetCaster()->CastCustomSpell(SPELL_PRIEST_GLYPH_OF_DISPEL_MAGIC_HEAL, SPELLVALUE_BASE_POINT0, heal, GetHitUnit());
                }
            }
            else
                GetCaster()->CastSpell(GetHitUnit(), SPELL_PRIEST_DISPEL_MAGIC_HOSTILE, true);
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_pri_dispel_magic_SpellScript::CheckCast);
            OnEffectHitTarget += SpellEffectFn(spell_pri_dispel_magic_SpellScript::AfterEffectHit, EFFECT_0, SPELL_EFFECT_DUMMY);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_dispel_magic_SpellScript();
    }
};

// -47509 - Divine Aegis
class spell_pri_divine_aegis : public SpellScriptLoader
{
    public:
        spell_pri_divine_aegis() : SpellScriptLoader("spell_pri_divine_aegis") { }

        class spell_pri_divine_aegis_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_divine_aegis_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_DIVINE_AEGIS))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                return eventInfo.GetProcTarget();
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                int32 absorb = CalculatePct(int32(eventInfo.GetHealInfo()->GetHeal()), aurEff->GetAmount());

                // Multiple effects stack, so let's try to find this aura.
                if (AuraEffect const* aegis = eventInfo.GetProcTarget()->GetAuraEffect(SPELL_PRIEST_DIVINE_AEGIS, EFFECT_0))
                    absorb += aegis->GetAmount();

                absorb = std::min(absorb, eventInfo.GetProcTarget()->getLevel() * 125);

                GetTarget()->CastCustomSpell(SPELL_PRIEST_DIVINE_AEGIS, SPELLVALUE_BASE_POINT0, absorb, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_divine_aegis_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_divine_aegis_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_divine_aegis_AuraScript();
        }
};

// 64844 - Divine Hymn
class spell_pri_divine_hymn : public SpellScriptLoader
{
    public:
        spell_pri_divine_hymn() : SpellScriptLoader("spell_pri_divine_hymn") { }

        class spell_pri_divine_hymn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_divine_hymn_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(RaidCheck(GetCaster()));

                uint32 const maxTargets = 3;

                if (targets.size() > maxTargets)
                {
                    targets.sort(Trinity::HealthPctOrderPred());
                    targets.resize(maxTargets);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_divine_hymn_SpellScript::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_divine_hymn_SpellScript();
        }
};

// 55680 - Glyph of Prayer of Healing
class spell_pri_glyph_of_prayer_of_healing : public SpellScriptLoader
{
    public:
        spell_pri_glyph_of_prayer_of_healing() : SpellScriptLoader("spell_pri_glyph_of_prayer_of_healing") { }

        class spell_pri_glyph_of_prayer_of_healing_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_glyph_of_prayer_of_healing_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL))
                    return false;
                return true;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();

                SpellInfo const* triggeredSpellInfo = sSpellMgr->EnsureSpellInfo(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL);
                int32 heal = int32(CalculatePct(int32(eventInfo.GetHealInfo()->GetHeal()), aurEff->GetAmount()) / triggeredSpellInfo->GetMaxTicks());
                GetTarget()->CastCustomSpell(SPELL_PRIEST_GLYPH_OF_PRAYER_OF_HEALING_HEAL, SPELLVALUE_BASE_POINT0, heal, eventInfo.GetProcTarget(), true, NULL, aurEff);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_glyph_of_prayer_of_healing_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_glyph_of_prayer_of_healing_AuraScript();
        }
};

// -14748
class spell_pri_improved_power_word_shield : public SpellScriptLoader
{
    public:
        spell_pri_improved_power_word_shield() : SpellScriptLoader("spell_pri_improved_power_word_shield") { }

        class spell_pri_improved_power_word_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_improved_power_word_shield_AuraScript);

            void HandleEffectCalcSpellMod(AuraEffect const* aurEff, SpellModifier*& spellMod)
            {
                if (!spellMod)
                {
                    spellMod = new SpellModifier(GetAura());
                    spellMod->op = SpellModOp(aurEff->GetMiscValue());
                    spellMod->type = SPELLMOD_PCT;
                    spellMod->spellId = GetId();
                    spellMod->mask = GetSpellInfo()->Effects[aurEff->GetEffIndex()].SpellClassMask;
                }

                spellMod->value = aurEff->GetAmount();
            }

            void Register() override
            {
                DoEffectCalcSpellMod += AuraEffectCalcSpellModFn(spell_pri_improved_power_word_shield_AuraScript::HandleEffectCalcSpellMod, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_improved_power_word_shield_AuraScript();
        }
};

// 37594 - Greater Heal Refund
class spell_pri_item_greater_heal_refund : public SpellScriptLoader
{
    public:
        spell_pri_item_greater_heal_refund() : SpellScriptLoader("spell_pri_item_greater_heal_refund") { }

        class spell_pri_item_greater_heal_refund_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_item_greater_heal_refund_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_ITEM_EFFICIENCY))
                    return false;
                return true;
            }

            void OnProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(GetTarget(), SPELL_PRIEST_ITEM_EFFICIENCY, true, NULL, aurEff);
            }

            void Register() override
            {
                OnEffectProc += AuraEffectProcFn(spell_pri_item_greater_heal_refund_AuraScript::OnProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_item_greater_heal_refund_AuraScript();
        }
};

// 47788 - Guardian Spirit
class spell_pri_guardian_spirit : public SpellScriptLoader
{
    public:
        spell_pri_guardian_spirit() : SpellScriptLoader("spell_pri_guardian_spirit") { }

        class spell_pri_guardian_spirit_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_guardian_spirit_AuraScript);

            uint32 healPct;

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL))
                    return false;
                return true;
            }

            bool Load() override
            {
                healPct = GetSpellInfo()->Effects[EFFECT_1].CalcValue();
                return true;
            }

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
            {
                // Set absorbtion amount to unlimited
                amount = -1;
            }

            void Absorb(AuraEffect* /*aurEff*/, DamageInfo & dmgInfo, uint32 & absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetDamage() < target->GetHealth())
                    return;

                int32 healAmount = int32(target->CountPctFromMaxHealth(healPct));
                // remove the aura now, we don't want 40% healing bonus
                Remove(AURA_REMOVE_BY_ENEMY_SPELL);
                target->CastCustomSpell(target, SPELL_PRIEST_GUARDIAN_SPIRIT_HEAL, &healAmount, NULL, NULL, true);
                absorbAmount = dmgInfo.GetDamage();
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_guardian_spirit_AuraScript::CalculateAmount, EFFECT_1, SPELL_AURA_SCHOOL_ABSORB);
                OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_guardian_spirit_AuraScript::Absorb, EFFECT_1);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_guardian_spirit_AuraScript();
        }
};

// 64904 - Hymn of Hope
class spell_pri_hymn_of_hope : public SpellScriptLoader
{
    public:
        spell_pri_hymn_of_hope() : SpellScriptLoader("spell_pri_hymn_of_hope") { }

        class spell_pri_hymn_of_hope_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_hymn_of_hope_SpellScript);

            void FilterTargets(std::list<WorldObject*>& targets)
            {
                targets.remove_if(PowerCheck(POWER_MANA));
                targets.remove_if(RaidCheck(GetCaster()));

                uint32 const maxTargets = 3;

                if (targets.size() > maxTargets)
                {
                    targets.sort(Trinity::PowerPctOrderPred(POWER_MANA));
                    targets.resize(maxTargets);
                }
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_hymn_of_hope_SpellScript::FilterTargets, EFFECT_ALL, TARGET_UNIT_SRC_AREA_ALLY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_hymn_of_hope_SpellScript();
        }
};

// 92833 - Leap of Faith
class spell_pri_leap_of_faith_effect_trigger : public SpellScriptLoader
{
    public:
        spell_pri_leap_of_faith_effect_trigger() : SpellScriptLoader("spell_pri_leap_of_faith_effect_trigger") { }

        class spell_pri_leap_of_faith_effect_trigger_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_leap_of_faith_effect_trigger_SpellScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_LEAP_OF_FAITH_EFFECT))
                    return false;
                return true;
            }

            void HandleEffectDummy(SpellEffIndex /*effIndex*/)
            {
                Position destPos = GetHitDest()->GetPosition();

                SpellCastTargets targets;
                targets.SetDst(destPos);
                targets.SetUnitTarget(GetCaster());
                GetHitUnit()->CastSpell(targets, sSpellMgr->GetSpellInfo(GetEffectValue()), NULL);
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_leap_of_faith_effect_trigger_SpellScript::HandleEffectDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_leap_of_faith_effect_trigger_SpellScript();
        }
};

// 7001 - Lightwell Renew
class spell_pri_lightwell_renew : public SpellScriptLoader
{
    public:
        spell_pri_lightwell_renew() : SpellScriptLoader("spell_pri_lightwell_renew") { }

        class spell_pri_lightwell_renew_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_lightwell_renew_AuraScript);

            void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
            {
                if (Unit* caster = GetCaster())
                {
                    // Bonus from Glyph of Lightwell
                    if (AuraEffect* modHealing = caster->GetAuraEffect(SPELL_PRIEST_GLYPH_OF_LIGHTWELL, EFFECT_0))
                        AddPct(amount, modHealing->GetAmount());
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_lightwell_renew_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_PERIODIC_HEAL);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_lightwell_renew_AuraScript();
        }
};

// 8129 - Mana Burn
class spell_pri_mana_burn : public SpellScriptLoader
{
    public:
        spell_pri_mana_burn() : SpellScriptLoader("spell_pri_mana_burn") { }

        class spell_pri_mana_burn_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mana_burn_SpellScript);

            void HandleAfterHit()
            {
                if (Unit* unitTarget = GetHitUnit())
                    unitTarget->RemoveAurasWithMechanic((1 << MECHANIC_FEAR) | (1 << MECHANIC_POLYMORPH));
            }

            void Register() override
            {
                AfterHit += SpellHitFn(spell_pri_mana_burn_SpellScript::HandleAfterHit);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mana_burn_SpellScript;
        }
};

// 28305 - Mana Leech (Passive) (Priest Pet Aura)
class spell_pri_mana_leech : public SpellScriptLoader
{
    public:
        spell_pri_mana_leech() : SpellScriptLoader("spell_pri_mana_leech") { }

        class spell_pri_mana_leech_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_mana_leech_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_MANA_LEECH_PROC))
                    return false;
                return true;
            }

            bool Load() override
            {
                _procTarget = nullptr;
                return true;
            }

            bool CheckProc(ProcEventInfo& /*eventInfo*/)
            {
                _procTarget = GetTarget()->GetOwner();
                return _procTarget;
            }

            void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->CastSpell(_procTarget, SPELL_PRIEST_MANA_LEECH_PROC, true, NULL, aurEff);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_mana_leech_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_mana_leech_AuraScript::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
            }

        private:
            Unit* _procTarget;
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_mana_leech_AuraScript();
        }
};

// 49821 - Mind Sear
class spell_pri_mind_sear : public SpellScriptLoader
{
    public:
        spell_pri_mind_sear() : SpellScriptLoader("spell_pri_mind_sear") { }

        class spell_pri_mind_sear_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_mind_sear_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                unitList.remove_if(Trinity::ObjectGUIDCheck(GetCaster()->GetUInt64Value(UNIT_FIELD_CHANNEL_OBJECT)));
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_mind_sear_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_mind_sear_SpellScript();
        }
};

// 47948 - Pain and Suffering (Proc)
class spell_pri_pain_and_suffering_proc : public SpellScriptLoader
{
    public:
        spell_pri_pain_and_suffering_proc() : SpellScriptLoader("spell_pri_pain_and_suffering_proc") { }

        class spell_pri_pain_and_suffering_proc_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_pain_and_suffering_proc_SpellScript);

            void HandleEffectScriptEffect(SpellEffIndex /*effIndex*/)
            {
                // Refresh Shadow Word: Pain on target
                if (Unit* unitTarget = GetHitUnit())
                    if (AuraEffect* aur = unitTarget->GetAuraEffect(SPELL_AURA_PERIODIC_DAMAGE, SPELLFAMILY_PRIEST, 0x8000, 0, 0, GetCaster()->GetGUID()))
                        aur->GetBase()->RefreshDuration();
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_pain_and_suffering_proc_SpellScript::HandleEffectScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_pain_and_suffering_proc_SpellScript;
        }
};

// 47540 - Penance
class spell_pri_penance : public SpellScriptLoader
{
    public:
        spell_pri_penance() : SpellScriptLoader("spell_pri_penance") { }

        class spell_pri_penance_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_penance_SpellScript);

            bool Load() override
            {
                if (GetCaster() && GetCaster()->GetTypeId() == TYPEID_UNIT && GetCaster()->ToCreature()->GetIAmABot()) return true;
                return GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            bool Validate(SpellInfo const* spellInfo) override
            {
                SpellInfo const* firstRankSpellInfo = sSpellMgr->GetSpellInfo(SPELL_PRIEST_PENANCE_R1);
                if (!firstRankSpellInfo)
                    return false;

                // can't use other spell than this penance due to spell_ranks dependency
                if (!spellInfo->IsRankOf(firstRankSpellInfo))
                    return false;

                uint8 rank = spellInfo->GetRank();
                if (!sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_DAMAGE, rank, true))
                    return false;
                if (!sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_HEAL, rank, true))
                    return false;

                return true;
            }

            void HandleDummy(SpellEffIndex /*effIndex*/)
            {
                Unit* caster = GetCaster();
                if (Unit* unitTarget = GetHitUnit())
                {
                    if (!unitTarget->IsAlive())
                        return;

                    uint8 rank = GetSpellInfo()->GetRank();

                    if (caster->IsFriendlyTo(unitTarget))
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_HEAL, rank), false);
                    else
                        caster->CastSpell(unitTarget, sSpellMgr->GetSpellWithRank(SPELL_PRIEST_PENANCE_R1_DAMAGE, rank), false);
                }
            }

            SpellCastResult CheckCast()
            {
                Player* caster = GetCaster()->ToPlayer();
                if (!caster && GetCaster()->GetTypeId() == TYPEID_UNIT && GetCaster()->ToCreature()->GetIAmABot())
                    caster = (Player*)GetCaster();
                if (Unit* target = GetExplTargetUnit())
                    if (!caster->IsFriendlyTo(target) && !caster->IsValidAttackTarget(target))
                        return SPELL_FAILED_BAD_TARGETS;
                return SPELL_CAST_OK;
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_penance_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
                OnCheckCast += SpellCheckCastFn(spell_pri_penance_SpellScript::CheckCast);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_penance_SpellScript;
        }
};

// -47569 - Phantasm
class spell_pri_phantasm : public SpellScriptLoader
{
    public:
        spell_pri_phantasm() : SpellScriptLoader("spell_pri_phantasm") { }

        class spell_pri_phantasm_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_phantasm_AuraScript);

            bool CheckProc(ProcEventInfo& /*eventInfo*/)
            {
                return roll_chance_i(GetEffect(EFFECT_0)->GetAmount());
            }

            void HandleEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& /*eventInfo*/)
            {
                PreventDefaultAction();
                GetTarget()->RemoveMovementImpairingAuras();
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_phantasm_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_phantasm_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_phantasm_AuraScript();
        }
};

// 17 - Power Word: Shield
class spell_pri_power_word_shield : public SpellScriptLoader
{
    public:
        spell_pri_power_word_shield() : SpellScriptLoader("spell_pri_power_word_shield") { }

        class spell_pri_power_word_shield_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_power_word_shield_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED))
                    return false;
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_REFLECTIVE_SHIELD_R1))
                    return false;
                return true;
            }

            void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
            {
                canBeRecalculated = false;
                if (Unit* caster = GetCaster())
                {
                    // +87% from sp bonus
                    float bonus = 0.87f;

                    // Borrowed Time
                    if (AuraEffect const* borrowedTime = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_BORROWED_TIME, EFFECT_1))
                        bonus += CalculatePct(1.0f, borrowedTime->GetAmount());

                    bonus *= caster->SpellBaseHealingBonusDone(GetSpellInfo()->GetSchoolMask());

                    // Improved PW: Shield: its weird having a SPELLMOD_ALL_EFFECTS here but its blizzards doing :)
                    // Improved PW: Shield is only applied at the spell healing bonus because it was already applied to the base value in CalculateSpellDamage
                    bonus = caster->ApplyEffectModifiers(GetSpellInfo(), aurEff->GetEffIndex(), bonus);
                    bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

                    amount += int32(bonus);

                    // Twin Disciplines
                    if (AuraEffect const* twinDisciplines = caster->GetAuraEffectOfRankedSpell(SPELL_PRIEST_TWIN_DISCIPLINES_RANK_1, EFFECT_1))
                        AddPct(amount, twinDisciplines->GetAmount());

                    // Mastery: Shield Discipline
                    if (caster->HasAura(77484))
                        amount += caster->ToPlayer()->GetAura(77484)->GetEffect(0)->GetAmount() * amount / 100;

                    // Focused Power
                    amount *= caster->GetTotalAuraMultiplier(SPELL_AURA_MOD_HEALING_DONE_PERCENT);
                }
            }

            void ReflectDamage(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
            {
                Unit* target = GetTarget();
                if (dmgInfo.GetAttacker() == target)
                    return;

                if (AuraEffect const* talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_PRIEST_REFLECTIVE_SHIELD_R1, EFFECT_0))
                {
                    int32 bp = CalculatePct(absorbAmount, talentAurEff->GetAmount());
                    target->CastCustomSpell(dmgInfo.GetAttacker(), SPELL_PRIEST_REFLECTIVE_SHIELD_TRIGGERED, &bp, NULL, NULL, true, NULL, aurEff);
                }
            }

            void Register() override
            {
                DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_power_word_shield_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
                AfterEffectAbsorb += AuraEffectAbsorbFn(spell_pri_power_word_shield_AuraScript::ReflectDamage, EFFECT_0);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_power_word_shield_AuraScript();
        }
};

// 33110 - Prayer of Mending Heal
class spell_pri_prayer_of_mending_heal : public SpellScriptLoader
{
    public:
        spell_pri_prayer_of_mending_heal() : SpellScriptLoader("spell_pri_prayer_of_mending_heal") { }

        class spell_pri_prayer_of_mending_heal_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_prayer_of_mending_heal_SpellScript);

            void HandleHeal(SpellEffIndex /*effIndex*/)
            {
                if (Unit* caster = GetOriginalCaster())
                {
                    if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_PRIEST_T9_HEALING_2P, EFFECT_0))
                    {
                        int32 heal = GetHitHeal();
                        AddPct(heal, aurEff->GetAmount());
                        SetHitHeal(heal);
                    }
                }
            }

            void Register() override
            {
                OnEffectHitTarget += SpellEffectFn(spell_pri_prayer_of_mending_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_prayer_of_mending_heal_SpellScript();
        }
};

// 139 - Renew
class spell_pri_renew : public SpellScriptLoader
{
    public:
        spell_pri_renew() : SpellScriptLoader("spell_pri_renew") { }

        class spell_pri_renew_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_renew_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_DIVINE_TOUCH))
                    return false;
                return true;
            }

            bool Load() override
            {
                if (GetCaster() && GetCaster()->GetTypeId() == TYPEID_UNIT && GetCaster()->ToCreature()->GetIAmABot()) return true;
                return GetCaster() && GetCaster()->GetTypeId() == TYPEID_PLAYER;
            }

            void HandleApplyEffect(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
            {
                if (Unit* caster = GetCaster())
                {
                    // Divine Touch
                    if (AuraEffect const* empoweredRenewAurEff = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_DIVINE_TOUCH_TALENT, EFFECT_0))
                    {
                        uint32 heal = caster->SpellHealingBonusDone(GetTarget(), GetSpellInfo(), aurEff->GetAmount(), DOT);
                        heal = GetTarget()->SpellHealingBonusTaken(caster, GetSpellInfo(), heal, DOT);
                        int32 basepoints0 = CalculatePct(int32(heal) * aurEff->GetTotalTicks(), empoweredRenewAurEff->GetAmount());
                        caster->CastCustomSpell(GetTarget(), SPELL_PRIEST_DIVINE_TOUCH, &basepoints0, NULL, NULL, true, NULL, aurEff);
                    }
                }
            }

            void Register() override
            {
                OnEffectApply += AuraEffectApplyFn(spell_pri_renew_AuraScript::HandleApplyEffect, EFFECT_0, SPELL_AURA_PERIODIC_HEAL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_renew_AuraScript();
        }
};

// 32379 - Shadow Word Death
class spell_pri_shadow_word_death : public SpellScriptLoader
{
    public:
        spell_pri_shadow_word_death() : SpellScriptLoader("spell_pri_shadow_word_death") { }

        class spell_pri_shadow_word_death_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_shadow_word_death_SpellScript);

            void HandleDamage()
            {
                int32 damage = GetHitDamage();

                // Pain and Suffering reduces damage
                if (AuraEffect* aurEff = GetCaster()->GetDummyAuraEffect(SPELLFAMILY_PRIEST, PRIEST_ICON_ID_PAIN_AND_SUFFERING, EFFECT_1))
                    AddPct(damage, aurEff->GetAmount());

                GetCaster()->CastCustomSpell(GetCaster(), SPELL_PRIEST_SHADOW_WORD_DEATH, &damage, 0, 0, true);
            }

            void Register() override
            {
                OnHit += SpellHitFn(spell_pri_shadow_word_death_SpellScript::HandleDamage);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_shadow_word_death_SpellScript();
        }
};

// 15473 - Shadowform
class spell_pri_shadowform : public SpellScriptLoader
{
    public:
        spell_pri_shadowform() : SpellScriptLoader("spell_pri_shadowform") { }

        class spell_pri_shadowform_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_shadowform_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH) ||
                    !sSpellMgr->GetSpellInfo(SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH))
                    return false;
                return true;
            }

            void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->CastSpell(GetTarget(), GetTarget()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW) ? SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH, true);
            }

            void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
            {
                GetTarget()->RemoveAurasDueToSpell(GetTarget()->HasAura(SPELL_PRIEST_GLYPH_OF_SHADOW) ? SPELL_PRIEST_SHADOWFORM_VISUAL_WITH_GLYPH : SPELL_PRIEST_SHADOWFORM_VISUAL_WITHOUT_GLYPH);
            }

            void Register() override
            {
                AfterEffectApply += AuraEffectApplyFn(spell_pri_shadowform_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
                AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadowform_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_SHAPESHIFT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_shadowform_AuraScript();
        }
};

// 15286 - Vampiric Embrace
class spell_pri_vampiric_embrace : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_embrace() : SpellScriptLoader("spell_pri_vampiric_embrace") { }

        class spell_pri_vampiric_embrace_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_embrace_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL))
                    return false;
                return true;
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                // Not proc from Mind Sear
                return !(eventInfo.GetDamageInfo()->GetSpellInfo()->SpellFamilyFlags[1] & 0x80000);
            }

            void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                PreventDefaultAction();
                int32 self = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount()));
                int32 team = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), aurEff->GetAmount() / 2));

                GetTarget()->CastCustomSpell((Unit*)NULL, SPELL_PRIEST_VAMPIRIC_EMBRACE_HEAL, &team, &self, NULL, true, NULL, aurEff);
            }

            void Register() override
            {
                DoCheckProc += AuraCheckProcFn(spell_pri_vampiric_embrace_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_embrace_AuraScript::HandleEffectProc, EFFECT_0, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_vampiric_embrace_AuraScript();
        }
};

// 15290 - Vampiric Embrace (heal)
class spell_pri_vampiric_embrace_target : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_embrace_target() : SpellScriptLoader("spell_pri_vampiric_embrace_target") { }

        class spell_pri_vampiric_embrace_target_SpellScript : public SpellScript
        {
            PrepareSpellScript(spell_pri_vampiric_embrace_target_SpellScript);

            void FilterTargets(std::list<WorldObject*>& unitList)
            {
                unitList.remove(GetCaster());
            }

            void Register() override
            {
                OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_vampiric_embrace_target_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_CASTER_AREA_PARTY);
            }
        };

        SpellScript* GetSpellScript() const override
        {
            return new spell_pri_vampiric_embrace_target_SpellScript();
        }
};

// 34914 - Vampiric Touch
class spell_pri_vampiric_touch : public SpellScriptLoader
{
    public:
        spell_pri_vampiric_touch() : SpellScriptLoader("spell_pri_vampiric_touch") { }

        class spell_pri_vampiric_touch_AuraScript : public AuraScript
        {
            PrepareAuraScript(spell_pri_vampiric_touch_AuraScript);

            bool Validate(SpellInfo const* /*spellInfo*/) override
            {
                if (!sSpellMgr->GetSpellInfo(SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL) ||
                    !sSpellMgr->GetSpellInfo(SPELL_GEN_REPLENISHMENT))
                    return false;
                return true;
            }

            void HandleDispel(DispelInfo* /*dispelInfo*/)
            {
                if (Unit* caster = GetCaster())
                    if (Unit* target = GetUnitOwner())
                        if (AuraEffect const* aurEff = GetEffect(EFFECT_1))
                        {
                            int32 damage = aurEff->GetAmount() * 8;
                            // backfire damage
                            caster->CastCustomSpell(target, SPELL_PRIEST_VAMPIRIC_TOUCH_DISPEL, &damage, NULL, NULL, true, NULL, aurEff);
                        }
            }

            bool CheckProc(ProcEventInfo& eventInfo)
            {
                return eventInfo.GetProcTarget() == GetCaster();
            }

            void HandleEffectProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
            {
                eventInfo.GetProcTarget()->CastSpell((Unit*)NULL, SPELL_GEN_REPLENISHMENT, true, NULL, aurEff);
            }

            void Register() override
            {
                AfterDispel += AuraDispelFn(spell_pri_vampiric_touch_AuraScript::HandleDispel);
                DoCheckProc += AuraCheckProcFn(spell_pri_vampiric_touch_AuraScript::CheckProc);
                OnEffectProc += AuraEffectProcFn(spell_pri_vampiric_touch_AuraScript::HandleEffectProc, EFFECT_2, SPELL_AURA_DUMMY);
            }
        };

        AuraScript* GetAuraScript() const override
        {
            return new spell_pri_vampiric_touch_AuraScript();
        }
};

// 528 Cure Disease
class spell_pri_cure_disease : public SpellScriptLoader
{
public:
    spell_pri_cure_disease() : SpellScriptLoader("spell_pri_cure_disease") { }

    class spell_pri_cure_disease_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_cure_disease_SpellScript);

        void HandleOnHit()
        {
            Unit* caster = GetCaster();
            if (GetHitUnit() != caster)
                return;

            // Body and soul
            if (AuraEffect* bodyAndSoul = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, 2218, EFFECT_1))
                if (roll_chance_i(bodyAndSoul->GetAmount()))
                    caster->CastSpell(caster, 64136, true);
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_pri_cure_disease_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_cure_disease_SpellScript();
    }
};

// 48045 Mind Sear
class spell_pri_mind_sear_spell : public SpellScriptLoader
{
public:
    spell_pri_mind_sear_spell() : SpellScriptLoader("spell_pri_mind_sear_spell") { }

    class spell_pri_mind_sear_spell_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_mind_sear_spell_SpellScript);

        SpellCastResult CheckCast()
        {
            if (GetCaster() == GetExplTargetUnit())
                return SPELL_FAILED_BAD_TARGETS;

            return SPELL_CAST_OK;
        }

        void Register()
        {
            OnCheckCast += SpellCheckCastFn(spell_pri_mind_sear_spell_SpellScript::CheckCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_mind_sear_spell_SpellScript();
    }
};

// 8092
class spell_pri_mind_blast : public SpellScriptLoader
{
public:
    spell_pri_mind_blast() : SpellScriptLoader("spell_pri_mind_blast") { }

    class spell_pri_mind_blast_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_mind_blast_SpellScript);

        void HandleOnHit()
        {
            Unit* caster = GetCaster();
            if (Unit* unitTarget = GetHitUnit())
            {
                if (Aura* aur = caster->GetAuraOfRankedSpell(15273, caster->GetGUID()))
                {
                    int32 chance = aur->GetSpellInfo()->Effects[EFFECT_1].BasePoints;
                    if (caster->GetShapeshiftForm() == FORM_SHADOW && roll_chance_i(chance))
                        caster->CastSpell(unitTarget, 48301, true);
                }
                // Remove Mind spike debuff
                unitTarget->RemoveAurasDueToSpell(87178, caster->GetGUID());
            }

            // Shadow orbs
            if (AuraEffect* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_GENERIC, 4941, EFFECT_0))
            {
                uint8 stacks = aurEff->GetBase()->GetStackAmount();
                int32 amount = aurEff->GetAmount() / (2.0f * stacks);
                if (AuraEffect* mastery = caster->GetAuraEffect(77486, EFFECT_0))
                    amount += mastery->GetAmount();
                caster->CastCustomSpell(caster, 95799, &amount, &amount, NULL, true);
                aurEff->GetBase()->Remove();
            }
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_pri_mind_blast_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_mind_blast_SpellScript;
    }
};

// 73510
class spell_pri_mind_spike : public SpellScriptLoader
{
public:
    spell_pri_mind_spike() : SpellScriptLoader("spell_pri_mind_spike") { }

    class spell_pri_mind_spike_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_mind_spike_SpellScript);

        void HandleDummy(SpellEffIndex /*effIndex*/)
        {
            Unit* caster = GetCaster();
            if (Unit* unitTarget = GetHitUnit())
            {
                unitTarget->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, caster->GetGUID(), NULL, true, false, SPELLFAMILY_PRIEST, SPELL_SCHOOL_MASK_SHADOW);
                unitTarget->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH, caster->GetGUID(), NULL, true, false, SPELLFAMILY_PRIEST, SPELL_SCHOOL_MASK_SHADOW);
            }
        }

        void HandleOnHit()
        {
            Unit* caster = GetCaster();
            // Shadow orbs
            if (AuraEffect* aurEff = caster->GetDummyAuraEffect(SPELLFAMILY_GENERIC, 4941, EFFECT_0))
            {
                uint8 stacks = aurEff->GetBase()->GetStackAmount();
                int32 amount = aurEff->GetAmount() / (2.0f * stacks);
                if (AuraEffect* mastery = caster->GetAuraEffect(77486, EFFECT_0))
                    amount += mastery->GetAmount();
                caster->CastCustomSpell(caster, 95799, &amount, &amount, NULL, true);
                aurEff->GetBase()->Remove();
            }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_mind_spike_SpellScript::HandleDummy, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
            OnHit += SpellHitFn(spell_pri_mind_spike_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_mind_spike_SpellScript;
    }
};

// 586
class spell_pri_fade : public SpellScriptLoader
{
public:
    spell_pri_fade() : SpellScriptLoader("spell_pri_fade") { }

    class spell_pri_fade_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_fade_SpellScript);

        void HandleOnHit()
        {
            Unit* caster = GetCaster();
            if (AuraEffect* aur = caster->GetDummyAuraEffect(SPELLFAMILY_PRIEST, 2901, EFFECT_0))
                if (roll_chance_i(aur->GetBaseAmount()))
                    caster->RemoveMovementImpairingAuras();
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_pri_fade_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_fade_SpellScript;
    }
};

// 588 Inner Fire
class spell_pri_inner_fire : public SpellScriptLoader
{
public:
    spell_pri_inner_fire() : SpellScriptLoader("spell_pri_inner_fire") { }

    class spell_pri_inner_fire_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_inner_fire_AuraScript);

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            Unit* target = GetTarget();
            if (AuraEffect* aurEff = target->GetDummyAuraEffect(SPELLFAMILY_PRIEST, 51, EFFECT_0))
                target->CastCustomSpell(91724, SPELLVALUE_BASE_POINT0, aurEff->GetAmount(), target, true);
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(91724);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_pri_inner_fire_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_MOD_RESISTANCE_PCT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pri_inner_fire_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_MOD_RESISTANCE_PCT, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_inner_fire_AuraScript();
    }
};

// 77487 Shadow orbs
class spell_pri_shadow_orbs : public SpellScriptLoader
{
public:
    spell_pri_shadow_orbs() : SpellScriptLoader("spell_pri_shadow_orbs") { }

    class spell_pri_shadow_orbs_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_shadow_orbs_AuraScript);

        void HandleEffectApply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            if (GetStackAmount() == GetSpellInfo()->StackAmount)
                GetTarget()->CastSpell(GetTarget(), 93683, true);
        }

        void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(93683);
        }

        void Register()
        {
            AfterEffectApply += AuraEffectApplyFn(spell_pri_shadow_orbs_AuraScript::HandleEffectApply, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
            AfterEffectRemove += AuraEffectRemoveFn(spell_pri_shadow_orbs_AuraScript::HandleEffectRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_shadow_orbs_AuraScript();
    }
};

// 89485 - Inner Focus
class spell_pri_inner_focus : public SpellScriptLoader
{
public:
    spell_pri_inner_focus() : SpellScriptLoader("spell_pri_inner_focus") { }

    class spell_pri_inner_focus_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_inner_focus_SpellScript);

        void HandleOnCast()
        {
            Unit* caster = GetCaster();
            if (AuraEffect* aurEff = caster->GetAuraEffectOfRankedSpell(89488, EFFECT_0))
            {
                uint32 spellId = 0;
                switch (aurEff->GetId())
                {
                case 89488:
                    spellId = 96266;
                    break;
                case 89489:
                    spellId = 96267;
                    break;
                }
                caster->CastSpell(caster, spellId, true);
            }
        }

        void Register()
        {
            OnCast += SpellCastFn(spell_pri_inner_focus_SpellScript::HandleOnCast);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_inner_focus_SpellScript();
    }
};

// 94472, 81751 - Atonement heal
class spell_pri_atonement_heal : public SpellScriptLoader
{
public:
    spell_pri_atonement_heal() : SpellScriptLoader("spell_pri_atonement_heal") { }

    class spell_pri_atonement_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_atonement_heal_SpellScript);

        void HandleHeal(SpellEffIndex /*effIndex*/)
        {
            if (Unit* caster = GetOriginalCaster())
                if (GetHitUnit()->GetGUID() == caster->GetGUID())
                {
                    int32 heal = GetHitHeal() / 2.0f;
                    SetHitHeal(heal);
                }
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_atonement_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_atonement_heal_SpellScript();
    }
};

// 81781 Power Word: Barrier
class spell_pri_power_word_barrier : public SpellScriptLoader
{
public:
    spell_pri_power_word_barrier() : SpellScriptLoader("spell_pri_power_word_barrier") {}

    class spell_pri_power_word_barrier_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_power_word_barrier_AuraScript);

        bool Load()
        {
            if (Unit* caster = GetCaster())
                if (Unit* owner = caster->GetOwner())
                    if (owner->GetTypeId() == TYPEID_PLAYER && owner->HasAura(55689))
                        return false;

            return true;
        }

        void HandlePeriodicTriggerSpell(AuraEffect const* aurEff)
        {
            PreventDefaultAction();
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_power_word_barrier_AuraScript::HandlePeriodicTriggerSpell, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_power_word_barrier_AuraScript();
    }
};

// 88686
class spell_pri_holyword_sanctuary_heal : public SpellScriptLoader
{
public:
    spell_pri_holyword_sanctuary_heal() : SpellScriptLoader("spell_pri_holyword_sanctuary_heal") { }

    class spell_pri_holyword_sanctuary_heal_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_holyword_sanctuary_heal_SpellScript);

        void FilterTargets(std::list<WorldObject*>& unitList)
        {
            hitCount = unitList.size();
        }

        void HandleHeal(SpellEffIndex /*effIndex*/)
        {
            if (hitCount <= 6)
                return;

            SetHitHeal(GetHitHeal() - GetHitHeal()*(hitCount - 6) / hitCount);

        }

        void Register()
        {
            OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_pri_holyword_sanctuary_heal_SpellScript::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ALLY);
            OnEffectHitTarget += SpellEffectFn(spell_pri_holyword_sanctuary_heal_SpellScript::HandleHeal, EFFECT_0, SPELL_EFFECT_HEAL);
        }

        uint8 hitCount;
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_holyword_sanctuary_heal_SpellScript();
    }
};

// 8122 Psychic scream
class spell_pri_fear : public SpellScriptLoader
{
public:
    spell_pri_fear() : SpellScriptLoader("spell_pri_fear") { }

    class spell_pri_fear_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_fear_SpellScript);

        void HandleOnHit()
        {
            if (!GetHitUnit())
                return;

            AuraEffect* glyph = GetCaster()->GetAuraEffect(55676, EFFECT_0);
            if (Aura* debuff = GetHitAura())
            {
                if (debuff->GetApplicationOfTarget(GetHitUnit()->GetGUID()))
                {
                    // Remove root effect
                    if (AuraEffect* root = debuff->GetEffect(EFFECT_2))
                        if (!glyph)
                            root->HandleEffect(GetHitUnit(), AURA_REMOVE_BY_DEFAULT, false);
                }
            }
        }

        void Register()
        {
            OnHit += SpellHitFn(spell_pri_fear_SpellScript::HandleOnHit);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_fear_SpellScript();
    }
};

// 97690 Friendly Dispel magic
class spell_pri_friendly_dispel : public SpellScriptLoader
{
public:
    spell_pri_friendly_dispel() : SpellScriptLoader("spell_pri_friendly_dispel") { }

    class spell_pri_friendly_dispel_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_friendly_dispel_SpellScript);

        void HandleOnDispel()
        {
            // Glyph of dispel magic
            if (AuraEffect* aura = GetCaster()->GetAuraEffect(55677, EFFECT_0))
            {
                int32 bp0 = int32(GetHitUnit()->CountPctFromMaxHealth(aura->GetAmount()));
                GetCaster()->CastCustomSpell(GetHitUnit(), 56131, &bp0, NULL, NULL, true);
            }
        }

        void Register()
        {
            OnSuccessfulDispel += SpellDispelFn(spell_pri_friendly_dispel_SpellScript::HandleOnDispel);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_friendly_dispel_SpellScript();
    }
};

// 27827 Spirit of Redemption (Shapeshift) - 
class spell_pri_spirit_of_redemption_form : public SpellScriptLoader
{
public:
    spell_pri_spirit_of_redemption_form() : SpellScriptLoader("spell_pri_spirit_of_redemption_form") { }

    class spell_pri_spirit_of_redemption_form_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_spirit_of_redemption_form_AuraScript);

        void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
        {
            if (Unit* caster = GetCaster())
            {
                caster->RemoveAura(PRIEST_SPELL_SPIRIT_OF_REDEMPTION_FORM);
                caster->SetDeathState(JUST_DIED);
            }
        }

        void Register()
        {
            OnEffectRemove += AuraEffectRemoveFn(spell_pri_spirit_of_redemption_form_AuraScript::OnRemove, EFFECT_1, SPELL_AURA_WATER_BREATHING, AURA_EFFECT_HANDLE_REAL);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_spirit_of_redemption_form_AuraScript();
    }
};

// 20711 Spirit of Redemption - 
class spell_pri_spirit_of_redemption : public SpellScriptLoader
{
public:
    spell_pri_spirit_of_redemption() : SpellScriptLoader("spell_pri_spirit_of_redemption") { }

    class spell_pri_spirit_of_redemption_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_spirit_of_redemption_AuraScript);

        void CalculateAmount(AuraEffect const* /*auraEffect*/, int32& amount, bool& /*canBeRecalculated*/)
        {
            amount = -1;
        }

        void Absorb(AuraEffect* /*auraEffect*/, DamageInfo& dmgInfo, uint32& absorbAmount)
        {
            if (Unit* caster = GetCaster())
            {
                if (dmgInfo.GetDamage() < caster->GetHealth())
                    return;

                if (caster->HasAura(PRIEST_SPELL_SPIRIT_OF_REDEMPTION_SHAPESHIFT))
                    return;

                caster->CastSpell(caster, PRIEST_SPELL_SPIRIT_OF_REDEMPTION_FORM, true);
                caster->CastSpell(caster, PRIEST_SPELL_SPIRIT_OF_REDEMPTION_SHAPESHIFT, true);

                absorbAmount = caster->GetHealth() - 1;
            }
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_spirit_of_redemption_AuraScript::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
            OnEffectAbsorb += AuraEffectAbsorbFn(spell_pri_spirit_of_redemption_AuraScript::Absorb, EFFECT_0);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_spirit_of_redemption_AuraScript();
    }
};

// 81208,81206 Chakra: Serenity and Chakra: Sanctuary spell swap supressor
class spell_pri_chakra_swap_supressor : public SpellScriptLoader
{
public:
    spell_pri_chakra_swap_supressor() : SpellScriptLoader("spell_pri_chakra_swap_supressor") { }

    class spell_pri_chakra_swap_supressor_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_chakra_swap_supressor_AuraScript);

        void CalculateAmount(AuraEffect const* /*aurEff*/, int32 & amount, bool & /*canBeRecalculated*/)
        {
            if (GetCaster())
            {
                // Revelations
                if (!GetCaster()->HasAura(88627))
                    amount = 88625;
            }
        }

        void Register()
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_pri_chakra_swap_supressor_AuraScript::CalculateAmount, EFFECT_2, SPELL_AURA_OVERRIDE_ACTIONBAR_SPELLS);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_chakra_swap_supressor_AuraScript();
    }
};

// 81585 Chakra: Serenity
class spell_pri_chakra_serenity_proc : public SpellScriptLoader
{
public:
    spell_pri_chakra_serenity_proc() : SpellScriptLoader("spell_pri_chakra_serenity_proc") {}

    class spell_pri_chakra_serenity_proc_SpellScript : public SpellScript
    {
        PrepareSpellScript(spell_pri_chakra_serenity_proc_SpellScript);

        void HandleScriptEffect(SpellEffIndex /*effIndex*/)
        {
            Unit* target = GetHitUnit();

            if (!target)
                return;

            if (Aura* renew = target->GetAura(PRIEST_SPELL_RENEW, GetCaster()->GetGUID()))
                renew->RefreshDuration();
        }

        void Register()
        {
            OnEffectHitTarget += SpellEffectFn(spell_pri_chakra_serenity_proc_SpellScript::HandleScriptEffect, EFFECT_0, SPELL_EFFECT_SCRIPT_EFFECT);
        }
    };

    SpellScript* GetSpellScript() const
    {
        return new spell_pri_chakra_serenity_proc_SpellScript();
    }
};

// 88685 Chakra: Sanctuary
class spell_pri_chakra_sanctuary_heal : public SpellScriptLoader
{
public:
    spell_pri_chakra_sanctuary_heal() : SpellScriptLoader("spell_pri_chakra_sanctuary_heal") { }

    class spell_pri_chakra_sanctuary_heal_AuraScript : public AuraScript
    {
        PrepareAuraScript(spell_pri_chakra_sanctuary_heal_AuraScript);

        void OnTick(AuraEffect const* aurEff)
        {
            if (DynamicObject* dynObj = GetCaster()->GetDynObject(88685))
                if (GetCaster()->GetMapId() == dynObj->GetMapId())
                    GetCaster()->CastSpell(dynObj->GetPositionX(), dynObj->GetPositionY(), dynObj->GetPositionZ(), 88686, true);
        }

        void Register()
        {
            OnEffectPeriodic += AuraEffectPeriodicFn(spell_pri_chakra_sanctuary_heal_AuraScript::OnTick, EFFECT_1, SPELL_AURA_PERIODIC_DUMMY);
        }
    };

    AuraScript* GetAuraScript() const
    {
        return new spell_pri_chakra_sanctuary_heal_AuraScript();
    }
};

void AddSC_priest_spell_scripts()
{
    new spell_pri_cure_disease();
    new spell_pri_mind_sear_spell();
    new spell_pri_mind_blast();
    new spell_pri_mind_spike();
    new spell_pri_fade();
    new spell_pri_inner_fire();
    new spell_pri_shadow_orbs();
    new spell_pri_inner_focus();
    new spell_pri_atonement_heal();
    new spell_pri_power_word_barrier();
    new spell_pri_holyword_sanctuary_heal();
    new spell_pri_fear();
    new spell_pri_friendly_dispel();
    new spell_pri_spirit_of_redemption_form();
    new spell_pri_spirit_of_redemption();
    new spell_pri_chakra_swap_supressor();
    new spell_pri_chakra_serenity_proc();
    new spell_pri_chakra_sanctuary_heal();
    new spell_pri_body_and_soul();
    new spell_pri_circle_of_healing();
    new spell_pri_dispel_magic();
    new spell_pri_divine_aegis();
    new spell_pri_divine_hymn();
    new spell_pri_glyph_of_prayer_of_healing();
    new spell_pri_hymn_of_hope();
    new spell_pri_improved_power_word_shield();
    new spell_pri_item_greater_heal_refund();
    new spell_pri_guardian_spirit();
    new spell_pri_leap_of_faith_effect_trigger();
    new spell_pri_lightwell_renew();
    new spell_pri_mana_burn();
    new spell_pri_mana_leech();
    new spell_pri_mind_sear();
    new spell_pri_pain_and_suffering_proc();
    new spell_pri_penance();
    new spell_pri_phantasm();
    new spell_pri_power_word_shield();
    new spell_pri_prayer_of_mending_heal();
    new spell_pri_renew();
    new spell_pri_shadow_word_death();
    new spell_pri_shadowform();
    new spell_pri_vampiric_embrace();
    new spell_pri_vampiric_embrace_target();
    new spell_pri_vampiric_touch();
}

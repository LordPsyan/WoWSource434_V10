 /*
 * Copyright (C) 2013-2015 DeathCore <http://www.noffearrdeathproject.net/>
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

#include "ScriptPCH.h"
#include "shadowfang_keep.h"

class instance_shadowfang_keep : public InstanceMapScript
{
public:
    instance_shadowfang_keep() : InstanceMapScript("instance_shadowfang_keep", 33) { }

    struct instance_shadowfang_keep_InstanceMapScript: public InstanceScript
    {
        instance_shadowfang_keep_InstanceMapScript(InstanceMap* map): InstanceScript(map)  { }

        void Initialize()
        {
            SetBossNumber(MAX_ENCOUNTER);

            BaronAshburyGUID        = 0;
            BaronSilverlaineGUID    = 0;
            CommanderSpringvaleGUID = 0;
            LordGodfreyGUID         = 0;
            LordWaldenGUID          = 0;

            doorBaronAshbury        = 0;
            doorGodfrey             = 0;
            doorLordWalden          = 0;
        }

        void OnCreatureCreate(Creature* creature)
        {
            switch (creature->GetEntry())
            {
                case BOSS_BARON_ASHBURY:
                    BaronAshburyGUID        = creature->GetGUID();
                    break;
                case BOSS_BARON_SILVERLAINE:
                    BaronSilverlaineGUID    = creature->GetGUID();
                    break;
                case BOSS_COMMANDER_SPRINGVALE:
                    CommanderSpringvaleGUID = creature->GetGUID();
                    break;
                case BOSS_LORD_GODFREY:
                    LordGodfreyGUID         = creature->GetGUID();
                    break;
                case BOSS_LORD_WALDEN:
                    LordWaldenGUID          = creature->GetGUID();
                    break;
            }
        }

        void OnGameObjectCreate(GameObject* go)
        {
            switch(go->GetEntry())
            {
                case GO_BARON_ASHBURY_DOOR: doorBaronAshbury    = go->GetGUID(); break;
                case GO_LORD_GODFREY_DOOR: doorGodfrey          = go->GetGUID(); break;
                case GO_LORD_WALDEN_DOOR: doorLordWalden        = go->GetGUID(); break;
            }
        }

        void OnPlayerEnter(Player* player)
        {
            if (!TeamInInstance)
                TeamInInstance = player->GetTeam();
        }

        uint32 GetData(uint32 identifier) const
        {
            if (identifier == TEAM_IN_INSTANCE)
                return TeamInInstance;

            return 0;
        }

        uint64 GetData64(uint32 identifier) const
        {
            switch (identifier)
            {
                case DATA_BARON_ASHBURY:
                    return BaronAshburyGUID;
                case DATA_BARON_SILVERLAINE:
                    return BaronSilverlaineGUID;
                case DATA_COMMANDER_SPRINGVALE:
                    return CommanderSpringvaleGUID;
                case DATA_LORD_GODFREY:
                    return LordGodfreyGUID;
                case DATA_LORD_WALDEN:
                    return LordWaldenGUID;
                case GO_BARON_ASHBURY_DOOR:
                    return doorBaronAshbury;
                case GO_LORD_GODFREY_DOOR:
                    return doorGodfrey;
                case GO_LORD_WALDEN_DOOR:
                    return doorLordWalden;
            }
            return 0;
        }

        bool SetBossState(uint32 type, EncounterState state)
        {
            if (!InstanceScript::SetBossState(type, state))
                return false;

            switch(type)
            {
                case DATA_BARON_ASHBURY_EVENT:
                    if (state == DONE || state == FAIL)
                        HandleGameObject(doorBaronAshbury, true);
                    break;
                case DATA_LORD_GODFREY_EVENT:
                    if (state == DONE || state == FAIL)
                        HandleGameObject(doorGodfrey, true);
                    break;
                case DATA_LORD_WALDEN_EVENT:
                    if (state == DONE || state == FAIL)
                        HandleGameObject(doorLordWalden, true);
                    break;
                default:
                    break;
            }

            return true;
        }

        std::string GetSaveData()
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << "S K " << GetBossSaveData();

            OUT_SAVE_INST_DATA_COMPLETE;
            return saveStream.str();
        }

        void Load(const char* str)
        {
            if (!str)
            {
                OUT_LOAD_INST_DATA_FAIL;
                return;
            }

            OUT_LOAD_INST_DATA(str);

            char dataHead1, dataHead2;

            std::istringstream loadStream(str);
            loadStream >> dataHead1 >> dataHead2;

            if (dataHead1 == 'S' && dataHead2 == 'K')
            {
                for (uint32 i = 0; i < MAX_ENCOUNTER; ++i)
                {
                    uint32 tmpState;
                    loadStream >> tmpState;
                    if (tmpState == IN_PROGRESS || tmpState > SPECIAL)
                        tmpState = NOT_STARTED;
                    SetBossState(i, EncounterState(tmpState));
                }

            }
            else
                OUT_LOAD_INST_DATA_FAIL;

            OUT_LOAD_INST_DATA_COMPLETE;
        }
        protected:
            uint32 TeamInInstance;

            uint64 BaronAshburyGUID;
            uint64 BaronSilverlaineGUID;
            uint64 CommanderSpringvaleGUID;
            uint64 LordGodfreyGUID;
            uint64 LordWaldenGUID;

            uint64 doorBaronAshbury;
            uint64 doorGodfrey;
            uint64 doorLordWalden;
    };

    InstanceScript* GetInstanceScript(InstanceMap* map) const
    {
        return new instance_shadowfang_keep_InstanceMapScript(map);
    }
};

void AddSC_instance_shadowfang_keep()
{
    new instance_shadowfang_keep();
}
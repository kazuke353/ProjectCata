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

#include "ObjectAccessor.h"
#include "ObjectMgr.h"
#include "Opcodes.h"
#include "Player.h"
#include "WorldPacket.h"
#include "WorldSession.h"
#include "Object.h"

#include "Battlefield.h"
#include "BattlefieldMgr.h"

/**
 * @fn void WorldSession::SendBfInvitePlayerToWar(uint64 guid, uint32 zoneId, uint32 acceptTime)
 *
 * @brief This send to player windows for invite player to join the war.
 *
 * @param guid          The guid of Bf
 * @param zoneId        The zone where the battle is (4197 for wg)
 * @param acceptTime    Time in second that the player have for accept
 */
void WorldSession::SendBfInvitePlayerToWar(uint64 guid, uint32 zoneId, uint32 acceptTime)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTRY_INVITE, 16);

    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[3]);
    data.WriteBit(guidBytes[7]);
    data.WriteBit(guidBytes[2]);
    data.WriteBit(guidBytes[6]);
    data.WriteBit(guidBytes[4]);
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[0]);

    data.WriteByteSeq(guidBytes[6]);
    data << uint32(zoneId);         // Zone Id
    data.WriteByteSeq(guidBytes[1]);
    data.WriteByteSeq(guidBytes[3]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[0]);
    data << uint32(time(NULL) + acceptTime); // Invite lasts until
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[5]);
    SendPacket(&data);
}

/**
 * @fn void WorldSession::SendBfInvitePlayerToQueue(uint64 guid, bool battleHasBegun)
 *
 * @brief This send invitation to player to join the queue.
 *
 * @param guid           The guid of Bf
 * @param battleHasBegun 
 */
void WorldSession::SendBfInvitePlayerToQueue(uint64 guid, bool battleHasBegun)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_INVITE, 5);

    data.WriteBit(1);               // unk
    data.WriteBit(battleHasBegun);  // Has Warmup
    data.WriteBit(1);               // unk
    data.WriteBit(guidBytes[0]);
    data.WriteBit(1);               // unk
    data.WriteBit(guidBytes[2]);
    data.WriteBit(guidBytes[6]);
    data.WriteBit(guidBytes[3]);
    data.WriteBit(1);               // unk
    data.WriteBit(0);               // unk
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[4]);
    data.WriteBit(1);               // unk
    data.WriteBit(guidBytes[7]);

    data.FlushBits();

    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[3]);
    data.WriteByteSeq(guidBytes[6]);
    data << uint8(battleHasBegun);   // Warmup
    data.WriteByteSeq(guidBytes[5]);
    data.WriteByteSeq(guidBytes[0]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[1]);
    data.WriteByteSeq(guidBytes[7]);
    SendPacket(&data);
}

/**
 * @fn void WorldSession::SendBfQueueInviteResponse(uint64 battlefieldGuid, uint64 queueGuid, uint32 ZoneId, bool CanQueue, bool Full, bool battleHasBegun)
 *
 * @brief This send packet for inform player that he join queue.
 *
 * @param battlefieldGuid  The guid of Bf
 * @param queueGuid
 * @param zoneId           The zone where the battle is (4197 for wg)
 * @param canQueue         if able to queue
 * @param full             on log in is full
 * @param battleHasBegun
 */
void WorldSession::SendBfQueueInviteResponse(uint64 battlefieldGuid, uint64 queueGuid, uint32 ZoneId, bool CanQueue, bool Full, bool battleHasBegun)
{
    ObjectGuid battlefieldGuidBytes = battlefieldGuid;
    ObjectGuid queueGuidBytes = queueGuid;

    const bool hasSecondGuid = false;
    
    WorldPacket data(SMSG_BATTLEFIELD_MGR_QUEUE_REQUEST_RESPONSE);

    data.WriteBit(battlefieldGuidBytes[1]);
    data.WriteBit(battlefieldGuidBytes[6]);
    data.WriteBit(battlefieldGuidBytes[5]);
    data.WriteBit(battlefieldGuidBytes[7]);
    data.WriteBit(Full);  // Logging In, VERIFYME
    data.WriteBit(battlefieldGuidBytes[0]);
    data.WriteBit(!hasSecondGuid);  // send the queue ID
    data.WriteBit(battlefieldGuidBytes[4]);

    // if (hasSecondGuid) 7 3 0 4 2 6 1 5

    data.WriteBit(queueGuidBytes[7]);
    data.WriteBit(queueGuidBytes[3]);
    data.WriteBit(queueGuidBytes[0]);
    data.WriteBit(queueGuidBytes[4]);
    data.WriteBit(queueGuidBytes[2]);
    data.WriteBit(queueGuidBytes[6]);
    data.WriteBit(queueGuidBytes[1]);
    data.WriteBit(queueGuidBytes[5]);

    data.WriteBit(battlefieldGuidBytes[3]);
    data.WriteBit(battlefieldGuidBytes[2]);

    data.FlushBits();

    // if (hasSecondGuid) 2 5 3 0 4 6 1 7

    data.WriteByteSeq(queueGuidBytes[2]);
    data.WriteByteSeq(queueGuidBytes[5]);
    data.WriteByteSeq(queueGuidBytes[3]);
    data.WriteByteSeq(queueGuidBytes[0]);
    data.WriteByteSeq(queueGuidBytes[4]);
    data.WriteByteSeq(queueGuidBytes[6]);
    data.WriteByteSeq(queueGuidBytes[1]);
    data.WriteByteSeq(queueGuidBytes[7]);

    data << uint8(CanQueue);                    // Accepted

    data.WriteByteSeq(battlefieldGuidBytes[1]);
    data.WriteByteSeq(battlefieldGuidBytes[3]);
    data.WriteByteSeq(battlefieldGuidBytes[6]);
    data.WriteByteSeq(battlefieldGuidBytes[7]);
    data.WriteByteSeq(battlefieldGuidBytes[0]);

    data << uint8(battleHasBegun);  // warmup?

    data.WriteByteSeq(battlefieldGuidBytes[2]);
    data.WriteByteSeq(battlefieldGuidBytes[4]);
    data.WriteByteSeq(battlefieldGuidBytes[5]);

    data << uint32(ZoneId);

    SendPacket(&data);
}

/**
 * @fn void WorldSession::SendBfEntered(uint64 guid)
 *
 * @brief This is call when player accept to join war.
 *
 * @param guid           The guid of Bf
 */
void WorldSession::SendBfEntered(uint64 guid)
{
    uint8 isAFK = _player->isAFK() ? 1 : 0;
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_ENTERED, 11);

    data.WriteBit(0);               // unk
    data.WriteBit(isAFK);           // Clear AFK
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[4]);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[0]);
    data.WriteBit(guidBytes[3]);
    data.WriteBit(0);               // unk
    data.WriteBit(guidBytes[6]);
    data.WriteBit(guidBytes[7]);
    data.WriteBit(guidBytes[2]);

    data.FlushBits();

    data.WriteByteSeq(guidBytes[5]);
    data.WriteByteSeq(guidBytes[3]);
    data.WriteByteSeq(guidBytes[0]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[1]);
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[6]);

    SendPacket(&data);
}

/**
 * @fn void WorldSession::SendBfLeaveMessage(uint64 guid, BFLeaveReason reason)
 *
 * @brief This is call when player leave battlefield zone.
 *
 * @param guid          The guid of Bf
 * @param reason        Reason why player left battlefield
 */
void WorldSession::SendBfLeaveMessage(uint64 guid, BFLeaveReason reason /*= BF_LEAVE_REASON_EXITED*/)
{
    ObjectGuid guidBytes = guid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_EJECTED, 11);

    data.WriteBit(guidBytes[2]);
    data.WriteBit(guidBytes[5]);
    data.WriteBit(guidBytes[1]);
    data.WriteBit(guidBytes[0]);
    data.WriteBit(guidBytes[3]);
    data.WriteBit(guidBytes[6]);
    data.WriteBit(0);               // Relocated
    data.WriteBit(guidBytes[7]);
    data.WriteBit(guidBytes[4]);

    data.FlushBits();

    data << uint8(2);               // BattleStatus
    data.WriteByteSeq(guidBytes[1]);
    data.WriteByteSeq(guidBytes[7]);
    data.WriteByteSeq(guidBytes[4]);
    data.WriteByteSeq(guidBytes[2]);
    data.WriteByteSeq(guidBytes[3]);
    data << uint8(reason);          // Reason
    data.WriteByteSeq(guidBytes[6]);
    data.WriteByteSeq(guidBytes[0]);
    data.WriteByteSeq(guidBytes[5]);

    SendPacket(&data);
}

/**
 * @fn void WorldSession::SendBfExitRequest(uint64 playerGuid, uint32 zoneId)
 *
 * @brief Send by client when exited battlefield
 *
 * @param playerGuid    The player guid of Bf
 * @param zoneId        The zone where the battle is (4197 for wg)
 */
void WorldSession::SendBfExitRequest(uint64 playerGuid, uint32 zoneId)
{
    ObjectGuid guid = playerGuid;

    WorldPacket data(SMSG_BATTLEFIELD_MGR_EXIT_REQUEST);

    data.WriteBit(guid[0]);
    data.WriteBit(guid[6]);
    data.WriteBit(guid[3]);
    data.WriteBit(guid[1]);
    data.WriteBit(guid[7]);
    data.WriteBit(guid[5]);
    data.WriteBit(guid[2]);
    data.WriteBit(guid[4]);

    data.FlushBits();

    data << uint32(0);                  // unk dword +7

    data.WriteByteSeq(guid[4]);
    data.WriteByteSeq(guid[1]);

    data << uint32(zoneId);
    data << uint32(0);                  // unk dword +6

    data.WriteByteSeq(guid[6]);
    data.WriteByteSeq(guid[5]);
    data.WriteByteSeq(guid[2]);
    data.WriteByteSeq(guid[7]);
    data.WriteByteSeq(guid[3]);
    data.WriteByteSeq(guid[0]);

    SendPacket(&data);
}

/**
 * @fn void WorldSession::HandleBfQueueInviteResponse(WorldPacket& recvData)
 *
 * @brief Send by client when he click on accept for queue.
 */
void WorldSession::HandleBfQueueInviteResponse(WorldPacket& recvData)
{
    uint8 accepted;
    ObjectGuid guid;

    guid[2] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    accepted = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[5]);

    TC_LOG_ERROR("misc", "HandleQueueInviteResponse: GUID: " UI64FMTD ", accepted: %u", uint64(guid), accepted);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    if (accepted)
        bf->PlayerAcceptInviteToQueue(_player);
}

/**
 * @fn void WorldSession::HandleBfEntryInviteResponse(WorldPacket& recvData)
 *
 * @brief Send by client on clicking in accept or refuse of invitation windows for join game.
 */
void WorldSession::HandleBfEntryInviteResponse(WorldPacket& recvData)
{
    uint8 accepted;
    ObjectGuid guid;

    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    accepted = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[5]);

    TC_LOG_ERROR("misc", "HandleBattlefieldInviteResponse: GUID: " UI64FMTD ", accepted: %u", uint64(guid), accepted);

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    // If player accept invitation
    if (accepted)
    {
        bf->PlayerAcceptInviteToWar(_player);
    }
    else
    {
        if (_player->GetZoneId() == bf->GetZoneId())
            bf->KickPlayerFromBattlefield(_player->GetGUID());
    }
}

/**
 * @fn void WorldSession::HandleBfExitRequest(WorldPacket& recvData)
 *
 * @brief Send by client when exited battlefield
 */
void WorldSession::HandleBfExitRequest(WorldPacket& recvData)
{
    ObjectGuid guid;

    guid[2] = recvData.ReadBit();
    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[0]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[6]);

    TC_LOG_ERROR("misc", "HandleBfExitRequest: GUID: " UI64FMTD, uint64(guid));

    Battlefield* bf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if (!bf)
        return;

    bf->AskToLeaveQueue(_player);
}

/**
 * @fn void WorldSession::HandleBfQueueRequest(WorldPacket & recvData)
 *
 * @brief Send by client when queued battlefield
 */
void WorldSession::HandleBfQueueRequest(WorldPacket & recvData)
{
    ObjectGuid guid;

    guid[0] = recvData.ReadBit();
    guid[3] = recvData.ReadBit();
    guid[7] = recvData.ReadBit();
    guid[4] = recvData.ReadBit();
    guid[6] = recvData.ReadBit();
    guid[2] = recvData.ReadBit();
    guid[1] = recvData.ReadBit();
    guid[5] = recvData.ReadBit();

    recvData.ReadByteSeq(guid[6]);
    recvData.ReadByteSeq(guid[3]);
    recvData.ReadByteSeq(guid[2]);
    recvData.ReadByteSeq(guid[4]);
    recvData.ReadByteSeq(guid[7]);
    recvData.ReadByteSeq(guid[1]);
    recvData.ReadByteSeq(guid[5]);
    recvData.ReadByteSeq(guid[0]);

    Battlefield *pBf = sBattlefieldMgr->GetBattlefieldByGUID(guid);
    if(pBf)
    {
        BattlefieldQueue *pQueue = sBattlefieldMgr->GetQueueForBattlefield(guid);
        if(pQueue)
        {
            bool canJoin = true;

            //check if player is queued in BG/LFG, if so - set canJoin to false
            if(pQueue->HasEnoughSpace(GetPlayer()) && canJoin)
                pQueue->AddPlayerToQueue(GetPlayer());

            SendBfQueueInviteResponse(guid,pQueue->GetId(),pBf->GetZoneId(),canJoin,!pQueue->HasEnoughSpace(GetPlayer()),pBf->IsWarTime());
        }
    }
}
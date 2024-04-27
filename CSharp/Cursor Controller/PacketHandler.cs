using Google.Protobuf;
using Google.Protobuf.Protocol;
using Network;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.AI;

class PacketHandler
{
	public static void S_EnterGameHandler(PacketSession session, IMessage packet)
	{
		S_EnterGame enterGamePacket = packet as S_EnterGame;

		Managers.Object.Add(enterGamePacket.Player, true);
	}
	public static void S_LeaveGameHandler(PacketSession session, IMessage packet)
	{
		S_LeaveGame leavePacket = packet as S_LeaveGame;
		
		Managers.Object.MyPlayer.Hp = -99;
	}
	public static void S_SpawnHandler(PacketSession session, IMessage packet)
	{
		S_Spawn spawnPacket = packet as S_Spawn;

		int packetSize = spawnPacket.Objects.Count;
		for (int i = 0; i < packetSize; ++i)
		{
			Managers.Object.Add(spawnPacket.Objects[i], spawnPacket.Dests[i].ToVector3(), spawnPacket.Poss[i].ToVector3(), spawnPacket.States[i]);
		}
	}
	public static void S_DespawnHandler(PacketSession session, IMessage packet)
	{
		S_Despawn despawnPacket = packet as S_Despawn;
		foreach (var id in despawnPacket.ObjectIds)
		{
			Managers.Object.Remove(id);
		}
	}
	public static void S_PlayerDespawnHandler(PacketSession session, IMessage packet)
	{
		S_PlayerDespawn despawnPacket = packet as S_PlayerDespawn;
		foreach (var id in despawnPacket.PlayerIds)
		{
			Managers.Object.PlayerRemove(id);
		}
	}
	public static void S_MoveHandler(PacketSession session, IMessage packet)
	{
		S_Move movePacket = packet as S_Move;
		ServerSession serverSession = session as ServerSession;
		
		//누가이동하는지를 찾는다(Managers.Object.Find()?)
		//찾은 게임오브젝트의 인포를 바꾼다.
		GameObject go = Managers.Object.FindById(GameObjectType.Player, movePacket.ObjectId);
		if (go == null)
			return;

		go.GetComponent<NavMeshAgent>().SetDestination(movePacket.Dest.ToVector3());
	}

	public static void S_TeleportHandler(PacketSession session, IMessage packet)
	{
		S_Teleport teleportPacket = packet as S_Teleport;
		
		GameObject go = Managers.Object.FindById(GameObjectType.Player, teleportPacket.ObjectId);
		if (go == null)
			return;
		
		//Vector3 vec = Managers.Map.GetPoint(teleportPacket.Target);
		//go.transform.position = vec;
	}

	public static void S_SyncPlayerHandler(PacketSession session, IMessage packet)
	{
		S_SyncPlayer syncPacket = packet as S_SyncPlayer;
		
		GameObject go = Managers.Object.FindById(GameObjectType.Player, syncPacket.PlayerId);
		if (go == null)
			return;

		go.GetComponent<Player>().SyncPos(syncPacket.Pos.ToVector3());
	}

	public static void S_SyncStateHandler(PacketSession session, IMessage packet)
	{
		S_SyncState syncPacket = packet as S_SyncState;
		
		GameObject go = Managers.Object.FindById(GameObjectType.Player, syncPacket.PlayerId);
		if (go == null)
			return;

		go.GetComponent<Player>().SyncState(syncPacket.State);
	}
}

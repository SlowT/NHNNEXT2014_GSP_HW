#pragma once

#include "SyncExecutable.h"
#include "ThreadLocal.h"

#define TIME_ALLOW_WITHOUT_ACT	3000

class ClientSession;


class Player : public SyncExecutable, PooledAllocatable
{
public:
	Player(ClientSession* session);
	virtual ~Player();

	int GetPlayerId() { return mPlayerId; }
	bool IsAlive() { return mIsAlive;  }
	void Start(int heartbeat);

	void OnTick(); ///< �α����� 1�ʸ��� �Ҹ��� ���

	void PlayerReset();

	/// �÷��̾�� ������ �ɾ��ִ� �Լ���� ġ��.
	void AddBuff(int fromPlayerId, int buffId, int duration);

	/// �ֱ������� ���� �ð� ������Ʈ�ϴ� �Լ�
	void DecayTickBuff();

	void UpdateLastTick(){ InterlockedExchange64(&mLastTick, LTickCount); }
private:

	int		mPlayerId;
	int		mHeartBeat;
	bool	mIsAlive;
	int64_t	mLastTick;

	/// ���� ����Ʈ�� lock���� GCE�� �غ���
	std::map<int, int> mBuffList; ///< (id, time)

	ClientSession* const mSession;
	friend class ClientSession;
};
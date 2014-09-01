#include "stdafx.h"
#include "SQLStatement.h"
#include "Log.h"
#include "PlayerDBContext.h"
#include "DBHelper.h"
#include "ClientSession.h"


//todo: CreatePlayerDataContext ����
bool CreatePlayerDataContext::OnSQLExecute()
{
	DBHelper dbHelper;
	/*
	dbHelper.BindParamText( mPlayerName );

	dbHelper.BindResultColumnInt( &mPlayerId );

	if( dbHelper.Execute( SQL_CreatePlayer ) )
	{
		if( dbHelper.FetchRow() )
		{
			return true;
		}
	}
	*/

	int result = 0;

	dbHelper.BindParamText(mPlayerName);
	dbHelper.BindResultColumnInt(&result);

	if (dbHelper.Execute(SQL_CreatePlayer))
	{
		if (dbHelper.FetchRow())
		{
			/// ������� ���� �ϳ��� ���ٸ�, ���ж�� ��������
			return result != 0;
		}
	}


	return false;
}

void CreatePlayerDataContext::OnSuccess()
{
	EVENT_LOG( "CreatePlayerDataContext success", mPlayerId );
	///for test
	mSessionObject->mPlayer.RequestLoad( mPlayerId );
}

void CreatePlayerDataContext::OnFail()
{
	EVENT_LOG( "CreatePlayerDataContext fail", mPlayerId );
}

void CreatePlayerDataContext::SetNewName( const wchar_t* name )
{
	wcscpy_s( mPlayerName, name );
}



//todo: DeletePlayerDataContext ����
bool DeletePlayerDataContext::OnSQLExecute()
{
	DBHelper dbHelper;
	int result = 0;

	dbHelper.BindParamInt( &mPlayerId );

	dbHelper.BindResultColumnInt( &result );

	if( dbHelper.Execute( SQL_CreatePlayer ) ) ///# ����� �׽�Ʈ ���غñ���...
	{
		if( dbHelper.FetchRow() )
		{
			return result != 0;
		}
	}

	return false;
}

void DeletePlayerDataContext::OnSuccess()
{
	EVENT_LOG( "DeletePlayerDataContext success", mPlayerId );
}

void DeletePlayerDataContext::OnFail()
{
	EVENT_LOG( "DeletePlayerDataContext fail", mPlayerId );
}



bool LoadPlayerDataContext::OnSQLExecute()
{
	DBHelper dbHelper;

	dbHelper.BindParamInt(&mPlayerId);

	dbHelper.BindResultColumnText(mPlayerName, MAX_NAME_LEN);
	dbHelper.BindResultColumnFloat(&mPosX);
	dbHelper.BindResultColumnFloat(&mPosY);
	dbHelper.BindResultColumnFloat(&mPosZ);
	dbHelper.BindResultColumnBool(&mIsValid);
	dbHelper.BindResultColumnText(mComment, MAX_COMMENT_LEN);

	if (dbHelper.Execute(SQL_LoadPlayer))
	{
		if (dbHelper.FetchRow())
		{
			return true;
		}
	}

	return false;
}

void LoadPlayerDataContext::OnSuccess()
{
	//todo: �÷��̾� �ε� ������ ó���ϱ�
	mSessionObject->mPlayer.ResponseLoad( mPlayerId, mPosX, mPosY, mPosZ, mIsValid, mPlayerName, mComment );
}

void LoadPlayerDataContext::OnFail()
{
	EVENT_LOG("LoadPlayerDataContext fail", mPlayerId);

	///for test
	wchar_t tmp[21] = { 0, };
	wchar_t tName[32] = L"TestPlayer_";
	_itow_s( (int)mSessionObject, tmp, 10 );
	wcscat_s( tName, tmp );
	mSessionObject->mPlayer.TestCreatePlayerData( tName );
}


bool UpdatePlayerPositionContext::OnSQLExecute()
{
	DBHelper dbHelper;
	int result = 0;

	dbHelper.BindParamInt(&mPlayerId);
	dbHelper.BindParamFloat(&mPosX);
	dbHelper.BindParamFloat(&mPosY);
	dbHelper.BindParamFloat(&mPosZ);

	dbHelper.BindResultColumnInt(&result);

	if (dbHelper.Execute(SQL_UpdatePlayerPosition))
	{
		if (dbHelper.FetchRow())
		{
			return result != 0;
		}
	}

	return false;
}

void UpdatePlayerPositionContext::OnSuccess()
{
	mSessionObject->mPlayer.ResponseUpdatePosition(mPosX, mPosY, mPosZ);
}




bool UpdatePlayerCommentContext::OnSQLExecute()
{
	DBHelper dbHelper;
	int result = 0;
	dbHelper.BindParamInt(&mPlayerId);
	dbHelper.BindParamText(mComment);

	dbHelper.BindResultColumnInt(&result);

	if (dbHelper.Execute(SQL_UpdatePlayerComment))
	{
		if (dbHelper.FetchRow())
		{
			return result != 0;
		}
	}

	return false;
}

void UpdatePlayerCommentContext::SetNewComment(const wchar_t* comment)
{
	wcscpy_s(mComment, comment);
}

void UpdatePlayerCommentContext::OnSuccess()
{
	mSessionObject->mPlayer.ResponseUpdateComment(mComment);
}



bool UpdatePlayerValidContext::OnSQLExecute()
{
	DBHelper dbHelper;
	int result = 0;

	dbHelper.BindParamInt(&mPlayerId);
	dbHelper.BindParamBool(&mIsValid);

	dbHelper.BindResultColumnInt(&result);

	if (dbHelper.Execute(SQL_UpdatePlayerValid))
	{
		if (dbHelper.FetchRow())
		{
			return result !=0 ;
		}
	}

	return false;
}

void UpdatePlayerValidContext::OnSuccess()
{
	mSessionObject->mPlayer.ResponseUpdateValidation(mIsValid);
}



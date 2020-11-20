#ifndef DPTOOLS_H
#define DPTOOLS_H


#define MSG_HOST        1
#define MSG_SYNC        2
#define MSG_FIRE		3
#define MSG_CONTROL     4


typedef struct _GENERICMSG
{
    BYTE        byType;

} GENERICMSG, *LPGENERICMSG;


typedef struct _HOSTMSG
{
    BYTE        byType;

    BYTE		bySlot;
} HOSTMSG, *LPHOSTMSG;


typedef struct _FIREMSG
{
	BYTE		byType;
	
	int			sddata;

} FIREMSG, *LPFIREMSG;


typedef struct _CONTROLMSG
{
    BYTE        byType;

	BYTE        bySlot;
    BYTE        byState;
} CONTROLMSG, *LPCONTROLMSG;


typedef struct _SYNCMSG
{
    BYTE        byType;
    
	int x;							//data for vehicle 1
	int y;
	int et;
	int data;						//the compressed data for v1
} SYNCMSG, *LPSYNCMSG;




BOOL	FAR PASCAL EnumPlayer( DPID, DWORD, LPCDPNAME, DWORD, LPVOID );
BOOL	WINAPI EnumConnection( LPCGUID, LPVOID, DWORD, LPDPNAME, DWORD, LPVOID );
BOOL	WINAPI EnumSession( LPDPSESSIONDESC2, LPDWORD, DWORD, LPVOID );
LRESULT CALLBACK DlgProcEnumPlayers( HWND, UINT, WPARAM, LPARAM );
BOOL	CreateGamePlayer( void );
BOOL	CALLBACK DlgProcDPStart( HWND, UINT, WPARAM, LPARAM );
void	EvaluateSystemMessage( DPMSG_GENERIC, HWND );
void	EvaluateGameMessage( DPMSG_GENERIC, DPID, HWND );

int		ReceiveFireMessage();


void	ReceiveMessages( HWND );
DWORD	WINAPI ReceiveThread( LPVOID );
int		StartDPSession( void );
BOOL	ShutDownDPSession( void );
void    SendSyncMessage();
void	SendFireMessage(int);

void	SendControlMessage( BYTE byControl );
void	SendGameMessage( LPGENERICMSG, DPID );

BOOL    InitializeDirectPlay( HANDLE);
void    UninitializeDirectPlay();



extern BOOL		g_bHost;				// are we host
extern DPID		g_dpPlayerID;           // our id
extern char		g_szPlayerName[31];		// our name






#endif // DPTOOLS_H

//edit list
// SIM 4-22-99 cleaned up. Removed all extraneous stuff.
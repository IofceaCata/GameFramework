//-----------------------------------------------------------------------------
// File: CGameApp.cpp
//
// Desc: Game Application class, this is the central hub for all app processing
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CGameApp Specific Includes
//-----------------------------------------------------------------------------
#include "CGameApp.h"

extern HINSTANCE g_hInst;
#define VK_KEY_Q	0x51 /* 'Q' key */
#define VK_KEY_W	0x57
#define VK_KEY_S	0x53
#define VK_KEY_A	0X41
#define VK_KEY_D	0X44

#define PLAYER false
#define ENEMY true

extern		HINSTANCE g_hInst;
bool		p1Shoot = false;
bool		p2Shoot = false;



//-----------------------------------------------------------------------------
// CGameApp Member Functions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CGameApp () (Constructor)
// Desc : CGameApp Class Constructor
//-----------------------------------------------------------------------------
CGameApp::CGameApp()
{
	// Reset / Clear all required values
	m_hWnd			= NULL;
	m_hIcon			= NULL;
	m_hMenu			= NULL;
	m_pBBuffer		= NULL;
	m_pPlayer		= NULL;
	m_pPlayer2      = NULL;
	m_LastFrameRate = 0;
}

//-----------------------------------------------------------------------------
// Name : ~CGameApp () (Destructor)
// Desc : CGameApp Class Destructor
//-----------------------------------------------------------------------------
CGameApp::~CGameApp()
{
	// Shut the engine down
	ShutDown();
}

//-----------------------------------------------------------------------------
// Name : InitInstance ()
// Desc : Initialises the entire Engine here.
//-----------------------------------------------------------------------------
bool CGameApp::InitInstance( LPCTSTR lpCmdLine, int iCmdShow )
{
	// Create the primary display device
	if (!CreateDisplay()) { ShutDown(); return false; }

	// Build Objects
	if (!BuildObjects()) 
	{ 
		MessageBox( 0, _T("Failed to initialize properly. Reinstalling the application may solve this problem.\nIf the problem persists, please contact technical support."), _T("Fatal Error"), MB_OK | MB_ICONSTOP);
		ShutDown(); 
		return false; 
	}

	// Set up all required game states
	SetupGameState();

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : CreateDisplay ()
// Desc : Create the display windows, devices etc, ready for rendering.
//-----------------------------------------------------------------------------
bool CGameApp::CreateDisplay()
{
	HMONITOR hmon = MonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
	MONITORINFO mi = { sizeof(mi) };
	if (!GetMonitorInfo(hmon, &mi)) return NULL;

	LPTSTR			WindowTitle		= _T("GameFramework");
	LPCSTR			WindowClass		= _T("GameFramework_Class");
	USHORT			Width			= mi .rcMonitor.right;
	USHORT			Height			= mi .rcMonitor.bottom;
	RECT			rc;
	WNDCLASSEX		wcex;


	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= CGameApp::StaticWndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= g_hInst;
	wcex.hIcon			= LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= WindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_ICON));

	if(RegisterClassEx(&wcex)==0)
		return false;

	// Retrieve the final client size of the window
	::GetClientRect( m_hWnd, &rc );
	m_nViewX		= rc.left;
	m_nViewY		= rc.top;
	m_nViewWidth	= rc.right - rc.left;
	m_nViewHeight	= rc.bottom - rc.top;

	m_hWnd = CreateWindow(WindowClass, WindowTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, Width, Height, NULL, NULL, g_hInst, this);

	if (!m_hWnd)
		return false;

	// Show the window
	ShowWindow(m_hWnd, SW_SHOW);

	// Success!!
	return true;
}

//-----------------------------------------------------------------------------
// Name : BeginGame ()
// Desc : Signals the beginning of the physical post-initialisation stage.
//		From here on, the game engine has control over processing.
//-----------------------------------------------------------------------------
int CGameApp::BeginGame()
{
	MSG		msg;

	// Start main loop
	while(true) 
	{
		// Did we recieve a message, or are we idling ?
		if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) ) 
		{
			if (msg.message == WM_QUIT) break;
			TranslateMessage( &msg );
			DispatchMessage ( &msg );
		} 
		else 
		{
			// Advance Game Frame.
			FrameAdvance();

		} // End If messages waiting
	
	} // Until quit message is receieved

	return 0;
}

//-----------------------------------------------------------------------------
// Name : ShutDown ()
// Desc : Shuts down the game engine, and frees up all resources.
//-----------------------------------------------------------------------------
bool CGameApp::ShutDown()
{
	// Release any previously built objects
	ReleaseObjects ( );
	
	// Destroy menu, it may not be attached
	if ( m_hMenu ) DestroyMenu( m_hMenu );
	m_hMenu		 = NULL;

	// Destroy the render window
	SetMenu( m_hWnd, NULL );
	if ( m_hWnd ) DestroyWindow( m_hWnd );
	m_hWnd		  = NULL;
	
	// Shutdown Success
	return true;
}

//-----------------------------------------------------------------------------
// Name : StaticWndProc () (Static Callback)
// Desc : This is the main messge pump for ALL display devices, it captures
//		the appropriate messages, and routes them through to the application
//		class for which it was intended, therefore giving full class access.
// Note : It is VITALLY important that you should pass your 'this' pointer to
//		the lpParam parameter of the CreateWindow function if you wish to be
//		able to pass messages back to that app object.
//-----------------------------------------------------------------------------
LRESULT CALLBACK CGameApp::StaticWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	// If this is a create message, trap the 'this' pointer passed in and store it within the window.
	if ( Message == WM_CREATE ) SetWindowLong( hWnd, GWL_USERDATA, (LONG)((CREATESTRUCT FAR *)lParam)->lpCreateParams);

	// Obtain the correct destination for this message
	CGameApp *Destination = (CGameApp*)GetWindowLong( hWnd, GWL_USERDATA );
	
	// If the hWnd has a related class, pass it through
	if (Destination) return Destination->DisplayWndProc( hWnd, Message, wParam, lParam );
	
	// No destination found, defer to system...
	return DefWindowProc( hWnd, Message, wParam, lParam );
}

//-----------------------------------------------------------------------------
// Name : DisplayWndProc ()
// Desc : The display devices internal WndProc function. All messages being
//		passed to this function are relative to the window it owns.
//-----------------------------------------------------------------------------
LRESULT CGameApp::DisplayWndProc( HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam )
{
	static UINT			fTimer;	

	// Determine message type
	switch (Message)
	{
		case WM_CREATE:
			break;
		
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		case WM_SIZE:
			if ( wParam == SIZE_MINIMIZED )
			{
				// App is inactive
				m_bActive = false;
			
			} // App has been minimized
			else
			{
				// App is active
				m_bActive = true;

				// Store new viewport sizes
				m_nViewWidth  = LOWORD( lParam );
				m_nViewHeight = HIWORD( lParam );
		
			
			} // End if !Minimized

			break;

		case WM_LBUTTONDOWN:
			// Capture the mouse
			SetCapture( m_hWnd );
			GetCursorPos( &m_OldCursorPos );
			break;

		case WM_LBUTTONUP:
			// Release the mouse
			ReleaseCapture( );
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			case VK_RETURN:
				fTimer = SetTimer(m_hWnd, 1, 250, NULL);
				m_pPlayer->Explode();
				
				break;
			case VK_SPACE:
				m_pPlayer->ExplodeLaser();
				//m_pPlayer2->ExplodeLaser();
				//m_pPlayer2->Explode();

			case VK_KEY_Q:
				fTimer = SetTimer(m_hWnd, 1, 250, NULL);
				m_pPlayer2->Explode();


			}
			break;

		case WM_TIMER:
			switch(wParam)
			{
			case 1:
				if(!m_pPlayer->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
				if (!m_pPlayer2->AdvanceExplosion())
					KillTimer(m_hWnd, 1);
			

			}
			break;

		case WM_COMMAND:
			break;

		default:
			return DefWindowProc(hWnd, Message, wParam, lParam);

	} // End Message Switch
	
	return 0;
}

//-----------------------------------------------------------------------------
// Name : BuildObjects ()
// Desc : Build our demonstration meshes, and the objects that instance them
//-----------------------------------------------------------------------------
bool CGameApp::BuildObjects()
{
	//_Buffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pBBuffer = new BackBuffer(m_hWnd, m_nViewWidth, m_nViewHeight);
	//m_pBBuffer2 = new BackBuffer2(m_hWnd, m_nViewWidth, m_nViewHeight);
	m_pPlayer = new CPlayer(m_pBBuffer);
	m_pPlayer2 = new CPlayer2 ( m_pBBuffer);
	m_pPlayer3 = new CPlayer(m_pBBuffer);

	if(!m_imgBackground.LoadBitmapFromFile("data/background.bmp", GetDC(m_hWnd)))
		return false;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
// Name : SetupGameState ()
// Desc : Sets up all the initial states required by the game.
//-----------------------------------------------------------------------------
void CGameApp::SetupGameState()
{
	m_pPlayer->Position() = Vec2(1300, 600);
	m_pPlayer2->Position() = Vec3(1000, 400);
}

//-----------------------------------------------------------------------------
// Name : ReleaseObjects ()
// Desc : Releases our objects and their associated memory so that we can
//		rebuild them, if required, during our applications life-time.
//-----------------------------------------------------------------------------
void CGameApp::ReleaseObjects( )
{
	if(m_pPlayer != NULL)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}

	if (m_pPlayer2 != NULL)
	{
		delete m_pPlayer2;
		m_pPlayer2 = NULL;
	}

	if(m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}

	if (m_pBBuffer != NULL)
	{
		delete m_pBBuffer;
		m_pBBuffer = NULL;
	}

	/*
	if (_Buffer != NULL)
	{
		delete _Buffer;
		_Buffer = NULL;
	}
	*/
}

//-----------------------------------------------------------------------------
// Name : FrameAdvance () (Private)
// Desc : Called to signal that we are now rendering the next frame.
//-----------------------------------------------------------------------------
void CGameApp::FrameAdvance()
{
	static TCHAR FrameRate[ 50 ];
	static TCHAR TitleBuffer[ 255 ];

	// Advance the timer
	m_Timer.Tick( );

	// Skip if app is inactive
	if ( !m_bActive ) return;
	
	// Get / Display the framerate
	if ( m_LastFrameRate != m_Timer.GetFrameRate() )
	{
		m_LastFrameRate = m_Timer.GetFrameRate( FrameRate, 50 );
		sprintf_s( TitleBuffer, _T("Game : %s"), FrameRate );
		SetWindowText( m_hWnd, TitleBuffer );

	} // End if Frame Rate Altered

	// Poll & Process input devices
	ProcessInput();

	// Animate the game objects
	AnimateObjects();

	// Drawing the game objects
	DrawObjects();
}

//-----------------------------------------------------------------------------
// Name : ProcessInput () (Private)
// Desc : Simply polls the input devices and performs basic input operations
//-----------------------------------------------------------------------------
void CGameApp::ProcessInput( )
{
	static UCHAR pKeyBuffer[ 256 ];
	ULONG		Direction = 0;
	ULONG		Direction2 = 0;
	POINT		CursorPos;
	float		X = 0.0f, Y = 0.0f;

	// Retrieve keyboard state
	if ( !GetKeyboardState( pKeyBuffer ) ) return;

	// Check the relevant keys
	if ( pKeyBuffer[ VK_UP	] & 0xF0 ) Direction |= CPlayer::DIR_FORWARD;
	if ( pKeyBuffer[ VK_DOWN  ] & 0xF0 ) Direction |= CPlayer::DIR_BACKWARD;
	if ( pKeyBuffer[ VK_LEFT  ] & 0xF0 ) Direction |= CPlayer::DIR_LEFT;
	if ( pKeyBuffer[ VK_RIGHT ] & 0xF0 ) Direction |= CPlayer::DIR_RIGHT;
	///////////////////////////////////////////////////////////////////////////////
	if (pKeyBuffer[VK_KEY_W] & 0xF0) Direction2 |= CPlayer2::DIR_FORWARD;
	if (pKeyBuffer[VK_KEY_S] & 0xF0) Direction2 |= CPlayer2::DIR_BACKWARD;
	if (pKeyBuffer[VK_KEY_A] & 0xF0) Direction2 |= CPlayer2::DIR_LEFT;
	if (pKeyBuffer[VK_KEY_D] & 0xF0) Direction2 |= CPlayer2::DIR_RIGHT;
	if (pKeyBuffer[VK_NUMPAD0] & 0xF0) {
		p2Shoot = true;
	}
	else {
		if (p2Shoot)
			SpawnBullet(m_pPlayer->Position(), Vec2(0, -800), PLAYER);
		p2Shoot = false;
	}


	
	// Move the player
		//if(m_pPlayer->Position > Vec2(60 , 100))
			m_pPlayer->Move(Direction);
			m_pPlayer2->Move(Direction2);
			m_pPlayer3->Move(Direction);


	// Now process the mouse (if the button is pressed)
	if ( GetCapture() == m_hWnd )
	{
		// Hide the mouse pointer
		SetCursor( NULL );

		// Retrieve the cursor position
		GetCursorPos( &CursorPos );

		// Reset our cursor position so we can keep going forever :)
		SetCursorPos( m_OldCursorPos.x, m_OldCursorPos.y );

	} // End if Captured
}

//-----------------------------------------------------------------------------
// Name : AnimateObjects () (Private)
// Desc : Animates the objects we currently have loaded.
//-----------------------------------------------------------------------------
void CGameApp::AnimateObjects()
{
	m_pPlayer->Update(m_Timer.GetTimeElapsed());
	m_pPlayer2->Update(m_Timer.GetTimeElapsed());
	m_pPlayer3->Update(m_Timer.GetTimeElapsed());



	for (auto bul : _bullets) 
		bul->update(m_Timer.GetTimeElapsed());


}

//-----------------------------------------------------------------------------
// Name : DrawObjects () (Private)
// Desc : Draws the game objects
//-----------------------------------------------------------------------------
void CGameApp::DrawObjects()
{


	//_Buffer->reset();
	m_pBBuffer->reset();
	//m_pBBuffer2->reset();
	//m_imgBackground.Paint(_Buffer->getDC(), 0, 0);

	m_imgBackground.Paint(m_pBBuffer->getDC(), 0, 0);
	//m_imgBackground.Paint(m_pBBuffer->getDC(), 0, 0);

	m_pPlayer->Draw();
	m_pPlayer2->Draw();
	m_pPlayer3->Draw();


	for (auto bul : _bullets) {
		bul->draw();
	}




	m_pBBuffer->present();
	//m_pBBuffer2->present();
	//_Buffer->present();
}

void CGameApp::SpawnBullet(const Vec2 position, const Vec2 velocity, const bool team)
{
	_bullets.push_back(new Sprite("data/projectile.bmp", RGB(0xff, 0x00, 0xff)));
	//_bullets.back()->setBackBuffer(_Buffer);
	_bullets.back()->setBackBuffer(m_pBBuffer);
	_bullets.back()->mPosition = position;
	_bullets.back()->mVelocity = velocity;
	_bullets.back()->team = team;

	if (velocity.y < 0) {
		_bullets.back()->mPosition.y -= 75;
	}
	else {
		_bullets.back()->mPosition.y += 75;
	}
}

bool CGameApp::bulletUnitCollision(const Sprite& bullet, CPlayer& unit)
{
	if (unit.isDead())
		return false;

	if (bullet.mPosition.x >= unit.Position().x - (unit.getSize().x / 2))
		if (bullet.mPosition.x <= unit.Position().x + (unit.getSize().x / 2))
			if (bullet.mPosition.y >= unit.Position().y - (unit.getSize().y / 2))
				if (bullet.mPosition.y <= unit.Position().y + (unit.getSize().y / 2))
					return true;

	return false;
}

bool CPlayer::isDead()
{
	return _isDead;
}

Vec2 CPlayer::getSize()
{
	return Vec2(m_pSprite->width(), m_pSprite->height());
}

bool CGameApp::detectCollision(const Sprite *bullet)
{
	if (bulletUnitCollision(*bullet, *m_pPlayer) && bullet->team) {
		m_pPlayer->Explode();
		return true;
	}
	
	if (bulletUnitCollision(*bullet, *m_pPlayer3) && bullet->team) {
		m_pPlayer3->Explode();
		return true;
	}


}
/***************************************************************************
 *  Original Diku Mud copyright (C) 1990, 1991 by Sebastian Hammer,        *
 *  Michael Seifert, Hans Henrik St{rfeldt, Tom Madsen, and Katja Nyboe.   *
 *                                                                         *
 *  Merc Diku Mud improvments copyright (C) 1992, 1993 by Michael          *
 *  Chastain, Michael Quan, and Mitchell Tse.                              *
 *                                                                         *
 *  In order to use any part of this Merc Diku Mud, you must comply with   *
 *  both the original Diku license in 'license.doc' as well the Merc       *
 *  license in 'license.txt'.  In particular, you may not remove either of *
 *  these copyright notices.                                               *
 *                                                                         *
 *  Dystopia Mud improvements copyright (C) 2000, 2001 by Brian Graversen  *
 *                                                                         *
 *  Much time and thought has gone into this software and you are          *
 *  benefitting.  We hope that you share your changes too.  What goes      *
 *  around, comes around.                                                  *
 ***************************************************************************/

/*
 * This file contains all of the OS-dependent stuff:
 *   startup, signals, BSD sockets for tcp/ip, i/o, timing.
 *
 * The data flow for input is:
 *    Game_loop ---> Read_from_descriptor ---> Read
 *    Game_loop ---> Read_from_buffer
 *
 * The data flow for output is:
 *    Game_loop ---> Process_Output ---> Write_to_descriptor -> Write
 *
 * The OS-dependent functions are Read_from_descriptor and Write_to_descriptor.
 * -- Furey  26 Jan 1993
 */

#include <sys/types.h>
#include <sys/time.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>   // we need these to call wait()

#include <execinfo.h>   // internal backtraces.

#include "merc.h"
#include "olc.h"


/*
 * Malloc debugging stuff.
 */
#if defined(sun)
#undef MALLOC_DEBUG
#endif

#if defined(MALLOC_DEBUG)
#include <malloc.h>
extern	int	malloc_debug	args( ( int  ) );
extern	int	malloc_verify	args( ( void ) );
#endif



/*
 * Signal handling.
 * Apollo has a problem with __attribute(atomic) in signal.h,
 *   I dance around it.
 */
#if defined(apollo)
#define __attribute(x)
#endif

#if defined(unix) || defined(WIN32)
#include <signal.h>
#if !defined(WIN32)
#include <unistd.h>
#include <sys/resource.h>	/* for RLIMIT_NOFILE */
#endif
#endif

#if defined(apollo)
#undef __attribute
#endif

/*
 * Socket and TCP/IP stuff.
 */
#if	defined(macintosh) || defined(MSDOS)
const	char	echo_off_str	[] = { '\0' };
const	char	echo_on_str	[] = { '\0' };
const	char 	go_ahead_str	[] = { '\0' };
const   char    compress_will   [] = { '\0' };
const   char    compress_do     [] = { '\0' };
const   char    compress_dont   [] = { '\0' };
const   char    compress_start  [] = { '\0' };
#endif

#if	defined(unix)
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/telnet.h>
const	char	echo_off_str	[] = { IAC, WILL, TELOPT_ECHO, '\0' };
const	char	echo_on_str	[] = { IAC, WONT, TELOPT_ECHO, '\0' };
const	char 	go_ahead_str	[] = { IAC, GA, '\0' };
const   char    compress_will   [] = { IAC, WILL, TELOPT_COMPRESS, '\0' };
const   char    compress_do     [] = { IAC, DO, TELOPT_COMPRESS, '\0' };
const   char    compress_dont   [] = { IAC, DONT, TELOPT_COMPRESS, '\0' };


void show_string args((DESCRIPTOR_DATA *d, char *input ));

#endif

#if	defined( WIN32 )
const   char echo_off_str	[] = { '\0' };
const   char echo_on_str	[] = { '\0' };
const   char go_ahead_str	[] = { '\0' };
void    gettimeofday    args( ( struct timeval *tp, void *tzp ) );
void show_string args((DESCRIPTOR_DATA *d, char *input ));
#endif

/*
 * OS-dependent declarations.
 */
#if	defined(_AIX)
#include <sys/select.h>
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(apollo)
#include <unistd.h>
void	bzero		args( ( char *b, int length ) );
#endif

#if	defined(__hpux)
int	accept		args( ( int s, void *addr, int *addrlen ) );
int	bind		args( ( int s, const void *addr, int addrlen ) );
void	bzero		args( ( char *b, int length ) );
int	getpeername	args( ( int s, void *addr, int *addrlen ) );
int	getsockname	args( ( int s, void *name, int *addrlen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	setsockopt	args( ( int s, int level, int optname,
 				const void *optval, int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
#endif

#if	defined(interactive)
#include <net/errno.h>
#include <sys/fcntl.h>
#endif

#if	defined(linux)
/* mercpoint: commented out to enable compiling
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
*/
int	close		args( ( int fd ) );
//int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
//int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
//int	listen		args( ( int s, int backlog ) );
/* kavirpoint: put this in for nin site.
int	read		args( ( int fd, char *buf, int nbyte ) );
*/
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	socket		args( ( int domain, int type, int protocol ) );
/* kavirpoint: put this in for nin site.
int	write		args( ( int fd, char *buf, int nbyte ) );
*/
#endif

#if	defined(macintosh)
#include <console.h>
#include <fcntl.h>
#include <unix.h>
struct	timeval
{
	time_t	tv_sec;
	time_t	tv_usec;
};
#if	!defined(isascii)
#define	isascii(c)		( (c) < 0200 )
#endif
static	long			theKeys	[4];

int	gettimeofday		args( ( struct timeval *tp, void *tzp ) );
#endif

#if	defined(MIPS_OS)
extern	int		errno;
#endif

#if	defined(MSDOS)
int	gettimeofday	args( ( struct timeval *tp, void *tzp ) );
int	kbhit		args( ( void ) );
#endif

#if	defined(NeXT)
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if	defined(sequent)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
int	close		args( ( int fd ) );
int	fcntl		args( ( int fd, int cmd, int arg ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
#if	!defined(htons)
u_short	htons		args( ( u_short hostshort ) );
#endif
int	listen		args( ( int s, int backlog ) );
#if	!defined(ntohl)
u_long	ntohl		args( ( u_long hostlong ) );
#endif
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, caddr_t optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

/*
 * This includes Solaris SYSV as well.
 */
#if defined(sun)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
#if defined(SYSV)
int	setsockopt	args( ( int s, int level, int optname, 
                            const char *optval, int optlen ) );
#else 
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
#endif
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif

#if defined(ultrix)
int	accept		args( ( int s, struct sockaddr *addr, int *addrlen ) );
int	bind		args( ( int s, struct sockaddr *name, int namelen ) );
void	bzero		args( ( char *b, int length ) );
int	close		args( ( int fd ) );
int	getpeername	args( ( int s, struct sockaddr *name, int *namelen ) );
int	getsockname	args( ( int s, struct sockaddr *name, int *namelen ) );
int	gettimeofday	args( ( struct timeval *tp, struct timezone *tzp ) );
int	listen		args( ( int s, int backlog ) );
int	read		args( ( int fd, char *buf, int nbyte ) );
int	select		args( ( int width, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, struct timeval *timeout ) );
int	setsockopt	args( ( int s, int level, int optname, void *optval,
			    int optlen ) );
int	socket		args( ( int domain, int type, int protocol ) );
int	write		args( ( int fd, char *buf, int nbyte ) );
#endif


/*
 * Global variables.
 */
DESCRIPTOR_DATA *   descriptor_free;	/* Free list for descriptors	*/
DESCRIPTOR_DATA *   descriptor_list;	/* All open descriptors		*/
DESCRIPTOR_DATA *   d_next;		/* Next descriptor in loop	*/
FILE *		    fpReserve;		/* Reserved file handle		*/
bool		    god;		/* All new chars are gods!	*/
bool		    merc_down;		/* Shutdown			*/
bool		    wizlock;		/* Game is wizlocked		*/
char		    str_boot_time[MAX_INPUT_LENGTH];
char		    crypt_pwd[MAX_INPUT_LENGTH];
time_t		    current_time;	/* Time of this pulse		*/
int		    arena;

/* Colour scale char list - Calamar */

char *scale[SCALE_COLS] = {
	L_RED,
	L_BLUE,
	L_GREEN,
	YELLOW
};

/*
 * OS-dependent local functions.
 */
#if defined(macintosh) || defined(MSDOS)
void	game_loop_mac_msdos	args( ( void ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
#endif

#if defined(unix) || defined( WIN32 )
void	game_loop_unix		args( ( int control ) );
int	init_socket		args( ( int port ) );
void	new_descriptor		args( ( int control ) );
bool	read_from_descriptor	args( ( DESCRIPTOR_DATA *d ) );
#endif




/*
 * Other local functions (OS-independent).
 */
bool	check_reconnect		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_kickoff		args( ( DESCRIPTOR_DATA *d, char *name,
				    bool fConn ) );
bool	check_playing		args( ( DESCRIPTOR_DATA *d, char *name ) );
int	main			args( ( int argc, char **argv ) );
void	nanny			args( ( DESCRIPTOR_DATA *d, char *argument ) );
bool	process_output		args( ( DESCRIPTOR_DATA *d, bool fPrompt ) );
void	read_from_buffer	args( ( DESCRIPTOR_DATA *d ) );
void	stop_idling		args( ( CHAR_DATA *ch ) );
void	bust_a_prompt		args( ( DESCRIPTOR_DATA *d ) );

void  lookup_address       args((DUMMY_ARG *dummyarg));  // Only threaded calls, please.
bool  check_banned         args((DESCRIPTOR_DATA *dnew ));  // Ban check
bool  check_newbiebanned   args((DESCRIPTOR_DATA *dnew ));  // Ban check

int proc_pid;
int port, control;

int main( int argc, char **argv )
{
    struct timeval now_time;
    bool fCopyOver = FALSE;
    /*
     * Memory debugging if needed.
     */
#if defined(MALLOC_DEBUG)
    malloc_debug( 2 );
#endif

#ifdef RLIMIT_NOFILE
#ifndef min
# define min(a,b)     (((a) < (b)) ? (a) : (b))
#endif
        { 
        struct  rlimit rlp;
        (void)getrlimit(RLIMIT_NOFILE, &rlp);
        rlp.rlim_cur=min(rlp.rlim_max,FD_SETSIZE);
        (void)setrlimit(RLIMIT_NOFILE, &rlp);
	}
#endif

    /*
     * Crash recovery by Mandrax
     */
    signal(SIGSEGV, crashrecov);
/*    signal(SIGSEGV, iBacktrace); */
    signal(SIGTERM, panic_shutdown);

    proc_pid = getpid();

    /*
     * Init time and encryption.
     */
    gettimeofday( &now_time, NULL );
    current_time = (time_t) now_time.tv_sec;
    strcpy( str_boot_time, ctime( &current_time ) );
    sprintf( crypt_pwd, "Don't bother." );

    /*
     * Macintosh console initialization.
     */
#if defined(macintosh)
    console_options.nrows = 31;
    cshow( stdout );
    csetmode( C_RAW, stdin );
    cecho2file( "log file", 1, stderr );
#endif

    /*
     * Reserve one channel for our use.
     */
    if ( ( fpReserve = fopen( NULL_FILE, "r" ) ) == NULL )
    {
	perror( NULL_FILE );
	exit( 1 );
    }


    /*
     * Get the port number.
     */
    port = 8888;
    if ( argc > 1 )
    {
	if ( !is_number( argv[1] ) )
	{
	    fprintf( stderr, "Usage: %s [port #]\n", argv[0] );
	    exit( 1 );
	}
	else if ( ( port = atoi( argv[1] ) ) <= 1024 )
	{
	    fprintf( stderr, "Port number must be above 1024.\n" );
	    exit( 1 );
	}
    }

 	if (argv[2] && argv[2][0])
 	{
 		fCopyOver = TRUE;
 		control = atoi(argv[3]);
 	}
 		
 	else
 		fCopyOver = FALSE;
    /*
     * Run the game.
     */
#if defined(macintosh) || defined(MSDOS)
    boot_db( );
    log_string( "Age of Heroes is ready to rock." );
    game_loop_mac_msdos( );
#endif

#if defined(unix) || defined(WIN32)
  	if (!fCopyOver) /* We have already the port if copyover'ed */
 		control = init_socket (port);
 	boot_db (fCopyOver);

    sprintf( log_buf, "Age of Heroes is ready to rock on port %d.", port );
    log_string( log_buf );
    game_loop_unix( control );
#if !defined( WIN32 )
    close( control );
#else
    closesocket( control );
    WSACleanup();
#endif
#endif


    /*
     * That's all, folks.
     */
    log_string( "Normal termination of game." );

    exit( 0 );
    return 0;
}

int init_socket(int port)
{
  static struct sockaddr_in sa_zero;
  struct sockaddr_in sa;
  int x = 1;
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {
    perror("Init_socket: socket");
    exit( 1 );
  }

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &x, sizeof(int)) < 0)
  {
    perror("Init_socket: SO_REUSEADDR");
    close(fd);
    exit(1);
  }

  sa		    = sa_zero;
  sa.sin_family     = AF_INET;
  sa.sin_port	    = htons(port);

  if (bind(fd, (struct sockaddr *) &sa, sizeof(sa) ) < 0)
  {
    perror("Init_socket: bind");
    close(fd);
    exit(1);
  }

  if (listen( fd, 3 ) < 0)
  {
    perror("Init_socket: listen");
    close(fd);
    exit(1);
  }

  return fd;
}


#if defined(macintosh) || defined(MSDOS)
void game_loop_mac_msdos( void )
{
    struct timeval last_time;
    struct timeval now_time;
    static DESCRIPTOR_DATA dcon;

    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /*
     * New_descriptor analogue.
     */
    dcon.descriptor	= 0;
    dcon.connected	= CON_GET_NAME;
    dcon.host		= str_dup( "localhost" );
    dcon.outsize	= 2000;
    dcon.outbuf		= alloc_mem( dcon.outsize );
    dcon.next		= descriptor_list;
    descriptor_list	= &dcon;

    /*
     * Send the greeting.
     */
    {
	extern char * help_greeting;
	if ( help_greeting[0] == '.' )
	    write_to_buffer( &dcon, help_greeting+1, 0 );
	else
	    write_to_buffer( &dcon, help_greeting  , 0 );
    }

    /* Main loop */
    while ( !merc_down )
    {
	DESCRIPTOR_DATA *d;

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

                /* OLC */
                if ( d->showstr_point )
                    show_string( d, d->incomm );
                else
                if ( d->pString )
                    string_add( d->character, d->incomm );
                else
		switch( d->connected )
		{
	     	        default:
 			nanny( d, d->incomm );
			break;
		   case CON_PLAYING:
                        if ( !run_olc_editor( d ) )
                            interpret( d->character, d->incomm );
			break;
		   case CON_EDITING:
			edit_buffer( d->character, d->incomm );
			break;
                   case CON_PFILE:
                        jope_interp( d->character, d->incomm );
                        break;
		  }

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

           if ( ( d->fcommand || d->outtop > 0 || d->out_compress )
            &&   FD_ISSET(d->descriptor, &out_set) 
		if ( !process_output( d, TRUE ) )
            {
                bool ok = TRUE;

                if ( d->fcommand || d->outtop > 0 )
                    ok = process_output( d, TRUE );

                if (ok && d->out_compress)
                    ok = processCompressed(d);

               if (!ok)
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Busy wait (blargh).
	 */
	now_time = last_time;
	for ( ; ; )
	{
	    int delta;

#if defined(MSDOS)
	    if ( kbhit( ) )
#endif
	    {
		if ( dcon.character != NULL )
		    dcon.character->timer = 0;
		if ( !read_from_descriptor( &dcon ) )
		{
		    if ( dcon.character != NULL )
			save_char_obj( d->character );
		    dcon.outtop	= 0;
		    close_socket( &dcon );
		}
#if defined(MSDOS)
		break;
#endif
	    }

	    gettimeofday( &now_time, NULL );
	    delta = ( now_time.tv_sec  - last_time.tv_sec  ) * 1000 * 1000
		  + ( now_time.tv_usec - last_time.tv_usec );
	    if ( delta >= 1000000 / PULSE_PER_SECOND )
		break;
	}
	last_time    = now_time;
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



#if defined(unix) || defined(WIN32)

void excessive_cpu(int blx)
{
    CHAR_DATA *vch;
    CHAR_DATA *vch_next;

    for ( vch = char_list; vch != NULL; vch = vch_next )
    {
	vch_next = vch->next;

	if ( !IS_NPC(vch) )
	{
	    send_to_char("Mud frozen: Autosave and quit.  The mud will reboot in 2 seconds.\n\r",vch);
	    interpret( vch, "quit" );
	}
    }
    exit(1);
}

void game_loop_unix( int control )
{
    static struct timeval null_time;
    struct timeval last_time;

#if !defined(WIN32)
    signal( SIGPIPE, SIG_IGN );
#endif
    gettimeofday( &last_time, NULL );
    current_time = (time_t) last_time.tv_sec;

    /* Main loop */
    while ( !merc_down )
    {
	fd_set in_set;
	fd_set out_set;
	fd_set exc_set;
	DESCRIPTOR_DATA *d;
	int maxdesc;

#if defined(MALLOC_DEBUG)
	if ( malloc_verify( ) != 1 )
	    abort( );
#endif

	/*
	 * Poll all active descriptors.
	 */
	FD_ZERO( &in_set  );
	FD_ZERO( &out_set );
	FD_ZERO( &exc_set );
	FD_SET( control, &in_set );
	maxdesc	= control;
/* kavirpoint
	maxdesc	= control * 2;
*/
	for ( d = descriptor_list; d != NULL; d = d->next )
	{
	    maxdesc = UMAX( maxdesc, d->descriptor );
	    FD_SET( d->descriptor, &in_set  );
	    FD_SET( d->descriptor, &out_set );
	    FD_SET( d->descriptor, &exc_set );
	}

	if ( select( maxdesc+1, &in_set, &out_set, &exc_set, &null_time ) < 0 )
	{
	    perror( "Game_loop: select: poll" );
	    exit( 1 );
	}

	/*
	 * New connection?
	 */
	if ( FD_ISSET( control, &in_set ) )
	    new_descriptor( control );

	/*
	 * Kick out the freaky folks.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;   
	    if ( FD_ISSET( d->descriptor, &exc_set ) )
	    {
		FD_CLR( d->descriptor, &in_set  );
		FD_CLR( d->descriptor, &out_set );
		if ( d->character )
		    save_char_obj( d->character );
		d->outtop	= 0;
		close_socket( d );
	    }
	}

	/*
	 * Process input.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next	= d->next;
	    d->fcommand	= FALSE;

	    if ( FD_ISSET( d->descriptor, &in_set ) )
	    {
		if ( d->character != NULL )
		    d->character->timer = 0;
		if ( !read_from_descriptor( d ) )
		{
		    FD_CLR( d->descriptor, &out_set );
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		    continue;
		}
	    }

	    if ( d->character != NULL && d->character->wait > 0 )
	    {
		--d->character->wait;
		continue;
	    }

	    read_from_buffer( d );
	    if ( d->incomm[0] != '\0' )
	    {
		d->fcommand	= TRUE;
		stop_idling( d->character );

                /* OLC */
                if ( d->showstr_point )
                    show_string( d, d->incomm );
                else
                if ( d->pString )
                    string_add( d->character, d->incomm );
                else 
		switch( d->connected )
			  {
			   default:
 				nanny( d, d->incomm );
				break;
			   case CON_PLAYING:
                        if ( !run_olc_editor( d ) )
				interpret( d->character, d->incomm );
				break;
			   case CON_EDITING:
				edit_buffer( d->character, d->incomm );
				break;
                           case CON_PFILE:
                                jope_interp( d->character, d->incomm );
                                break;
			  }

		d->incomm[0]	= '\0';
	    }
	}



	/*
	 * Autonomous game motion.
	 */
	update_handler( );



	/*
	 * Output.
	 */
	for ( d = descriptor_list; d != NULL; d = d_next )
	{
	    d_next = d->next;

	    if ( ( d->fcommand || d->outtop > 0 )
	    &&   FD_ISSET(d->descriptor, &out_set) )
	    {
		if ( !process_output( d, TRUE ) )
		{
		    if ( d->character != NULL )
			save_char_obj( d->character );
		    d->outtop	= 0;
		    close_socket( d );
		}
	    }
	}



	/*
	 * Synchronize to a clock.
	 * Sleep( last_time + 1/PULSE_PER_SECOND - now ).
	 * Careful here of signed versus unsigned arithmetic.
	 */
#if !defined(WIN32)
	{
	    struct timeval now_time;
	    long secDelta;
	    long usecDelta;

	    gettimeofday( &now_time, NULL );
	    usecDelta	= ((int) last_time.tv_usec) - ((int) now_time.tv_usec)
			+ 1000000 / PULSE_PER_SECOND;
	    secDelta	= ((int) last_time.tv_sec ) - ((int) now_time.tv_sec );
	    while ( usecDelta < 0 )
	    {
		usecDelta += 1000000;
		secDelta  -= 1;
	    }

	    while ( usecDelta >= 1000000 )
	    {
		usecDelta -= 1000000;
		secDelta  += 1;
	    }

	    if ( secDelta > 0 || ( secDelta == 0 && usecDelta > 0 ) )
	    {
		struct timeval stall_time;

		stall_time.tv_usec = usecDelta;
		stall_time.tv_sec  = secDelta;
		if ( select( 0, NULL, NULL, NULL, &stall_time ) < 0 )
		{
		    perror( "Game_loop: select: stall" );
		    exit( 1 );
		}
	    }
	}
#else
	{
	    int times_up;
	    int nappy_time;
	    struct _timeb start_time;
	    struct _timeb end_time;
	    _ftime( &start_time );
	    times_up = 0;

	    while( times_up == 0 )
	    {
		_ftime( &end_time );
		if ( ( nappy_time =
		      (int) ( 1000 *
			     (double) ( ( end_time.time - start_time.time ) +
				       ( (double) ( end_time.millitm -
						   start_time.millitm ) /
					1000.0 ) ) ) ) >=
		    (double)( 1000 / PULSE_PER_SECOND ) )
		  times_up = 1;
		else
		{
		    Sleep( (int) ( (double) ( 1000 / PULSE_PER_SECOND ) -
				  (double) nappy_time ) );
		    times_up = 1;
		}
	    }
	}
#endif

	gettimeofday( &last_time, NULL );
	current_time = (time_t) last_time.tv_sec;
    }

    return;
}
#endif



 void init_descriptor (DESCRIPTOR_DATA *dnew, int desc)
 {
 	static DESCRIPTOR_DATA d_zero;
 
 	*dnew = d_zero;
 	dnew->descriptor = desc;
 	dnew->character = NULL;
 	dnew->connected = CON_GET_NAME;
        dnew->lookup_status = STATUS_LOOKUP; 
	dnew->showstr_head = str_dup ("");
 	dnew->showstr_point = 0;
 	dnew->pEdit = NULL;			/* OLC */
 	dnew->pString = NULL;		/* OLC */
 	dnew->editor = 0;			/* OLC */
 	dnew->outsize = 2000;
 	dnew->outbuf = alloc_mem (dnew->outsize);
 	
 }

#if defined(unix) || defined(WIN32)
void new_descriptor( int control )
{
    char buf[MAX_STRING_LENGTH];
    DESCRIPTOR_DATA *dnew;
    struct sockaddr_in sock;
    int desc;
    socklen_t size;
    size = (socklen_t)sizeof(sock);
    pthread_attr_t attr;
    pthread_t thread_lookup;
    DUMMY_ARG *dummyarg;
    bool DOS_ATTACK = FALSE;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);


    /* New Dummy Argument */
    if (dummy_free == NULL)
    {
      dummyarg = alloc_perm(sizeof(*dummyarg));
    }
    else
    {
      dummyarg = dummy_free;
      dummy_free = dummy_free->next;
    }
    dummyarg->status = 1;
    dummyarg->next = dummy_list;
    dummy_list = dummyarg;


    size = sizeof(sock);
    getsockname( control, (struct sockaddr *) &sock, &size );
    if ( ( desc = accept( control, (struct sockaddr *) &sock, &size) ) < 0 )
    {
        perror( "New_descriptor: accept" );
        return;
    }
  
#if !defined(FNDELAY)
#define FNDELAY O_NDELAY
#endif
 
#if !defined(WIN32)
    if ( fcntl( desc, F_SETFL, FNDELAY ) == -1 )
    {
        perror( "New_descriptor: fcntl: FNDELAY" );
        return;
    }
#endif

    /*
     * Cons a new descriptor.
     */
    if ( descriptor_free == NULL )
    {
        dnew            = alloc_perm( sizeof(*dnew) );
    }
    else
    {
        dnew            = descriptor_free;
        descriptor_free = descriptor_free->next;
    }

    init_descriptor (dnew, desc);
  
    size = sizeof(sock);
    if ( getpeername( desc, (struct sockaddr *) &sock, &size ) < 0 )
    { 
        perror( "New_descriptor: getpeername" );
        dnew->host = str_dup( "(unknown)" );
    }
    else
    {
        /*
         * Would be nice to use inet_ntoa here but it takes a struct arg,
         * which ain't very compatible between gcc and system libraries.
         */
        int addr;
        addr = ntohl( sock.sin_addr.s_addr );
        sprintf( buf, "%d.%d.%d.%d",
            ( addr >> 24 ) & 0xFF, ( addr >> 16 ) & 0xFF,
            ( addr >>  8 ) & 0xFF, ( addr       ) & 0xFF
            );
     
        dnew->host = str_dup(buf);      // set the temporary ip as the host.

        /* we do a hostlookup on everyone but localhost */
        if (str_cmp(dnew->host, "127.0.0.1"))
        {
          /* Set up the dummyarg for use in lookup_address */
          dummyarg->buf      = str_dup((char *) &sock.sin_addr);
          dummyarg->d        = dnew;
    
          if (thread_count < 50) /* should be more than plenty */
          {
            /* just use the ip, then make the thread do the lookup */
            pthread_create( &thread_lookup, &attr, (void*)&lookup_address, (void*) dummyarg);
          }
          else DOS_ATTACK = TRUE;
        }
        else dnew->lookup_status++;
    }
          
    /*
     * Init descriptor data.
     */
    dnew->next                  = descriptor_list;
    descriptor_list             = dnew;
        
           
    if (DOS_ATTACK)
    {
      write_to_buffer(dnew, "Sorry, currently under attack, try again later.\n\r", 0 );
      close_socket(dnew);
      return;
    }
            
    /* mccp: tell the client we support compression */
    write_to_buffer( dnew, compress_will, 0 );

    /* send gretting */
    {
        extern char * help_greeting;
        if ( help_greeting[0] == '.' )
            write_to_buffer( dnew, help_greeting+1, 0 );
        else
            write_to_buffer( dnew, help_greeting  , 0 );
    }
          
    return;
}
#endif

void lookup_address(DUMMY_ARG *darg)   
{  
  struct hostent *from = 0;
  struct hostent ent;
  char buf[16384]; // enough ??
  int err;

  thread_count++;

  gethostbyaddr_r( darg->buf, sizeof(darg->buf), AF_INET, &ent, buf, 16384, &from, &err);

  if (from && from->h_name)
  {
    free_string(darg->d->host);
    darg->d->host = str_dup(from->h_name);
  }
     
  /*
   * Brilliant system there Mr. Jobo
   */
  darg->d->lookup_status++;

  free_string(darg->buf);
  darg->status = 0;

  thread_count--;

  pthread_exit(0);
}


bool check_banned( DESCRIPTOR_DATA *dnew )
{
  BAN_DATA *pban;

  for (pban = ban_list; pban != NULL; pban = pban->next)
    if (!str_suffix(pban->name, dnew->host)) return TRUE;
  return FALSE;
}

bool check_newbiebanned( DESCRIPTOR_DATA *dnew )
{
  BAN_DATA *pban;

  for (pban = newbieban_list; pban != NULL; pban = pban->next)
    if (!str_suffix(pban->name, dnew->host)) return TRUE;
  return FALSE;
}

void close_socket( DESCRIPTOR_DATA *dclose )
{  
  CHAR_DATA *ch;
   
  if (dclose->lookup_status > STATUS_DONE) return;
  dclose->lookup_status += 2;
   
  if (dclose->outtop > 0) process_output( dclose, FALSE );
  if ( dclose->snoop_by != NULL )
    write_to_buffer( dclose->snoop_by, "Your victim has left the game.\n\r", 0 );
     
  if (dclose->connected == CON_PFILE) jope_done(dclose->character, "");

  /*
   * Clear snoops
   */
  {  
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
      if (d->snoop_by == dclose) d->snoop_by = NULL;
  }  

  /* 
   * Loose link or free char
   */
  if ((ch = dclose->character) != NULL)
  {
    sprintf( log_buf, "Closing link to %s.", ch->name );
    log_string( log_buf );
    /* If ch is writing note or playing, just lose link otherwise clear char */
    if ((dclose->connected == CON_PLAYING) || ((dclose->connected >= CON_NOTE_TO) &&
       (dclose->connected <= CON_NOTE_FINISH)))
    {
      if (IS_SET(ch->extra, EXTRA_AFK)) REMOVE_BIT(ch->extra,EXTRA_AFK);
      do_call(ch,"all");
      act( "$n has lost $s link.", ch, NULL, NULL, TO_ROOM );
      ch->desc = NULL;
    }
    else
    {
      free_char( dclose->character );
    }
  } 

  if (d_next == dclose) d_next = d_next->next;
  dclose->connected = CON_NOT_PLAYING;
  return;
}
     
/* For a better kickoff message :) KaVir */
void close_socket2( DESCRIPTOR_DATA *dclose, bool kickoff )
{
  CHAR_DATA *ch;
     
  if (dclose->lookup_status > STATUS_DONE) return;
  dclose->lookup_status += 2;
   
  if (dclose->outtop > 0) process_output( dclose, FALSE );
  if ( dclose->snoop_by != NULL )
    write_to_buffer( dclose->snoop_by, "Your victim has left the game.\n\r", 0 );

  if (dclose->connected == CON_PFILE) jope_done(dclose->character, "");

 /*
  * Clear snoops
  */
  {
    DESCRIPTOR_DATA *d;

    for ( d = descriptor_list; d != NULL; d = d->next )
      if (d->snoop_by == dclose) d->snoop_by = NULL;
  }

  if ((ch = dclose->character) != NULL)
  {
    if (dclose->connected == CON_PLAYING || dclose->connected == CON_EDITING)
    {
      if (kickoff)
        act( "$n doubles over in agony and $s eyes roll up into $s head.", ch, NULL, NULL, TO_ROOM);
      save_char_obj( ch );
      ch->desc = NULL;
    }
    else
    {
      free_char( dclose->character );
    }
  }

  if (d_next == dclose) d_next = d_next->next;
  dclose->connected = CON_NOT_PLAYING;
  return;
}

bool read_from_descriptor( DESCRIPTOR_DATA *d )
{
    int iStart;

    /* Hold horses if pending command already. */
    if ( d->incomm[0] != '\0' )
	return TRUE;

    /* one dirty patch to avoid spams of EOF's */
    if (d->connected == CON_NOT_PLAYING)
        return TRUE;

    /* Check for overflow. */
    iStart = strlen(d->inbuf);
    if ( iStart >= sizeof(d->inbuf) - 10 )
    {
	if (d != NULL && d->character != NULL) {
	    sprintf( log_buf, "%s input overflow!", d->character->lasthost );
            log_string( log_buf );
        }
	else if (d->lookup_status != STATUS_LOOKUP) {
	    sprintf( log_buf, "%s input overflow!", d->host );
	    log_string( log_buf );
        }
	write_to_descriptor( d,
	    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
	return FALSE;
    }

    /* Snarf input. */
#if defined(macintosh)
    for ( ; ; )
    {
	int c;
	c = getc( stdin );
	if ( c == '\0' || c == EOF )
	    break;
	putc( c, stdout );
	if ( c == '\r' )
	    putc( '\n', stdout );
	d->inbuf[iStart++] = c;
	if ( iStart > sizeof(d->inbuf) - 10 )
	    break;
    }
#endif

#if defined(MSDOS) || defined(unix) || defined(WIN32)
    for ( ; ; )
    {
	int nRead;

#if !defined(WIN32)
	nRead = read( d->descriptor, d->inbuf + iStart,
	    sizeof(d->inbuf) - 10 - iStart );
#else
	nRead = recv( d->descriptor, d->inbuf + iStart,
		     sizeof( d->inbuf ) - 10 - iStart, 0 );
#endif
	if ( nRead > 0 )
	{
	    iStart += nRead;
	    if ( d->inbuf[iStart-1] == '\n' || d->inbuf[iStart-1] == '\r' )
		break;
	}
	else if ( nRead == 0 )
	{
	    log_string( "EOF encountered on read." );
	    return FALSE;
	}
#if !defined(WIN32)
	else if ( errno == EWOULDBLOCK )
	    break;
#endif
#if defined( WIN32 )
        else if ( WSAGetLastError() == WSAEWOULDBLOCK || errno == EAGAIN )
	    break;
#endif
	else
	{
	    perror( "Read_from_descriptor" );
	    return FALSE;
	}
    }
#endif

    d->inbuf[iStart] = '\0';
    return TRUE;
}



/*
 * Transfer one line from input buffer to input line.
 */
void read_from_buffer( DESCRIPTOR_DATA *d )
{
    int i, j, k;

    /*
     * Hold horses if pending command already.
     */
    if ( d->incomm[0] != '\0' )
	return;

    /*
     * Look for at least one new line.
     */
    for ( i = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( d->inbuf[i] == '\0' )
	    return;
    }

    /*
     * Canonical input processing.
     */
    for ( i = 0, k = 0; d->inbuf[i] != '\n' && d->inbuf[i] != '\r'; i++ )
    {
	if ( k >= MAX_INPUT_LENGTH - 2 )
	{
	    write_to_descriptor( d, "Line too long.\n\r", 0 );

	    /* skip the rest of the line */
	    for ( ; d->inbuf[i] != '\0'; i++ )
	    {
		if ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
		    break;
	    }
	    d->inbuf[i]   = '\n';
	    d->inbuf[i+1] = '\0';
	    break;
	}

	if ( d->inbuf[i] == '\b' && k > 0 )
	    --k;
	else if ( isascii(d->inbuf[i]) && isprint(d->inbuf[i]) )
	    d->incomm[k++] = d->inbuf[i];
        /* handling vt100 feedback */
        else if (d->inbuf[i] == '\e')
        {
          char xbuf[30];  // plenty
          char *xptr;
          int xk = 1;

          xbuf[0] = '\0';
          xptr = xbuf;

          /* first we scan to see if this is really a reply to our request */
          while (d->inbuf[i+xk] != 'R' && xk < 10) xk++;

          /* I'm sure most screens use at most 999x999 chars */
          if (xk < 10)
          {
            i += 2; xk = 0;
            while (d->inbuf[i] != ';' && d->inbuf[i] != '\0' && ++xk < 30)
            {
              *xptr = d->inbuf[i];
              xptr++; i++;
            }
            *xptr = '\0';
            init_vt100(d, xbuf);
          }
        }
        else if (d->inbuf[i] == (signed char)IAC) {
            if (!memcmp(&d->inbuf[i], compress_do, strlen(compress_do))) {
                i += strlen(compress_do) - 1;
                compressStart(d);
            }
            else if (!memcmp(&d->inbuf[i], compress_dont, strlen(compress_dont))) {
                i += strlen(compress_dont) - 1;
                compressEnd(d);
            }
        }

    }

    /*
     * Finish off the line.
     */
    if ( k == 0 )
	d->incomm[k++] = ' ';
    d->incomm[k] = '\0';

    /*
     * Deal with bozos with #repeat 1000 ...
     */
    if ( k > 1 || d->incomm[0] == '!' )
    {
    	if ( d->incomm[0] != '!' && strcmp( d->incomm, d->inlast ) )
	{
	    d->repeat = 0;
	}
	else
	{
	    if ( ++d->repeat >= 40 )
	    {
              if (d != NULL && d->character != NULL) {
                sprintf( log_buf, "%s input overflow!", d->character->lasthost );
                log_string( log_buf );
              }   
              else if (d->lookup_status != STATUS_LOOKUP) {
                sprintf( log_buf, "%s input overflow!", d->host );
                log_string( log_buf );
              }
		write_to_descriptor( d,
		    "\n\r*** PUT A LID ON IT!!! ***\n\r", 0 );
		sprintf( d->incomm, "quit" );
	    }
	}
    }

    /*
     * Do '!' substitution.
     */
    if ( d->incomm[0] == '!' )
	strcpy( d->incomm, d->inlast );
    else
	strcpy( d->inlast, d->incomm );

    /*
     * Shift the input buffer.
     */
    while ( d->inbuf[i] == '\n' || d->inbuf[i] == '\r' )
	i++;
    for ( j = 0; ( d->inbuf[j] = d->inbuf[i+j] ) != '\0'; j++ )
	;
    return;
}



#define COPYOVER_FILE "copyover.data"
#define EXE_FILE      "../src/aoh"

void panic_shutdown(int iSignal)
{
  DESCRIPTOR_DATA *d;
  CHAR_DATA *gch;
  FILE *fp;
  char buf[200];

  sprintf(buf, "\n\rServer reboot, saving and doing a panic shutdown.");

  /* We really don't want to lose this info */
  save_coreinfo();

  /* save all connected players */
  for (d = descriptor_list; d; d = d->next)
  {
    if ((gch = d->character) == NULL) continue;
    call_all(gch);
    save_char_obj(gch);
    write_to_descriptor_2(d->descriptor, buf, 0);
  }

  /* create a bug message so it's easy to know what happended */
  bug("Server Rebooted, handling it safely.", 0);

  /* shutdown file - probably not needed, but hey */
  if ((fp = fopen(SHUTDOWN_FILE, "w")) != NULL)
  {
    fprintf(fp, "Server Reboot\n");
    fclose (fp);
  }

  /* and we die */
  exit(0);
}

/*
 * To do internal backtracs at crashes
 */
void iBacktrace(int iSignal)
{
  void *aTrace[10];
  char **tString;
  char buf[400]; //plenty
  int size, i, j, k;

  size = backtrace(aTrace, 10);
  printf("%s\n", (char *) aTrace[1]);
  tString = backtrace_symbols(aTrace, size);
  for (i = 0; i < size; i++)
  {
    bool found = FALSE;

    printf("%s\n", tString[i]);
    for (j = 0; tString[i][j] != '\0' && !found; j++)
    {
      if (tString[i][j] == '[')
      {
        j++;
        found = TRUE;
        for (k = j ; tString[i][j] != '\0' && tString[i][j] != ']'; j++)
        {
          buf[j-k] = tString[i][j];
        }
        buf[j-k] = '\0';
      }
    }
    printf("%s translates to : %s\n", buf, (char *) strtol(buf, NULL, 16));
  }

  exit(1);
}

/*
 * Crash recovery system written by Mandrax, based on copyover
 * Includes call to signal() in main.
 * Mostly copied from copyover.
 */
void crashrecov (int iSignal)
{
  CHAR_DATA *gch;
  OBJ_DATA *obj;
  FILE *fp;
  DESCRIPTOR_DATA *d, *d_next;
  char buf [200], buf2[100];
  int pid;
  int iFork;
  int i;
  FILE *fReturn;
  FILE *fCrash;

  /*
   * An attempt to avoid spam crashes
   */
  if((fReturn = fopen(CRASH_TEMP_FILE, "r")) != NULL)
  {
        signal(SIGSEGV, SIG_IGN);
        raise(SIGSEGV);
        return;
  }
  fCrash = fopen(CRASH_TEMP_FILE, "w");
  fprintf(fCrash, "0");
  fclose(fCrash);

  save_polls();
  dump_last_command();

  /*
   * This will cause a core dump, even though the signal was handled
   */
  iFork = fork();
  wait(NULL);
  if((pid = getpid()) != proc_pid)
  {
        signal(SIGSEGV, SIG_IGN);
        raise(SIGSEGV);
        return;
  }
 
  fp = fopen (COPYOVER_FILE, "w");
  if (!fp)
   {
      perror ("crashrecov:fopen");
      return;
   }


  for (gch = char_list; gch != NULL; gch = gch->next)
  {
    if (IS_NPC(gch)) continue;

    /* Fix any possibly head/object forms */
    if (IS_HEAD(gch,LOST_HEAD) || IS_SET(gch->extra, EXTRA_OSWITCH))
    {
      REMOVE_BIT(gch->loc_hp[0], LOST_HEAD);
      REMOVE_BIT(gch->affected_by, AFF_POLYMORPH);
      REMOVE_BIT(gch->extra, EXTRA_OSWITCH);
      gch->morph = str_dup("");
      gch->pcdata->chobj = NULL;
      gch->pcdata->obj_vnum = 0;
      char_from_room(gch);
      char_to_room(gch,get_room_index(ROOM_VNUM_ALTAR));
    }
    
    /* Take care of possibly artifacts */
    for (i = 0; i < MAX_WEAR; i++)
    {
      if ((obj = get_eq_char(gch, i)) != NULL)
      {
        if (IS_SET(obj->quest, QUEST_ARTIFACT))
          unequip_char(gch, obj);
      }
    }

    gch->hit = gch->max_hit;
    gch->position = POS_STANDING;
    do_call(gch, "all");
    save_char_obj(gch); 
  }

   sprintf (buf, "\n\r <*>          AOH has crashed           <*>\n\r\n\r <*>   Attempting to restore last savefile   <*>\n\r");

     /* For each playing descriptor, save its state */
    for (d = descriptor_list; d ; d = d_next)
     {
       CHAR_DATA * och = d->character;

       compressEnd2(d);
       d_next = d->next; /* We delete from the list , so need to save this */
       if (!d->character || d->connected != 0) /* drop those logging on */
         {
            write_to_descriptor_2(d->descriptor, "\n\rSorry, the mud has crashed.\n\rPlease log on another char and report this, your char MAY be bugged.\n\r", 0);
            close_socket2 (d, FALSE); /* throw'em out */
          }
        else
          {
             fprintf (fp, "%d %s %s\n", d->descriptor, och->name, d->host);
             write_to_descriptor_2(d->descriptor, buf, 0);
           }
      }

        fprintf (fp, "-1\n");
        fclose (fp);

        /* Close reserve and other always-open files and release other resources */
        fclose (fpReserve);
//        fclose (fpLOG);

        /* recycle descriptors */
        recycle_descriptors();
 
        /* exec - descriptors are inherited */
 
        sprintf (buf, "%d", port);
        sprintf (buf2, "%d", control);
 

        execl (EXE_FILE, "dystopia", buf, "crashrecov", buf2, (char *) NULL);
 
        /* Failed - sucessful exec will not return */
 
        perror ("crashrecov: execl");
        log_string ("Crash recovery FAILED!\n\r");

        /* The least we can do is exit gracefully :P */
        exit(1);
}

void retell_mccp( DESCRIPTOR_DATA *d)
{
  write_to_buffer( d, compress_will, 0 );
  return;
}


bool process_output( DESCRIPTOR_DATA *d, bool fPrompt )
{
    extern bool merc_down;

    /*
     * Bust a prompt.
     */
    if ( fPrompt && !merc_down && (d->connected == CON_PLAYING || d->connected == CON_PFILE))
    {
        char buf[200]; //more than plenty
	CHAR_DATA *ch;
	CHAR_DATA *victim;

        ch = d->character;

        if (IS_SET(ch->act, PLR_BRIEF5))
        {
          if (ch->amount_attacks_dealt > 0)
          {
            sprintf(buf, "#GYou've dealt #C%d #Gattacks causing #C%d #Gdamage. #0(#y%d #Gaverage#0)#n\n\r",
              ch->amount_attacks_dealt, ch->amount_damage_dealt,
              ch->amount_damage_dealt/ch->amount_attacks_dealt);
            send_to_char(buf, ch);
          }
          if (ch->amount_attacks_recieved > 0)
          {
            sprintf(buf, "#GYou've been struck by #C%d #Gattacks, causing #C%d #Gdamage. #0(#y%d #Gaverage#0)#n\n\r",
              ch->amount_attacks_recieved, ch->amount_damage_recieved,
              ch->amount_damage_recieved/ch->amount_attacks_recieved);
            send_to_char(buf, ch);
          }
          ch->amount_damage_dealt = 0;   
          ch->amount_damage_recieved = 0;
          ch->amount_attacks_dealt = 0;   
          ch->amount_attacks_recieved = 0;
        }

        if (!IS_NPC(ch) && IS_SET(ch->pcdata->tempflag, TEMP_VT100))
        {  
          sprintf(buf, "%s\e[%d;1H", VT_SAVECURSOR, ch->pcdata->vt100_size);
          write_to_buffer(d, buf, 0);
        }

        write_to_buffer( d, "\n\r", 2 );

	if (IS_SET(ch->act, PLR_PROMPT) && IS_EXTRA(ch, EXTRA_PROMPT))
	    bust_a_prompt( d );
	else if ( IS_SET(ch->act, PLR_PROMPT))
	{
	    char buf[MAX_STRING_LENGTH];
	    char cond[MAX_INPUT_LENGTH];
	    char hit_str[MAX_INPUT_LENGTH];
	    char mana_str[MAX_INPUT_LENGTH];
	    char move_str[MAX_INPUT_LENGTH];
	    char exp_str[MAX_INPUT_LENGTH];

	    ch = d->character;
	    if (IS_HEAD(ch,LOST_HEAD) || IS_EXTRA(ch,EXTRA_OSWITCH))
	    {
		sprintf(exp_str, "%2.0f", ch->exp);
		//COL_SCALE(exp_str, ch, ch->exp, 1000);
	        sprintf( buf, "#n<[#C%sX#n] [#C?#lH #C?#lM #C?#lV#n]> ",exp_str );
	    }
	    else if (ch->position == POS_FIGHTING)
	    {
	        victim = ch->fighting;
		if (victim == NULL)
		{
		    sprintf(cond, "NA");
		    ADD_COLOUR(ch, cond, L_RED);
		
		}
		else if ((victim->hit*100/victim->max_hit) < 25)
		{
		    sprintf(cond, "Awful");
		    ADD_COLOUR(ch, cond, L_RED);
		}
		else if ((victim->hit*100/victim->max_hit) < 50)
		{
		    sprintf(cond, "Poor");
		    ADD_COLOUR(ch, cond, L_BLUE);
		}
		else if ((victim->hit*100/victim->max_hit) < 75)
		{
		    sprintf(cond, "Fair");
		    ADD_COLOUR(ch, cond, L_GREEN);
		}
		else if ((victim->hit*100/victim->max_hit) < 100)
		{
		    sprintf(cond, "Good");
		    ADD_COLOUR(ch, cond, YELLOW);
		}
		else if ((victim->hit*100/victim->max_hit) >= 100)
		{
		    sprintf(cond, "Perfect");
		    ADD_COLOUR(ch, cond, L_CYAN);
		}
		sprintf(hit_str, "%d", ch->hit);
		COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
		sprintf(mana_str, "%d", ch->mana);
		COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
		sprintf(move_str, "%d", ch->move);
		COL_SCALE(move_str, ch, ch->move, ch->max_move);
		sprintf( buf, "#n<[%s] [%sH %sM %sV] [#C%d#n ft]> ",
                  cond, hit_str, mana_str, move_str, ch->fight_timer);
	    }
	    else
	    {
		sprintf(hit_str, "%d", ch->hit);
		COL_SCALE(hit_str, ch, ch->hit, ch->max_hit);
		sprintf(mana_str, "%d", ch->mana);
		COL_SCALE(mana_str, ch, ch->mana, ch->max_mana);
		sprintf(move_str, "%d", ch->move);
		COL_SCALE(move_str, ch, ch->move, ch->max_move);
		sprintf(exp_str, "%2.0f", ch->exp);
		//COL_SCALE(exp_str, ch, ch->exp, 1000);
	        sprintf( buf, "#n<[%s] [%sH %sM %sV] [#C%d#n ft]> ",
                  exp_str, hit_str, mana_str, move_str, ch->fight_timer );
	    }
	    write_to_buffer( d, buf, 0 );
	}

        if (!IS_NPC(ch) && IS_SET(ch->pcdata->tempflag, TEMP_VT100))
        {
          write_to_buffer(d, VT_RESTORECURSOR, 0);
        }

	if ( IS_SET(ch->act, PLR_TELNET_GA) )
	    write_to_buffer( d, go_ahead_str, 0 );
    }

    /*
     * Short-circuit if nothing to write.
     */
    if ( d->outtop == 0 )
	return TRUE;

    /*
     * Snoop-o-rama.
     */
    if ( d->snoop_by != NULL )
    {
	write_to_buffer( d->snoop_by, "% ", 2 );
	write_to_buffer( d->snoop_by, d->outbuf, d->outtop );
    }

    /*
     * OS-dependent output.
     */
    if ( !write_to_descriptor( d, d->outbuf, d->outtop ) )
    {
	d->outtop = 0;
	return FALSE;
    }
    else
    {
	d->outtop = 0;
	return TRUE;
    }
}
/*
 * Append onto an output buffer.
 */
#define  COLOR_TAG              '#'
#define  BG_COLOR_TAG           '^'
#define  ANSI_STRING            "^0123456789bBcCdDgGiIlLnNopPrRsuwWyYxz.-+(?fFvV@e/)"
#define  SPECIAL_STRING         "-+(?fFvV@e/)"
#define  REPLACE_STRING         "-#+^"


/*
bool is_special (char code)
{
   int i = 0, j = 0;

   while (SPECIAL_STRING[i] != '\0')
   {
      if (SPECIAL_STRING[i] == code)
	 return TRUE;
      i--;
   }

   while (REPLACE_STRING[j] != '\0')
   {
      if (REPLACE_STRING[j] == code)
	 return TRUE;
      j--;

   }
   return FALSE;
}
*/
bool is_color (char code)
{
   int i = 0, j = 0;


   while (ANSI_STRING[i] != '\0')
   {
      if (ANSI_STRING[i] == code)
	 return TRUE;
      i++;
   }

   while (REPLACE_STRING[j] != '\0')
   {
      if (REPLACE_STRING[j] == code)
	 return TRUE;
      j++;
   }


   return FALSE;
}
char *format_buffer (DESCRIPTOR_DATA * d, char *txt)
{
   char show[32000];
   char *p = '\0';
   int cnt = 0;

   if (strlen (txt) < 30000)
   {
      bug ("Format_buffer: Invalid size %d", strlen (txt));
      return txt;
   }

   p = show;

   for (;;)
   {
      if (cnt++ < 30000)
	 *p++ = *txt++;
      else
      {
	 if (*txt != ' ')
	    p--, txt--;
	 else
	    break;
      }
   }

   p = '\0';
   cnt = strlen (show);

   write_to_descriptor (d, show, cnt);
   return txt;
}
void write_to_buffer (DESCRIPTOR_DATA * d, const char *txt, int length)
{
   char tb[32000], ccode[20];
   int i, j, cnt;
   bool ansi;

   if (length <= 0)
      length = strlen (txt);

   if (d->character == NULL)
      ansi = TRUE;
   else if (d->connected != CON_PLAYING)
      ansi = TRUE;
   else
      ansi = (IS_SET (d->character->act, PLR_ANSI)) ? TRUE : FALSE;

   if (d->outtop == 0 && !d->fcommand)
   {
      d->outbuf[0] = '\n';
      d->outbuf[1] = '\r';
      d->outtop = 2;
   }

   j = 0;
   tb[0] = '\0';

   for (i = 0; i < length; i++)
   {
      ccode[0] = '\0';
      if (txt[i] == COLOR_TAG && txt[i+1] != BG_COLOR_TAG && is_color(txt[i+1]))
      {

	 switch (txt[++i])
	 {

	 default:
	    sprintf (ccode, "#%c", txt[i]);
	    break;
	 case '-':
	    sprintf (ccode, "%c", '~');
	    break;
	 case '+':
	    sprintf (ccode, "%c", '%');
	    break;
	 case '(':
	    sprintf (ccode, "%c", '�');
	    break;
	 case '?':
	    sprintf (ccode, "%c", '�');
	    break;
	 case 'f':
	    sprintf (ccode, "%c", '�');
	    break;
	 case 'F':
	    sprintf (ccode, "%c", '�');
	    break;
	 case 'v':
	    sprintf (ccode, "%c", '�');
	    break;
	 case 'V':
	    sprintf (ccode, "%c", '�');
	    break;
	 case '@':
	    sprintf (ccode, "%c", '�');
	    break;
	 case 'e':
	    sprintf (ccode, "%c", '�');
	    break;
	 case '/':
	    sprintf (ccode, "%c", '�');
	    break;
	 case ')':
	    sprintf (ccode, "%c", '�');
	    break;

	 case '#':
	    sprintf (ccode, "%c", '#');
	    break;
	 case 'I':
	 case 'i':
	    strcpy (ccode, INVERTED);
	    break;
	 case 'u':
	    strcpy (ccode, UNDERLINE);
	    break;
	 case 'z':
	    strcpy (ccode, FLASHING);
	    break;
	 case 'N':
	 case 'x':
	 case 'n':
	    strcpy (ccode, NO_COLOUR);
	    break;
	 case '0':
	    if (!ansi)
	       continue;
	    strcpy (ccode, GREY);
	    break;
	 case '1':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_RED);
	    break;
	 case '2':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_GREEN);
	    break;
	 case '3':
	    if (!ansi)
	       continue;
	    strcpy (ccode, YELLOW);
	    break;
	 case '4':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_BLUE);
	    break;
	 case '5':
	    if (!ansi)
	       continue;
	    strcpy (ccode, PINK);
	    break;
	 case '6':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_CYAN);
	    break;
	 case '7':
	    if (!ansi)
	       continue;
	    strcpy (ccode, WHITE);
	    break;
	 case '8':
	    if (!ansi)
	       continue;
	    strcpy (ccode, D_WHITE);
	    break;
	 case '9':
	    if (!ansi)
	       continue;
	    strcpy (ccode, WHITE);
	    break;
	 case 'b':
	    if (!ansi)
	       continue;
	    strcpy (ccode, D_BLUE);
	    break;
	 case 'B':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_BLUE);
	    break;
	 case 'c':
	    if (!ansi)
	       continue;
	    strcpy (ccode, D_CYAN);
	    break;
	 case 'C':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_CYAN);
	    break;
	 case 'D':
	    if (!ansi)
	       continue;
	    strcpy (ccode, GREY);
	    break;
	 case 'g':
	    if (!ansi)
	       continue;
	    strcpy (ccode, D_GREEN);
	    break;
	 case 'G':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_GREEN);
	    break;
	 case 'l':
	    if (!ansi)
	       continue;
	    strcpy (ccode, D_BLUE);
	    break;
	 case 'L':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_BLUE);
	    break;
	 case 'o':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BROWN);
	    break;
	 case 'p':
	    if (!ansi)
	       continue;
	    strcpy (ccode, MAGENTA);
	    break;
	 case 'P':
	    if (!ansi)
	       continue;
	    strcpy (ccode, PINK);
	    break;
	 case 'r':
	    if (!ansi)
	       continue;
	    strcpy (ccode, D_RED);
	    break;
	 case 'R':
	    if (!ansi)
	       continue;
	    strcpy (ccode, L_RED);
	    break;
	 case 's':
	    if (!ansi)
	       continue;

	    switch (number_range (1, 15))
	    {
	    case 1:
	       strcpy (ccode, L_RED);
	       break;
	    case 2:
	       strcpy (ccode, L_CYAN);
	       break;
	    case 3:
	       strcpy (ccode, L_BLUE);
	       break;
	    case 4:
	       strcpy (ccode, L_GREEN);
	       break;
	    case 5:
	       strcpy (ccode, PINK);
	       break;
	    case 6:
	       strcpy (ccode, YELLOW);
	       break;
	    case 7:
	       strcpy (ccode, WHITE);
	       break;
	    case 8:
	       strcpy (ccode, D_RED);
	       break;
	    case 9:
	       strcpy (ccode, D_CYAN);
	       break;
	    case 10:
	       strcpy (ccode, D_BLUE);
	       break;
	    case 11:
	       strcpy (ccode, D_GREEN);
	       break;
	    case 12:
	       strcpy (ccode, MAGENTA);
	       break;
	    case 13:
	       strcpy (ccode, BROWN);
	       break;
	    case 14:
	       strcpy (ccode, D_WHITE);
	       break;
	    case 15:
	       strcpy (ccode, GREY);
	       break;
	    default:
	       strcpy (ccode, GREY);
	       break;
	    }
	    break;
	 case 'w':
	    if (!ansi)
	       continue;
	    strcpy (ccode, D_WHITE);
	    break;
	 case 'W':
	    if (!ansi)
	       continue;
	    strcpy (ccode, WHITE);
	    break;
	 case 'Y':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BROWN);
	    break;
	 case 'y':
	    if (!ansi)
	       continue;
	    strcpy (ccode, YELLOW);
	    break;
	 }
	 if (ccode[0] != '\0')
	 {
            int clength = strlen(ccode);
	    ccode[strlen (ccode) + 1] = '\0';
	    for (cnt = 0; cnt < clength; cnt++)
	       tb[j++] = ccode[cnt];
	 }
      }
      else if (txt[i] == BG_COLOR_TAG  && txt[i+1] != COLOR_TAG)
      {
	 switch (txt[++i])
	 {
	 default:
	    sprintf (ccode, "^%c", txt[i]);
	    break;
	 case '^':
	    sprintf (ccode, "%c", '^');
	    break;
	 case 'N':
	 case 'n':
	    strcpy (ccode, BG_CLEAR);
	    break;
	 case '1':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BG_RED);
	    break;
	 case '2':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BG_GREEN);
	    break;
	 case '3':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BG_BROWN);
	    break;
	 case '4':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BG_BLUE);
	    break;
	 case '5':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BG_PURPLE);
	    break;
	 case '6':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BG_CYAN);
	    break;
	 case '7':
	    if (!ansi)
	       continue;
	    strcpy (ccode, BG_WHITE);
	    break;
	 case 's':
	    if (!ansi)
	       continue;

	    switch (number_range (1, 7))
	    {
	    case 1:
	       strcpy (ccode, BG_RED);
	       break;
	    case 2:
	       strcpy (ccode, BG_CYAN);
	       break;
	    case 3:
	       strcpy (ccode, BG_BLUE);
	       break;
	    case 4:
	       strcpy (ccode, BG_GREEN);
	       break;
	    case 5:
	       strcpy (ccode, BG_PURPLE);
	       break;
	    case 6:
	       strcpy (ccode, BG_BROWN);
	       break;
	    case 7:
	       strcpy (ccode, BG_WHITE);
	       break;
	    default:
	       strcpy (ccode, BG_WHITE);
	       break;
	    }
	 }
	 if (ccode[0] != '\0')
	 {
            int clength = strlen(ccode);
	    for (cnt = 0; cnt < clength; cnt++)
	       tb[j++] = ccode[cnt];
	 }
      }
      else
	 tb[j++] = txt[i];
   }
   if (ansi)
   {

      tb[j++] = 27;

      tb[j++] = '[';

      tb[j++] = '0';

      tb[j++] = 'm';
   }

   tb[j] = '\0';
   length = strlen (tb);

   if ((d->outtop + length) >= 32000)
   {
      write_to_descriptor (d, d->outbuf, d->outtop);
      d->connected = CON_PLAYING;
      d->outtop = 0;
   }

   while (length >= 30000)
   {
      char tmp[MAX_STRING_LENGTH];

      sprintf (tmp, "%s", format_buffer (d, tb));
      tb[0] = '\0';
      strcat (tb, tmp);
      length = strlen (tb);
   }
   while (d->outtop + length >= d->outsize)
   {
      char *outbuf;

      outbuf = alloc_mem (2 * d->outsize);
      strncpy (outbuf, d->outbuf, d->outtop);
      free_mem (d->outbuf, d->outsize);
      d->outbuf = outbuf;
      d->outsize *= 2;
   }

   /* Copy. */
   strncpy (d->outbuf + d->outtop, tb, length);
   d->outtop += length;
   return;
}

void owrite_to_buffer( DESCRIPTOR_DATA *d, const char *txt, int length)
{
  static char output[32000];
  char *ptr;
  int i = 0;

  // clear the output buffer, and set the pointer
  output[0] = '\0';
  ptr = output;

  if (length <= 0)
    length = strlen(txt);

  if (length >= MAX_STRING_LENGTH )
  {
    bug( "Write_to_buffer: Way too big. Closing.", 0 );
    return;
  }

  // initial linebreak
  if ( d->outtop == 0 && !d->fcommand )
  {
    d->outbuf[0]    = '\n';
    d->outbuf[1]    = '\r';
    d->outtop       = 2;
  }

  while (*txt != '\0' && i++ < length)
  {
    switch(*txt)
    {
      default:
        *ptr++ = *txt++;
        break;
      case '#':
        i++;
        switch (*++txt)
        {
          default:
            txt++;
            break;
          case '#':
            *ptr++ = '#';
            txt++;
            break;
          case '-':
            *ptr++ = '~';
            txt++;
            break;
          case '+':
            *ptr++ = '%';
            txt++;
            break;
          case 'i':  // bold
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '7';
            *ptr++ = 'm';   txt++;          break;
          case 'u':  // underline
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '4';
            *ptr++ = 'm';   txt++;          break;
          case 'n':  // stock color
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = 'm';   txt++;          break;
          case '7':  // White
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '7';   *ptr++ = 'm';
            txt++;          break;
          case '8':  // Black
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '0';   *ptr++ = 'm';
            txt++;          break;
          case '9':  // White 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '7';   *ptr++ = 'm';
            txt++;          break;
          case '0':  // Black 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '0';   *ptr++ = 'm';
            txt++;          break;
          case 'r':  // Red
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '1';   *ptr++ = 'm';
            txt++;          break;
          case 'R':  // red 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '1';   *ptr++ = 'm';
            txt++;          break;
          case 'g': // Green
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '2';   *ptr++ = 'm';
            txt++;          break;
          case 'G': // Green 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '2';   *ptr++ = 'm';
            txt++;          break;
          case 'o':  // Yellow (Brown)
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '3';   *ptr++ = 'm';
            txt++;          break;
          case 'y':  // Yellow 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '3';   *ptr++ = 'm';
            txt++;          break;
          case 'l':  // Blue
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '4';   *ptr++ = 'm';
            txt++;          break;
          case 'L':  // Blue 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '4';   *ptr++ = 'm';
            txt++;          break;
          case 'p':  // Purple
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '5';   *ptr++ = 'm';
            txt++;          break;
          case 'P':  // Purple 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '5';   *ptr++ = 'm';
            txt++;          break;
          case 'c':  // Cyan
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '6';   *ptr++ = 'm';
            txt++;          break;
          case 'C':  // Cyan 2
            *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
            *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
            *ptr++ = '3';   *ptr++ = '6';   *ptr++ = 'm';
            txt++;          break;
          case 's':
            switch(number_range(1,15))                                
            {
              case 1:  // White
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '7';   *ptr++ = 'm';
                break;
              case 2:  // Black
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '0';   *ptr++ = 'm';
                break;
              case 3:  // Black 2
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '0';   *ptr++ = 'm';
                break;
              case 4:  // Red
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '1';   *ptr++ = 'm';
                break;
              case 5:  // red 2
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '1';   *ptr++ = 'm';
                break;
              case 6: // Green
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '2';   *ptr++ = 'm';
                break;
              case 7: // Green 2
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '2';   *ptr++ = 'm';
                break;
              case 8:  // Yellow (Brown)
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '3';   *ptr++ = 'm';
                break;
              case 9:  // Yellow 2
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '3';   *ptr++ = 'm';
                break;
              case 10:  // Blue
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '4';   *ptr++ = 'm';
                break;
              case 11:  // Blue 2
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '4';   *ptr++ = 'm';
                break;
              case 12:  // Purple
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '5';   *ptr++ = 'm';
                break;
              case 13:  // Purple 2
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '5';   *ptr++ = 'm';
                break;
              case 14:  // Cyan
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '0';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '6';   *ptr++ = 'm';
              case 15:  // Cyan 2
                *ptr++ = '\e';  *ptr++ = '[';   *ptr++ = '0';
                *ptr++ = ';';   *ptr++ = '1';   *ptr++ = ';';
                *ptr++ = '3';   *ptr++ = '6';   *ptr++ = 'm';
                break;
            }
            txt++;
            break;
        }
        break;
    }
  }

  // and terminate it with the standard color
  *ptr++ = '\e';  *ptr++ = '[';
  *ptr++ = '0';   *ptr++ = 'm';

  // terminate
  *ptr = '\0';

  length = strlen(output);

  // Expand the buffer as needed.
  while (d->outtop + length >= d->outsize)
  {
    char *obuf;

    // There is a limit to our patience
    if (d->outsize >= 32000)
    {
      bug("Buffer overflow. Closing.",0);
      close_socket(d);
      return;
    }
    obuf = alloc_mem(2 * d->outsize);
    strncpy(obuf, d->outbuf, d->outtop);
    free_mem(d->outbuf, d->outsize);
    d->outbuf = obuf;
    d->outsize *= 2;
  }

  // Copy.
  strncpy(d->outbuf + d->outtop, output, length);
  d->outtop += length;
  return;
}


/*
 * Lowest level output function.
 * Write a block of text to the file descriptor.
 * If this gives errors on very long blocks (like 'ofind all'),
 * try lowering the max block size.
 */
bool write_to_descriptor_2( int desc, char *txt, int length )
{
    int iStart;
    int nWrite;
    int nBlock;

#if defined(macintosh) || defined(MSDOS)
    if ( desc == 0 )
	desc = 1;
#endif

    if ( length <= 0 )
	length = strlen(txt);

    increase_total_output(length);

    for ( iStart = 0; iStart < length; iStart += nWrite )
    {
	nBlock = UMIN( length - iStart, 4096 );
#if !defined(WIN32)
	if ( ( nWrite = write( desc, txt + iStart, nBlock ) ) < 0 )
#else
	if ( ( nWrite = send( desc, txt + iStart, nBlock , 0) ) < 0 )
#endif
	    { perror( "Write_to_descriptor" ); return
FALSE; }
    } 

    return TRUE;
}

/* mccp: write_to_descriptor wrapper */
bool write_to_descriptor(DESCRIPTOR_DATA *d, char *txt, int length)
{
    if (d->out_compress)
        return writeCompressed(d, txt, length);
    else
        return write_to_descriptor_2(d->descriptor, txt, length);
}



/*
 * Deal with sockets that haven't logged in yet.
 */
void nanny( DESCRIPTOR_DATA *d, char *argument )
{
    char buf[MAX_STRING_LENGTH];
    char kav[MAX_STRING_LENGTH];
    CHAR_DATA *ch;
    OBJ_DATA *obj;
    char *pwdnew;
    char *p;
    char *strtime;
    int char_age = 17;
    bool fOld; 

	if (d->connected != CON_NOTE_TEXT)
    while ( isspace(*argument) )
	argument++;

    ch = d->character;


    switch ( d->connected )
    {

    default:
	bug( "Nanny: bad d->connected %d.", d->connected );
	close_socket( d );
	return;

    case CON_NOT_PLAYING: break;

    case CON_GET_NAME:
	if ( argument[0] == '\0' )
	{
	    close_socket( d );
	    return;
	}

        if (d->lookup_status != STATUS_DONE)
        {
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
          write_to_buffer( d, "#Y[#R*#Y]#0Making a dns lookup, please have patience. (this could take 10-15 seconds)#Y[#R*#Y]#n\n\r #0What be thy name?#n\n\r", 0 );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
          return;
        }

        if (check_banned(d))
        {
            write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
            write_to_buffer( d, " #0Your site has been banned from this mud#n\n\r", 0 );
            write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
            close_socket( d );
            return;    
        }

	argument[0] = UPPER(argument[0]);
	if (!check_parse_name(argument))
	{
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
	    write_to_buffer( d, " Illegal name, try another.\n\r Enter thy name brave traveler: ", 0 );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
	    return;
	}

	sprintf(kav,"#7%s #rtrying to connect to #7%s#n.", argument, MUDNAME);
	log_string( kav );
	fOld = load_char_short( d, argument );
	ch   = d->character;

        char_age = years_old(ch);
	if ( IS_SET(ch->act, PLR_DENY))
        {
            if (ch->pcdata->denied > current_time)
            {
	    sprintf( log_buf, "#YDenying access to #7%s@%s.#n", argument, ch->lasthost );
	    log_string( log_buf );
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
	    write_to_buffer( d, " #0You are denied access.#n\n\r", 0 );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
              close_socket( d );
	      return;
            }
	}
	else if ( IS_EXTRA(ch, EXTRA_BORN) && char_age < 15 )
	{
	    char agebuf [MAX_INPUT_LENGTH];
	    if (char_age == 14)
	{
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
		sprintf(agebuf, "You cannot play for another year.\n\r" );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
	}

	    else
	{
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
          sprintf(agebuf, "You cannot play for another %d years.\n\r", 
		(15 - years_old(ch)) );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
	}
	    write_to_buffer(d, agebuf, 0);
	    close_socket( d );
	    return;
	}

	if ( check_reconnect( d, argument, FALSE ) )
	{
	    fOld = TRUE;
	}
	else
	{
	    /* Check max number of players - KaVir */

	    DESCRIPTOR_DATA *dcheck;
	    DESCRIPTOR_DATA *dcheck_next;
	    int countdesc = 0;
	    int max_players = 150;

	    for (dcheck = descriptor_list; dcheck != NULL; dcheck = dcheck_next)
	    {
		dcheck_next = dcheck->next;
		countdesc++;
	    }

	    if ( countdesc > max_players && !IS_IMMORTAL(ch) )
	    {
		write_to_buffer( d, "Too many players connected, please try again in a couple of minutes.\n\r", 0 );
		close_socket( d );
		return;
	    }

	    if ( wizlock && !IS_IMMORTAL(ch) )
	    {
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
		write_to_buffer( d, " #0Currenly wizlocked.#n\n\r", 0 );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);

		close_socket( d );
		return;
	    }
	}

	if ( fOld )
	{
	    /* Old player */
	    write_to_buffer( d, " #Y[#R*#Y]#0Please enter password:#n ", 0 );
	    write_to_buffer( d, echo_off_str, 0 );
	    d->connected = CON_GET_OLD_PASSWORD;
	    return;
	}
	else
	{
	    /* New player */

            if (check_newbiebanned(d))
            {
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
              write_to_buffer( d, "Your site is not allowed to create new chars on this mud\n\r", 0 );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
              close_socket( d );
              return;
            }
            sprintf( buf, "#0Reminder: If you have chosen a name which isn't suitable, you'll get deleted.#n\n\r");
            write_to_buffer( d, buf, 0 );
	    sprintf( buf, " #0You want #7%s #0engraved on your tombstone #R(#YY#7/#YN#R)#n? ", argument );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_CONFIRM_NEW_NAME;
	    return;
	}
	break;

    case CON_GET_OLD_PASSWORD:
      #if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
      #endif


	if ( ch == NULL || (!IS_EXTRA(ch,EXTRA_NEWPASS) &&
	    strcmp( argument, ch->pcdata->pwd ) &&
	    strcmp( crypt ( argument, ch->pcdata->pwd ),ch->pcdata->pwd )))
	{
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
	    write_to_buffer( d, " Wrong password.\n\r", 0 );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
	    close_socket( d );
	    return;
	}

	else if ( ch == NULL || (IS_EXTRA(ch,EXTRA_NEWPASS)  &&
	    strcmp( crypt ( argument, ch->pcdata->pwd ), ch->pcdata->pwd )))
	{
          write_to_buffer( d, "#Y<#r----------#R==========#Y[#r***#Y[#0Notice#Y]#r***#Y]#R==========#r----------#Y>#n\n\r", 0);
	    write_to_buffer( d, " Wrong password.\n\r", 0 );
	    write_to_buffer( d, "#Y<#r----------#R==========#Y[#r**************#Y]#R==========#r----------#Y>#n\n\r", 0);
	    close_socket( d );
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );

	if ( check_reconnect( d, GET_PC_NAME(ch), TRUE ) )
	    return;

	if ( check_playing( d, GET_PC_NAME(ch) ) )
	    return;

	if (ch->level > 1)
	{
	    sprintf(kav,ch->pcdata->switchname);
            free_char(d->character);
            d->character = NULL;
            fOld = load_char_obj( d, kav );
            ch   = d->character;

            if (IS_SET(ch->act, PLR_DENY))
            {
              if (ch->pcdata->denied < current_time)
              {
                REMOVE_BIT(ch->act, PLR_DENY);
                sprintf(log_buf, "Removing deny flag on %s.", ch->name);
                log_string( log_buf);
              }
            }
	}

	if ( !IS_EXTRA(ch,EXTRA_NEWPASS) && strlen(argument) > 1) {
		sprintf(kav,"%s %s",argument,argument);
		do_password(ch,kav);}

	if (ch->lasthost != NULL) free_string(ch->lasthost);
	if (ch->desc != NULL && ch->desc->host != NULL)
	{
        ch->lasthost = str_dup(ch->desc->host);
	}
	else 
	{
	    ch->lasthost = str_dup("(unknown)");
	}
	strtime = ctime( &current_time );
	strtime[strlen(strtime)-1] = '\0';
	free_string(ch->lasttime);
	ch->lasttime = str_dup( strtime );
	sprintf( log_buf, "%s@%s has connected.",ch->name, ch->lasthost );
	log_string( log_buf );

	/* In case we have level 4+ players from another merc mud, or 
	 * players who have somehow got file access and changed their pfiles.
	 */

	if (ch->level < 4 && ch->trust < 4)
	ch->trust = 0;
	if ( ch->level > 3 && ch->trust == 0)
	    ch->level = 3;
	else
	{

	    if ( ch->level > MAX_LEVEL )
		ch->level = MAX_LEVEL;
	    if ( ch->trust > MAX_LEVEL)
		ch->trust = MAX_LEVEL;
	}

	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;

        if (IS_HERO(ch)) do_help( ch, "motd" ); 


	if (ch->mounted == IS_RIDING) do_dismount(ch,"");
        send_to_char("\n\r", ch); // blank line to char

	if ( ch->level == 0 )
	{
            ch->pcdata->awins        = 0; /* arena wins           */
            ch->pcdata->alosses      = 0; /* arena losses         */
		ch->pcdata->board = &boards[0];
	    ch->challenger = NULL;
	    ch->level	= 1;
	    ch->exp	= 0;
	    ch->hit	= ch->max_hit;
	    ch->mana	= ch->max_mana;
	    ch->move	= ch->max_move;
            ch->special = 0;
	    set_switchname(ch, ch->name);
	    char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    do_look( ch, "auto" );
	}
	else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
	{
	    if (ch->in_room != NULL) char_to_room( ch, ch->in_room );
	    else char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    bind_char(ch);
	    break;
	}
	else if ( ch->in_room != NULL )
	{
	    char_to_room( ch, ch->in_room );
	    do_look( ch, "auto" );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	    do_look( ch, "auto" );
	}
        ch->pcdata->time_tick = 0; // resets on a _true_ login
        ch->pcdata->safe_counter = 5; // a few good ticks.

	do_board(ch,"");

        players_logged++;
        if (ch->level > 6) ; /* no info */
        else if (IS_SET(ch->pcdata->jflags, JFLAG_SETLOGIN)) login_message(ch);
	else
	{
  	  if (!ragnarok) sprintf(buf,"#g%s #7enters #RAge of Heroes.#n", ch->name );
          else sprintf(buf,"#g%s #7enters #RAge of Heroes#0Ragnarok#y).#n", ch->name );
          enter_info(buf);
	}
	ch->fight_timer = 0;
	ch->home = 2002;
        update_revision(ch); // changed the pfile structure ?
	act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );

	if ( !IS_NPC(ch) )
       {
       for ( obj = ch->carrying; obj != NULL; obj = obj->next_content )
       {
	int value = 0;
	if( obj->item_type == ITEM_WEAPON )
	{
	    value = obj->value[0];
	    if( value >= 1000 )
	    {
		int temp = value / 1000;
		temp = temp * 1000;
		value -= temp;
	    }
	    if( value == 81 )
	    { do_remove(ch,obj->name);extract_obj(obj);}
	}
    }
  }

	room_text(ch,">ENTER<");

        if (multicheck(ch))
        {
          send_to_char("\n\r#R====#0[#y**#0]#R====  #GWARNING  #R====#0[#y**#0]#R====#n\n\r",ch);
          send_to_char("You have connected from the same\n\r",ch);
          send_to_char("IP as someone who is already on the\n\r",ch);
          send_to_char("mud. Multiplaying is illegal, and\n\r",ch);
          send_to_char("your actions will be monitored.\n\r",ch);
          send_to_char("#R====#0[#y**#0]#R====  #GWARNING  #R====#0[#y**#0]#R====#n\n\r",ch);
        }
	break;


    case CON_CONFIRM_NEW_NAME:
	switch ( *argument )
	{
	case 'y': case 'Y':
	    sprintf( buf, "New character.\n\rGive me a password for %s: %s",
		ch->name, echo_off_str );
	    write_to_buffer( d, buf, 0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    break;

	case 'n': case 'N':
	    write_to_buffer( d, "Ok, what IS it, then? ", 0 );
	    free_char( d->character );
	    d->character = NULL;
	    d->connected = CON_GET_NAME;
	    break;

	default:
	    write_to_buffer( d, "Please type Yes or No? ", 0 );
	    break;
	}
	break;

    case CON_GET_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

	if ( strlen(argument) < 5 )
	{
	    write_to_buffer( d,
		"Password must be at least five characters long.\n\rPassword: ",
		0 );
	    return;
	}

	pwdnew = crypt ( argument, ch->name );

	for ( p = pwdnew; *p != '\0'; p++ )
	{
	    if ( *p == '~' )
	    {
		write_to_buffer( d,
		    "New password not acceptable, try again.\n\rPassword: ",
   0 );
		return;
	    }
	}

	free_string( ch->pcdata->pwd );
	ch->pcdata->pwd	= str_dup( pwdnew );

	write_to_buffer( d, "Please retype password: ", 0 );
	d->connected = CON_CONFIRM_NEW_PASSWORD;
	break;

    case CON_CONFIRM_NEW_PASSWORD:
#if defined(unix)
	write_to_buffer( d, "\n\r", 2 );
#endif

  if ( strcmp( crypt ( argument, ch->pcdata->pwd ), ch->pcdata->pwd ) )
	{
	    write_to_buffer( d, "Passwords don't match.\n\rRetype password: ",
		0 );
	    d->connected = CON_GET_NEW_PASSWORD;
	    return;
	}

	write_to_buffer( d, echo_on_str, 0 );
	write_to_buffer( d, "What is your sex (M/F)? ", 0 );
	d->connected = CON_GET_NEW_SEX;
	break;

    case CON_GET_NEW_SEX:
	switch ( argument[0] )
	{
	case 'm': case 'M': ch->sex = SEX_MALE;    break;
	case 'f': case 'F': ch->sex = SEX_FEMALE;  break;
	default:
	    write_to_buffer( d, "That's not a sex.\n\rWhat IS your sex? ", 0 );
	    return;
	}
	write_to_buffer( d, echo_on_str, 0 );
//	write_to_buffer( d, "Do you wish to use ANSI color (y/n)? ", 0);
//	d->connected = CON_GET_NEW_ANSI;

  write_to_buffer(d,  "\n\r\n\r", 0);
  write_to_buffer(d,  "#0<<-Races--------------------------------------------------------------------------->> \n\r", 0);
  write_to_buffer(d,  "#0BugBear			Giant, Fire		Kender\n\r", 0);
  write_to_buffer(d,  "#0Centaur			Giant, Frost		Kobold, Aquatic\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Aquatic		Giant, Hill		Kobold, Arctic\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Arctic		Gnome, Air		Kobold, Desert\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Deep		Gnome, Aquatic		Kobold, Earth\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Desert		Gnome, Arctic		Kobold, Jungle\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Dream		Gnome, Chaos		Minotaur\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Earth		Gnome, Deep		Ogre\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Gold		Gnome, Desert		OgreMage\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Grey		Gnome, Forest		Orc\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Hill		Gnome, Jungle		Pixie\n\r", 0);
  write_to_buffer(d,  "#0Dwarf, Jungle		Gnome, Rock		Troll\n\r", 0);
  write_to_buffer(d,  "#0Elf, Aquatic		Goblin, Air\n\r", 0);
  write_to_buffer(d,  "#0Elf, Arctic		Goblin, Aquatic\n\r", 0);
  write_to_buffer(d,  "#0Elf, Dark(drow)		Goblin, Arctic\n\r", 0);
  write_to_buffer(d,  "#0Elf, Desert		Goblin, Desert\n\r", 0);
  write_to_buffer(d,  "#0Elf, Fire		Goblin, Forestkith\n\r", 0);
  write_to_buffer(d,  "#0Elf, Ghost		Goblin, Jungle\n\r", 0);
  write_to_buffer(d,  "#0Elf, Gray		Half-Elf\n\r", 0);
  write_to_buffer(d,  "#0Elf, High		Half-Ogre\n\r", 0);
  write_to_buffer(d,  "#0Elf, Jungle		Half-Orc\n\r", 0);
  write_to_buffer(d,  "#0Elf, Wild		Half-Troll\n\r", 0);
  write_to_buffer(d,  "#0Elf, Wood		Hobbit\n\r", 0);
  write_to_buffer(d,  "#0Gnoll			Human\n\r", 0);
  write_to_buffer(d,  "#0<<--------------------------------------------------------------------------------->> \n\r", 0);
  write_to_buffer(d,  "#0Please select your race. #n\n\r", 0);

  d->connected = CON_GET_NEW_RACE;
  break;


  case CON_GET_NEW_RACE:


if (!str_cmp(argument,"human"))
{
	ch->pcRace = RACE_HUMAN;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become Human.\n\r", ch);
}
else if (!str_cmp(argument,"highelf"))
{
	ch->pcRace = RACE_ELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a High Elf.\n\r", ch);
}
/*else if (!str_cmp(argument,"gnome"))
{
	ch->pcRace = RACE_GNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You have Become a Gnome.\n\r", ch);
}
*/
else if (!str_cmp(argument,"gullydwarf"))
{
	ch->pcRace = RACE_GULLYDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a Gully Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"hilldwarf"))
{
	ch->pcRace = RACE_HILLDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a Hill Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"mountaindwarf"))
{
	ch->pcRace = RACE_MTDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You have Become a MT. Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"dragonkind"))
{
	ch->pcRace = RACE_DRAGONKIND;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You have Become Dragonkind.\n\r", ch);
}
else if (!str_cmp(argument,"hobbit"))
{
	ch->pcRace = RACE_HOBBIT;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You have become a Shireling.\n\r", ch);
}
else if (!str_cmp(argument,"kender"))
{
	ch->pcRace = RACE_KENDER;
	send_to_char( "Your Feet feel the calling of Wanderlust.\n\r", ch);
}
else if (!str_cmp(argument,"drow"))
{
	ch->pcRace = RACE_DROW;
	ch->pcdata->pc_eyes = 2;
	ch->pcdata->pc_looks = 5;
	ch->pcdata->pc_hair = number_range(2,3);
	ch->pcdata->pc_build = 1;
	send_to_char( "The Suns becomes Brilliant and painful to your eyes.\n\r", ch);
}
/*
else if (!str_cmp(argument,"giant"))
{
	ch->pcRace = RACE_GIANT;
	send_to_char( "You choose to stay a Giant.\n\r", ch);
}
*/
else if (!str_cmp(argument,"halfelf"))
{
	ch->pcRace = RACE_HALFELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Half Elf.\n\r", ch);
}
else if (!str_cmp(argument,"ogre"))
{
	ch->pcRace = RACE_OGRE;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You are an Ogre.\n\r", ch);
}
else if (!str_cmp(argument,"halfogre"))
{
	ch->pcRace = RACE_HALFOGRE;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You are a Half Ogre.\n\r", ch);
}
else if (!str_cmp(argument,"ogremage"))
{
	ch->pcRace = RACE_OGREMAGE;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You are an Ogre Mage.\n\r", ch);
}
else if (!str_cmp(argument,"minotaur"))
{
	ch->pcRace = RACE_MINOTAUR;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "You grow in stature, Your skin turns red and Bull Horns sprout from your head.\n\r", ch);
}
/*else if (!str_cmp(argument,"goblin"))
{
	ch->pcRace = RACE_GOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
}
*/
else if (!str_cmp(argument,"airgoblin"))
{
	ch->pcRace = RACE_AIRGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
}
else if (!str_cmp(argument,"aquaticgoblin"))
{
	ch->pcRace = RACE_AQUATICGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
}
else if (!str_cmp(argument,"arcticgoblin"))
{
	ch->pcRace = RACE_ARCTICGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
}
else if (!str_cmp(argument,"desertgoblin"))
{
	ch->pcRace = RACE_DESERTGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
}
else if (!str_cmp(argument,"forestkith"))
{
	ch->pcRace = RACE_GOBLINFORESTKITH;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
}
else if (!str_cmp(argument,"junglegoblin"))
{
	ch->pcRace = RACE_JUNGLEGOBLIN;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a stinky, thieving little goblin.\n\r", ch);
}
else if (!str_cmp(argument,"hobgoblin"))
{
	ch->pcRace = RACE_HOBGOBLIN;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Hobgoblin Commander of a Goblin Squad.\n\r", ch);
}
else if (!str_cmp(argument,"duergar"))
{
	ch->pcRace = RACE_DUERGARDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are now a Duergar Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"troll"))
{
	ch->pcRace = RACE_TROLL;
	ch->pcsize = SIZE_LARGE;
	SET_BIT(ch->vuln, VULN_FIRE);
	send_to_char( "You are now a Troll.\n\r", ch);
}
else if (!str_cmp(argument,"orc"))
{
	ch->pcRace = RACE_ORC;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are now an Orc.\n\r", ch);
}
else if (!str_cmp(argument,"halforc"))
{
	ch->pcRace = RACE_HALFORC;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are now a Half-Orc.\n\r", ch);
}
else if (!str_cmp(argument,"centaur"))
{
	ch->pcRace = RACE_CENTAUR;
	ch->pcsize = SIZE_LARGE;
	send_to_char( "you are now a Centaur.\n\r", ch);
}
else if (!str_cmp(argument,"wildelf"))
{
	ch->pcRace = RACE_WILDELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Wild Elf.\n\r", ch);
}
else if (!str_cmp(argument,"aquaticelf"))
{
	ch->pcRace = RACE_AQUATICELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Aquatic Elf.\n\r", ch);
}
else if (!str_cmp(argument,"arcticelf"))
{
	ch->pcRace = RACE_ARCTICELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Arctic Elf.\n\r", ch);
}
else if (!str_cmp(argument,"desertelf"))
{
	ch->pcRace = RACE_DESERTELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Desert Elf.\n\r", ch);
} 
else if (!str_cmp(argument,"fireelf"))
{
	ch->pcRace = RACE_FIREELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Fire Elf.\n\r", ch);
}
else if (!str_cmp(argument,"ghostelf"))
{
	ch->pcRace = RACE_GHOSTELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Ghost Elf.\n\r", ch);
}
else if (!str_cmp(argument,"grayelf"))
{
	ch->pcRace = RACE_GRAYELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Gray Elf.\n\r", ch);
}
else if (!str_cmp(argument,"jungleelf"))
{
	ch->pcRace = RACE_JUNGLEELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Jungle Elf.\n\r", ch);
}
else if (!str_cmp(argument,"woodelf"))
{
	ch->pcRace = RACE_WOODELF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Wood Elf.\n\r", ch);
}
else if (!str_cmp(argument,"aquaticdwarf"))
{
	ch->pcRace = RACE_AQUATICDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Aquatic Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"arcticdwarf"))
{
	ch->pcRace = RACE_ARCTICDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are an Arctic Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"deepdwarf"))
{
	ch->pcRace = RACE_DEEPDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Deep Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"desertdwarf"))
{
	ch->pcRace = RACE_DESERTDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Desert Dwarf.\n\r", ch);
}
else if (!str_cmp(argument, "dreamdwarf"))
{
	ch->pcRace = RACE_DREAMDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Dream Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"earthdwarf"))
{
	ch->pcRace = RACE_EARTHDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Earth Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"golddwarf"))
{
	ch->pcRace = RACE_GOLDDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Gold Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"greydwarf"))
{
	ch->pcRace = RACE_GREYDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Grey Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"jungledwarf"))
{
	ch->pcRace = RACE_JUNGLEDWARF;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Jungle Dwarf.\n\r", ch);
}
else if (!str_cmp(argument,"aquatickobold"))
{
	ch->pcRace = RACE_AQUATICKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Aquatic Kobold.\n\r", ch);
}
else if (!str_cmp(argument,"arctickobold"))
{
	ch->pcRace = RACE_ARCTICKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Arctic Kobold.\n\r", ch);
}
else if (!str_cmp(argument,"desertkobold"))
{
	ch->pcRace = RACE_DESERTKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Desert Kobold.\n\r", ch);
}
else if (!str_cmp(argument,"earthkobold"))
{
	ch->pcRace = RACE_EARTHKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Earth Kobold.\n\r", ch);
}
else if (!str_cmp(argument,"junglekobold"))
{
	ch->pcRace = RACE_JUNGLEKOBOLD;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Jungle Kobold.\n\r", ch);
}
else if (!str_cmp(argument,"hillgiant"))
{
	ch->pcRace = RACE_HILLGIANT;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You are a Hill Giant.\n\r", ch);
}
else if (!str_cmp(argument,"firegiant"))
{
	ch->pcRace = RACE_FIREGIANT;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You are a Fire Giant.\n\r", ch);
}
else if (!str_cmp(argument,"frostgiant"))
{
	ch->pcRace = RACE_FROSTGIANT;
	ch->pcsize = SIZE_GIANT;
	send_to_char( "You are a Frost Giant.\n\r", ch);
}
else if (!str_cmp(argument,"pixie"))
{
	ch->pcRace = RACE_PIXIE;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Pixie.\n\r", ch);
}
else if (!str_cmp(argument,"aquaticgnome"))
{
	ch->pcRace = RACE_AQUATICGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Aquatic Gnome\n\r", ch);
}
else if (!str_cmp(argument,"arcticgnome"))
{
	ch->pcRace = RACE_ARCTICGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are an Arctic Gnome\n\r", ch);
}
else if (!str_cmp(argument,"chaosgnome"))
{
	ch->pcRace = RACE_CHAOSGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Chaos Gnome\n\r", ch);
}
else if (!str_cmp(argument,"deepgnome"))
{
	ch->pcRace = RACE_DEEPGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Deep Gnome\n\r", ch);
}
else if (!str_cmp(argument,"desertgnome"))
{
	ch->pcRace = RACE_DESERTGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Desert Gnome\n\r", ch);
}
else if (!str_cmp(argument,"forestgnome"))
{
	ch->pcRace = RACE_FORESTGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Forest Gnome\n\r", ch);
}
else if (!str_cmp(argument,"junglegnome"))
{
	ch->pcRace = RACE_JUNGLEGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Jungle Gnome\n\r", ch);
}
else if (!str_cmp(argument,"rockgnome"))
{
	ch->pcRace = RACE_ROCKGNOME;
	ch->pcsize = SIZE_SMALL;
	send_to_char( "You are a Rock Gnome\n\r", ch);
}
else if (!str_cmp(argument,"bugbear"))
{
	ch->pcRace = RACE_BUGBEAR;
	ch->pcsize = SIZE_MEDIUM;
	send_to_char( "You are a Bugbear\n\r", ch);
}
else
{
write_to_buffer( d, "That's not a race.\n\r", 0 );
write_to_buffer( d, "Try with no spaces.\n\r ", 0 );
return;
}


        write_to_buffer(d,  "#Y[#0-------#Y[#RProfession#Y]#0-------#Y]\n\r", 0);
        write_to_buffer(d, "#y--#CMercenary     #y--#CSoldier\n\r", 0);
        write_to_buffer(d, "#y--#CCleric        #y--#CMonk\n\r" ,0);
        write_to_buffer(d, "#y--#CThief         #y--#CAssassin\n\r",0);
	write_to_buffer(d, "#y--#CKnight        #y--#CBarbarian\n\r",0);
        write_to_buffer(d, "#Y[#0--------------------------#Y]\n\r#n", 0);
	d->connected = CON_GET_NEW_PROF;
	break;

    case CON_GET_NEW_PROF:

	if (!str_cmp(argument,"mercenary"))
	{
	send_to_char( "You have become a mercenary.\n\r", ch);
	ch->profession = PROF_MERCENARY;
	}
        else if (!str_cmp(argument,"soldier"))
        {
        send_to_char( "You have become a soldier.\n\r", ch);
        ch->profession = PROF_SOLDIER;
        }
        else if (!str_cmp(argument,"cleric"))
        {
        send_to_char( "You have become a cleric.\n\r", ch);
        ch->profession = PROF_CLERIC;
        }
        else if (!str_cmp(argument,"monk"))
        {
        send_to_char( "You have become a monk.\n\r", ch);
        ch->profession = PROF_MONK;
        }
        else if (!str_cmp(argument,"thief"))
        {
        send_to_char( "You have become a thief.\n\r", ch);
        ch->profession = PROF_THIEF;
        }
        else if (!str_cmp(argument,"assassin"))
        {
        send_to_char( "You have become a assassin.\n\r", ch);
        ch->profession = PROF_ASSASSIN;
        }
        else if (!str_cmp(argument,"knight"))
        {
        send_to_char( "You have become a knight.\n\r", ch);
        ch->profession = PROF_KNIGHT;
        }
        else if (!str_cmp(argument,"barbarian"))
        {
        send_to_char( "You have become a barbarian.\n\r", ch);
        ch->profession = PROF_BARBARIAN;
        }
	else
	{
        write_to_buffer( d, "That's not a profession.\n\r", 0 );
	write_to_buffer( d, "What IS your profession?\n\r ", 0 );
        return;
	}

        write_to_buffer(d, "#Y[#0-------#Y[#RGuild#Y]#0-------#Y]\n\r", 0);
        write_to_buffer(d, "#y--#CDarkHand      #y--#CGuardian\n\r", 0);
        write_to_buffer(d, "#y--#CBeast         #y--#CDamned\n\r" ,0);
        write_to_buffer(d, "#y--#CCrusaders     #y--#CDruid\n\r",0);
        write_to_buffer(d, "#Y[#0---------------------#Y]\n\r#n", 0);
        d->connected = CON_GET_NEW_GUILD;
        break;

//        write_to_buffer( d, "Do you wish to use ANSI color (y/n)? ", 0);
//        d->connected = CON_GET_NEW_ANSI;
//	break;

   case CON_GET_NEW_GUILD:
        if (!str_cmp(argument,"darkhand"))
        {
        send_to_char( "You have joined Dark Hand.\n\r", ch);
        ch->guild = GUILD_DARK_HAND;
        }
        else if (!str_cmp(argument,"guardian"))
        {
        send_to_char( "You have joined Guardian.\n\r", ch);
        ch->guild = GUILD_GUARDIAN;
        }
        else if (!str_cmp(argument,"beast"))
        {
        send_to_char( "You have joined beast.\n\r", ch);
        ch->guild = GUILD_BEAST;
        }
        else if (!str_cmp(argument,"damned"))
        {
        send_to_char( "You have joined The Damned.\n\r", ch);
        ch->guild = GUILD_DAMNED;
        }
        else if (!str_cmp(argument,"crusaders"))
        {
        send_to_char( "You have joined Crusaders.\n\r", ch);
        ch->guild = GUILD_CRUSADERS;
        }
        else if (!str_cmp(argument,"druid"))
        {
        send_to_char( "You have joined the Druids.\n\r", ch);
        ch->guild = GUILD_DRUID;
        }
        else
        {
        write_to_buffer( d, "That's not a Guild.\n\r", 0 );
        write_to_buffer( d, "What IS your Guild?\n\r ", 0 );
        return;
        }

        write_to_buffer( d, "Do you wish to use ANSI color (y/n)? ", 0);
        d->connected = CON_GET_NEW_ANSI;
        break;


    case CON_GET_NEW_ANSI:
	switch ( argument[0] )
	{
	case 'y': case 'Y': SET_BIT(ch->act,PLR_ANSI);    break;
	case 'n': case 'N': break;
	default:
	    write_to_buffer( d, "Do you wish to use ANSI (y/n)? ", 0 );
	    return;
	}

	ch->pcdata->perm_str=number_range(10,16);
        ch->pcdata->perm_int=number_range(10,16);
        ch->pcdata->perm_wis=number_range(10,16);
        ch->pcdata->perm_dex=number_range(10,16);
	ch->pcdata->perm_con=number_range(10,16);
	ch->class = 0;
	sprintf( log_buf, "%s@%s new player.",ch->name, d->host );
	log_string( log_buf );
	write_to_buffer( d, "\n\r", 2 );
	do_help( ch, "motd" );
	d->connected = CON_READ_MOTD;
	break;

    case CON_READ_MOTD:
	ch->next	= char_list;
	char_list	= ch;
	d->connected	= CON_PLAYING;


	if (ch->mounted == IS_RIDING) do_dismount(ch,"");

	if ( ch->level == 0 )
	{
            ch->pcdata->awins        = 0; /* arena wins           */
            ch->pcdata->alosses      = 0; /* arena losses         */
		ch->pcdata->board = &boards[0];
	    ch->challenger = NULL;
	    ch->level	= 1;
            ch->generation = 6;
            ch->stance[19] = -1;
            ch->stance[20] = -1;
            ch->stance[21] = -1;
            ch->stance[22] = -1;
            ch->stance[23] = -1;
	    ch->exp	= 0;
	    ch->hit	= 2000; // ch->max_hit;
	    ch->mana	= ch->max_mana;
	    ch->move	= ch->max_move;
            ch->special = 0;
	    set_switchname(ch, ch->name);
	    char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
            clear_stats(ch);
	    do_look( ch, "auto" );
	    do_newbiepack(ch, "");
	}
	else if (!IS_NPC(ch) && ch->pcdata->obj_vnum != 0)
	{
	    if (ch->in_room != NULL) char_to_room( ch, ch->in_room );
	    else char_to_room( ch, get_room_index( ROOM_VNUM_SCHOOL ) );
	    bind_char(ch);
	    break;
	}
	else if ( ch->in_room != NULL )
	{
	    char_to_room( ch, ch->in_room );
	    do_look( ch, "auto" );
	}
	else
	{
	    char_to_room( ch, get_room_index( ROOM_VNUM_TEMPLE ) );
	    do_look( ch, "auto" );
	}
        players_logged++;
	sprintf(buf,"#7A #Rnew player#7 named #g%s #7enters #RAge of Heroes.#n", ch->name );
	enter_info(buf);
	ch->fight_timer = 0;
        ch->pcdata->safe_counter = 0;
        ch->pcdata->revision = CURRENT_REVISION;
	act( "$n has entered the game.", ch, NULL, NULL, TO_ROOM );
	room_text(ch,">ENTER<");

        if (multicheck(ch))
        {
          send_to_char("\n\r#R====#0[#y**#0]#R====  #GWARNING  #R====#0[#y**#0]#R====#n\n\r",ch);
          send_to_char("You have connected from the same\n\r",ch);
          send_to_char("IP as someone who is already on the\n\r",ch);
          send_to_char("mud. Multiplaying is illegal, and\n\r",ch);
          send_to_char("your actions will be monitored.\n\r",ch);
          send_to_char("#R====#0[#y**#0]#R====  #GWARNING  #R====#0[#y**#0]#R====#n\n\r",ch);
        }
	break;

	/* states for new note system, (c)1995-96 erwin@pip.dknet.dk */
	/* ch MUST be PC here; have nwrite check for PC status! */
		
	case CON_NOTE_TO:
		handle_con_note_to (d, argument);
		break;
		
	case CON_NOTE_SUBJECT:
		handle_con_note_subject (d, argument);
		break; /* subject */
	
	case CON_NOTE_EXPIRE:
		handle_con_note_expire (d, argument);
		break;

	case CON_NOTE_TEXT:
		handle_con_note_text (d, argument);
		break;
		
	case CON_NOTE_FINISH:
		handle_con_note_finish (d, argument);
		break;

    }

    return;
}



/*
 * Parse a name for acceptability.
 */
bool check_parse_name( char *name )
{
    /*
     * Reserved words.
     */
    if (is_name( name, "all wtf auto immortal self someone gaia none save quit why who noone level"))
      return FALSE;

    if (is_contained("fuck",name) || is_contained("bitch",name) || is_contained("whore",name))
      return FALSE;

    /*
     * Length restrictions.
     */
    if ( strlen(name) <  3 )
	return FALSE;

    if ( strlen(name) > 12 )
	return FALSE;

    /*
     * Alphanumerics only.
     * Lock out IllIll twits.
     */
    {
	char *pc;
	bool fIll;

	fIll = TRUE;
	for ( pc = name; *pc != '\0'; pc++ )
	{
	    if ( !isalpha(*pc) )
		return FALSE;
	    if ( LOWER(*pc) != 'i' && LOWER(*pc) != 'l' )
		fIll = FALSE;
	}

	if ( fIll )
	    return FALSE;
    }

    /*
     * Prevent players from naming themselves after mobs.
     */
    {
	extern MOB_INDEX_DATA *mob_index_hash[MAX_KEY_HASH];
	MOB_INDEX_DATA *pMobIndex;
	int iHash;

	for ( iHash = 0; iHash < MAX_KEY_HASH; iHash++ )
	{
	    for ( pMobIndex  = mob_index_hash[iHash];
		  pMobIndex != NULL;
		  pMobIndex  = pMobIndex->next )
	    {
		if ( is_name( name, pMobIndex->player_name ))
		    return FALSE;
	    }
	}
    }

    return TRUE;
}



/*
 * Look for link-dead player to reconnect.
 */
bool check_reconnect( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch) && ( !fConn || ch->desc == NULL )
	&&   !str_cmp( GET_PC_NAME(d->character), GET_PC_NAME(ch) ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
                if (ch->desc != NULL)
                {
                  close_socket2(ch->desc, TRUE);
                }
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "Reconnecting.\n\r", ch );
	    	if (IS_NPC(ch) || ch->pcdata->obj_vnum == 0)
		    act( "$n has reconnected.", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s reconnected.",ch->name, ch->lasthost );
		log_string( log_buf );
		d->connected = CON_PLAYING;
 		/* Inform the character of a note in progress and the possbility of continuation! */		
 		if (ch->pcdata->in_progress)
 			send_to_char ("You have a note in progress. Type NWRITE to continue it.\n\r",ch);

	    }
	    return TRUE;
	}
    }

    return FALSE;
}

/*
 * Kick off old connection.  KaVir.
 */
bool check_kickoff( DESCRIPTOR_DATA *d, char *name, bool fConn )
{
    CHAR_DATA *ch;

    for ( ch = char_list; ch != NULL; ch = ch->next )
    {
	if ( !IS_NPC(ch)
	&& ( !fConn || ch->desc == NULL )
	&&   !str_cmp( GET_PC_NAME(d->character),GET_PC_NAME(ch) ) )
	{
	    if ( fConn == FALSE )
	    {
		free_string( d->character->pcdata->pwd );
		d->character->pcdata->pwd = str_dup( ch->pcdata->pwd );
	    }
	    else
	    {
		free_char( d->character );
		d->character = ch;
		ch->desc	 = d;
		ch->timer	 = 0;
		send_to_char( "You take over your body, which was already in use.\n\r", ch );
		act( "...$n's body has been taken over by another spirit!", ch, NULL, NULL, TO_ROOM );
		sprintf( log_buf, "%s@%s kicking off old link.", ch->name, ch->lasthost );
		log_string( log_buf );
		d->connected = CON_PLAYING;
	    }
	    return TRUE;
	}
    }

    return FALSE;
}



/*
 * Check if already playing - KaVir.
 * Using kickoff code from Malice, as mine is v. dodgy.
 */
bool check_playing( DESCRIPTOR_DATA *d, char *name )
{
    DESCRIPTOR_DATA *dold;

    for ( dold = descriptor_list; dold != NULL; dold = dold->next )
    {
	if ( dold != d
	&&   dold->character != NULL
	&&   dold->connected != CON_GET_NAME
	&&   dold->connected != CON_GET_OLD_PASSWORD
        &&   !str_cmp(name, GET_PC_NAME(dold->character)))
	{
	    char	buf [MAX_STRING_LENGTH];
	    if ( d->character != NULL )
	    {
		free_char( d->character );
		d->character = NULL;
	    }
	    send_to_char("This body has been taken over!\n\r",dold->character);
	    d->connected = CON_PLAYING;
	    d->character = dold->character;
	    d->character->desc = d;
	    send_to_char( "You take over your body, which was already in use.\n\r", d->character );
	    act( "$n doubles over in agony and $s eyes roll up into $s head.", d->character, NULL, NULL, TO_ROOM );
	    act( "...$n's body has been taken over by another spirit!", d->character, NULL, NULL, TO_ROOM );
	    dold->character=NULL;
	    
	    sprintf(buf,"Kicking off old connection %s@%s",d->character->name,d->host);
	    log_string(buf);
	    close_socket(dold);	/*Slam the old connection into the ether*/
	    return TRUE;
	}
    }

    return FALSE;
}



void stop_idling( CHAR_DATA *ch )
{
    if ( ch == NULL
    ||   ch->desc == NULL
    ||   (ch->desc->connected != CON_PLAYING && ch->desc->connected != CON_EDITING)
    ||   ch->was_in_room == NULL 
    ||   ch->in_room != get_room_index( ROOM_VNUM_LIMBO ) )
	return;

    ch->timer = 0;
    char_from_room( ch );
    char_to_room( ch, ch->was_in_room );
    ch->was_in_room	= NULL;
    act( "$n has returned from the void.", ch, NULL, NULL, TO_ROOM );
    return;
}


void stc( const char *txt, CHAR_DATA *ch )
{
    if ( txt != NULL && ch->desc != NULL )
	write_to_buffer( ch->desc, txt, strlen(txt) );
    return;
}

int col_str_len(char *txt)
{
    int pos, len;

    len = 0;
    for (pos = 0; txt[pos] != '\0'; pos++)
    {
	if (txt[pos] != '#')
	{
	    len++;
	    continue;
	}

	pos++;
	if (txt[pos] == '\0') return len;
	if (txt[pos] == '#' || txt[pos] == '-') len++;
    }

    return len;
}

/*
 * Write to one char.
 */
void send_to_char( const char *txt, CHAR_DATA *ch )
{
  if (txt != NULL && ch->desc != NULL)
    write_to_buffer( ch->desc, txt, 0);
  return;
}

/* source: EOD, by John Booth */
void printf_to_char(CHAR_DATA *ch, char *fmt, ...)
{
  char buf [MAX_STRING_LENGTH];
  va_list args;

  va_start(args, fmt);
  vsprintf(buf, fmt, args);
  va_end(args);

  send_to_char (buf, ch);
}

/*
 * The primary output interface for formatted output.
 */
void act( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type )
{
    static char * const he_she	[] = { "it",  "he",  "she" };
    static char * const him_her	[] = { "it",  "him", "her" };
    static char * const his_her	[] = { "its", "his", "her" };

    char buf[MAX_STRING_LENGTH];
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;

    CHAR_DATA *to_old;

    CHAR_DATA *vch = (CHAR_DATA *) arg2;

    CHAR_DATA *familiar = NULL;
//    CHAR_DATA *wizard = NULL;

    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;

    bool is_fam;

    bool is_ok;
    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
	return;

/*	Skummel crash bug, tror dette check spiser buggen, vi faar se */
	if ( ch == NULL || ch->in_room == NULL)
	{
	    return;
	}

    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
	if ( vch == NULL || vch->in_room == NULL)
	{
/*	    bug( "Act: null vch with TO_VICT.", 0 );*/
	    return;
	}
	to = vch->in_room->people;
    }
    
    for ( ; to != NULL; to = to->next_in_room )
    {

	is_fam = FALSE;
	to_old = to;

	if ( type == TO_CHAR && to != ch ) continue;
	if ( type == TO_VICT && ( to != vch || to == ch ) ) continue;
	if ( type == TO_ROOM && to == ch ) continue;
	if ( type == TO_NOTVICT && (to == ch || to == vch) ) continue;
/*
	if ( to->desc == NULL && IS_NPC(to) && (wizard = to->wizard) != NULL )
	{
	    if (!IS_NPC(wizard) && ((familiar=wizard->pcdata->familiar) != NULL)
		&& familiar == to)
	    {
		if (to->in_room == ch->in_room && 
		    wizard->in_room != to->in_room)
		{
		    to = wizard;
		    is_fam = TRUE;
		}
	    }
	}
*/
	if ( to->desc == NULL || !IS_AWAKE(to) )
	{

	    if (is_fam) to = to_old;

	    continue;
	}

    	if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
    	{
	    is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_room != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_room != NULL &&
	    	ch->in_room == to->in_room)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!IS_NPC(ch) && ch->pcdata->chobj != NULL && 
	    	ch->pcdata->chobj->in_obj != NULL &&
	    	!IS_NPC(to) && to->pcdata->chobj != NULL && 
	    	to->pcdata->chobj->in_obj != NULL &&
	    	ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
		    is_ok = TRUE; else is_ok = FALSE;

	    if (!is_ok)
	    {

		if (is_fam) to = to_old;

		continue;
	    }
    	}

	point	= buf;
	str	= format;
	while ( *str != '\0' )
	{
	    if ( *str != '$' )
	    {
		*point++ = *str++;
		continue;
	    }
	    ++str;

	    if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' )
	    {
/*		bug( "Act: missing arg2 for code %d.", *str );*/
		i = " <@@@> ";
	    }
	    else
	    {
		switch ( *str )
		{
		default:  bug( "Act: bad code %d.", *str );
			  i = " <@@@> ";				break;
		/* Thx alex for 't' idea */
		case 't': i = (char *) arg1;				break;
		case 'T': i = (char *) arg2;          			break;
		case 'n': i = PERS( ch,  to  );				break;
		case 'N': i = PERS( vch, to  );				break;
		case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];	break;
		case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];	break;
		case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'M': i = him_her [URANGE(0, vch ->sex, 2)];	break;
		case 's': i = his_her [URANGE(0, ch  ->sex, 2)];	break;
		case 'S': i = his_her [URANGE(0, vch ->sex, 2)];	break;

		case 'p':
		    i = can_see_obj( to, obj1 )
			    ? ( (obj1->chobj != NULL && obj1->chobj == to)
			    ? "you" : obj1->short_descr)
			    : "something";
		    break;

		case 'P':
		    i = can_see_obj( to, obj2 )
			    ? ( (obj2->chobj != NULL && obj2->chobj == to)
			    ? "you" : obj2->short_descr)
			    : "something";
		    break;

		case 'd':
		    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
		    {
			i = "door";
		    }
		    else
		    {
			one_argument( (char *) arg2, fname );
			i = fname;
		    }
		    break;
		}
	    }
		
	    ++str;
	    while ( ( *point = *i ) != '\0' )
		++point, ++i;
	}

	*point++ = '\n';
	*point++ = '\r';

	if (is_fam)
	{
	    if (to->in_room != ch->in_room && familiar != NULL &&
		familiar->in_room == ch->in_room)
		send_to_char("[ ", to);
	    else
	    {
		to = to_old;
		continue;
	    }
	}

	buf[0]   = UPPER(buf[0]);
	if (to->desc && (to->desc->connected == CON_PLAYING))
	write_to_buffer( to->desc, buf, point - buf );

	if (is_fam) to = to_old;

    }
    return;
}


void act_brief( const char *format, CHAR_DATA *ch, const void *arg1, const void *arg2, int type, int iBrief)
{        
    static char * const he_she  [] = { "it",  "he",  "she" };
    static char * const him_her [] = { "it",  "him", "her" };
    static char * const his_her [] = { "its", "his", "her" };

    char buf[MAX_STRING_LENGTH];  
    char fname[MAX_INPUT_LENGTH];
    CHAR_DATA *to;

    CHAR_DATA *to_old;
                
    CHAR_DATA *vch = (CHAR_DATA *) arg2;

    CHAR_DATA *familiar = NULL;
//    CHAR_DATA *wizard = NULL;

    OBJ_DATA *obj1 = (OBJ_DATA  *) arg1;
    OBJ_DATA *obj2 = (OBJ_DATA  *) arg2;
    const char *str;
    const char *i;
    char *point;
                
    bool is_fam;
                
    bool is_ok;
    /*
     * Discard null and zero-length messages.
     */
    if ( format == NULL || format[0] == '\0' )
        return;
           
    if ( ch == NULL || ch->in_room == NULL) return;
         
    to = ch->in_room->people;
    if ( type == TO_VICT )
    {
        if ( vch == NULL || vch->in_room == NULL) return;
        to = vch->in_room->people;
    }
                
    for ( ; to != NULL; to = to->next_in_room )
    {
                
        is_fam = FALSE;
        to_old = to;
                    
        if ( type == TO_CHAR && to != ch ) continue;
        if ( type == TO_VICT && ( to != vch || to == ch ) ) continue;
        if ( type == TO_ROOM && to == ch ) continue;
        if ( type == TO_NOTVICT && (to == ch || to == vch) ) continue;
/*
        if ( to->desc == NULL && IS_NPC(to) && (wizard = to->wizard) != NULL )
        {
            if (!IS_NPC(wizard) && ((familiar=wizard->pcdata->familiar) != NULL)
                && familiar == to)
            {
                if (to->in_room == ch->in_room &&
                    wizard->in_room != to->in_room)
                {
                    to = wizard;
                    is_fam = TRUE;
                }
            }
        }  
*/
        if ( to->desc == NULL || !IS_AWAKE(to) )
        {
            if (is_fam) to = to_old;
            continue;
        }
        if (ch->in_room->vnum == ROOM_VNUM_IN_OBJECT)
        {
            is_ok = FALSE;
                
            if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                ch->pcdata->chobj->in_room != NULL &&
                !IS_NPC(to) && to->pcdata->chobj != NULL &&
                to->pcdata->chobj->in_room != NULL &&
                ch->in_room == to->in_room)
                    is_ok = TRUE; else is_ok = FALSE;
             
            if (!IS_NPC(ch) && ch->pcdata->chobj != NULL &&
                ch->pcdata->chobj->in_obj != NULL &&
                !IS_NPC(to) && to->pcdata->chobj != NULL &&
                to->pcdata->chobj->in_obj != NULL &&
                ch->pcdata->chobj->in_obj == to->pcdata->chobj->in_obj)
                    is_ok = TRUE; else is_ok = FALSE;
             
            if (!is_ok)
            {
                if (is_fam) to = to_old;
                continue;
            }
        }
        /*
         * Checking for brief
         */
        if (!IS_NPC(to) && IS_SET(to->act, iBrief)) continue;
     
        point   = buf;
        str     = format;
        while ( *str != '\0' )
        {       
            if ( *str != '$' )
            {
                *point++ = *str++;
                continue;
            }
            ++str;  

            if ( arg2 == NULL && *str >= 'A' && *str <= 'Z' ) i = " <@@@> ";
            else
            {
                switch ( *str )
                {
                default:  bug( "Act: bad code %d.", *str );
                          i = " <@@@> ";                                break;
                /* Thx alex for 't' idea */
                case 't': i = (char *) arg1;                            break;
                case 'T': i = (char *) arg2;                            break;
                case 'n': i = PERS( ch,  to  );                         break;
                case 'N': i = PERS( vch, to  );                         break;
                case 'e': i = he_she  [URANGE(0, ch  ->sex, 2)];        break;
                case 'E': i = he_she  [URANGE(0, vch ->sex, 2)];        break;
                case 'm': i = him_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'M': i = him_her [URANGE(0, vch ->sex, 2)];        break;
                case 's': i = his_her [URANGE(0, ch  ->sex, 2)];        break;
                case 'S': i = his_her [URANGE(0, vch ->sex, 2)];        break;
            
                case 'p':
                    i = can_see_obj( to, obj1 )
                            ? ( (obj1->chobj != NULL && obj1->chobj == to)
                            ? "you" : obj1->short_descr)
                            : "something";
                    break;
            
                case 'P':
                    i = can_see_obj( to, obj2 )
                            ? ( (obj2->chobj != NULL && obj2->chobj == to)
                            ? "you" : obj2->short_descr)
                            : "something";
                    break;
            
                case 'd':
                    if ( arg2 == NULL || ((char *) arg2)[0] == '\0' )
                    {
                        i = "door";
                    }
                    else
                    {  
                        one_argument( (char *) arg2, fname );
                        i = fname;
                    }
                    break;
                }
            }
            ++str;
            while ( ( *point = *i ) != '\0' )
                ++point, ++i;
        }
        
        *point++ = '\n'; 
        *point++ = '\r';
                
        if (is_fam)
        {
            if (to->in_room != ch->in_room && familiar != NULL &&
                familiar->in_room == ch->in_room)
                send_to_char("[ ", to);
            else    
            {
                to = to_old;
                continue;
            }
        }
                 
        buf[0]   = UPPER(buf[0]);
        if (to->desc && (to->desc->connected == CON_PLAYING))
        write_to_buffer( to->desc, buf, point - buf );
                
        if (is_fam) to = to_old;
                
    }
    return;
}

/*
 * Bust a prompt (player settable prompt)
 * coded by Morgenes for Aldara Mud
 */
void bust_a_prompt( DESCRIPTOR_DATA *d )
{
  CHAR_DATA *ch;
  CHAR_DATA *victim;
  CHAR_DATA *tank;
  const char *str;
  const char *i;
  char *point;
  char buf  [ MAX_STRING_LENGTH ];
  char buf2 [ MAX_STRING_LENGTH ];
  bool is_fighting = TRUE;

  if ((ch = d->character ) == NULL) return;
  if (ch->pcdata == NULL )
  {
    send_to_char( "\n\r\n\r", ch );
    return;
  }
  if ( ch->position == POS_FIGHTING && ch->cprompt[0] == '\0' )
  {
    if ( ch->prompt[0] == '\0' )
    {
      send_to_char( "\n\r\n\r", ch );
      return;
    }
    is_fighting = FALSE;
  }
  else if ( ch->position != POS_FIGHTING && ch->prompt[0] == '\0' )
  {
    send_to_char( "\n\r\n\r", ch );
    return;
  }
  point = buf;
  if ( ch->position == POS_FIGHTING && is_fighting )
    str = d->character->cprompt;
  else
    str = d->character->prompt;
  while( *str != '\0' )
  {
    if( *str != '%' )
    {
      *point++ = *str++;
      continue;
    }
    ++str;
    switch( *str )
    {
      default :
        i = " "; break;
      case 'h' :
        sprintf( buf2, "%d", ch->hit );
        COL_SCALE(buf2, ch, ch->hit, ch->max_hit);
        i = buf2; break;
      case 'H' :
        sprintf( buf2, "%d", ch->max_hit );
        ADD_COLOUR(ch, buf2, L_CYAN);
        i = buf2; break;
      case 'm' :
        sprintf( buf2, "%d", ch->mana                              );
        COL_SCALE(buf2, ch, ch->mana, ch->max_mana);
        i = buf2; break;
      case 'M' :
        sprintf( buf2, "%d", ch->max_mana                          );
        ADD_COLOUR(ch, buf2, L_CYAN);
        i = buf2; break;
      case 'v' :
        sprintf( buf2, "%d", ch->move                              ); 
        COL_SCALE(buf2, ch, ch->move, ch->max_move);
        i = buf2; break;
      case 'V' :
        sprintf( buf2, "%d", ch->max_move                          );
        ADD_COLOUR(ch, buf2, L_CYAN);
        i = buf2; break;
      case 'x' :
        sprintf( buf2, "%2.0f", ch->exp                               );
//        COL_SCALE(buf2, ch, ch->exp, 1000);
        i = buf2; break;
      case 'X' :
        sprintf( buf2, "\n\r");
        i = buf2; break;
      case 'q' :
        sprintf( buf2, "%d", ch->pcdata->quest                     );
        ADD_COLOUR(ch, buf2, L_CYAN);
        i = buf2; break;
      case 'f' :
        if ( ( victim = ch->fighting ) == NULL )
        {
          sprintf( buf2, "N/A" );
          ADD_COLOUR(ch, buf2, L_CYAN);
        }
        else
        {
          if ((victim->hit*100/victim->max_hit) < 25)
            {sprintf(buf2, "Awful");ADD_COLOUR(ch, buf2, L_RED);}
          else if ((victim->hit*100/victim->max_hit) < 50)
            {sprintf(buf2, "Poor");ADD_COLOUR(ch, buf2, L_BLUE);}
          else if ((victim->hit*100/victim->max_hit) < 75)
            {sprintf(buf2, "Fair");ADD_COLOUR(ch, buf2, L_GREEN);}
          else if ((victim->hit*100/victim->max_hit) < 100)
            {sprintf(buf2, "Good");ADD_COLOUR(ch, buf2, YELLOW);}
          else if ((victim->hit*100/victim->max_hit) >= 100)
            {sprintf(buf2, "Perfect");ADD_COLOUR(ch, buf2, L_CYAN);}
        }
        i = buf2; break;
      case 'F' :
        if ( ( victim = ch->fighting ) == NULL )
        {
          sprintf( buf2, "N/A" );
          ADD_COLOUR(ch, buf2, L_CYAN);
        }
        else if ( ( tank = victim->fighting ) == NULL )
        {
          sprintf( buf2, "N/A" );
          ADD_COLOUR(ch, buf2, L_CYAN);
        }
        else
        {
          if ((tank->hit*100/tank->max_hit) < 25)
            {sprintf(buf2, "Awful");ADD_COLOUR(ch, buf2, L_RED);}
          else if ((tank->hit*100/tank->max_hit) < 50)
            {sprintf(buf2, "Poor");ADD_COLOUR(ch, buf2, L_BLUE);}
          else if ((tank->hit*100/tank->max_hit) < 75)
            {sprintf(buf2, "Fair");ADD_COLOUR(ch, buf2, L_GREEN);}
          else if ((tank->hit*100/tank->max_hit) < 100)
            {sprintf(buf2, "Good");ADD_COLOUR(ch, buf2, YELLOW);}
          else if ((tank->hit*100/tank->max_hit) >= 100)
            {sprintf(buf2, "Perfect");ADD_COLOUR(ch, buf2, L_CYAN);}
         }
         i = buf2; break;
       case 'n' :
         if ( ( victim = ch->fighting ) == NULL )
	   sprintf( buf2, "N/A" );
         else
         {
           if ( IS_AFFECTED(victim, AFF_POLYMORPH) )
             strcpy(buf2, victim->morph);
           else if ( IS_NPC(victim) )
             strcpy(buf2, victim->short_descr);
           else
             strcpy(buf2, victim->name);
           buf2[0] = UPPER(buf2[0]);
         }
         i = buf2; break;
       case 'N' :
         if ( ( victim = ch->fighting ) == NULL )
	   sprintf( buf2, "N/A" );
         else if ( ( tank = victim->fighting ) == NULL )
	   sprintf( buf2, "N/A" );
         else
         {
           if ( ch == tank )
             sprintf(buf2, "You");
           else if ( IS_AFFECTED(tank, AFF_POLYMORPH) )
             strcpy(buf2, tank->morph);
           else if ( IS_NPC(victim) )
             strcpy(buf2, tank->short_descr);
           else
             strcpy(buf2, tank->name);
           buf2[0] = UPPER(buf2[0]);
         }
         i = buf2; break;
       case 't' :
         sprintf(buf2, "%d", ch->fight_timer			  );
         ADD_COLOUR(ch, buf2, L_CYAN);
         i = buf2; break;
       case 'A':
         if (!ch->fighting) sprintf(buf2, "N/A");
         else
         {
           sprintf(buf2, "*");
           if (ch->fighting->max_hit != 0)
           {
             CHAR_DATA *och = ch->fighting;
             int k;

             for (k = 1; k < 10; k++)
             {
               if (och->hit > (och->max_hit * k / 10))
                 strcat(buf2, "*");
               else strcat (buf2, " ");
             }
           }
           else strcat(buf2, "         ");
         }
         i = buf2;
         break;
       case 'a':
         if (!ch->fighting) sprintf(buf2, "N/A");
         else 
         {
           if ((ch->fighting->hit*100/ch->fighting->max_hit) < 25)
             sprintf(buf2, "#R*");
           else if ((ch->fighting->hit*100/ch->fighting->max_hit) < 50)
             sprintf(buf2, "#L*");
           else if ((ch->fighting->hit*100/ch->fighting->max_hit) < 75)
             sprintf(buf2, "#G*");
           else if ((ch->fighting->hit*100/ch->fighting->max_hit) < 100)
             sprintf(buf2, "#y*");
           else
             sprintf(buf2, "#C*");
           if (ch->fighting->max_hit != 0)
           {
             CHAR_DATA *och = ch->fighting;
             int k;

             for (k = 1; k < 10; k++)
             {
               if (och->hit > (och->max_hit * k / 10))
                 strcat(buf2, "*");
               else strcat (buf2, " ");
             }
             strcat(buf2, "#n");
           }
           else strcat(buf2, "         ");
         }
         i = buf2;
         break;
     case 'p' :
       sprintf( buf2, "%d", char_hitroll(ch) );
       COL_SCALE(buf2, ch, char_hitroll(ch), 200);
       i = buf2; break;
     case 'P' :
       sprintf( buf2, "%d", char_damroll(ch) );
       COL_SCALE(buf2, ch, char_damroll(ch), 200);
       i = buf2; break;
     case '%' :
       sprintf( buf2, "%%"                                        );
       i = buf2; break;
    } 
    ++str;
    while( ( *point = *i ) != '\0' ) ++point, ++i;      
 }
 write_to_buffer( d, buf, point - buf );
 return;
}


/*
 * Macintosh support functions.
 */
#if defined(macintosh)
int gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif
/*

int len(char s)
{
	int l = 0;
	int x;

	if (s[0] == '\0') return 0;

	for (x = 0; x < length(s); x++)
	{
		if (s[x] == '#')
		{
			if s[x+1
		}

	}

	return l;
}
*/

/* OLC, new pager for editing long descriptions. */
/* ========================================================================= */
/* - The heart of the pager.  Thanks to N'Atas-Ha, ThePrincedom for porting  */
/*   this SillyMud code for MERC 2.0 and laying down the groundwork.         */
/* - Thanks to Blackstar, hopper.cs.uiowa.edu 4000 for which the improvements*/
/*   to the pager was modeled from.  - Kahn                                  */
/* - Safer, allows very large pagelen now, and allows to page while switched */
/*   Zavod of jcowan.reslife.okstate.edu 4000.                               */
/* ========================================================================= */

void show_string( DESCRIPTOR_DATA *d, char *input )
{
    char               *start, *end;
    char                arg[MAX_INPUT_LENGTH];
    int                 lines = 0, pagelen;

    /* Set the page length */
    /* ------------------- */

    pagelen = d->character->pcdata->pagelen;


    /* Check for the command entered */
    /* ----------------------------- */

    one_argument( input, arg );

    switch( UPPER( *arg ) )
    {
        /* Show the next page */

        case '\0':
        case 'C': lines = 0;
                  break;
        
        /* Scroll back a page */

        case 'B': lines = -2 * pagelen;
                  break;

        /* Help for show page */

        case 'H': write_to_buffer( d, "B     - Scroll back one page.\n\r", 0 );
                  write_to_buffer( d, "C     - Continue scrolling.\n\r", 0 );
                  write_to_buffer( d, "H     - This help menu.\n\r", 0 );
                  write_to_buffer( d, "R     - Refresh the current page.\n\r",
                                   0 );
                  write_to_buffer( d, "Enter - Continue Scrolling.\n\r", 0 );
                  return;

        /* refresh the current page */

        case 'R': lines = -1 - pagelen;
                  break;

        /* stop viewing */

        default:  free_string( d->showstr_head );
                  d->showstr_head  = NULL;
                  d->showstr_point = NULL;
                  return;
    }

    /* do any backing up necessary to find the starting point */
    /* ------------------------------------------------------ */

    if ( lines < 0 )
    {
        for( start= d->showstr_point; start > d->showstr_head && lines < 0;
             start-- )
            if ( *start == '\r' )
                lines++;
    }
    else
        start = d->showstr_point;

    /* Find the ending point based on the page length */
    /* ---------------------------------------------- */

    lines  = 0;

    for ( end= start; *end && lines < pagelen; end++ )
        if ( *end == '\r' )
            lines++;

    d->showstr_point = end;

    if ( end - start )
        write_to_buffer( d, start, end - start );

    /* See if this is the end (or near the end) of the string */
    /* ------------------------------------------------------ */

    for ( ; isspace( *end ); end++ );

    if ( !*end )
    {
        free_string( d->showstr_head );
        d->showstr_head  = NULL;
        d->showstr_point = NULL;
    }

    return;
}

#if defined( WIN32 )
void gettimeofday( struct timeval *tp, void *tzp )
{
    tp->tv_sec  = time( NULL );
    tp->tv_usec = 0;
}
#endif

void logt (char * fmt, ...)
{
	char buf [2*MAX_STRING_LENGTH];
	va_list args;
	va_start (args, fmt);
	vsprintf (buf, fmt, args);
	va_end (args);
	
	log_string (buf);
}

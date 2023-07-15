/* vim: set tabstop=3:softtabstop=3:shiftwidth=3:noexpandtab */

#include <stdio.h>
#include <termios.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <shadow.h>
#include <crypt.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>

#define false 0
#define true  1
#define bool  int
#define VERSION "0.1 (2023-07-16)"

static int checkPasswd(const char *user, const char *password);

int main(int argc, char *argv[])
{
	char account[100];
	char passwd[100];
	char *ip_adr = "unknown";

   openlog("alt-login", LOG_PID, LOG_AUTH);

	int i;
   bool ip = false;
	for(i = 0; i < argc; i++) {
		if (ip == true) {
			ip_adr = argv[i];
			ip = false;
		} else if (strcasecmp(argv[i], "--src-ip") == 0) {
			ip = true;
		} else if (strncasecmp(argv[i], "ip=", 3) == 0) {
			ip_adr = argv[i] + 3;
		}
	}

	syslog(LOG_INFO, "v%s - connect from address %s", VERSION, ip_adr);

	printf("account: ");
	scanf("%80s", account);

	if (strcmp(account, "root") == 0) {
		syslog(LOG_ERR, "v%s - root access not permitted from address %s", VERSION, ip_adr);
		syslog(LOG_ERR, "v%s - %s access denied from address %s", VERSION, account, ip_adr);
		printf("not permitted\n");
		closelog();
		return 2;
	}

	printf("password: ");

	struct termios t;
	int fd = fileno(stdin);
	tcgetattr(fd, &t);
	t.c_lflag &= ~ECHO;
	tcsetattr(fd, TCSANOW, &t);

	scanf("%80s", passwd);

	t.c_lflag += ECHO;
	tcsetattr(fd, TCSANOW, &t);

	printf("\n");

	if (checkPasswd(account, passwd) == 0) {
		syslog(LOG_INFO, "v%s - %s logged in, starting shell for address %s", VERSION, account, ip_adr);
		char cmd[1024];
		sprintf(cmd, "login -f %s", account);
		system(cmd);
	} else {
		syslog(LOG_ERR, "v%s - %s access denied from address %s", VERSION, account, ip_adr);
		closelog();
		return 1;
	}

	printf("exited\n");
	closelog();
	return 0;
}

int checkPasswd( const char* user, const char* password )
{
    struct passwd* passwdEntry = getpwnam( user );
    if ( !passwdEntry )
    {
        printf( "User '%s' doesn't exist\n", user );
        return 1;
    }

    if ( 0 != strcmp( passwdEntry->pw_passwd, "x" ) )
    {
        return strcmp( passwdEntry->pw_passwd, crypt( password, passwdEntry->pw_passwd ) );
    }
    else
    {
        // password is in shadow file
        struct spwd* shadowEntry = getspnam( user );
        if ( !shadowEntry )
        {
            printf( "Failed to read shadow entry for user '%s'\n", user );
            return 1;
        }

        return strcmp( shadowEntry->sp_pwdp, crypt( password, shadowEntry->sp_pwdp ) );
    }
}


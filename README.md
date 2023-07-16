# alt-login for ttyd

https://github.com/tsl0922/ttyd is a simple command-line tool for sharing terminal over the web.

alt-login is an alternative to the 'login' program that can be used with ttyd. It will deny root access. 
Also it will log to /var/log/auth.log the user and, in conjunction with the right Apache configuration, the remote address of the user logging in. 

See the apache2/shell.conf example configuration for how to configure apache2 as a reverse proxy to ttyd and passing the remote address to alt-login. 

Running install.sh will 
* compile alt-login.c to /opt/ttyd/alt-login
* Ask you to put a ttyd binary in /opt/ttyd
* install /opt/ttyd/ttyd as a systemd service
* install a fail2ban filter for alt-login that will block the remote address after 5 failed login attempts



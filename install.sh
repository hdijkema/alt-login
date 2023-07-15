#!/bin/bash

USER=`whoami`

if [ "$USER" != "root" ]; then
  echo "Run this installer as root"
  exit 1
fi

INS_DIR=/opt/ttyd

if [ ! -d $INS_DIR ]; then
   echo "Making $INS_DIR"
   mkdir -p $INS_DIR
   echo ""
   echo "Now put a ttyd binary in /opt/ttyd"
   exit 0;
else 
   if [ ! -e $INS_DIR/ttyd ]; then
      echo "Make sure a executable ttyd binary is in /opt/ttyd before you continue"
      exit 1
   fi
fi

echo "Checking stuff"
echo "Fail2ban"
if [ ! -d /etc/fail2ban/filter.d ]; then
   echo "No /etc/fail2ban/filter.d - cannot install alt-login"
   exit 1
fi
if [ ! -d /etc/fail2ban/jail.d ]; then
   echo "No /etc/fail2ban/jail.d - cannot install alt-login"
   exit 1
fi
if [ ! -d /etc/systemd/system ]; then
   echo "No /etc/systemd/system - cannot install alt-login"
   exit 1
fi

echo "Making installation dir $INS_DIR"
mkdir -p $INS_DIR

echo ""

echo "Compiling alt-login to $INS_DIR"
(cd src; make)

echo ""

echo "Installing fail2ban files"
echo "filter to /etc/fail2ban/filter.d"
cp fail2ban/filter.d/* /etc/fail2ban/filter.d
echo "jail to /etc/fail2ban/jail.d"
cp fail2ban/jail.d/* /etc/fail2ban/jail.d
systemctl restart fail2ban

echo ""
echo "Systemd"
cp systemd/ttyd.service /etc/systemd/system
systemctl start ttyd
systemctl enable ttyd

echo ""
echo "Congratulations!"
echo ""
echo "Installed alt-login to /opt/ttyd/alt-login"

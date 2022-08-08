#!/bin/bash

if [ "$1" == "--clear-iptable" ]; then

  sudo iptables --table nat --flush

fi

if [ "$1" == "--list-iptable" ]; then

  sudo iptables -t nat -L -n -v

elif [ "$1" == "--config" ]; then

 if [ "$#" -eq 1 ]; then
    echo "please provide the public IP address to redirect from"
  else

  echo "1. Please copy the follow configuration and paste in /etc/ufw/before.rules"
  echo ""

  printf \
"\---------------------------------------------------- COPY BELOW
*nat
:PREROUTING ACCEPT [0:0]
-A PREROUTING -i eth0 -d 206.189.0.154 -p tcp --dport 5000 -j  DNAT --to-destination 127.0.0.1:5000
-A POSTROUTING -s 127.0.0.1/24 ! -d 127.0.0.1/24 -j MASQUERADE
COMMIT
\----------------------------------------------------  COPY ABOVE "

 echo ""; echo ""
 echo "Copy the above and type any key when you are ready."
 read 
 sudo vim /etc/ufw/before.rules

 # bash -c ""
 echo " "
 echo "Uncomment'net.ipv4.ip_forward=1' in /etc/sysctl.conf"

 echo "type any key when you are ready..."
 read 
 sudo vim /etc/sysctl.conf

 echo "restarting ufw..."
 sudo systemctl restart ufw
 echo "done!"

 echo "Openning port 5000"
 sudo ufw allow proto tcp from any to 127.0.0.1 port 5000

 echo "Check status ufw below"
 sudo ufw status
 fi

 # sudo vim 
elif [ "$1" == "--redirect" ]; then
  # Maps incoming packages from hardware connection port 5000 to localhost:5000

    echo "Redirecting dport 5000 to localhost:5000."

    sudo iptables \
      --table nat \
      --append PREROUTING \
      --in-interface eth0 \
      --protocol tcp \
      --destination 206.189.0.154 \
      --dport 5000 \
      --jump DNAT \
      --to 127.0.0.1:5000 

   # To allow LAN nodes with private IP addresses to communicate with external public networks,
   # configure the firewall for IP masquerading,
   # which masks requests from LAN nodes with the IP address of the firewall’s external device such as eth0
   sudo iptables \
     --table nat \
     --append POSTROUTING \
     --out-interface eth0 \
     --jump MASQUERADE

  fi
fi

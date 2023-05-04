#!/usr/bin/env sh

usermod -a -G input $USER
grep -r "<username>" client.service -l | xargs sed -i "s/<username>/$USER/g";
cp build/client /usr/bin
cp client.service /etc/systemd/system
systemctl daemon-reload
systemctl enable client

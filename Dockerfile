FROM base/archlinux
RUN pacman-key --refresh-keys
RUN pacman -Syyu --noconfirm
RUN pacman-db-upgrade
RUN pacman -S --noconfirm make cmake gcc git
RUN pacman -S --noconfirm gstreamer 
RUN pacman -S --noconfirm gst-plugins-base
RUN git clone --recursive https://github.com/Glebka/jenkins-vr.git /root/jenkins-vr

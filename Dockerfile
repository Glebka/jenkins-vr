FROM base/archlinux
RUN pacman -Syyy --noconfirm
RUN pacman -S --noconfirm make cmake gstreamer gst-plugins-base gst-plugins-good gst-plugins-bad gcc git
RUN git clone --recursive https://github.com/Glebka/jenkins-vr.git /root/jenkins-vr
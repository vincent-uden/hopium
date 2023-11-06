FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update
RUN apt-get install -y \
    build-essential \
    git \
    cmake \
    software-properties-common \
    qtbase5-dev \
    qtchooser \
    qt5-qmake \
    qtbase5-dev-tools
RUN apt-get -y install tcl tcl-dev tk tk-dev libfreeimage-dev
RUN apt-get -y install libxmu-dev libxi-dev
RUN apt-get -y install libglfw3-dev libgl1-mesa-dev libglu1-mesa-dev
RUN apt-get -y install xvfb

RUN git clone https://github.com/Open-Cascade-SAS/OCCT.git opencascade
WORKDIR /opencascade
RUN git checkout V7_4_0 -b dev-branch
RUN mkdir -p build
WORKDIR /opencascade/build
RUN cmake .. \
       -DCMAKE_BUILD_TYPE=release \
       -DCMAKE_INSTALL_RPATH="" \
       -DCMAKE_INSTALL_PREFIX=/usr \
       -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
       -DUSE_FREEIMAGE=OFF \
       -DUSE_FFMPEG=OFF \
       -DUSE_VTK=OFF \
       -DUSE_TBB=OFF
RUN make
RUN make install
RUN apt-get -y install xfonts-scalable libocct-data-exchange-dev libocct-draw-dev libocct-foundation-dev libocct-modeling-algorithms-dev libocct-modeling-data-dev libocct-ocaf-dev libocct-visualization-dev

RUN add-apt-repository ppa:neovim-ppa/unstable
RUN apt-get -y update
RUN apt-get -y install neovim curl
RUN sh -c 'curl -fLo "${XDG_DATA_HOME:-$HOME/.local/share}"/nvim/site/autoload/plug.vim --create-dirs https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim'
RUN git clone https://github.com/vincent-uden/dotfiles && cd dotfiles && bash ./update_system.sh && cd ..
RUN timeout 20 nvim -c PlugInstall > /dev/null; exit 0

RUN apt-get -y install clangd
RUN apt-get -y install tmux gdb

WORKDIR /opt/sources

# ADD . /opt/sources
# RUN mkdir build && \
#     cd build && \
#     cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && \
#     ls /usr/lib && \
#     make Lesson_04 && \
#     ls /opt/sources/build

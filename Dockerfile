FROM ubuntu:latest

# Docker CE/EE on Linux: Inside the container, any mounted files/folders will have the 
# exact same permissions as outside the container - including the owner user ID (UID) 
# and group ID (GID). Because of this, your container user will either need to have the 
# same UID or be in a group with the same GID. 
# The actual name of the user / group does not matter. 
# The first user on a machine typically gets a UID of 1000, so most containers use this 
# as the ID of the user to try to avoid this problem.

ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

ARG DEBIAN_FRONTEND=noninteractive
ARG CLANG_VERSION=12

RUN apt-get update \
    && apt-get -y install --no-install-recommends apt-utils 2>&1 \
    #
    # base tools
    && apt-get -y install git procps lsb-release wget build-essential 
    
# Create a non-root user to use if preferred - see https://aka.ms/vscode-remote/containers/non-root-user.
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd -s /bin/bash --uid $USER_UID --gid $USER_GID -m $USERNAME \
    # [Optional] Uncomment the next three lines to add sudo support
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME


# [Optional] Update UID/GID if needed
RUN if [ "$USER_GID" != "1000" ] || [ "$USER_UID" != "1000" ]; then \
    groupmod --gid $USER_GID $USERNAME \
    && usermod --uid $USER_UID --gid $USER_GID $USERNAME \
    && chown -R $USER_UID:$USER_GID /home/$USERNAME; \
    fi

RUN apt-get update \
    && apt-get -y install --no-install-recommends \
    #&& apt-get -y install \
    clang \
    clangd \
    clang-format \
    clang-tidy \
    cmake \
    ninja-build \
    valgrind \
    #
    # OpenMP support for clang
    libomp-dev \
    # OpenMPI support
    libopenmpi-dev
    #&& ln -s /usr/bin/clang-tidy /usr/bin/clang-tidy \
    #&& ln -s /usr/bin/lldb /usr/bin/lldb \
    #&& ln -sf /usr/bin/lldb-server /usr/lib/llvm-10/bin/lldb-server.0.1 \
    # Fixes clangd
    #&& ln -sf /usr/lib/llvm/include/c++/v1 /usr/include/c++/v1 \
    #&& ln -sf /usr/lib/llvm/include/ /usr/include/

RUN for c in $(ls /usr/bin/clang*); do link=$(echo $c | sed "s///"); ln -sf $c $link; done && \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100 && \
    update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100

RUN apt-get autoremove -y \
    && apt-get clean -y

ENV CC="/usr/bin/clang" \
    CXX="/usr/bin/clang++" \
    COV="/usr/bin/llvm-cov" \
    LLDB="/usr/bin/lldb"

# Copy the parent folder which contains C++ source code to the Docker image under /usr/src/
COPY . /usr/src/fha-parallelprogrammierung/

# Specify the working directory
WORKDIR /usr/src/fha-parallelprogrammierung/

#RUN export OMP_NUM_THREADS=5

# Use clang to compile the hello_world.c source file
RUN clang -o hello_world -fopenmp=libomp hello_world.c

ENTRYPOINT ["/bin/bash"]

# Run the output program from the previous step
CMD ["./hello_world"]

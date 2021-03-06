ARG CARLA_VERSION=0.9.3

FROM nvidia/opengl:1.1-glvnd-runtime-ubuntu16.04 as nvidia-opengl
FROM carlasim/carla:$CARLA_VERSION as carla

FROM autoware/autoware:1.9.1-kinetic

ARG CARLA_VERSION

USER root
RUN apt-get update \
	&& apt-get install -y python-pip libpng16-16 python-pygame ros-kinetic-ackermann-msgs ros-kinetic-derived-object-msgs --no-install-recommends \
	&& rm -rf /var/lib/apt/lists/*
RUN pip install simple-pid

COPY --from=nvidia-opengl /usr/local/lib/x86_64-linux-gnu /usr/local/lib/x86_64-linux-gnu
COPY --from=nvidia-opengl /usr/local/lib/i386-linux-gnu /usr/local/lib/i386-linux-gnu
COPY --from=nvidia-opengl /usr/local/share/glvnd/egl_vendor.d/10_nvidia.json /usr/local/share/glvnd/egl_vendor.d/10_nvidia.json

COPY --from=nvidia-opengl /etc/ld.so.conf.d/glvnd.conf /etc/ld.so.conf.d/glvnd.conf

ENV NVIDIA_VISIBLE_DEVICES \
         ${NVIDIA_VISIBLE_DEVICES:-all}
ENV NVIDIA_DRIVER_CAPABILITIES \
         ${NVIDIA_DRIVER_CAPABILITIES:+$NVIDIA_DRIVER_CAPABILITIES,}graphics,compat32,display

ENV LD_LIBRARY_PATH /usr/local/lib/x86_64-linux-gnu:/usr/local/lib/i386-linux-gnu${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}

COPY --from=carla --chown=autoware /home/carla/PythonAPI home/$USERNAME/carla-autoware/PythonAPI
ENV PYTHONPATH=/home/$USERNAME/carla-autoware/PythonAPI/carla-$CARLA_VERSION-py2.7-linux-x86_64.egg:/home/$USERNAME/carla-autoware/PythonAPI

USER autoware

RUN cd && mkdir -p /home/$USERNAME/carla-autoware
COPY --chown=autoware catkin_ws /home/$USERNAME/carla-autoware/catkin_ws

RUN /bin/bash -c 'source /home/$USERNAME/Autoware/ros/devel/setup.bash; cd /home/$USERNAME/carla-autoware/catkin_ws/src; catkin_init_workspace; cd ../; catkin_make -DCMAKE_BUILD_TYPE=Release'

COPY --chown=autoware autoware_data /home/$USERNAME/carla-autoware/autoware_data
COPY --chown=autoware autoware_launch /home/$USERNAME/carla-autoware/autoware_launch
COPY --chown=autoware docker/param.yaml /home/$USERNAME/Autoware/ros/src/util/packages/runtime_manager/scripts/param.yaml

ENV CARLA_AUTOWARE_ROOT=/home/$USERNAME/carla-autoware

RUN echo "source /home/$USERNAME/carla-autoware/catkin_ws/devel/setup.bash" >> /home/$USERNAME/.bashrc


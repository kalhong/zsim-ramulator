#-*- mode: ruby -*-
# vi: set ft=ruby :

Vagrant::Config.run do |config|
  config.vm.box = "hashicorp/precise64"
  config.vm.box_url = "https://app.vagrantup.com/hashicorp/boxes/precise64"

  config.vm.customize ["modifyvm", :id, "--memory", 40960]
  config.vm.customize ["modifyvm", :id, "--cpus", 16]

  config.vm.provision :shell, :inline => <<-SH
    # Packages
    export DEBIAN_FRONTEND=noninteractive
    apt-get -y update
    # apt-get -y dist-upgrade

    # zsim dependencies
    apt-get -y install build-essential g++ git scons
    apt-get -y install libelfg0-dev libhdf5-serial-dev libconfig++-dev

    # dependencies for zsim's Fortran and Java hooks (misc/hooks)
    apt-get -y install gfortran openjdk-7-jdk

    # other packages
    apt-get -y install vim

    # HJHJ
    apt-get install -y python-software-properties
    add-apt-repository -y ppa:ubuntu-toolchain-r/test
    apt-get update
    apt-get install -y gcc-4.8
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 50
    apt-get install -y g++-4.8
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 50
    apt-get install -y clang-3.4
    
    # Get Pin & set PINPATH
    #PINVER="pin-2.12-58423-gcc.4.4.7-linux"
    #PINVER="pin-2.13-62732-gcc.4.4.7-linux"
    #if [ ! -d ~vagrant/${PINVER} ]; then
    #    echo "Downloading Pin version ${PINVER}"
    #    sudo -u vagrant wget -nc -nv http://software.intel.com/sites/landingpage/pintool/downloads/${PINVER}.tar.gz
    #    sudo -u vagrant tar xzf ${PINVER}.tar.gz
    #    echo "export PINPATH=~/${PINVER}" >> ~vagrant/.bashrc
    #fi

    # HJHJ
    echo "source /vagrant/env.sh" >> ~vagrant/.bashrc
    echo "cd /vagrant" >> ~vagrant/.bashrc
    echo "set cindent shiftwidth=4 expandtab smarttab" >> ~vagrant/.vimrc

    # Configure system flags
    sysctl -w kernel.shmmax=1073741824
    sysctl -w kernel.yama.ptrace_scope=0
  SH
end

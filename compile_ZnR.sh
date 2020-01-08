
pushd /vagrant/
    cd /vagrant/ramulator/
    make -j libramulator
popd

scons -j40

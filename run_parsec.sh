
ARGC=$#
BENCH=$1

pushd /vagrant/
    cd /vagrant/parsec/
    ../build/opt/zsim ../bench_tests/$BENCH.cfg
popd

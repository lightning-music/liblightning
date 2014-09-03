function pkg_install {
    local pkg="$1"; shift
    local tarball=$(basename $pkg)
    local dir=${tarball/%.tar.gz/}
    wget $pkg
    tar xzf $tarball
    cd $dir
    ./configure && make && sudo make install
    cd ..
    rm -rf $dir $tarball
}

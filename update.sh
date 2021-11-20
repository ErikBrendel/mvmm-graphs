# pull latest changes and build the new executable, directly replacing any old build
git pull && cmake . -DCMAKE_BUILD_TYPE=RelWithDebInfo && make -j

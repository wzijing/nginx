./../sbin/nginx -s stop

./configure --prefix=/usr/local/nginx_test/   --add-module=/usr/local/nginx_test/mytest_module
make
make install

./../sbin/nginx 

编译请执行./makefile命令，得到的程序分别在client和server文件夹下

本程序是一个远程文件复制程序

服务端使用
./remote_cp_server监听端口

服务端增加账号密码
./remote_cp_server -u username -p password

客户端使用
./remote_cp_client -u username -p password -f xxxx:/path1/to1/file1 -t /path2/to2/file2 
xxx为远程ip,将远程服务器上的file1复制到本地/path2/to2/file2上

客户端使用
./remote_cp_client -u username -p password -f /path1/to1/file1 -t xxxxxx:/path2/to2/file2 
xxx为远程ip,将本地的file1复制到远程服务器的/path2/to2/file2

安全技术：
1.本程序在连接建立时先使用openssl diff hellman协议进行密钥协商，后面所有的流量都使用该密钥进行aes加密
2.程序交换密钥后会验证username和password，服务端的password经过sha256哈希后和username存在config文件中
3.编译环境安装ollvm后使用makefile_o进行编译会对生成的二进制文件进行混淆

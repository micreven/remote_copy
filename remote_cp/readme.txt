������ִ��./makefile����õ��ĳ���ֱ���client��server�ļ�����

��������һ��Զ���ļ����Ƴ���

�����ʹ��
./remote_cp_server�����˿�

����������˺�����
./remote_cp_server -u username -p password

�ͻ���ʹ��
./remote_cp_client -u username -p password xxxx:/path1/to1/file1 /path2/to2/file2 
xxxΪԶ��ip,��Զ�̷������ϵ�file1���Ƶ�����/path2/to2/file2��

�ͻ���ʹ��
./remote_cp_client -u username -p password /path1/to1/file1 xxxxxx:/path2/to2/file2 
xxxΪԶ��ip,�����ص�file1���Ƶ�Զ�̷�������/path2/to2/file2

��ȫ������
1.�����������ӽ���ʱ��ʹ��openssl diff hellmanЭ�������ԿЭ�̣��������е�������ʹ�ø���Կ����aes����
2.���򽻻���Կ�����֤username��password������˵�password����sha256��ϣ���username����config�ļ���
3.���뻷����װollvm��ʹ��makefile_o���б��������ɵĶ������ļ����л���

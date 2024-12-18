curl -H "Host: example.com" http://123.45.67.89
curl --resolve test3:10002:127.0.0.1 test3:10002

curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit" 127.0.0.1:10005/cgi-bin/print_env_body.py

curl -X POST -H "Content-Type: plain/text" --data "BODY IS HERE write something shorter or longer than body limit" 127.0.0.1:10005/uploads/

curl -X POST --data-binary ./root/test/42_2.png 127.0.0.1:10005/uploads/


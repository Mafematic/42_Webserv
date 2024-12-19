curl -v -i -X GET -H "Host: test2" http://127.0.0.1:10002/index.html

curl -v -i --resolve test2:10002:127.0.0.1 test2:10002

curl -v -i --resolve test2:10003:127.0.0.1 test2:10003
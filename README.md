
# I always use cesanta's mongoose web server ( not MongoDB DB ) as my application web UI server , it is really good , why not trying to write one myself?


this demo shows one possible Implementation of web server by linux's system call and c++ method


## program architecture

to make story more complicated we has a main socket loop to waiting for web requests, when receiving the request are immediately assign to a thread-pool, then the socket loop continue to wait other coming request.

When the thread-pool getting the request, it can chose where to send where the task to , when have following options to represent possible web server architecture

1. transfer the task to other thread in same program
2. transfer the task through IPC (inter process communication) to other process
3. transfer the task through socket to other process ( can be other ip )
 
when the handler completed the task either successful of fail, the handler return the response to main socket loop , main socket loop would send back the response to the file descriptor. At this moment , the requester can get response.



     +-----------------------------------------------------------------------------+--------------------------------------+
     |                                                                             |                                      |        
 web |   +--------+                                                                |                                      |        
 =====>  | select |   send task   +-------------+   inner task +--------------+    |                                      |        
     |   | socket |  ===========> |   task      | ===========> |    busy job  |    |                                      |        
 <=====  +________+    response   |  transfer   |    response  |     looping  |    |                                      |        
     |               <=========== |             | <=========== |              |    |                                      |        
     |                            |             |              +--------------+    |          +------------------+        |
     |                            |             |           inter process task                |                  |        |
     |                            |             |   =========================================>|   other process  |        |
     |                            |             |                   response                  |                  |        |
     |                            |             |   <=========================================|                  |        |
     |                            |             |                                             +------------------+        |
     |                            |             |                                  |                                      |
     |                            |             |                                  |                                                    +------------------+
     |                            |             |                 cross socket task                                                     |                  |
     |                            |             |======================================================================================>|   other pc       |
     |                            |             |                 response                                                              |                  |
     |                            |             | <=====================================================================================|                  |
     |                            |             |                                                                                       +------------------+
     |                            +-------------+                                                                         |
     +--------------------------------------------------------------------------------------------------------------------+


# install

go to the folder with makefile

```
make 
```

there are three program here, a main demo server and additional 2 program ( ipc server and socket server )

all of them can directly use make to compile


##demo api

```
GET /api/5566
```

desription:

simplest GET api to make sure the server is running

request:
```
curl --location --request GET 'http://0.0.0.0:8080/api/5566' --header 'Content-Type: text/plain' --data-raw 'hi'
```

response:
```
5566 is good
```



```
POST /api/jobInThread
```

desription:

This api demo pass the task to a exist working thread in the ,  the number ++ every 5 second and +4 every request . 

request:
```
curl --location --request POST 'http://0.0.0.0:8080/api/jobInThread' --header 'Content-Type: text/plain' --data-raw 'hi'
```

response:
'''
this is job result : 55
'''


'''
POST /api/jobToIPC
'''

desription:

This api demo pass the task to a exist working thread in the ,  the number ++ every 5 second and +4 every request . 


request:
'''
curl --location --request POST 'http://0.0.0.0:8080/api/jobToIPC' --header 'Content-Type: text/plain' --data-raw 'hi'
'''

response:
'''
Message received! 
'''


'''
POST /api/jobToSocket
'''

request:
'''
curl --location --request POST 'http://0.0.0.0:8080/api/jobToIPC' --header 'Content-Type: text/plain' --data-raw 'hi'
'''

response:
'''
Server received: from simple web server ... 
'''



web page

the following link would lead to corresponsd webpage, or it would show a 404 not found page
'''
http://0.0.0.0:8080/index.html

http://0.0.0.0:8080/page1.html

http://0.0.0.0:8080/page2.html

http://0.0.0.0:8080/page3.html
'''



## p.s.

The main structure of the TCP select connection code and thread-pool is writed by ChatGPT , I modified it to fit web server purpose. Thanks to ChatGPT!

the makefile is modified from the auto-generated makefile form qmake, the format is beautiful and I don't see any disadvantage inside. I keep the qmake pro file here if someone interested.

you can use libmodbus or modbus poll to verify the modbus's behavior




cheer!



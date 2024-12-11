Required Env Var Fields:

CONTENT_LENGTH -> size of message body, otherwise NULL or unset
CONTENT_TYPE -> only when request body, copy from client request, otherwise omit
GATEWAY_INTERFACE -> dialect of cgi, e.g. CGI/1.1
PATH_INFO -> everything that comes in uri after actual cgi script
PATH_TRANSLATED -> map path_info to server document structure (alias, root, etc.)
QUERY_STRING -> empty string if not in uri, otherwise query string from uri
REMOTE_ADDR -> ip address of client
REMOTE_HOST -> domain name of client sending request to server
REQUEST_METHOD -> get etc.
SCRIPT_NAME -> path to script without the Path_INFO (everything after the script)
SERVER_NAME -> hostname or ip_address of server, the client request is going to
SERVER_PORT -> server_port on which client request is received
SERVER_PROTOCOL -> e.g. HTTP/1.1
SERVER_SOFTWARE -> socket_squad_404/1.0
-> send all HTTP_HEADER_Fields as HTPP_
-> remove authentication/authorisation
-> remove stuff already available like CONTENT_LENGTH, CONTENT_TYPE

- make a location, where all cgi scripts can be found
- specify valid extensions for cgi scripts, e.g. .py .php .sh
- specify, where the cgi interpreters can be found e.g. interpreter .py /usr/bin/python3;
- execute cgi in correct relative diretory
- send post and get request via html form

- set relevant env vars before executing cgi
- fork
- write body to pipe
- read body from pipe


- getCgiEnvironment();
- fork();
- changeDirectory();
- parent: writeBodyToPipe();
- child: readBodyFromPipe();
- child: to its thing
- child: write response to pipe
- parent read from pipe

- run cgi in correct directory

- delete file in cgi controller, after child has finished
- handle errors within cgi
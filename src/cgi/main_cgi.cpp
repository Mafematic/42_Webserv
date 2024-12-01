
#include "webserv.hpp"
#include "Cgi_Controller.hpp"

int main(void)
{
    Cgi_Controller controller;
    controller.start_cgi();
}
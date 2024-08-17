#include "httpRequestParser.hpp"
#include "ServerConfig.hpp"

std::string processRequest(std::string request, std::vector<ServerConfig> settings, unsigned char * client_ip) {
	(void)settings;
	(void)client_ip;
	httpRequestParser parser(request);
	parser.parse();

	// verify if the request is valid
	int requestCode = parser.checkRequest();
	if (requestCode != 200)
		return "";


	std::cout << "Request: " << request << std::endl;
	std::cout << "Method: " << parser.getMethod() << std::endl;
    std::cout << "URI: " << parser.getUri() << std::endl;
    std::cout << "Version: " << parser.getVersion() << std::endl;

    std::cout << "Headers:" << std::endl;
    std::map<std::string, std::string> headers = parser.getHeaders();
    for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
    {
        std::cout << "\t" << it->first << ": " << it->second << std::endl;
    }

    std::cout << "Body: " << parser.getBody() << std::endl;

    uriComponents uriComps = parser.getUriComponents();
    std::cout << "\tURI Components:" << std::endl;
    std::cout << "\t Scheme:\t " << uriComps.scheme << std::endl;
    std::cout << "\t Host:\t " << uriComps.host << std::endl;
    std::cout << "\t Port:\t " << uriComps.port << std::endl;
    std::cout << "\t Path:\t " << uriComps.path << std::endl;
    std::cout << "\t Query:\t " << uriComps.query << std::endl;
    std::cout << "\t Fragment:\t " << uriComps.fragment << std::endl;
	return ""; // tmp
}

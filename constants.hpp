#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define UNDEFINED (-2)

// @defgroup status codes
#define OK 200
#define BAD_REQUEST 400
#define FORBIDDEN 403
#define NOT_FOUND 404
#define INTERNAL_SERVER_ERROR 500
#define INSUFFICIENT_STORAGE 507
#define SERVICE_UNAVAILABLE 503

// @def default response body content
#define DEFAULT_BODY "Hello, World!"

// @defgroup error messages
#define STATE_NOT_FOUND "State not found"
#define SERVER_DUPLICATE "Server instance already exists"

#define READ_FILE_FAILURE "Failed to read the file"
#define SAVE_FILE_FAILURE "Failed to save the file"
#define DELETE_FILE_FAILURE "Failed to delete the file"

#define READ_CGI_FAILURE "Failed to read CGI"
#define CGI_DATA_SEND_FAILURE "Failed to send data to CGI"
#define CGI_EXECUTION_FAILURE "Failed to execute CGI"

#define READ_CGI_OUTPUT_FAILURE "Failed to read CGI output"

#define TIMEOUT_ERROR "Timeout error"

#define MISSING_CONTENT_LENGTH "Missing Content-Length value"
#define INVALID_CONTENT_LENGTH "Content-Length value is not valid"
#define FORBIDDEN_REQUEST_METHOD "Request method is forbidden"
#define INVALID_REQUEST_FORMAT "Request format is not valid"

#endif // CONSTANTS_HPP
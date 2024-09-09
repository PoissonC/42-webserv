#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#define SUCCESS 0
#define FAILURE (-1)
#define UNDEFINED (-2)

// @defgroup status codes
#define OK 200
#define CREATED 201
#define NO_CONTENT 204
#define MOVED_PERMANENTLY 301
#define FOUND 302
#define SEE_OTHER 303
#define TEMPORARY_REDIRECT 307
#define PERMANENT_REDIRECT 308
#define BAD_REQUEST 400
#define FORBIDDEN 403
#define NOT_FOUND 404
#define METHOD_NOT_ALLOWED 405
#define LENGTH_REQUIRED 411
#define INTERNAL_SERVER_ERROR 500
#define SERVICE_UNAVAILABLE 503
#define HTTP_VERSION_NOT_SUPPORTED 505
#define INSUFFICIENT_STORAGE 507

// @defgroup cookie
#define COOKIE_MAX_AGE 600

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
#define WRONG_FORMAT_CGI_OUTPUT "CGI output parsing failed"

#define TIMEOUT_ERROR "Timeout error"

#define MISSING_CONTENT_LENGTH "Missing Content-Length value"
#define EXTRA_CONTENT_LENGTH "Method shouldn't contain Content-Length header"
#define INVALID_CONTENT_LENGTH "Content-Length value is not valid"
#define CONTENT_LENGTH_TOO_LARGE "Content-Length value is too large"
#define FORBIDDEN_REQUEST_METHOD "Request method is forbidden"
#define INVALID_REQUEST_FORMAT "Request format is not valid"

#endif // CONSTANTS_HPP
